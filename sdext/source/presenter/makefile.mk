#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
#
# Copyright 2008 by Sun Microsystems, Inc.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# $RCSfile: makefile.mk,v $
#
# $Revision: 1.10 $
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

PRJ=..$/..
PRJNAME=sdext
TARGET=PresenterScreen
GEN_HID=FALSE
EXTNAME=presenter

ENABLE_EXCEPTIONS=TRUE

# --- Settings ----------------------------------

.INCLUDE : rtlbootstrap.mk
.INCLUDE : settings.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

.IF "$(ENABLE_PRESENTER_SCREEN)" == "NO"
@all:
    @echo "Presenter Screen build disabled."
.ELSE

DLLPRE=
common_build_zip=

PRIVATERDB = slideshow.rdb
UNOUCRRDB = $(SOLARBINDIR)$/$(PRIVATERDB)
UNOUCRDEP = $(UNOUCRRDB)
UNOUCROUT = $(OUT)$/inc

CPPUMAKERFLAGS += -C -X$(SOLARBINDIR)$/types.rdb

# --- Files -------------------------------------

SLOFILES=										\
    $(SLO)$/PresenterAnimation.obj				\
    $(SLO)$/PresenterAnimator.obj				\
    $(SLO)$/PresenterBitmapContainer.obj		\
    $(SLO)$/PresenterButton.obj					\
    $(SLO)$/PresenterCanvasHelper.obj			\
    $(SLO)$/PresenterConfigurationAccess.obj	\
    $(SLO)$/PresenterControlCreator.obj			\
    $(SLO)$/PresenterController.obj				\
    $(SLO)$/PresenterCurrentSlideObserver.obj	\
    $(SLO)$/PresenterFrameworkObserver.obj		\
    $(SLO)$/PresenterGeometryHelper.obj			\
    $(SLO)$/PresenterHelper.obj					\
    $(SLO)$/PresenterHelpView.obj				\
    $(SLO)$/PresenterNotesView.obj				\
    $(SLO)$/PresenterPaintManager.obj			\
    $(SLO)$/PresenterPane.obj					\
    $(SLO)$/PresenterPaneAnimator.obj			\
    $(SLO)$/PresenterPaneBase.obj				\
    $(SLO)$/PresenterPaneBorderManager.obj		\
    $(SLO)$/PresenterPaneBorderPainter.obj		\
    $(SLO)$/PresenterPaneContainer.obj			\
    $(SLO)$/PresenterPaneFactory.obj			\
    $(SLO)$/PresenterProtocolHandler.obj		\
    $(SLO)$/PresenterScreen.obj					\
    $(SLO)$/PresenterScrollBar.obj				\
    $(SLO)$/PresenterSlidePreview.obj			\
    $(SLO)$/PresenterSlideShowView.obj			\
    $(SLO)$/PresenterSlideSorter.obj			\
    $(SLO)$/PresenterSprite.obj					\
    $(SLO)$/PresenterSpritePane.obj				\
    $(SLO)$/PresenterTheme.obj					\
    $(SLO)$/PresenterTimer.obj					\
    $(SLO)$/PresenterToolBar.obj				\
    $(SLO)$/PresenterUIPainter.obj				\
    $(SLO)$/PresenterViewFactory.obj			\
    $(SLO)$/PresenterWindowManager.obj			\
    $(SLO)$/PresenterComponent.obj


# --- Library -----------------------------------

SHL1TARGET=		$(TARGET).uno

SHL1STDLIBS=	$(CPPUHELPERLIB)	\
                $(CPPULIB)			\
                $(SALLIB)
SHL1DEPN=
SHL1IMPLIB=		i$(SHL1TARGET)
SHL1LIBS=		$(SLB)$/$(TARGET).lib
SHL1DEF=		$(MISC)$/$(SHL1TARGET).def
SHL1VERSIONMAP=	exports.map
SHL1RPATH=      OXT
DEF1NAME=		$(SHL1TARGET)

ZIP1TARGET=		presenter-screen
ZIP1DIR=		$(MISC)$/$(TARGET)
ZIP1EXT=		.oxt
ZIP1FLAGS=-r
ZIP1LIST=		*

