/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUSImageObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkUSImageObjectRepresentation_h
#define __igstkUSImageObjectRepresentation_h

#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkUSImageObject.h"

namespace igstk
{

/** \class USImageObjectRepresentation
 *
 *  \brief This class implements a spatial object representation 
 *         for US modality.
 *
 *  ImageRepresentation objects are responsible for rendering images according
 *  to their specific characteristics of PixelType and Dimension. In this case,
 *  this image representation class is tailored for displaying Ultrasound
 *  images.
 *
 * \ingroup SpatialObjectRepresentation
 */

class USImageObjectRepresentation : 
         public ImageSpatialObjectRepresentation< USImageObject >
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( USImageObjectRepresentation, \
                        ImageSpatialObjectRepresentation< USImageObject > )

  /** Return a copy of the current object representation.
   * \warning This method does not connect the ImageSpatialObject of the
   * original. */
  Pointer Copy() const;

protected:

  USImageObjectRepresentation( void );
  virtual ~USImageObjectRepresentation( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:
 
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  USImageObjectRepresentation(const Self&);     //purposely not implemented
  void operator=(const Self&);                  //purposely not implemented


};

} // end namespace igstk

#endif // __igstkUSImageObjectRepresentation_h
