/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCTImageSpatialObjectImagePlaneRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkCTImageSpatialObjectImagePlaneRepresentation_h
#define __igstkCTImageSpatialObjectImagePlaneRepresentation_h

#include "igstkImageSpatialObjectImagePlaneRepresentation.h"
#include "igstkCTImageSpatialObject.h"
#include "igstkStateMachine.h"

namespace igstk
{

/** \class CTImageSpatialObjectImagePlaneRepresentation
 *  \brief This class implements a spatial object representation for CT images.
 *
 * \image html  igstkCTImageSpatialObjectImagePlaneRepresentation.png
 *             "CT Image Spatial Object Representation State Diagram"
 *
 * \image latex igstkCTImageSpatialObjectImagePlaneRepresentation.eps
 *             "CT Image Spatial Object Representation State Diagram"
 
 * \ingroup Spatial object representation
 */

class CTImageSpatialObjectImagePlaneRepresentation : 
         public ImageSpatialObjectImagePlaneRepresentation< CTImageSpatialObject >
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CTImageSpatialObjectImagePlaneRepresentation, \
                    ImageSpatialObjectImagePlaneRepresentation< CTImageSpatialObject > )

protected:

  CTImageSpatialObjectImagePlaneRepresentation( void );
  virtual ~CTImageSpatialObjectImagePlaneRepresentation( void ) {};

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

};

} // end namespace igstk

#endif // __igstkCTImageSpatialObjectImagePlaneRepresentation_h
