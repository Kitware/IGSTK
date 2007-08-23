/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMRImageSpatialObjectImagePlaneRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkMRImageSpatialObjectImagePlaneRepresentation_h
#define __igstkMRImageSpatialObjectImagePlaneRepresentation_h

#include "igstkImageSpatialObjectImagePlaneRepresentation.h"
#include "igstkMRImageSpatialObject.h"

namespace igstk
{

/** \class MRImageSpatialObjectImagePlaneRepresentation
 *  \brief This class implements a spatial object represenation 
 *  for MRI modality.
 *
 * \ingroup Spatial object representation
 */

class MRImageSpatialObjectImagePlaneRepresentation : 
         public ImageSpatialObjectImagePlaneRepresentation< MRImageSpatialObject >
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( MRImageSpatialObjectImagePlaneRepresentation, \
                    ImageSpatialObjectImagePlaneRepresentation< MRImageSpatialObject > )

protected:

  MRImageSpatialObjectImagePlaneRepresentation( void );
  virtual ~MRImageSpatialObjectImagePlaneRepresentation( void ) {};

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

};

} // end namespace igstk

#endif // __igstkMRImageSpatialObjectImagePlaneRepresentation_h
