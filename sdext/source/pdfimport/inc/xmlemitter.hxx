/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: xmlemitter.hxx,v $
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

#ifndef INCLUDED_PDFI_XMLEMITTER_HXX
#define INCLUDED_PDFI_XMLEMITTER_HXX

#include "pdfihelper.hxx"
#include <boost/shared_ptr.hpp>

namespace pdfi
{
    /** Output interface to ODF

        Should be easy to implement using either SAX events or plain ODF
     */
    class XmlEmitter
    {
    public:
        virtual ~XmlEmitter() {}
        
        /** Open up a tag with the given properties
         */
        virtual void beginTag( const char* pTag, const PropertyMap& rProperties ) = 0;
        /** Write PCTEXT as-is to output
         */
        virtual void write( const rtl::OUString& rString ) = 0;
        /** Close previously opened tag
         */
        virtual void endTag( const char* pTag ) = 0;
    };

    typedef boost::shared_ptr<XmlEmitter> XmlEmitterSharedPtr;
}

#endif

