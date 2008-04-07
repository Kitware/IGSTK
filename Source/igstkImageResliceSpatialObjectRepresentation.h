/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageResliceSpatialObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageResliceSpatialObjectRepresentation_h
#define __igstkImageResliceSpatialObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkImageSpatialObject.h"
#include "igstkStateMachine.h"

#include "vtkImageMapToWindowLevelColors.h"
#include "vtkImageReslice.h"
#include "vtkImageActor.h"

#include "vtkMatrix4x4.h"
#include "vtkCamera.h"

namespace igstk
{


/** \class ImageResliceSpatialObjectRepresentation
 *
 * \brief This class represents an oblique image object.
 *
 * \ingroup ObjectRepresentation
 */
template < class TImageSpatialObject >
class ImageResliceSpatialObjectRepresentation : public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro(
                                ImageResliceSpatialObjectRepresentation,ObjectRepresentation )

  /** Create the vtkActors */
  virtual void CreateActors() { } ;


public:

  typedef TImageSpatialObject                 ImageSpatialObjectType;

  typedef Transform                           TransformType;

  typedef typename ImageSpatialObjectType::ConstPointer
                                              ImageSpatialObjectConstPointer;

  typedef typename ImageSpatialObjectType::PointType  PointType;

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

protected:

  /** Constructor */
  ImageResliceSpatialObjectRepresentation();

  /** Destructor */
  ~ImageResliceSpatialObjectRepresentation();

private:

  ImageResliceSpatialObjectRepresentation(const Self&);
  void operator=(const Self&);   //purposely not implemented

  /** Internal itkSpatialObject */
  ImageSpatialObjectConstPointer          m_ImageSpatialObject;

  virtual void UpdateRepresentationProcessing() { };
private:

  /** Inputs to the State Machine */

  /** States for the State Machine */

};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkImageResliceSpatialObjectRepresentation.txx"
#endif


#endif // __igstkImageResliceSpatialObjectRepresentation_h
