/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: pdfioutdev_gpl.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: thb $ $Date: 2007-12-05 14:16:44 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU General Public License Version 2.
 *
 *
 *    GNU General Public License, version 2
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This program is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU General Public License as
 *    published by the Free Software Foundation; either version 2 of
 *    the License, or (at your option) any later version.
 *    
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *    
 *    You should have received a copy of the GNU General Public
 *    License along with this program; if not, write to the Free
 *    Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *    Boston, MA 02110-1301, USA.
 *
 ************************************************************************/

#ifndef INCLUDED_PDFI_OUTDEV_HXX
#define INCLUDED_PDFI_OUTDEV_HXX

#if defined __GNUC__
#pragma GCC system_header
#elif defined __SUNPRO_CC
#pragma disable_warn
#elif defined _MSC_VER
#pragma warning(push, 1)
#endif

#include "GfxState.h"
#include "GfxFont.h"
#include "UnicodeMap.h"
#include "Link.h"
#include "Object.h"
#include "OutputDev.h"
#include "parseargs.h"
#include "GlobalParams.h"
#include "PDFDoc.h"

#if defined __SUNPRO_CC
#pragma enable_warn
#elif defined _MSC_VER
#pragma warning(pop)
#endif

#include <hash_map>

class GfxPath;
class GfxFont;
class PDFDoc;

namespace pdfi
{
    struct FontAttributes
    {
        FontAttributes( const GString& familyName_,
                        bool           isEmbedded_,
                        bool           isBold_,
                        bool           isItalic_,
                        bool           isUnderline_,
                        double         size_ ) :
            familyName(),
            isEmbedded(isEmbedded_),
            isBold(isBold_),
            isItalic(isItalic_),
            isUnderline(isUnderline_),
            size(size_)
        {
            familyName.append(const_cast<GString*>(&familyName_));
        }

        FontAttributes() :
            familyName(),
            isEmbedded(false),
            isBold(false),
            isItalic(false),
            isUnderline(false),
            size(0.0)
        {}

        // xdpf goo stuff is so totally borked...
        // ...need to hand-code assignment
        FontAttributes( const FontAttributes& rSrc ) :
            familyName(),
            isEmbedded(rSrc.isEmbedded),
            isBold(rSrc.isBold),
            isItalic(rSrc.isItalic),
            isUnderline(rSrc.isUnderline),
            size(rSrc.size)
        {
            familyName.append(const_cast<GString*>(&rSrc.familyName));
        }

        FontAttributes& operator=( const FontAttributes& rSrc )
        {
            familyName.clear();
            familyName.append(const_cast<GString*>(&rSrc.familyName));

            isEmbedded  = rSrc.isEmbedded;
            isBold      = rSrc.isBold;
            isItalic    = rSrc.isItalic;
            isUnderline = rSrc.isUnderline;
            size        = rSrc.size;

            return *this;
        }

        bool operator==(const FontAttributes& rFont) const
        {
            return const_cast<GString*>(&familyName)->cmp(
                const_cast<GString*>(&rFont.familyName))==0 &&
                isEmbedded == rFont.isEmbedded &&
                isBold == rFont.isBold &&
                isItalic == rFont.isItalic &&
                isUnderline == rFont.isUnderline &&
                size == rFont.size;
        }

        GString     familyName;
        bool        isEmbedded;
        bool        isBold;
        bool        isItalic;
        bool        isUnderline;
        double      size;            
    };

    class PDFOutDev : public OutputDev
    {
        // not owned by this class
        PDFDoc*                                 m_pDoc;
        mutable std::hash_map< long long, 
                               FontAttributes > m_aFontMap;
        UnicodeMap*                             m_pUtf8Map;

        int  parseFont( long long nNewId, GfxFont* pFont, GfxState* state ) const;
        void writeFontFile( GfxFont* gfxFont ) const;
        void printPath( GfxPath* pPath ) const;

    public:
        explicit PDFOutDev( PDFDoc* pDoc );
        
        //----- get info about output device
        
        // Does this device use upside-down coordinates?
        // (Upside-down means (0,0) is the top left corner of the page.)
        virtual GBool upsideDown() { return gTrue; }
        
        // Does this device use drawChar() or drawString()?
        virtual GBool useDrawChar() { return gTrue; }
        
        // Does this device use beginType3Char/endType3Char?  Otherwise,
        // text in Type 3 fonts will be drawn with drawChar/drawString.
        virtual GBool interpretType3Chars() { return gFalse; }
        
