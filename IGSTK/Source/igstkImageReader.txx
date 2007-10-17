/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageReader.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageReader_txx
#define __igstkImageReader_txx

#include "igstkImageReader.h"

namespace igstk
{ 
/** Constructor */
template < class TImageSpatialObject >
ImageReader< TImageSpatialObject >
::ImageReader():m_StateMachine(this)
{
  // Create the Image spatial object that will be provided as output.
  m_ImageSpatialObject = ImageSpatialObjectType::New();
} 

/** Destructor */
template < class TImageSpatialObject >
ImageReader< TImageSpatialObject >
::~ImageReader()  
{

}

/** Print Self function */
template < class TImageSpatialObject >
void 
ImageReader< TImageSpatialObject >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

template < class TImageSpatialObject >
void
ImageReader< TImageSpatialObject >
::ConnectImage() 
{
  typedef Friends::ImageReaderToImageSpatialObject  HelperType;
  HelperType::ConnectImage( this, m_ImageSpatialObject.GetPointer() );
}


} // end namespace igstk

#endif