DESCRIPTION:=$(ZIP1DIR)$/description.xml

.IF "$(GUI)" == "WIN" || "$(GUI)" == "WNT"
PACKLICS:=$(foreach,i,$(alllangiso) $(ZIP1DIR)$/registry$/license_$i)
.ELSE
PACKLICS:=$(foreach,i,$(alllangiso) $(ZIP1DIR)$/registry$/LICENSE_$i)
.ENDIF



COMPONENT_FILES=																			\
    $(ZIP1DIR)$/registry$/data$/org$/openoffice$/Office$/Jobs.xcu							\
    $(ZIP1DIR)$/registry$/data$/org$/openoffice$/Office$/ProtocolHandler.xcu				\
    $(ZIP1DIR)$/registry$/data$/org$/openoffice$/Office$/extension$/PresenterScreen.xcu		\
    $(ZIP1DIR)$/registry$/schema/org$/openoffice$/Office$/extension$/PresenterScreen.xcs

COMPONENT_BITMAPS=												\
    $(ZIP1DIR)$/bitmaps$/BorderTop.png							\
    $(ZIP1DIR)$/bitmaps$/BorderTopLeft.png						\
    $(ZIP1DIR)$/bitmaps$/BorderTopRight.png						\
    $(ZIP1DIR)$/bitmaps$/BorderLeft.png							\
    $(ZIP1DIR)$/bitmaps$/BorderRight.png						\
    $(ZIP1DIR)$/bitmaps$/BorderBottomLeft.png					\
    $(ZIP1DIR)$/bitmaps$/BorderBottomRight.png					\
    $(ZIP1DIR)$/bitmaps$/BorderBottom.png						\
                                                                \
    $(ZIP1DIR)$/bitmaps$/BorderActiveTop.png					\
    $(ZIP1DIR)$/bitmaps$/BorderActiveTopLeft.png				\
    $(ZIP1DIR)$/bitmaps$/BorderActiveTopRight.png				\
    $(ZIP1DIR)$/bitmaps$/BorderActiveLeft.png					\
    $(ZIP1DIR)$/bitmaps$/BorderActiveRight.png					\
    $(ZIP1DIR)$/bitmaps$/BorderActiveBottomLeft.png				\
    $(ZIP1DIR)$/bitmaps$/BorderActiveBottomRight.png			\
    $(ZIP1DIR)$/bitmaps$/BorderActiveBottom.png					\
    $(ZIP1DIR)$/bitmaps$/BorderActiveBottomCallout.png			\
                                                                \
    $(ZIP1DIR)$/bitmaps$/BorderCurrentSlideTop.png				\
    $(ZIP1DIR)$/bitmaps$/BorderCurrentSlideTopLeft.png			\
    $(ZIP1DIR)$/bitmaps$/BorderCurrentSlideTopRight.png			\
    $(ZIP1DIR)$/bitmaps$/BorderCurrentSlideLeft.png				\
    $(ZIP1DIR)$/bitmaps$/BorderCurrentSlideRight.png			\
    $(ZIP1DIR)$/bitmaps$/BorderCurrentSlideBottomLeft.png		\
    $(ZIP1DIR)$/bitmaps$/BorderCurrentSlideBottomRight.png		\
    $(ZIP1DIR)$/bitmaps$/BorderCurrentSlideBottom.png			\
                                                                \
    $(ZIP1DIR)$/bitmaps$/BorderToolbarTop.png					\
    $(ZIP1DIR)$/bitmaps$/BorderToolbarTopLeft.png				\
    $(ZIP1DIR)$/bitmaps$/BorderToolbarTopRight.png				\
    $(ZIP1DIR)$/bitmaps$/BorderToolbarLeft.png					\
    $(ZIP1DIR)$/bitmaps$/BorderToolbarRight.png					\
    $(ZIP1DIR)$/bitmaps$/BorderToolbarBottom.png				\
                                                                \
    $(ZIP1DIR)$/bitmaps$/Background.png							\
    $(ZIP1DIR)$/bitmaps$/ViewBackground.png						\
                                                                \
    $(ZIP1DIR)$/bitmaps$/ButtonSlidePreviousNormal.png			\
    $(ZIP1DIR)$/bitmaps$/ButtonSlidePreviousMouseOver.png		\
    $(ZIP1DIR)$/bitmaps$/ButtonSlidePreviousSelected.png		\
    $(ZIP1DIR)$/bitmaps$/ButtonSlidePreviousDisabled.png		\
    $(ZIP1DIR)$/bitmaps$/ButtonEffectNextNormal.png				\
    $(ZIP1DIR)$/bitmaps$/ButtonEffectNextMouseOver.png			\
    $(ZIP1DIR)$/bitmaps$/ButtonEffectNextSelected.png			\
    $(ZIP1DIR)$/bitmaps$/ButtonEffectNextDisabled.png			\
    $(ZIP1DIR)$/bitmaps$/ButtonNotesNormal.png					\
    $(ZIP1DIR)$/bitmaps$/ButtonNotesMouseOver.png				\
    $(ZIP1DIR)$/bitmaps$/ButtonNotesSelected.png				\
    $(ZIP1DIR)$/bitmaps$/ButtonNotesDisabled.png				\
    $(ZIP1DIR)$/bitmaps$/ButtonSlideSorterNormal.png			\
    $(ZIP1DIR)$/bitmaps$/ButtonSlideSorterMouseOver.png			\
    $(ZIP1DIR)$/bitmaps$/ButtonSlideSorterSelected.png			\
    $(ZIP1DIR)$/bitmaps$/ButtonSlideSorterDisabled.png			\
    $(ZIP1DIR)$/bitmaps$/ButtonHelpNormal.png					\
    $(ZIP1DIR)$/bitmaps$/ButtonHelpMouseOver.png				\
    $(ZIP1DIR)$/bitmaps$/ButtonHelpSelected.png					\
    $(ZIP1DIR)$/bitmaps$/ButtonHelpDisabled.png					\
                                                                \
    $(ZIP1DIR)$/bitmaps/ScrollbarArrowUpNormal.png				\
    $(ZIP1DIR)$/bitmaps/ScrollbarArrowUpMouseOver.png			\
    $(ZIP1DIR)$/bitmaps/ScrollbarArrowUpSelected.png			\
    $(ZIP1DIR)$/bitmaps/ScrollbarArrowUpDisabled.png			\
    $(ZIP1DIR)$/bitmaps/ScrollbarArrowDownNormal.png			\
    $(ZIP1DIR)$/bitmaps/ScrollbarArrowDownMouseOver.png			\
    $(ZIP1DIR)$/bitmaps/ScrollbarArrowDownSelected.png			\
    $(ZIP1DIR)$/bitmaps/ScrollbarArrowDownDisabled.png			\
    $(ZIP1DIR)$/bitmaps/ScrollbarPagerMiddleNormal.png			\
    $(ZIP1DIR)$/bitmaps/ScrollbarPagerMiddleMouseOver.png		\
    $(ZIP1DIR)$/bitmaps/ScrollbarThumbTopNormal.png				\
    $(ZIP1DIR)$/bitmaps/ScrollbarThumbTopMouseOver.png			\
    $(ZIP1DIR)$/bitmaps/ScrollbarThumbBottomNormal.png			\
    $(ZIP1DIR)$/bitmaps/ScrollbarThumbBottomMouseOver.png		\
    $(ZIP1DIR)$/bitmaps/ScrollbarThumbMiddleNormal.png			\
    $(ZIP1DIR)$/bitmaps/ScrollbarThumbMiddleMouseOver.png		\
                                                                \
    $(ZIP1DIR)$/bitmaps$/ButtonPlusNormal.png					\
    $(ZIP1DIR)$/bitmaps$/ButtonPlusMouseOver.png				\
    $(ZIP1DIR)$/bitmaps$/ButtonPlusSelected.png					\
    $(ZIP1DIR)$/bitmaps$/ButtonPlusDisabled.png					\
    $(ZIP1DIR)$/bitmaps$/ButtonMinusNormal.png					\
    $(ZIP1DIR)$/bitmaps$/ButtonMinusMouseOver.png				\
    $(ZIP1DIR)$/bitmaps$/ButtonMinusSelected.png				\
    $(ZIP1DIR)$/bitmaps$/ButtonMinusDisabled.png				\
                                                                \
    $(ZIP1DIR)$/bitmaps$/ButtonFrameLeftNormal.png				\
    $(ZIP1DIR)$/bitmaps$/ButtonFrameCenterNormal.png			\
    $(ZIP1DIR)$/bitmaps$/ButtonFrameRightNormal.png				\
    $(ZIP1DIR)$/bitmaps$/ButtonFrameLeftMouseOver.png			\
    $(ZIP1DIR)$/bitmaps$/ButtonFrameCenterMouseOver.png			\
    $(ZIP1DIR)$/bitmaps$/ButtonFrameRightMouseOver.png			\
                                                                \
    $(ZIP1DIR)$/bitmaps$/LabelMouseOverLeft.png					\
    $(ZIP1DIR)$/bitmaps$/LabelMouseOverCenter.png				\
    $(ZIP1DIR)$/bitmaps$/LabelMouseOverRight.png

