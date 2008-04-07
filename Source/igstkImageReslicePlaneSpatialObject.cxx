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
ImageReslicePlaneSpatialObject::ImageReslicePlaneSpatialObject():m_StateMachine(this)
{
  //Default reslicing mode
  m_ReslicingMode = Orthogonal;

  //Default orientation type
  m_OrientationType = Axial;

  //List of states
  igstkAddStateMacro( Initial );
  igstkAddStateMacro( ReslicingModeSet );
  igstkAddStateMacro( OrientationTypeSet );

  // List of state machine inputs
  igstkAddInputMacro( ValidReslicingMode );
  igstkAddInputMacro( InValidReslicingMode );
  igstkAddInputMacro( ValidOrientationType );
  igstkAddInputMacro( InValidOrientationType );


  // List of state machine transitions

  // From Initial
  igstkAddTransitionMacro( Initial, ValidReslicingMode, ReslicingModeSet, SetReslicingMode );
  igstkAddTransitionMacro( Initial, InValidReslicingMode, Initial, ReportInvalidReslicingMode );
  igstkAddTransitionMacro( Initial, ValidOrientationType, Initial, ReportInvalidRequest);
  igstkAddTransitionMacro( Initial, InValidOrientationType, Initial, ReportInvalidRequest);

  igstkSetInitialStateMacro( Initial );
  this->m_StateMachine.SetReadyToRun();
} 

/** Destructor */
ImageReslicePlaneSpatialObject::~ImageReslicePlaneSpatialObject()  
{
}

void 
ImageReslicePlaneSpatialObject
::RequestSetReslicingMode( ReslicingMode reslicingMode )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestSetReslicingMode called...\n");

  m_ReslicingModeToBeSet = reslicingMode;
  m_StateMachine.PushInput( m_ValidReslicingModeInput );

  //FIXME: Check conditions for InValidReslicing mode
  m_StateMachine.ProcessInputs();
}

void 
ImageReslicePlaneSpatialObject
::SetReslicingModeProcessing()
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::SetReslicingModeProcessing called...\n");
  m_ReslicingMode = m_ReslicingModeToBeSet;
}
void 
ImageReslicePlaneSpatialObject
::RequestSetOrientationType( OrientationType orientationType )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestSetOrientationType called...\n");

  m_OrientationTypeToBeSet = orientationType;
  m_StateMachine.PushInput( m_ValidOrientationTypeInput );

  //FIXME: Check conditions for InValidOrientation type 
  m_StateMachine.ProcessInputs();
}
void 
ImageReslicePlaneSpatialObject
::SetOrientationTypeProcessing()
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::SetOrientationTypeProcessing called...\n");
  m_OrientationType = m_OrientationTypeToBeSet;
}

/** Report invalid reslicing mode */
void
ImageReslicePlaneSpatialObject
::ReportInvalidReslicingModeProcessing( void )
{
  igstkLogMacro( WARNING, 
    "igstk::ImageReslicePlaneSpatialObject::ReportInvalidReslicingModeProcessing called...\n");
}

/** Report invalid orientation type */
void
ImageReslicePlaneSpatialObject
::ReportInvalidOrientationTypeProcessing( void )
{
  igstkLogMacro( WARNING, 
    "igstk::ImageReslicePlaneSpatialObject::ReportInvalidOrientationTypeProcessing called...\n");
}

/** Report invalid request */
void 
ImageReslicePlaneSpatialObject
::ReportInvalidRequestProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::ImageReslicePlaneSpatialObject::ReportInvalidRequestProcessing called...\n");

  this->InvokeEvent( InvalidRequestErrorEvent() );
}

/** Print object information */
void ImageReslicePlaneSpatialObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

}

} // end namespace igstk
