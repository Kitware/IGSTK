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
#include "igstkImageReslicePlaneSpatialObject.h"

#include "vtkImageMapToWindowLevelColors.h"
#include "vtkImageReslice.h"
#include "vtkImageActor.h"

#include "vtkMatrix4x4.h"
#include "vtkCamera.h"
#include "vtkPlane.h"

namespace igstk
{


/** \class ImageResliceSpatialObjectRepresentation
 *
 * \brief This class generates representation of resliced image plane. 
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

public:

  /** Typedefs */
  typedef TImageSpatialObject                 ImageSpatialObjectType;
  typedef typename ImageSpatialObjectType::Pointer
                                              ImageSpatialObjectPointer;

  typedef ImageReslicePlaneSpatialObject< ImageSpatialObjectType >
                                                   ReslicePlaneSpatialObjectType;

  typedef typename ReslicePlaneSpatialObjectType::Pointer
                                              ReslicePlaneSpatialObjectPointer;
  
  typedef Transform                           TransformType;

  void RequestSetReslicePlaneSpatialObject( const ReslicePlaneSpatialObjectType *
                                                             planeSpatialObject);

  void RequestSetImageSpatialObject( const ImageSpatialObjectType * image );

  /** Create the vtkActors */
  virtual void CreateActors() { } ;

  /** Update the visual representation with changes in the geometry */
  virtual void RequestUpdateRepresentation( 
    const TimeStamp & time, 
    const CoordinateSystem* cs ) { };

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

  virtual void UpdateRepresentationProcessing() { };
private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidImageSpatialObject );
  igstkDeclareInputMacro( InValidImageSpatialObject );
  igstkDeclareInputMacro( ValidReslicePlaneSpatialObject );
  igstkDeclareInputMacro( InValidReslicePlaneSpatialObject );

  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( ImageSpatialObjectSet );
  igstkDeclareStateMacro( ReslicePlaneSpatialObjectSet );

  /** Set the image spatial object */
  void SetImageSpatialObjectProcessing( void );

  /** Set the reslice plane spatial object */
  void SetReslicePlaneSpatialObjectProcessing( void );

  /** Report invalid image spatial object type */
  void ReportInvalidImageSpatialObjectProcessing( void );

  /** Report invalid tool spatial object type */
  void ReportInvalidReslicePlaneSpatialObjectProcessing( void );

  /** Report invalid request */
  void ReportInvalidRequestProcessing( void );


  /** Variables for managing image spatial object type */
  ImageSpatialObjectPointer     m_ImageSpatialObjectToBeSet;
  ImageSpatialObjectPointer     m_ImageSpatialObject;

  /** Variables for maanging reslice plane spatial object */
  ReslicePlaneSpatialObjectPointer  m_ReslicePlaneSpatialObjectToBeSet;
  ReslicePlaneSpatialObjectPointer  m_ReslicePlaneSpatialObject;
  
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkImageResliceSpatialObjectRepresentation.txx"
#endif


#endif // __igstkImageResliceSpatialObjectRepresentation_h