COMPONENT_MANIFEST= 							\
    $(ZIP1DIR)$/META-INF$/manifest.xml

COMPONENT_LIBRARY= 								\
    $(ZIP1DIR)$/$(TARGET).uno$(DLLPOST)

COMPONENT_HELP= 								\
    $(ZIP1DIR)$/help/component.txt

ZIP1DEPS=					\
    $(PACKLICS) 			\
    $(DESCRIPTION)			\
    $(COMPONENT_MANIFEST)	\
    $(COMPONENT_FILES)		\
    $(COMPONENT_BITMAPS)	\
    $(COMPONENT_LIBRARY)	\
    $(COMPONENT_HELP)

PLATFORMID:=$(RTL_OS:l)_$(RTL_ARCH:l)


# --- Targets ----------------------------------

.INCLUDE : target.mk

$(SLO)$/PresenterComponent.obj : $(INCCOM)$/PresenterExtensionIdentifier.hxx

$(INCCOM)$/PresenterExtensionIdentifier.hxx : PresenterExtensionIdentifier.txx
    $(TYPE) $< | sed s/UPDATED_PLATFORM/$(PLATFORMID)/ > $@

$(COMPONENT_MANIFEST) : $$(@:f)
    @-$(MKDIRHIER) $(@:d)
    +$(TYPE) $< | $(SED) "s/SHARED_EXTENSION/$(DLLPOST)/" > $@

