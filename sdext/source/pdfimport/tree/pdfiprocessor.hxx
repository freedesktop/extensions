/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

#ifndef INCLUDED_PDFI_PROCESSOR_HXX
#define INCLUDED_PDFI_PROCESSOR_HXX

#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#include <com/sun/star/task/XStatusIndicator.hpp>
#include <com/sun/star/rendering/XVolatileBitmap.hpp>
#include <com/sun/star/geometry/RealSize2D.hpp>
#include <com/sun/star/geometry/RealPoint2D.hpp>
#include <com/sun/star/geometry/RealRectangle2D.hpp>
#include <com/sun/star/geometry/Matrix2D.hpp>

#include <basegfx/polygon/b2dpolypolygon.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/range/b2drange.hxx>

#include <rtl/ustring.hxx>
#include <rtl/ustrbuf.hxx>

#include <boost/shared_ptr.hpp>
#include <list>
#include <hash_map>

#include "imagecontainer.hxx"
#include "pdfihelper.hxx"
#include "contentsink.hxx"
#include "treevisitorfactory.hxx"
#include "genericelements.hxx"

namespace pdfi
{

    class  PDFIProcessor;
    struct Element;
    struct DocumentElement;
    struct PageElement;
    class  ElementFactory;
    class  XmlEmitter;
    class  CharGlyph;

    /** Main entry from the parser

        Creates the internal DOM tree from the render calls
     */
    class PDFIProcessor : public ContentSink
    {
    public:
        com::sun::star::uno::Reference< 
            com::sun::star::uno::XComponentContext >  m_xContext;
        double fYPrevTextPosition;
        double fPrevTextHeight;
        double fXPrevTextPosition;
        double fPrevTextWidth;
        enum DocumentTextDirecion { LrTb, RlTb, TbLr };

        explicit PDFIProcessor( const com::sun::star::uno::Reference< com::sun::star::task::XStatusIndicator >& xStat,
            com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext > xContext) ;

        /// TEMP - enable writer-like text:p on doc level
        void enableToplevelText();

        void emit( XmlEmitter&               rEmitter,
                   const TreeVisitorFactory& rVisitorFactory );

        sal_Int32 getGCId( const GraphicsContext& rGC );
        const GraphicsContext& getGraphicsContext( sal_Int32 nGCId ) const;
        GraphicsContext& getCurrentContext() { return m_aGCStack.back(); }
        const GraphicsContext& getCurrentContext() const { return m_aGCStack.back(); }

        ImageContainer& getImages() { return m_aImages; }
        boost::shared_ptr<ElementFactory> getElementFactory() const { return m_pElFactory; }

        const com::sun::star::uno::Reference< com::sun::star::task::XStatusIndicator >& getStatusIndicator() const
        { return m_xStatusIndicator; }
        void setStatusIndicator( const com::sun::star::uno::Reference< com::sun::star::task::XStatusIndicator >& xStatus )
        { m_xStatusIndicator = xStatus; }

        const FontAttributes& getFont( sal_Int32 nFontId ) const;
        sal_Int32 getFontId( const FontAttributes& rAttr ) const;

        void sortElements( Element* pElement, bool bDeep = false );
        void sortDocument( bool bDeep = false );

    private:
        void processGlyphLine();
        void processGlyph(   double       fPreAvarageSpaceValue,
                             CharGlyph&   rGlyph,
                             ParagraphElement* pPara,
                             FrameElement* pFrame,
                             bool         bIsWhiteSpaceInLine );

        void drawGlyphLine( const rtl::OUString&                               rGlyphs,
                            const ::com::sun::star::geometry::RealRectangle2D& rRect,
                            const ::com::sun::star::geometry::Matrix2D&        rFontMatrix  );

        void drawCharGlyphs( rtl::OUString&             rGlyphs,
                             ::com::sun::star::geometry::RealRectangle2D&  rRect,
                             ::com::sun::star::geometry::Matrix2D&         rFontMatrix,
                             GraphicsContext aGC,
                             Element* pCurElement,
                             ParagraphElement* pPara,
                             FrameElement* pFrame,
                             bool bSpaceFlag );

        GraphicsContext& getTransformGlyphContext( CharGlyph& rGlyph );

        // ContentSink interface implementation

