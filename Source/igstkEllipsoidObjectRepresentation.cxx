/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkEllipsoidObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkEllipsoidObjectRepresentation.h"
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
EllipsoidObjectRepresentation::EllipsoidObjectRepresentation():m_StateMachine(this)
{
  // We create the ellipse spatial object
  m_EllipsoidObject = NULL;
  this->RequestSetSpatialObject( m_EllipsoidObject );
  m_EllipsoidSource = vtkSuperquadricSource::New();
  
  igstkAddInputMacro( ValidEllipsoidObjectInput );
  igstkAddInputMacro( NullEllipsoidObjectInput  );

  igstkAddStateMacro( NullEllipsoidObjectState  );
  igstkAddStateMacro( ValidEllipsoidObjectState );

  igstkAddTransitionMacro( NullEllipsoidObjectState, NullEllipsoidObjectInput, NullEllipsoidObjectState,  NoAction );
  igstkAddTransitionMacro( NullEllipsoidObjectState, ValidEllipsoidObjectInput, ValidEllipsoidObjectState,  SetEllipsoidObject );
  igstkAddTransitionMacro( ValidEllipsoidObjectState, NullEllipsoidObjectInput, NullEllipsoidObjectState,  NoAction ); 
  igstkAddTransitionMacro( ValidEllipsoidObjectState, ValidEllipsoidObjectInput, ValidEllipsoidObjectState,  NoAction ); 

  m_StateMachine.SelectInitialState( m_NullEllipsoidObjectState );

  m_StateMachine.SetReadyToRun();

} 

/** Destructor */
EllipsoidObjectRepresentation::~EllipsoidObjectRepresentation()  
{
  if( m_EllipsoidSource )
    {
    m_EllipsoidSource->Delete();
    m_EllipsoidSource = NULL;
    }
  this->DeleteActors();
}



/** Print Self function */
void EllipsoidObjectRepresentation::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  if(m_EllipsoidObject)
    { 
    os << indent << m_EllipsoidObject << std::endl; 
    }
}


/** Set the Ellipsoidal Spatial Object */
void EllipsoidObjectRepresentation::RequestSetEllipsoidObject( const EllipsoidObjectType * ellipsoid )
{
  m_EllipsoidObjectToAdd = ellipsoid;
  if( !m_EllipsoidObjectToAdd )
    {
    m_StateMachine.PushInput( m_NullEllipsoidObjectInput );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    m_StateMachine.PushInput( m_ValidEllipsoidObjectInput );
    m_StateMachine.ProcessInputs();
    }


}


/** Null operation for a State Machine transition */
void EllipsoidObjectRepresentation::NoAction()
{
}


/** Set the Ellipsoidal Spatial Object */
void EllipsoidObjectRepresentation::SetEllipsoidObject()
{
  // We create the ellipse spatial object
  m_EllipsoidObject = m_EllipsoidObjectToAdd;
  this->RequestSetSpatialObject( m_EllipsoidObject );

  if( m_EllipsoidSource != NULL )
    {
    m_EllipsoidSource->SetCenter(0, 0, 0);

    EllipsoidObjectType::ArrayType radius = m_EllipsoidObject->GetRadius();
    m_EllipsoidSource->SetScale( radius[0], radius[1], radius[2] );
    m_EllipsoidSource->SetThetaResolution( 10 );
    m_EllipsoidSource->SetPhiResolution( 10 );
    }
} 


/** Update the visual representation in response to changes in the geometric
 * object */
void EllipsoidObjectRepresentation::UpdateRepresentation()
{
  EllipsoidObjectType::ArrayType radius = m_EllipsoidObject->GetRadius();
  m_EllipsoidSource->SetScale( radius[0], radius[1], radius[2] );
}


/** Create the vtk Actors */
void EllipsoidObjectRepresentation::CreateActors()
{
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  vtkPolyDataMapper *ellipsoidMapper = vtkPolyDataMapper::New();
  vtkActor* ellipsoidActor = vtkActor::New();
  m_EllipsoidSource->SetCenter(0, 0, 0);

  ellipsoidActor->GetProperty()->SetColor(this->GetRed(),
                                          this->GetGreen(),
                                          this->GetBlue());

  ellipsoidActor->GetProperty()->SetOpacity(this->GetOpacity());

  ellipsoidMapper->SetInput( m_EllipsoidSource->GetOutput() );
  ellipsoidActor->SetMapper(ellipsoidMapper);

  // We should check if the actor doesn't exist
  this->AddActor( ellipsoidActor );
  ellipsoidMapper->Delete();
}

/** Create a copy of the current object representation */
EllipsoidObjectRepresentation::Pointer
EllipsoidObjectRepresentation::Copy() const
{
  Pointer newOR = EllipsoidObjectRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetEllipsoidObject(m_EllipsoidObject);

  return newOR;
}


} // end namespace igstk