$(COMPONENT_HELP) : help$/$$(@:f)
    @@-$(MKDIRHIER) $(@:d)
    $(COPY) $< $@

#$(COMPONENT_FILES) : $$(@:f)
#	@-$(MKDIRHIER) $(@:d)
#    +$(COPY) $< $@

$(COMPONENT_BITMAPS) : bitmaps$/$$(@:f)
    @-$(MKDIRHIER) $(@:d)
    +$(COPY) $< $@

$(COMPONENT_LIBRARY) : $(DLLDEST)$/$$(@:f)
    @-$(MKDIRHIER) $(@:d)
    +$(COPY) $< $@
.IF "$(OS)$(CPU)"=="WNTI"
 .IF "$(COM)"=="GCC"
    $(GNUCOPY) $(SOLARBINDIR)$/mingwm10.dll $(ZIP1DIR)
 .ELSE
    .IF "$(PACKMS)"!=""
        .IF "$(CCNUMVER)" <= "001399999999"
            $(GNUCOPY) $(PACKMS)$/msvcr71.dll $(ZIP1DIR)
            $(GNUCOPY) $(PACKMS)$/msvcp71.dll $(ZIP1DIR)
        .ELSE
            .IF "$(CCNUMVER)" <= "001499999999"
                $(GNUCOPY) $(PACKMS)$/msvcr80.dll $(ZIP1DIR)
                $(GNUCOPY) $(PACKMS)$/msvcp80.dll $(ZIP1DIR)
                $(GNUCOPY) $(PACKMS)$/msvcm80.dll $(ZIP1DIR)
                $(GNUCOPY) $(PACKMS)$/Microsoft.VC80.CRT.manifest $(ZIP1DIR)
            .ELSE
                $(GNUCOPY) $(PACKMS)$/msvcr90.dll $(ZIP1DIR)
                $(GNUCOPY) $(PACKMS)$/msvcp90.dll $(ZIP1DIR)
                $(GNUCOPY) $(PACKMS)$/msvcm90.dll $(ZIP1DIR)
                $(GNUCOPY) $(PACKMS)$/Microsoft.VC90.CRT.manifest $(ZIP1DIR)
            .ENDIF
        .ENDIF
    .ELSE        # "$(PACKMS)"!=""
        .IF "$(CCNUMVER)" <= "001399999999"
            $(GNUCOPY) $(SOLARBINDIR)$/msvcr71.dll $(ZIP1DIR)
            $(GNUCOPY) $(SOLARBINDIR)$/msvcp71.dll $(ZIP1DIR)
        .ELSE
            .IF "$(CCNUMVER)" <= "001499999999"
                $(GNUCOPY) $(SOLARBINDIR)$/msvcr80.dll $(ZIP1DIR)
                $(GNUCOPY) $(SOLARBINDIR)$/msvcp80.dll $(ZIP1DIR)
                $(GNUCOPY) $(SOLARBINDIR)$/msvcm80.dll $(ZIP1DIR)
                $(GNUCOPY) $(SOLARBINDIR)$/Microsoft.VC80.CRT.manifest $(ZIP1DIR)
            .ELSE
                $(GNUCOPY) $(SOLARBINDIR)$/msvcr90.dll $(ZIP1DIR)
                $(GNUCOPY) $(SOLARBINDIR)$/msvcp90.dll $(ZIP1DIR)
                $(GNUCOPY) $(SOLARBINDIR)$/msvcm90.dll $(ZIP1DIR)
                $(GNUCOPY) $(SOLARBINDIR)$/Microsoft.VC90.CRT.manifest $(ZIP1DIR)
            .ENDIF
        .ENDIF
    .ENDIF         # "$(PACKMS)"!=""
 .ENDIF	#"$(COM)"=="GCC"
