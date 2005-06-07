/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCylinderObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkCylinderObjectRepresentation.h"
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include "igstkEvents.h"


namespace igstk
{ 

/** Constructor */
CylinderObjectRepresentation::CylinderObjectRepresentation():m_StateMachine(this)
{
  // We create the ellipse spatial object
  m_CylinderSpatialObject = NULL;
  this->RequestSetSpatialObject( m_CylinderSpatialObject );
  m_CylinderSource = vtkCylinderSource::New();
  
  m_StateMachine.AddInput( m_ValidCylinderObjectInput,  "ValidCylinderObjectInput" );
  m_StateMachine.AddInput( m_NullCylinderObjectInput,   "NullCylinderObjectInput"  );

  m_StateMachine.AddState( m_NullCylinderObjectState,  "NullCylinderObjectState"     );
  m_StateMachine.AddState( m_ValidCylinderObjectState, "ValidCylinderObjectState"     );

  const ActionType NoAction = 0;

  m_StateMachine.AddTransition( m_NullCylinderObjectState, m_NullCylinderObjectInput, m_NullCylinderObjectState,  NoAction );
  m_StateMachine.AddTransition( m_NullCylinderObjectState, m_ValidCylinderObjectInput, m_ValidCylinderObjectState,  & CylinderObjectRepresentation::SetCylinderObject );
  m_StateMachine.AddTransition( m_ValidCylinderObjectState, m_NullCylinderObjectInput, m_NullCylinderObjectState,  NoAction ); 
  m_StateMachine.AddTransition( m_ValidCylinderObjectState, m_ValidCylinderObjectInput, m_ValidCylinderObjectState,  NoAction ); 

  m_StateMachine.SelectInitialState( m_NullCylinderObjectState );

  m_StateMachine.SetReadyToRun();


} 

/** Destructor */
CylinderObjectRepresentation::~CylinderObjectRepresentation()  
{
  if( m_CylinderSource )
    {
    m_CylinderSource->Delete();
    m_CylinderSource = NULL;
    }
}




/** Set the Cylinderal Spatial Object */
void CylinderObjectRepresentation::RequestSetCylinderObject( const CylinderSpatialObjectType * cylinder )
{
  m_CylinderObjectToAdd = cylinder;
  if( !m_CylinderObjectToAdd )
    {
    m_StateMachine.PushInput( m_NullCylinderObjectInput );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    m_StateMachine.PushInput( m_ValidCylinderObjectInput );
    m_StateMachine.ProcessInputs();
    }


}




/** Set the Cylindrical Spatial Object */
void CylinderObjectRepresentation::SetCylinderObject()
{
  // We create the ellipse spatial object
  m_CylinderSpatialObject = m_CylinderObjectToAdd;
  this->RequestSetSpatialObject( m_CylinderSpatialObject );

  if( m_CylinderSource != NULL )
    {
    m_CylinderSource->SetCenter(0, 0, 0);
    m_CylinderSource->SetRadius(m_CylinderSpatialObject->GetRadius());
    m_CylinderSource->SetHeight(m_CylinderSpatialObject->GetHeight());
    m_CylinderSource->SetResolution(10);
    }
} 


/** Print Self function */
void CylinderObjectRepresentation::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if( this->m_CylinderSource )
  {
    os << indent << this->m_CylinderSource << std::endl;
  }
}


/** Update the visual representation in response to changes in the geometric
 * object */
void CylinderObjectRepresentation::UpdateRepresentation()
{
  m_CylinderSource->SetRadius(m_CylinderSpatialObject->GetRadius());
  m_CylinderSource->SetHeight(m_CylinderSpatialObject->GetHeight());
}


/** Create the vtk Actors */
void CylinderObjectRepresentation::CreateActors()
{
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();
   
  vtkPolyDataMapper *cylinderMapper = vtkPolyDataMapper::New();
  vtkActor* cylinderActor = vtkActor::New();

  cylinderActor->GetProperty()->SetColor(this->GetRed(),
                                         this->GetGreen(),
                                         this->GetBlue()); 
        
  cylinderActor->GetProperty()->SetOpacity(this->GetOpacity()); 
  cylinderMapper->SetInput(m_CylinderSource->GetOutput());
  cylinderActor->SetMapper( cylinderMapper );

  // We should check if the actor doesn't exist
  this->AddActor( cylinderActor );
  cylinderMapper->Delete();
}

/** Create a copy of the current object representation */
CylinderObjectRepresentation::Pointer
CylinderObjectRepresentation::Copy() const
{
  Pointer newOR = CylinderObjectRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetCylinderObject(m_CylinderSpatialObject);

  return newOR;
}


} // end namespace igstk

