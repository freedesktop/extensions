/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: genericelements.hxx,v $
 *
 * $Revision: 1.2 $
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

#ifndef INCLUDED_PDFI_GENERICELEMENTS_HXX
#define INCLUDED_PDFI_GENERICELEMENTS_HXX

#include "pdfihelper.hxx"
#include "treevisiting.hxx"

#include <com/sun/star/task/XStatusIndicator.hpp>
#include <basegfx/polygon/b2dpolypolygon.hxx>
#include <basegfx/range/b2drange.hxx>
#include <rtl/ustring.hxx>
#include <rtl/ustrbuf.hxx>

#include <list>

namespace pdfi
{
    class XmlEmitter;
    class StyleContainer;
    class ImageContainer;
    class PDFIProcessor;
    class ElementFactory;
    

    struct EmitContext
    {
        EmitContext(
            XmlEmitter&                              _rEmitter,
            StyleContainer&                          _rStyles,
            ImageContainer&                          _rImages,
            PDFIProcessor&                           _rProcessor,
            const com::sun::star::uno::Reference<
            com::sun::star::task::XStatusIndicator>& _xStatusIndicator ) :
            rEmitter(_rEmitter),
            rStyles(_rStyles),
            rImages(_rImages),
            rProcessor(_rProcessor),
            xStatusIndicator(_xStatusIndicator)
        {}

        XmlEmitter&     rEmitter;
        StyleContainer& rStyles;
        ImageContainer& rImages;
        PDFIProcessor&  rProcessor;
        com::sun::star::uno::Reference<
            com::sun::star::task::XStatusIndicator> xStatusIndicator;
    };

    struct Element : public ElementTreeVisitable
    {
    protected:
        Element( Element* pParent )
            : x( 0 ), y( 0 ), w( 0 ), h( 0 ), StyleId( -1 ), Parent( pParent )
        {
            if( pParent )
                pParent->Children.push_back( this );
        }

    public:
        virtual ~Element();

        /// Apply visitor to all children
        void applyToChildren( ElementTreeVisitor& );
        /// Union element geometry with given element
        void updateGeometryWith( const Element* pMergeFrom );

#if OSL_DEBUG_LEVEL > 1
        // xxx refac TODO: move code to visitor
        virtual void emitStructure( int nLevel );
#endif
        /** el must be a valid dereferencable iterator of el->Parent->Children
            pNewParent must not be NULL
        */
        static void setParent( std::list<Element*>::iterator& el, Element* pNewParent );
        
        double              x, y, w, h;
        sal_Int32           StyleId;
        Element*            Parent;
        std::list<Element*> Children;
    };
    
    struct ListElement : public Element
    {
        ListElement() : Element( NULL ) {}
        // ElementTreeVisitable
        virtual void visitedBy( ElementTreeVisitor&, const std::list< Element* >::const_iterator& );
    };
    
    struct HyperlinkElement : public Element
    {
        friend class ElementFactory;
    protected:
        HyperlinkElement( Element* pParent, const rtl::OUString& rURI )
        : Element( pParent ), URI( rURI ) {}
    public:
        // ElementTreeVisitable
        virtual void visitedBy( ElementTreeVisitor&, const std::list< Element* >::const_iterator& );

        rtl::OUString URI;
    };
    
    struct GraphicalElement : public Element
    {
    protected: 
        GraphicalElement( Element* pParent, sal_Int32 nGCId )
        : Element( pParent ), GCId( nGCId ), MirrorVertical( false ) {}

    public:
        sal_Int32 GCId;
        bool      MirrorVertical;
    };
    
    struct DrawElement : public GraphicalElement
    {
    protected:
        DrawElement( Element* pParent, sal_Int32 nGCId )
        : GraphicalElement( pParent, nGCId ), isCharacter(false), ZOrder(0) {}

    public:
        bool      isCharacter;
        sal_Int32 ZOrder;
    };
    
    struct FrameElement : public DrawElement
    {
        friend class ElementFactory;
    protected:
        FrameElement( Element* pParent, sal_Int32 nGCId )
        : DrawElement( pParent, nGCId ) {}

    public:
        // ElementTreeVisitable
        virtual void visitedBy( ElementTreeVisitor&, const std::list< Element* >::const_iterator& );
    };
    
    struct TextElement : public GraphicalElement
    {
        friend class ElementFactory;
    protected:
        TextElement( Element* pParent, sal_Int32 nGCId, sal_Int32 nFontId )
        : GraphicalElement( pParent, nGCId ), FontId( nFontId ) {}

    public:
        // ElementTreeVisitable
        virtual void visitedBy( ElementTreeVisitor&, const std::list< Element* >::const_iterator& );
       
        rtl::OUStringBuffer Text;
        sal_Int32           FontId;
    };

    struct ParagraphElement : public Element
    {
        friend class ElementFactory;
    protected:
        ParagraphElement( Element* pParent ) : Element( pParent ), Type( Normal ) {}

