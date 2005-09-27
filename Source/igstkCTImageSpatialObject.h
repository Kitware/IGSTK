/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCTImageSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkCTImageSpatialObject_h
#define __igstkCTImageSpatialObject_h

#include "igstkImageSpatialObject.h"

namespace igstk
{

/** \class CTImageSpatialObject
 * \brief  Represents a CT scan placed correctly in physical coordinates.
 *
 * \ingroup Object
 */

class CTImageSpatialObject : 
      public ImageSpatialObject< signed short, 3 >
{

public:

  /** Typedefs */
  typedef CTImageSpatialObject                  Self;
  typedef ImageSpatialObject< signed short, 3>  Superclass;
  typedef itk::SmartPointer< Self >             Pointer;
  typedef itk::SmartPointer< const Self >       ConstPointer;


  /**  Run-time type information (and related methods). */
  igstkTypeMacro( CTImageSpatialObject, ImageSpatialObject );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

protected:

  CTImageSpatialObject();
  virtual ~CTImageSpatialObject();

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

};

} // end namespace igstk

#endif // __igstkCTImageSpatialObject_h