.ENDIF


.IF "$(GUI)" == "WIN" || "$(GUI)" == "WNT"
$(PACKLICS) : $(SOLARBINDIR)$/osl$/license$$(@:b:s/_/./:e:s/./_/)$$(@:e).txt
    @@-$(MKDIRHIER) $(@:d)
    $(GNUCOPY) $< $@
.ELSE
$(PACKLICS) : $(SOLARBINDIR)$/osl$/LICENSE$$(@:b:s/_/./:e:s/./_/)$$(@:e)
    @@-$(MKDIRHIER) $(@:d)
    $(GNUCOPY) $< $@
.ENDIF

$(ZIP1DIR)/%.xcu : %.xcu
    @@-$(MKDIRHIER) $(@:d)
    $(GNUCOPY) $< $@

$(ZIP1DIR)$/%.xcs : %.xcs
    @@-$(MKDIRHIER) $(@:d)
    $(GNUCOPY) $< $@

# Temporary file that is used to replace some placeholders in description.xml.
DESCRIPTION_TMP:=$(ZIP1DIR)$/description.xml.tmp

.INCLUDE .IGNORE : $(ZIP1DIR)_lang_track.mk
.IF "$(LAST_WITH_LANG)"!="$(WITH_LANG)"
PHONYDESC=.PHONY
.ENDIF			# "$(LAST_WITH_LANG)"!="$(WITH_LANG)"
$(DESCRIPTION) $(PHONYDESC) : $$(@:f)
    @@-$(MKDIRHIER) $(@:d)
    $(PERL) $(SOLARENV)$/bin$/licinserter.pl description.xml registry/LICENSE_xxx $(DESCRIPTION_TMP)
    @echo LAST_WITH_LANG=$(WITH_LANG) > $(ZIP1DIR)_lang_track.mk
    $(TYPE) $(DESCRIPTION_TMP) | sed s/UPDATED_PLATFORM/$(PLATFORMID)/ > $@
    @@-$(RM) $(DESCRIPTION_TMP)


.ENDIF # "$(ENABLE_PRESENTER_SCREEN)" != "NO"