    public:
        // ElementTreeVisitable
        virtual void visitedBy( ElementTreeVisitor&, const std::list< Element* >::const_iterator& rParentIt );
        
        // returns true only if only a single line is contained
        bool isSingleLined( PDFIProcessor& rProc ) const;
        // returns the highest line height of the contained textelements
        // line height is font height if the text element is itself multilined
        double getLineHeight( PDFIProcessor& rProc ) const;
        // returns the first text element child; does not recurse through subparagraphs
        TextElement* getFirstTextChild() const;
        
        enum ParagraphType { Normal, Headline };
        ParagraphType       Type;
    };

    struct PolyPolyElement : public DrawElement 
    {
        friend class ElementFactory;
    protected:
        PolyPolyElement( Element* pParent, sal_Int32 nGCId,
                         const basegfx::B2DPolyPolygon& rPolyPoly,
                         sal_Int8 nAction );
    public:
        // ElementTreeVisitable
        virtual void visitedBy( ElementTreeVisitor&, const std::list< Element* >::const_iterator& rParentIt );
     
        void updateGeometry();

#if OSL_DEBUG_LEVEL > 1
        virtual void emitStructure( int nLevel );
#endif
        
        basegfx::B2DPolyPolygon PolyPoly;
        sal_Int8                Action;
    };
    
    struct ImageElement : public DrawElement
    {
        friend class ElementFactory;
    protected:
        ImageElement( Element* pParent, sal_Int32 nGCId, ImageId nImage )
        : DrawElement( pParent, nGCId ), Image( nImage ) {}

    public:
        // ElementTreeVisitable
        virtual void visitedBy( ElementTreeVisitor&, const std::list< Element* >::const_iterator& );
       
        ImageId Image;
    };

    struct PageElement : public Element
    {
        friend class ElementFactory;
    protected:
        PageElement( Element* pParent, sal_Int32 nPageNr )
        : Element( pParent ), PageNumber( nPageNr ), Hyperlinks(),
        TopMargin( 0.0 ), BottomMargin( 0.0 ), LeftMargin( 0.0 ), RightMargin( 0.0 ),
        HeaderElement( NULL ), FooterElement( NULL )
        {}
    private:
        // helper method for resolveHyperlinks
        bool resolveHyperlink( std::list<Element*>::iterator link_it, std::list<Element*>& rElements );
        public:
        virtual ~PageElement();

        // ElementTreeVisitable
        virtual void visitedBy( ElementTreeVisitor&, const std::list< Element* >::const_iterator& rParentIt );
       
        void emitPageAnchoredElements( EmitContext& rEmitContext );        
        static void updateParagraphGeometry( Element* pEle );
        void resolveHyperlinks();
        void resolveFontStyles( PDFIProcessor& rProc );
        void resolveUnderlines( PDFIProcessor& rProc );
        
        sal_Int32      PageNumber;
        ListElement    Hyperlinks; // contains not yet realized links on this page
        double         TopMargin;
        double         BottomMargin;
        double         LeftMargin;
        double         RightMargin;
        Element*       HeaderElement;
        Element*       FooterElement;
    };

    struct DocumentElement : public Element
    {
        friend class ElementFactory;
    protected:
        DocumentElement() : Element( NULL ) {}
    public:
        virtual ~DocumentElement();
        
        // ElementTreeVisitable
        virtual void visitedBy( ElementTreeVisitor&, const std::list< Element* >::const_iterator& ); 
       
    };
    
    // this class is the differentiator of document types: it will create
    // Element objects with an optimize() method suitable for the document type
    class ElementFactory
    {
    public:
        ElementFactory() {}
        virtual ~ElementFactory();
        
        virtual HyperlinkElement* createHyperlinkElement( Element* pParent, const rtl::OUString& rURI )
        { return new HyperlinkElement( pParent, rURI ); }
        
        virtual TextElement* createTextElement( Element* pParent, sal_Int32 nGCId, sal_Int32 nFontId )
        { return new TextElement( pParent, nGCId, nFontId ); }
        virtual ParagraphElement* createParagraphElement( Element* pParent )
        { return new ParagraphElement( pParent ); }
        
        virtual FrameElement* createFrameElement( Element* pParent, sal_Int32 nGCId )
        { return new FrameElement( pParent, nGCId ); }
        virtual PolyPolyElement*
            createPolyPolyElement( Element* pParent,
                                   sal_Int32 nGCId,
                                   const basegfx::B2DPolyPolygon& rPolyPoly,
                                   sal_Int8 nAction)
        { return new PolyPolyElement( pParent, nGCId, rPolyPoly, nAction ); }
        virtual ImageElement* createImageElement( Element* pParent, sal_Int32 nGCId, ImageId nImage )
        { return new ImageElement( pParent, nGCId, nImage ); }
        
        virtual PageElement* createPageElement( Element* pParent,
                                                sal_Int32 nPageNr )
        { return new PageElement( pParent, nPageNr ); }
        virtual DocumentElement* createDocumentElement()
        { return new DocumentElement(); }
    };    
}

#endif
