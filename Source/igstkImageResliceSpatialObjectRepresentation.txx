/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageResliceSpatialObjectRepresentation.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageResliceSpatialObjectRepresentation_txx
#define __igstkImageResliceSpatialObjectRepresentation_txx


#include "igstkImageResliceSpatialObjectRepresentation.h"

#include "igstkEvents.h"

namespace igstk
{

/** Constructor */
template < class TImageSpatialObject >
ImageResliceSpatialObjectRepresentation < TImageSpatialObject >
::ImageResliceSpatialObjectRepresentation():m_StateMachine(this)
{
}

/** Destructor */

template < class TImageSpatialObject >
ImageResliceSpatialObjectRepresentation < TImageSpatialObject >
::~ImageResliceSpatialObjectRepresentation()
{

}


/** Print Self function */
template < class TImageSpatialObject >
void
ImageResliceSpatialObjectRepresentation < TImageSpatialObject >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk

#endif
