/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageReslicePlaneSpatialObject.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkImageReslicePlaneSpatialObject_txx
#define __igstkImageReslicePlaneSpatialObject_txx

#include "igstkImageReslicePlaneSpatialObject.h"
#include "igstkEvents.h"

#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkPlane.h"
#include "vtkMath.h"

namespace igstk
{ 

/** Constructor */
template < class TImageSpatialObject >
ImageReslicePlaneSpatialObject< TImageSpatialObject>::ImageReslicePlaneSpatialObject():m_StateMachine(this)
{
  //Default reslicing mode
  m_ReslicingMode = Orthogonal;

  //Default orientation type
  m_OrientationType = Axial;

  m_ImageData = NULL;

  //Create vtk plane 
  m_ImageReslicePlane = vtkPlane::New();

  //Create reslice axes matrix
  m_ResliceAxes = vtkMatrix4x4::New();

  // Create the observer to VTK image events 
  m_VTKImageObserver = VTKImageObserver::New();

  //List of states
  igstkAddStateMacro( Initial );
  igstkAddStateMacro( ReslicingModeSet );
  igstkAddStateMacro( OrientationTypeSet );
  igstkAddStateMacro( ImageSpatialObjectSet );
  igstkAddStateMacro( ToolSpatialObjectSet );
  igstkAddStateMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem );

  // List of state machine inputs
  igstkAddInputMacro( ValidReslicingMode );
  igstkAddInputMacro( InValidReslicingMode );
  igstkAddInputMacro( ValidOrientationType );
  igstkAddInputMacro( InValidOrientationType );
  igstkAddInputMacro( ValidImageSpatialObject );
  igstkAddInputMacro( InValidImageSpatialObject );
  igstkAddInputMacro( ValidToolSpatialObject );
  igstkAddInputMacro( InValidToolSpatialObject );
  igstkAddInputMacro( GetToolTransformWRTImageCoordinateSystem );
  igstkAddInputMacro( ToolTransformWRTImageCoordinateSystem );


  // List of state machine transitions

  //From Initial
  igstkAddTransitionMacro( Initial, ValidReslicingMode, ReslicingModeSet, SetReslicingMode );
  igstkAddTransitionMacro( Initial, InValidReslicingMode, Initial, ReportInvalidReslicingMode );
  igstkAddTransitionMacro( Initial, ValidOrientationType, Initial, ReportInvalidRequest);
  igstkAddTransitionMacro( Initial, InValidOrientationType, Initial, ReportInvalidRequest);

  //From ReslicingModeSet
  igstkAddTransitionMacro( ReslicingModeSet, ValidOrientationType, OrientationTypeSet, SetOrientationType );
  igstkAddTransitionMacro( ReslicingModeSet, InValidOrientationType, ReslicingModeSet, ReportInvalidOrientationType);

  //From OrientationTypeSet
  igstkAddTransitionMacro( OrientationTypeSet,
                           ValidImageSpatialObject,
                           ImageSpatialObjectSet,
                           SetImageSpatialObject );

  igstkAddTransitionMacro( OrientationTypeSet, 
                           InValidImageSpatialObject, 
                           OrientationTypeSet,
                           ReportInvalidImageSpatialObject );

  //From ImageSpatialObjectSet
  igstkAddTransitionMacro( ImageSpatialObjectSet, 
                           ValidToolSpatialObject,
                           ToolSpatialObjectSet, 
                           SetToolSpatialObject );

  igstkAddTransitionMacro( ImageSpatialObjectSet,
                           InValidToolSpatialObject,
                           ImageSpatialObjectSet,
                           ReportInvalidToolSpatialObject );

  //From ToolSpatialObjectSet
  igstkAddTransitionMacro( ToolSpatialObjectSet,
                           GetToolTransformWRTImageCoordinateSystem,
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           RequestGetToolTransformWRTImageCoordinateSystem );

  // From AttemptingToGetToolTransformWRTImageCoordinateSystem
  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ToolTransformWRTImageCoordinateSystem,
                           ToolSpatialObjectSet,
                           ReceiveToolTransformWRTImageCoordinateSystem );
 
  igstkSetInitialStateMacro( Initial );
  this->m_StateMachine.SetReadyToRun();
} 

