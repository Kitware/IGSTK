/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCTImageSpatialObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkCTImageSpatialObjectRepresentation_h
#define __igstkCTImageSpatialObjectRepresentation_h

#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkCTImageSpatialObject.h"

namespace igstk
{

/** \class CTImageSpatialObjectRepresentation
 *  \brief This class implements a reader specific for CT modality.
 *
 * \ingroup Spatial object representation
 */

class CTImageSpatialObjectRepresentation : 
         public ImageSpatialObjectRepresentation< CTImageSpatialObject >
{

public:

  /** Typedefs */
  typedef CTImageSpatialObjectRepresentation                               Self;
  typedef ImageSpatialObjectRepresentation< CTImageSpatialObject >         Superclass;
  typedef itk::SmartPointer< Self >                                        Pointer;
  typedef itk::SmartPointer< const Self >                                  ConstPointer;


  /**  Run-time type information (and related methods). */
  igstkTypeMacro( CTImageSpatialObjectRepresentation, ImageSpatialObjectRepresentation );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

protected:

  CTImageSpatialObjectRepresentation( void );
  virtual ~CTImageSpatialObjectRepresentation( void ) {};

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

};

} // end namespace igstk

#endif // __igstkCTImageSpatialObjectRepresentation_h

