/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPETImageSpatialObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkPETImageSpatialObjectRepresentation_h
#define __igstkPETImageSpatialObjectRepresentation_h

#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkPETImageSpatialObject.h"
#include "igstkStateMachine.h"

namespace igstk
{

/** \class PETImageSpatialObjectRepresentation
 *  \brief This class implements a spatial object representation for PET images.
 *
 * \image html  igstkPETImageSpatialObjectRepresentation.png
 *             "PET Image Spatial Object Representation State Diagram"
 *
 * \image latex igstkPETImageSpatialObjectRepresentation.eps
 *             "PET Image Spatial Object Representation State Diagram"
 
 * \ingroup Spatial object representation
 */

class PETImageSpatialObjectRepresentation : 
         public ImageSpatialObjectRepresentation< PETImageSpatialObject >
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( PETImageSpatialObjectRepresentation, \
                    ImageSpatialObjectRepresentation< PETImageSpatialObject > )

  /** Return a copy of the current object representation.
   * \warning This method does not connect the ImageSpatialObject of the
   * original. */
  Pointer Copy() const;

protected:

  PETImageSpatialObjectRepresentation( void );
  virtual ~PETImageSpatialObjectRepresentation( void ) {};

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

};

} // end namespace igstk

#endif // __igstkPETImageSpatialObjectRepresentation_h