/** Destructor */
template < class TImageSpatialObject >
ImageReslicePlaneSpatialObject<TImageSpatialObject>::~ImageReslicePlaneSpatialObject()  
{
  if( ! m_ResliceAxes )
    {
    m_ResliceAxes->Delete();
    m_ResliceAxes = NULL;
    }

  if( ! m_ImageReslicePlane )
    {
    m_ImageReslicePlane->Delete();
    m_ImageReslicePlane = NULL;
    }
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject<TImageSpatialObject>
::RequestSetReslicingMode( ReslicingMode reslicingMode )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestSetReslicingMode called...\n");

  m_ReslicingModeToBeSet = reslicingMode;
  m_StateMachine.PushInput( m_ValidReslicingModeInput );

  //FIXME: Check conditions for InValidReslicing mode
  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject<TImageSpatialObject>
::SetReslicingModeProcessing()
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::SetReslicingModeProcessing called...\n");
  m_ReslicingMode = m_ReslicingModeToBeSet;
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject<TImageSpatialObject>
::RequestSetOrientationType( OrientationType orientationType )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestSetOrientationType called...\n");

  m_OrientationTypeToBeSet = orientationType;
  m_StateMachine.PushInput( m_ValidOrientationTypeInput );

  //FIXME: Check conditions for InValidOrientation type 
  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::SetOrientationTypeProcessing()
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::SetOrientationTypeProcessing called...\n");
  m_OrientationType = m_OrientationTypeToBeSet;
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestSetImageSpatialObject( const ImageSpatialObjectType * imageSpatialObject )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestSetImageSpatialObject called...\n");

  m_ImageSpatialObjectToBeSet = imageSpatialObject;

  if( !m_ImageSpatialObjectToBeSet )
    {
    m_StateMachine.PushInput( m_InValidImageSpatialObjectInput );
    }
  else
    {
    m_StateMachine.PushInput( m_ValidImageSpatialObjectInput );
    }

  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::SetImageSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::SetImageSpatialObjectProcessing called...\n");

  m_ImageSpatialObject = m_ImageSpatialObjectToBeSet;

  m_ImageSpatialObject->AddObserver( VTKImageModifiedEvent(), 
                                     m_VTKImageObserver );

  this->m_VTKImageObserver->Reset();

  this->m_ImageSpatialObject->RequestGetVTKImage();

  if( this->m_VTKImageObserver->GotVTKImage() ) 
    {
    this->m_ImageData = this->m_VTKImageObserver->GetVTKImage();
    }
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReportInvalidImageSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::ReportInvalidImageSpatialObjectProcessing called...\n");
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestSetToolSpatialObject( const ToolSpatialObjectType * toolSpatialObject )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestSetToolSpatialObject called...\n");

  m_ToolSpatialObjectToBeSet = const_cast< ToolSpatialObjectType *>(toolSpatialObject);

  if( !m_ToolSpatialObjectToBeSet )
    {
    m_StateMachine.PushInput( m_InValidToolSpatialObjectInput );
    }
  else
    {
    m_StateMachine.PushInput( m_ValidToolSpatialObjectInput );
    }

  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::SetToolSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::SetToolSpatialObjectProcessing called...\n");

  m_ToolSpatialObject = m_ToolSpatialObjectToBeSet;
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReportInvalidToolSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::ReportInvalidToolSpatialObjectProcessing called...\n");
}

template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestUpdateToolTransform()
{
  igstkLogMacro( DEBUG,
                 "igstk::ImageReslicePlaneSpatialObject::\
                 RequestUpdateToolTransform called ...\n");
  
  igstkPushInputMacro( GetToolTransformWRTImageCoordinateSystem );
  this->m_StateMachine.ProcessInputs();
}

/** Request to receive the tool transform with respect to the Image coordinate system */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestGetToolTransformWRTImageCoordinateSystemProcessing()
{
  igstkLogMacro( DEBUG,
                 "igstk::ImageReslicePlaneSpatialObject::\
                 RequestGetToolTransformWRTImageCoordinateSystemProcessing called ...\n");

  typedef igstk::Friends::CoordinateSystemHelper 
                          CoordinateSystemHelperType;

  const CoordinateSystem* ImageSpatialObjectCoordinateSystem = 
    CoordinateSystemHelperType::GetCoordinateSystem( m_ImageSpatialObject );

  CoordinateSystem * ImageSpatialObjectCoordinateSystemNC =
          const_cast< CoordinateSystem *>(ImageSpatialObjectCoordinateSystem);

  m_ToolSpatialObject->RequestComputeTransformTo( ImageSpatialObjectCoordinateSystemNC ); 
}

