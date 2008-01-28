/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAxesObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkAxesObjectRepresentation.h"
#include "igstkEvents.h"

#include <vtkActor.h>
#include <vtkAxesActor.h>


namespace igstk
{ 

/** Constructor */
AxesObjectRepresentation::AxesObjectRepresentation():m_StateMachine(this)
{
  // We create the ellipse spatial object
  m_AxesSpatialObject = NULL;
  this->RequestSetSpatialObject( m_AxesSpatialObject );
  
  igstkAddInputMacro( ValidAxesObject );
  igstkAddInputMacro( NullAxesObject  );

  igstkAddStateMacro( NullAxesObject  );
  igstkAddStateMacro( ValidAxesObject );

  igstkAddTransitionMacro( NullAxesObject, NullAxesObject, 
                           NullAxesObject,  No );
  igstkAddTransitionMacro( NullAxesObject, ValidAxesObject, 
                           ValidAxesObject,  SetAxesObject );
  igstkAddTransitionMacro( ValidAxesObject, NullAxesObject, 
                           NullAxesObject,  No ); 
  igstkAddTransitionMacro( ValidAxesObject, ValidAxesObject, 
                           ValidAxesObject,  No ); 

  m_StateMachine.SelectInitialState( m_NullAxesObjectState );

  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
AxesObjectRepresentation::~AxesObjectRepresentation()  
{
  this->DeleteActors();
}

/** Request to Set the AxesSpatial Object */
void AxesObjectRepresentation::RequestSetAxesObject( 
                                          const AxesSpatialObjectType * Axes )
{
  m_AxesObjectToAdd = Axes;
  if( !m_AxesObjectToAdd )
    {
    m_StateMachine.PushInput( m_NullAxesObjectInput );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    m_StateMachine.PushInput( m_ValidAxesObjectInput );
    m_StateMachine.ProcessInputs();
    }
}

/** Set the Axes Spatial Object */
void AxesObjectRepresentation::NoProcessing()
{
}

/** Set the Axes Spatial Object */
void AxesObjectRepresentation::SetAxesObjectProcessing()
{
  m_AxesSpatialObject = m_AxesObjectToAdd;
  this->RequestSetSpatialObject( m_AxesSpatialObject );
} 

/** Print Self function */
void AxesObjectRepresentation::PrintSelf( std::ostream& os, 
                                          itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** Update the visual representation in response to changes in the geometric
 * object */
void AxesObjectRepresentation::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "UpdateRepresentationProcessing called ....\n");
}

/** Create the vtk Actors */
void AxesObjectRepresentation::CreateActors()
{
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();
  vtkAxesActor* axesActor = vtkAxesActor::New();
  axesActor->SetTotalLength(m_AxesSpatialObject->GetSizeX(),
                            m_AxesSpatialObject->GetSizeY(),
                            m_AxesSpatialObject->GetSizeZ());

  axesActor->AxisLabelsOff();

  this->AddActor( axesActor );
}

/** Create a copy of the current object representation */
AxesObjectRepresentation::Pointer
AxesObjectRepresentation::Copy() const
{
  Pointer newOR = AxesObjectRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetAxesObject(m_AxesSpatialObject);

  return newOR;
}

} // end namespace igstk
