/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMRImageSpatialObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkMRImageSpatialObjectRepresentation_h
#define __igstkMRImageSpatialObjectRepresentation_h

#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkMRImageSpatialObject.h"

namespace igstk
{

/** \class MRImageSpatialObjectRepresentation
 *  \brief This class implements a reader specific for CT modality.
 *
 * \ingroup Spatial object representation
 */

class MRImageSpatialObjectRepresentation : 
         public ImageSpatialObjectRepresentation< MRImageSpatialObject >
{

public:

  /** Typedefs */
  typedef MRImageSpatialObjectRepresentation                               Self;
  typedef ImageSpatialObjectRepresentation< MRImageSpatialObject >         Superclass;
  typedef itk::SmartPointer< Self >                                        Pointer;
  typedef itk::SmartPointer< const Self >                                  ConstPointer;


  /**  Run-time type information (and related methods). */
  igstkTypeMacro( MRImageSpatialObjectRepresentation, ImageSpatialObjectRepresentation );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

protected:

  MRImageSpatialObjectRepresentation( void );
  virtual ~MRImageSpatialObjectRepresentation( void ) {};

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

};

} // end namespace igstk

#endif // __igstkMRImageSpatialObjectRepresentation_h