        virtual void setPageNum( sal_Int32 nNumPages );
        virtual void startPage( const ::com::sun::star::geometry::RealSize2D& rSize );
        virtual void endPage();

        virtual void hyperLink( const ::com::sun::star::geometry::RealRectangle2D& rBounds,
                                const ::rtl::OUString&                             rURI );
        virtual void pushState();
        virtual void popState();
        virtual void setFlatness( double );
        virtual void setTransformation( const ::com::sun::star::geometry::AffineMatrix2D& rMatrix );
        virtual void setLineDash( const ::com::sun::star::uno::Sequence<double>& dashes,
                                  double                                         start );
        virtual void setLineJoin(sal_Int8);
        virtual void setLineCap(sal_Int8);
        virtual void setMiterLimit(double);
        virtual void setLineWidth(double);
        virtual void setFillColor( const ::com::sun::star::rendering::ARGBColor& rColor );
        virtual void setStrokeColor( const ::com::sun::star::rendering::ARGBColor& rColor );
        virtual void setBlendMode(sal_Int8);
        virtual void setFont( const FontAttributes& rFont );
        virtual void setTextRenderMode( sal_Int32 );

        virtual void strokePath( const ::com::sun::star::uno::Reference<
                                       ::com::sun::star::rendering::XPolyPolygon2D >& rPath );
        virtual void fillPath( const ::com::sun::star::uno::Reference<
                                     ::com::sun::star::rendering::XPolyPolygon2D >& rPath );
        virtual void eoFillPath( const ::com::sun::star::uno::Reference<
                                       ::com::sun::star::rendering::XPolyPolygon2D >& rPath );

        virtual void intersectClip(const ::com::sun::star::uno::Reference<
                                         ::com::sun::star::rendering::XPolyPolygon2D >& rPath);
        virtual void intersectEoClip(const ::com::sun::star::uno::Reference<
                                           ::com::sun::star::rendering::XPolyPolygon2D >& rPath);

        virtual void drawGlyphs( const rtl::OUString&                               rGlyphs,
                                 const ::com::sun::star::geometry::RealRectangle2D& rRect,
                                 const ::com::sun::star::geometry::Matrix2D&        rFontMatrix );
        virtual void endText();

        virtual void drawMask(const ::com::sun::star::uno::Sequence<
                                    ::com::sun::star::beans::PropertyValue>& xBitmap,
                              bool                                           bInvert );
        /// Given image must already be color-mapped and normalized to sRGB.
        virtual void drawImage(const ::com::sun::star::uno::Sequence<
                                     ::com::sun::star::beans::PropertyValue>& xBitmap );
        /** Given image must already be color-mapped and normalized to sRGB.

            maskColors must contain two sequences of color components
         */
        virtual void drawColorMaskedImage(const ::com::sun::star::uno::Sequence<
                                                ::com::sun::star::beans::PropertyValue>& xBitmap,
                                          const ::com::sun::star::uno::Sequence<
                                                ::com::sun::star::uno::Any>&             xMaskColors );
        virtual void drawMaskedImage(const ::com::sun::star::uno::Sequence<
                                           ::com::sun::star::beans::PropertyValue>& xBitmap,
                                     const ::com::sun::star::uno::Sequence<
                                           ::com::sun::star::beans::PropertyValue>& xMask,
                                     bool                                             bInvertMask);
        virtual void drawAlphaMaskedImage(const ::com::sun::star::uno::Sequence<
                                                ::com::sun::star::beans::PropertyValue>& xImage,
                                          const ::com::sun::star::uno::Sequence<
                                                ::com::sun::star::beans::PropertyValue>& xMask);

        /// nElements == -1 means fill in number of pages
        void startIndicator( const rtl::OUString& rText, sal_Int32 nElements = -1 );
        void endIndicator();

        void setupImage(ImageId nImage);

        typedef std::hash_map<sal_Int32,FontAttributes> IdToFontMap;
        typedef std::hash_map<FontAttributes,sal_Int32,FontAttrHash> FontToIdMap;
        
        typedef std::hash_map<sal_Int32,GraphicsContext> IdToGCMap;
        typedef std::hash_map<GraphicsContext,sal_Int32,GraphicsContextHash> GCToIdMap;

        typedef std::vector<GraphicsContext> GraphicsContextStack;