/** Receive the tool spatial object transform with respect to the Image
 * coordinate system using a transduction macro */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReceiveToolTransformWRTImageCoordinateSystemProcessing()
{
  this->m_ToolTransformWRTImageCoordinateSystem =
      this->m_ToolTransformWRTImageCoordinateSystemInputToBeSet.GetTransform();

  igstkLogMacro( DEBUG, "Received Tool spatial object Transform " 
                 << this->m_ToolTransformWRTImageCoordinateSystem );
}

/** Request compute reslicing plane */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestComputeReslicingPlane( )
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestComputeReslicingPlane called...\n");

  //Update the tool transform
  this->RequestUpdateToolTransform();

  switch( m_ReslicingMode )
    {
    case Orthogonal:
      {
      this->ComputeOrthgonalReslicingPlane();
      break;
      }
    case Oblique:
      {
      this->ComputeObliqueReslicingPlane();
      break;
      }
    case OffOrthogonal:
      {
      this->ComputeOffOrthgonalReslicingPlane();
      break;
      }
    default:
      break;
    }

} 

/**Compute orthgonal reslicing plane */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ComputeOrthgonalReslicingPlane( )
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::ComputeOrthgonalReslicingPlane called...\n");
 
  Transform::VectorType   translation;
  translation = 
        m_ToolTransformWRTImageCoordinateSystem.GetTranslation();

  double planeCenter[3];
  planeCenter[0] = translation[0];
  planeCenter[1] = translation[1];
  planeCenter[2] = translation[2];

  /* Compute input image bounds */
  double imageSpacing[3];
  m_ImageData->GetSpacing( imageSpacing );
  std::cout << "Image spacing: " << "(" << imageSpacing[0] << "," 
                                 << imageSpacing[1] << ","
                                 << imageSpacing[2] << ")" << std::endl;
  double imageOrigin[3];
  m_ImageData->GetOrigin( imageOrigin );
  std::cout << "Image origin: " << "(" << imageOrigin[0] << "," 
                                 << imageOrigin[1] << ","
                                 << imageOrigin[2] << ")" << std::endl;

  int imageExtent[6];
  m_ImageData->GetWholeExtent( imageExtent );
  std::cout << "Image extent: " << "(" << imageExtent[0] << "," 
                                << imageExtent[1] << ","
                                << imageExtent[2] << ","
                                << imageExtent[3] << ","
                                << imageExtent[4] << ","
                                << imageExtent[5] << ")" << std::endl;

  double bounds[] = { imageOrigin[0] + imageSpacing[0]*imageExtent[0], //xmin
                       imageOrigin[0] + imageSpacing[0]*imageExtent[1], //xmax
                       imageOrigin[1] + imageSpacing[1]*imageExtent[2], //ymin
                       imageOrigin[1] + imageSpacing[1]*imageExtent[3], //ymax
                       imageOrigin[2] + imageSpacing[2]*imageExtent[4], //zmin
                       imageOrigin[2] + imageSpacing[2]*imageExtent[5]};//zmax

  for ( unsigned int i = 0; i <= 4; i += 2 ) // reverse bounds if necessary
      {
      if ( bounds[i] > bounds[i+1] )
        {
        double t = bounds[i+1];
        bounds[i+1] = bounds[i];
        bounds[i] = t;
        }
      }

  std::cout << "Image bounds: " << "(" << bounds[0] << "," 
                                << bounds[1] << ","
                                << bounds[2] << ","
                                << bounds[3] << ","
                                << bounds[4] << ","
                                << bounds[5] << ")" << std::endl;

  //Determine two points using the bounds and 
  double point1[3];
  double point2[3];
  double planeNormal[3];

  switch( m_OrientationType )
    {
    case Axial:
      {
      planeNormal[0] = 0.0;
      planeNormal[1] = 0.0;
      planeNormal[2] = 1.0;

      point1[0] = bounds[1];
      point1[1] = planeCenter[1];
      point1[2] = bounds[4];

      point2[0] = bounds[0];
      point2[1] = planeCenter[1];
      point2[2] = bounds[5];
   
      break;
      }
    case Coronal:
      {
      planeNormal[0] = 0.0;
      planeNormal[1] = 1.0;
      planeNormal[2] = 0.0;

      point1[0] = bounds[1];
      point1[1] = bounds[2];
      point1[2] = planeCenter[2];

      point2[0] = bounds[0];
      point2[1] = bounds[3];
      point2[2] = planeCenter[2];

      break;
      }
    case Sagittal:
      {

      planeNormal[0] = 1.0;
      planeNormal[1] = 0.0;
      planeNormal[2] = 0.0;

      point1[0] = bounds[1];
      point1[1] = bounds[2];
      point1[2] = planeCenter[2];

      point2[0] = planeCenter[0];
      point2[1] = bounds[2];
      point2[2] = bounds[5];

      break;
      }
    default:
      {
      std::cerr << "Invalid orientation type " << std::endl;
      break;
      }
    }

  m_ImageReslicePlane->SetOrigin( planeCenter[0],
                                  planeCenter[1],
                                  planeCenter[2] );

  std::cout << "Plane center: " << "(" << planeCenter[0] << "," 
                                 << planeCenter[1] << ","
                                 << planeCenter[2] << ")" << std::endl;
 
  m_ImageReslicePlane->SetNormal( planeNormal[0],
                                  planeNormal[1],
                                  planeNormal[2] );

  std::cout << "Plane normal: " << "(" << planeNormal[0] << "," 
                                 << planeNormal[1] << ","
                                 << planeNormal[2] << ")" << std::endl;
  
  //Using the two points define two coordinate axes
  double axes1[3];
  axes1[0] = point1[0] - planeCenter[0]; 
  axes1[1] = point1[1] - planeCenter[1]; 
  axes1[2] = point1[2] - planeCenter[2]; 

  double axes2[3];
  axes2[0] = point2[0] - planeCenter[0]; 
  axes2[1] = point2[1] - planeCenter[1]; 
  axes2[2] = point2[2] - planeCenter[2]; 

  //Normalize the axes
  double planeSizeX = vtkMath::Normalize(axes1);
  double planeSizeY = vtkMath::Normalize(axes2);

  // Generate the reslicing matrix
  m_ResliceAxes->Identity();
  for ( unsigned int i = 0; i < 3; i++ )
     {
     m_ResliceAxes->SetElement(0,i,axes1[i]);
     m_ResliceAxes->SetElement(1,i,axes2[i]);
     m_ResliceAxes->SetElement(2,i,planeNormal[i]);
     }

  m_ResliceAxes->SetElement(0, 3, planeCenter[0]);
  m_ResliceAxes->SetElement(1, 3, planeCenter[1]);
  m_ResliceAxes->SetElement(2, 3, planeCenter[2]);
}

