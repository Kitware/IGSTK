/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageReslicePlaneSpatialObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkImageReslicePlaneSpatialObject.h"
#include "igstkEvents.h"

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

  //List of states
  igstkAddStateMacro( Initial );
  igstkAddStateMacro( ReslicingModeSet );
  igstkAddStateMacro( OrientationTypeSet );
  igstkAddStateMacro( ImageSpatialObjectSet );

  // List of state machine inputs
  igstkAddInputMacro( ValidReslicingMode );
  igstkAddInputMacro( InValidReslicingMode );
  igstkAddInputMacro( ValidOrientationType );
  igstkAddInputMacro( InValidOrientationType );
  igstkAddInputMacro( ValidImageSpatialObject );
  igstkAddInputMacro( InValidImageSpatialObject );


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
  igstkAddTransitionMacro( OrientationTypeSet, ValidImageSpatialObject, ImageSpatialObjectSet, SetImageSpatialObject );
  igstkAddTransitionMacro( OrientationTypeSet, InValidImageSpatialObject, OrientationTypeSet, ReportInvalidImageSpatialObject );
  

  igstkSetInitialStateMacro( Initial );
  this->m_StateMachine.SetReadyToRun();
} 

/** Destructor */
template < class TImageSpatialObject >
ImageReslicePlaneSpatialObject<TImageSpatialObject>::~ImageReslicePlaneSpatialObject()  
{
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
::RequestSetImageSpatialObject( ImageSpatialObjectType imageSpatialObject )
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
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReportInvalidImageSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::ReportInvalidImageSpatialObjectProcessing called...\n");
}

/** Request compute reslicing plane */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestComputeReslicingPlane( )
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestComputeReslicingPlane called...\n");

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
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestGetReslicingPlane()
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestGetReslicingPlane called...\n");
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