        // Does this device need non-text content?
        virtual GBool needNonText() { return gTrue; }
        
        //----- initialization and control
        
        // Set default transform matrix.
        virtual void setDefaultCTM(double *ctm);
        
        // Start a page.
        virtual void startPage(int pageNum, GfxState *state);
        
        // End a page.
        virtual void endPage();
        
        // Dump page contents to display.
        // virtual void dump() {}
        
        //----- coordinate conversion
        
        // Convert between device and user coordinates.
        // virtual void cvtDevToUser(double dx, double dy, double *ux, double *uy);
        // virtual void cvtUserToDev(double ux, double uy, int *dx, int *dy);
        
        //----- link borders
        virtual void processLink(Link *link, Catalog *catalog);
        
        //----- save/restore graphics state
        virtual void saveState(GfxState *state);
        virtual void restoreState(GfxState *state);
        
        //----- update graphics state
        // virtual void updateAll(GfxState *state);
        virtual void updateCTM(GfxState *state, double m11, double m12,
                               double m21, double m22, double m31, double m32);
        virtual void updateLineDash(GfxState *state);
        virtual void updateFlatness(GfxState *state);
        virtual void updateLineJoin(GfxState *state);
        virtual void updateLineCap(GfxState *state);
        virtual void updateMiterLimit(GfxState *state);
        virtual void updateLineWidth(GfxState *state);
        virtual void updateFillColor(GfxState *state);
        virtual void updateStrokeColor(GfxState *state);
        virtual void updateFillOpacity(GfxState *state);
        virtual void updateStrokeOpacity(GfxState *state);
        virtual void updateBlendMode(GfxState *state);
        
        //----- update text state
        virtual void updateFont(GfxState *state);
        // virtual void updateTextMat(GfxState *state);
        // virtual void updateCharSpace(GfxState *state) {}
        // virtual void updateRender(GfxState *state) {}
        // virtual void updateRise(GfxState *state) {}
        // virtual void updateWordSpace(GfxState *state) {}
        // virtual void updateHorizScaling(GfxState *state) {}
        // virtual void updateTextPos(GfxState *state) {}
        // virtual void updateTextShift(GfxState *state, double shift) {}
        
        //----- path painting
        virtual void stroke(GfxState *state);
        virtual void fill(GfxState *state);
        virtual void eoFill(GfxState *state);
        
        //----- path clipping
        virtual void clip(GfxState *state);
        virtual void eoClip(GfxState *state);
        
        //----- text drawing
        virtual void drawChar(GfxState *state, double x, double y,
                              double dx, double dy,
                              double originX, double originY,
                              CharCode code, int nBytes, Unicode *u, int uLen);
        virtual void drawString(GfxState *state, GString *s);
        virtual void endTextObject(GfxState *state);
        
        //----- image drawing
        virtual void drawImageMask(GfxState *state, Object *ref, Stream *str,
                                   int width, int height, GBool invert,
                                   GBool inlineImg);
        virtual void drawImage(GfxState *state, Object *ref, Stream *str,
                               int width, int height, GfxImageColorMap *colorMap,
                               int *maskColors, GBool inlineImg);
        virtual void drawMaskedImage(GfxState *state, Object *ref, Stream *str,
                                     int width, int height,
                                     GfxImageColorMap *colorMap,
                                     Stream *maskStr, int maskWidth, int maskHeight,
                                     GBool maskInvert);
        virtual void drawSoftMaskedImage(GfxState *state, Object *ref, Stream *str,
                                         int width, int height,
                                         GfxImageColorMap *colorMap,
                                         Stream *maskStr,
                                         int maskWidth, int maskHeight,
                                         GfxImageColorMap *maskColorMap);
        
        //----- OPI functions
        // virtual void opiBegin(GfxState *state, Dict *opiDict);
        // virtual void opiEnd(GfxState *state, Dict *opiDict);
        
        //----- Type 3 font operators
        // virtual void type3D0(GfxState *state, double wx, double wy) {}
        // virtual void type3D1(GfxState *state, double wx, double wy,
        //                      double llx, double lly, double urx, double ury) {}
        
        //----- PostScript XObjects
        // virtual void psXObject(Stream *psStream, Stream *level1Stream) {}        

        void setPageNum( int nNumPages );
    };
}

extern FILE* g_binary_out;

#endif /* INCLUDED_PDFI_OUTDEV_HXX */

