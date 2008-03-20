/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkGenericImageReader.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkGenericImageReader.h"

#include "vtkKWImage.h"
#include "vtkKWImageIO.h"


namespace igstk {


/* Constructor. It will initialize the point to (0,0,0), and an unknown
 * coordinate system. */
GenericImageReader::GenericImageReader()
{
  m_ImageIO = vtkKWImageIO::New();
}

/* Destructor. */
GenericImageReader::~GenericImageReader()
{
  delete m_ImageIO;
}

} // end namespace igstk
