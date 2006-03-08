/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkContourObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkContourObjectRepresentation.h"
#include "igstkEvents.h"

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkIdList.h>
#include <vtkDataSetMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCutter.h>

namespace igstk
{ 

/** Constructor */
ContourObjectRepresentation
::ContourObjectRepresentation():m_StateMachine(this)
{
  // We create the ellipse spatial object
  m_Orientation = Axial;
  
  igstkAddInputMacro( SetSlicePosition  );
  igstkAddInputMacro( ValidSlicePosition  );
  igstkAddInputMacro( InvalidSlicePosition  );
  igstkAddInputMacro( ValidOrientation  );

  igstkAddStateMacro( Initial );
  igstkAddStateMacro( ValidOrientation );
  igstkAddStateMacro( ValidSlicePosition );
  igstkAddStateMacro( AttemptingToSetSlicePosition );

  // NullObject 
  igstkAddTransitionMacro( Initial, SetSlicePosition, 
                           Initial,  No );
  igstkAddTransitionMacro( Initial, ValidSlicePosition,
                           Initial,  No );
  igstkAddTransitionMacro( Initial, InvalidSlicePosition, 
                           Initial,  No );
  igstkAddTransitionMacro( Initial, ValidOrientation, 
                           ValidOrientation,  SetOrientation );

  igstkAddTransitionMacro( ValidOrientation, SetSlicePosition, 
                           AttemptingToSetSlicePosition,  
                           AttemptSetSlicePosition ); 
  igstkAddTransitionMacro( ValidOrientation, ValidSlicePosition, 
                           ValidSlicePosition,  SetSlicePosition ); 
  igstkAddTransitionMacro( ValidOrientation, InvalidSlicePosition, 
                           ValidOrientation,  No ); 
  igstkAddTransitionMacro( ValidOrientation, ValidOrientation, 
                           ValidOrientation,  SetOrientation ); 

  // Valid Slice Number
  igstkAddTransitionMacro( ValidSlicePosition, SetSlicePosition,
                           AttemptingToSetSlicePosition,
                           AttemptSetSlicePosition );
  igstkAddTransitionMacro( ValidSlicePosition, ValidSlicePosition,
                           ValidSlicePosition,  SetSlicePosition ); 
  igstkAddTransitionMacro( ValidSlicePosition, InvalidSlicePosition,
                           ValidOrientation,  No ); 
  igstkAddTransitionMacro( ValidSlicePosition, ValidOrientation,
                           ValidOrientation,  SetOrientation ); 
  
  // AttemptingToSetSlicePosition
  igstkAddTransitionMacro( AttemptingToSetSlicePosition, SetSlicePosition,
                           AttemptingToSetSlicePosition,  
                           AttemptSetSlicePosition );
  igstkAddTransitionMacro( AttemptingToSetSlicePosition, ValidSlicePosition,
                           ValidSlicePosition,  SetSlicePosition ); 
  igstkAddTransitionMacro( AttemptingToSetSlicePosition, InvalidSlicePosition,
                           ValidOrientation,  No ); 
  igstkAddTransitionMacro( AttemptingToSetSlicePosition, ValidOrientation,
                           ValidOrientation,  SetOrientation ); 

  igstkSetInitialStateMacro( Initial );

  m_StateMachine.SetReadyToRun();

  m_Plane = vtkPlane::New();
} 

/** Destructor */
ContourObjectRepresentation::~ContourObjectRepresentation()  
{
  m_Plane->Delete();
}

/** Null operation for a State Machine transition */
void ContourObjectRepresentation::NoProcessing()
{
}

/** Print Self function */
void ContourObjectRepresentation
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

void 
ContourObjectRepresentation
::RequestSetOrientation( OrientationType orientation )
{
  igstkLogMacro( DEBUG, "igstk::ContourObjectRepresentation\
                        ::RequestSetOrientation called...\n");
  
  m_OrientationToBeSet = orientation;

  m_StateMachine.PushInput( m_ValidOrientationInput );
  m_StateMachine.ProcessInputs();
}

void 
ContourObjectRepresentation
::SetOrientationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ContourObjectRepresentation\
                        ::SetOrientationProcessing called...\n");

  m_Orientation = m_OrientationToBeSet;
}
  
void 
ContourObjectRepresentation
::RequestSetSlicePosition( float slicePosition )
{
  igstkLogMacro( DEBUG, "igstk::ContourObjectRepresentation\
                        ::RequestSetSlicePosition called...\n");

  m_SlicePositionToBeSet = slicePosition;

  m_StateMachine.PushInput( m_SetSlicePositionInput );
  m_StateMachine.ProcessInputs();
}

void 
ContourObjectRepresentation
::AttemptSetSlicePositionProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ContourObjectRepresentation\
                        ::AttemptSetSlicePositionProcessing called...\n");

  m_StateMachine.PushInput( m_ValidSlicePositionInput );

  //m_StateMachine.PushInput( m_InvalidSlicePositionInput );
  m_StateMachine.ProcessInputs();
}

void 
ContourObjectRepresentation
::SetSlicePositionProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ContourObjectRepresentation\
                        ::SetSlicePosition called...\n");

  m_SlicePosition = m_SlicePositionToBeSet;

  switch( m_Orientation )
    {
    case Axial:
      m_Plane->SetOrigin(0,0,m_SlicePosition+0.01);
      break;
    case Sagittal:
      m_Plane->SetOrigin(m_SlicePosition+0.01,0,0);
      break;
    case Coronal:
      m_Plane->SetOrigin(0,m_SlicePosition+0.01,0);
      break;
    }
}

} // end namespace igstk
