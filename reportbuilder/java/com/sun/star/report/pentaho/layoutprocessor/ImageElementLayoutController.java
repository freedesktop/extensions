/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: ImageElementLayoutController.java,v $
 * $Revision: 1.7 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
package com.sun.star.report.pentaho.layoutprocessor;

import com.sun.star.report.pentaho.OfficeNamespaces;
import com.sun.star.report.OfficeToken;
import com.sun.star.report.pentaho.model.ImageElement;
import org.pentaho.reporting.libraries.formula.Formula;
import org.pentaho.reporting.libraries.formula.lvalues.LValue;
import org.pentaho.reporting.libraries.formula.parser.ParseException;
import org.jfree.layouting.util.AttributeMap;
import org.jfree.report.DataSourceException;
import org.jfree.report.JFreeReportInfo;
import org.jfree.report.ReportDataFactoryException;
import org.jfree.report.ReportProcessingException;
import org.jfree.report.util.TextUtilities;
import org.jfree.report.data.GlobalMasterRow;
import org.jfree.report.data.ReportDataRow;
import org.jfree.report.expressions.FormulaExpression;
import org.jfree.report.flow.FlowController;
import org.jfree.report.flow.ReportTarget;
import org.jfree.report.flow.layoutprocessor.LayoutController;
import org.jfree.report.flow.layoutprocessor.LayoutControllerUtil;
import org.jfree.report.structure.Element;
import org.jfree.report.structure.Section;
import org.jfree.report.structure.Node;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.pentaho.reporting.libraries.base.util.ObjectUtilities;

/**
 * Produces an image. The image-structures itself (draw:frame and so on) are not generated here. This element produces a
 * place-holder element and relies on the output target to compute a sensible position for the element. The report
 * definition does not give any hints about the size of the image, so we have to derive this from the surrounding
 * context.
 *
 * @author Thomas Morgner
 * @since 05.03.2007
 */