        ::basegfx::B2DRange& calcTransformedRectBounds( ::basegfx::B2DRange&			outRect,
                                                        const ::basegfx::B2DRange&		inRect,
                                                        const ::basegfx::B2DHomMatrix& 	transformation );
        std::vector<CharGlyph>             m_GlyphsList;
        
        boost::shared_ptr<ElementFactory>  m_pElFactory;
        boost::shared_ptr<DocumentElement> m_pDocument;
        PageElement*                       m_pCurPage;
        Element*                           m_pCurElement;
        sal_Int32                          m_nNextFontId;
        IdToFontMap                        m_aIdToFont;
        FontToIdMap                        m_aFontToId;
        
        GraphicsContextStack               m_aGCStack;
        GraphicsContext                    m_prev_aGC;
        sal_Int32                          m_nNextGCId;
        IdToGCMap                          m_aIdToGC;
        GCToIdMap                          m_aGCToId;
        
        ImageContainer                     m_aImages;
        
        DocumentTextDirecion               m_eTextDirection;

        sal_Int32                          m_nPages;        
        sal_Int32                          m_nNextZOrder;
        double                             m_fWordSpace;
        bool                               m_bIsWhiteSpaceInLine;
        com::sun::star::uno::Reference< 
            com::sun::star::task::XStatusIndicator >
                                           m_xStatusIndicator;

        bool                               m_bHaveTextOnDocLevel;
    };
    class CharGlyph
    {
        public:
            CharGlyph(){};
            virtual ~CharGlyph(){};
            rtl::OUString& getGlyph(){ return m_rGlyphs; } 
            com::sun::star::geometry::RealRectangle2D& getRect(){ return m_rRect; } 
            com::sun::star::geometry::Matrix2D&  getFontMatrix(){ return m_rFontMatrix; } 
            GraphicsContext&  getGC(){ return m_rCurrentContext; } 
            Element*  getCurElement(){ return m_pCurElement; } 

            void  setGlyph (const rtl::OUString& rGlyphs ){ m_rGlyphs=rGlyphs; } 
            void  setRect  (const ::com::sun::star::geometry::RealRectangle2D& rRect ){ m_rRect=rRect; } 
            void  setFontMatrix (const ::com::sun::star::geometry::Matrix2D& rFontMatrix ){ m_rFontMatrix= rFontMatrix; } 
            void  setGraphicsContext (GraphicsContext&  rCurrentContext ){ m_rCurrentContext= rCurrentContext; } 
            void  setCurElement( Element* pCurElement ){ m_pCurElement= pCurElement; } 
        
            double getYPrevGlyphPosition() const { return m_fYPrevGlyphPosition; }
            double getXPrevGlyphPosition() const { return m_fXPrevGlyphPosition; }
            double getPrevGlyphHeight() const { return m_fPrevGlyphHeight; }
            double getPrevGlyphWidth () const { return m_fPrevGlyphWidth; }
            double getPrevGlyphsSpace() const
            {
                if( (m_rRect.X1-m_fXPrevGlyphPosition)<0 )
                    return 0;
                else
                    return m_rRect.X1-m_fXPrevGlyphPosition;
            }

            void setYPrevGlyphPosition( double fYPrevTextPosition ){ m_fYPrevGlyphPosition= fYPrevTextPosition; } 
            void setXPrevGlyphPosition( double fXPrevTextPosition ){ m_fXPrevGlyphPosition= fXPrevTextPosition; } 
            void setPrevGlyphHeight   ( double fPrevTextHeight ){ m_fPrevGlyphHeight= fPrevTextHeight; } 
            void setPrevGlyphWidth    ( double fPrevTextWidth ){ m_fPrevGlyphWidth= fPrevTextWidth; } 
        
        private:

            double                      m_fYPrevGlyphPosition ;
            double                      m_fXPrevGlyphPosition ;
            double                      m_fPrevGlyphHeight ;
            double                      m_fPrevGlyphWidth ;
            Element*                    m_pCurElement ;
            GraphicsContext             m_rCurrentContext ;
            com::sun::star::geometry::Matrix2D          m_rFontMatrix ;
            com::sun::star::geometry::RealRectangle2D   m_rRect ;
            rtl::OUString               m_rGlyphs ;
    };
}

#define USTR(x) rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( x ) )

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