/**Compute oblique reslicing plane */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ComputeObliqueReslicingPlane( )
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::ComputeObliqueReslicingPlane called...\n");
}

/**Compute off-orthgonal reslicing plane */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ComputeOffOrthgonalReslicingPlane( )
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::ComputeOffOrthgonalReslicingPlane called...\n");
}

/** Request Get reslcing plane equation */
template < class TImageSpatialObject >
vtkPlane *
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestGetReslicingPlane()
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestGetReslicingPlane called...\n");
  return m_ImageReslicePlane;
}

/** Request Get reslicing axes matrix */
template < class TImageSpatialObject >
vtkMatrix4x4 *
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestGetReslicingMatrix()
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestGetReslicingPlane called...\n");
  return m_ResliceAxes;
}

/** Report invalid reslicing mode */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReportInvalidReslicingModeProcessing( void )
{
  igstkLogMacro( WARNING, 
    "igstk::ImageReslicePlaneSpatialObject::ReportInvalidReslicingModeProcessing called...\n");
}

/** Report invalid orientation type */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReportInvalidOrientationTypeProcessing( void )
{
  igstkLogMacro( WARNING, 
    "igstk::ImageReslicePlaneSpatialObject::ReportInvalidOrientationTypeProcessing called...\n");
}

/** Report invalid request */
template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReportInvalidRequestProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::ImageReslicePlaneSpatialObject::ReportInvalidRequestProcessing called...\n");

  this->InvokeEvent( InvalidRequestErrorEvent() );
}

/** Print object information */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk

#endif