public class ImageElementLayoutController
        extends AbstractReportElementLayoutController
{
    private static final Log LOGGER = LogFactory.getLog(ImageElementLayoutController.class);
    private ImageElementContext context;

    public ImageElementLayoutController()
    {
    }

    protected LayoutController delegateContentGeneration(final ReportTarget target)
            throws ReportProcessingException, ReportDataFactoryException,
            DataSourceException
    {
        final ImageElement imageElement = (ImageElement) getNode();
        final FormulaExpression formulaExpression = imageElement.getFormula();
        if (formulaExpression == null)
        {
            // A static image is easy. At least at this level. Dont ask about the weird things we have to do in the
            // output targets ...
            final String linkTarget = imageElement.getImageData();
            generateImage(target, linkTarget, imageElement.isScale(), imageElement.isPreserveIRI());
        }
        else
        {
            final Object value =
                    LayoutControllerUtil.evaluateExpression(getFlowController(), imageElement, formulaExpression);
            generateImage(target, value, imageElement.isScale(), imageElement.isPreserveIRI());
        }
        return join(getFlowController());
    }

    private void generateImage(final ReportTarget target,
            final Object linkTarget,
            final boolean scale,
            final boolean preserveIri)
            throws ReportProcessingException, DataSourceException
    {
        if (linkTarget == null)
        {
            return;
        }

        final AttributeMap image = new AttributeMap();
        image.setAttribute(JFreeReportInfo.REPORT_NAMESPACE, Element.NAMESPACE_ATTRIBUTE, JFreeReportInfo.REPORT_NAMESPACE);
        image.setAttribute(JFreeReportInfo.REPORT_NAMESPACE, Element.TYPE_ATTRIBUTE, OfficeToken.IMAGE);
        image.setAttribute(JFreeReportInfo.REPORT_NAMESPACE, OfficeToken.SCALE, String.valueOf(scale));
        image.setAttribute(JFreeReportInfo.REPORT_NAMESPACE, OfficeToken.PRESERVE_IRI, String.valueOf(preserveIri));
        image.setAttribute(JFreeReportInfo.REPORT_NAMESPACE, "image-context", createContext());
        image.setAttribute(JFreeReportInfo.REPORT_NAMESPACE, OfficeToken.IMAGE_DATA, linkTarget);
        target.startElement(image);
        target.endElement(image);
    }

    protected ImageElementContext createContext()
    {
        if (context == null)
        {

            // Step 1: Find the parent cell.
            final LayoutController cellController = findParentCell();
            if (cellController == null)
            {
                LOGGER.warn("Image is not contained in a table. Unable to calculate the image-size.");
                return null;
            }
            final Element tableCell = (Element) cellController.getNode();
            final int rowSpan = TextUtilities.parseInt((String) tableCell.getAttribute(OfficeNamespaces.TABLE_NS, "number-rows-spanned"), 1);
            final int colSpan = TextUtilities.parseInt((String) tableCell.getAttribute(OfficeNamespaces.TABLE_NS, "number-columns-spanned"), 1);
            if (rowSpan < 1 || colSpan < 1)
            {
                LOGGER.warn("Rowspan or colspan for image-size calculation was invalid.");
                return null;
            }

            final LayoutController rowController = cellController.getParent();
            if (rowController == null)
            {
                LOGGER.warn("Table-Cell has no parent. Unable to calculate the image-size.");
                return null;
            }
            final Section tableRow = (Section) rowController.getNode();
            // we are now making the assumption, that the row is a section, that contains the table-cell.
            // This breaks the ability to return nodes or to construct reports on the fly, but the OO-report format
            // is weird anyway and wont support such advanced techniques for the next few centuries ..
            final int columnPos = findNodeInSection(tableRow, tableCell, OfficeToken.COVERED_TABLE_CELL);
            if (columnPos == -1)
            {
                LOGGER.warn("Table-Cell is not a direct child of the table-row. Unable to calculate the image-size.");
                return null;
            }

            final LayoutController tableController = rowController.getParent();
            if (tableController == null)
            {
                LOGGER.warn("Table-Row has no Table. Unable to calculate the image-size.");
                return null;
            }

            final Section table = (Section) tableController.getNode();
            // ok, we got a table, so as next we have to search for the columns now.
            final Section columns = (Section) table.findFirstChild(OfficeNamespaces.TABLE_NS, OfficeToken.TABLE_COLUMNS);
            if (columns.getNodeCount() <= columnPos + colSpan)
            {
                // the colspan is to large. The table definition is therefore invalid. We do not try to fix this.
                LOGGER.warn(
                        "The Table's defined columns do not match the col-span or col-position. Unable to calculate the image-size.");
                return null;
            }

            final ImageElementContext context = new ImageElementContext(colSpan, rowSpan);
            addColumnStyles(context, columns, columnPos, colSpan);
            // finally search the styles for the row now.
            final int rowPos = findNodeInSection(table, tableRow, null);
            if (rowPos == -1)
            {
                LOGGER.warn("Table-Cell is not a direct child of the table-row. Unable to calculate the image-size.");
                return null;
            }

            addRowStyles(context, table,rowPos,rowSpan);
            this.context = context;
        }
        return this.context;
    }

    private int findNodeInSection(final Section tableRow,
            final Element tableCell,
            final String secondType)
    {
        int retval = 0;
        final Node[] nodes = tableRow.getNodeArray();
        final String namespace = tableCell.getNamespace();
        final String type = tableCell.getType();
        for (int i = 0; i < nodes.length; i++)
        {
            final Node node = nodes[i];
            if (!(node instanceof Element))
            {
                continue;
            }
            final Element child = (Element) node;
            /*
            if (! OfficeToken.COVERED_TABLE_CELL.equals(child.getType()) &&
            (ObjectUtilities.equal(child.getNamespace(), namespace) == false ||
            ObjectUtilities.equal(child.getType(), type) == false))
             */
            if (!ObjectUtilities.equal(child.getNamespace(), namespace) ||
                    (!ObjectUtilities.equal(child.getType(), type) && (secondType == null || !ObjectUtilities.equal(child.getType(), secondType))))
            {
                continue;
            }

            if (node == tableCell)
            {
                return retval;
            }
            retval += 1;
        }
        return -1;
    }

    private LayoutController findParentCell()
    {
        LayoutController parent = getParent();
        while (parent != null)
        {
            final Object node = parent.getNode();
            if (node instanceof Element)
            {
                final Element element = (Element) node;
                if (OfficeNamespaces.TABLE_NS.equals(element.getNamespace()) && "table-cell".equals(element.getType()))
                {
                    return parent;
                }
            }
            parent = parent.getParent();
        }
        return null;
    }

    protected boolean isValueChanged()
    {
        final ImageElement imageElement = (ImageElement) getNode();
        final FormulaExpression formulaExpression = imageElement.getFormula();
        if (formulaExpression == null)
        {
            final FlowController controller = getFlowController();
            final GlobalMasterRow masterRow = controller.getMasterRow();
            final ReportDataRow reportDataRow = masterRow.getReportDataRow();
            if (reportDataRow.getCursor() == 0)
            {
                return true;
            }
            return false;
        }

        try
        {
            final Formula formula = formulaExpression.getCompiledFormula();
            final LValue lValue = formula.getRootReference();
            return isReferenceChanged(lValue);
        }
        catch (ParseException e)
        {
            return false;
        }
    }

    void addColumnStyles(final ImageElementContext context, final Section columns, final int columnPos, final int colSpan)
    {
        final Node[] columnDefs = columns.getNodeArray();
        int columnCounter = 0;
        for (int i = 0; i < columnDefs.length; i++)
        {
            final Element column = (Element) columnDefs[i];

            if (!ObjectUtilities.equal(column.getNamespace(), OfficeNamespaces.TABLE_NS) ||
                    !ObjectUtilities.equal(column.getType(), OfficeToken.TABLE_COLUMN))
            {
                continue;
            }
            if (columnCounter >= columnPos)
            {
                final String colStyle = (String) column.getAttribute(OfficeNamespaces.TABLE_NS, OfficeToken.STYLE_NAME);
                context.setColStyle(columnCounter - columnPos, colStyle);
            }

            columnCounter += 1;

            if (columnCounter >= (columnPos + colSpan))
            {
                break;
            }

        }
    }

    void addRowStyles(final ImageElementContext context, final Section table, final int rowPos, final int rowSpan)
    {
        final Node[] rows = table.getNodeArray();
        int rowCounter = 0;
        for (int i = 0; i < rows.length; i++)
        {
            final Element row = (Element) rows[i];

            if (!ObjectUtilities.equal(row.getNamespace(), OfficeNamespaces.TABLE_NS) ||
                    !ObjectUtilities.equal(row.getType(), OfficeToken.TABLE_ROW))
            {
                continue;
            }
            if (rowCounter >= rowPos)
            {
                final String rowStyle = (String) row.getAttribute(OfficeNamespaces.TABLE_NS, OfficeToken.STYLE_NAME);
                context.setRowStyle(rowCounter - rowPos, rowStyle);
            }

            rowCounter += 1;

            if (rowCounter >= (rowPos + rowSpan))
            {
                break;
            }
        }
    }
}
