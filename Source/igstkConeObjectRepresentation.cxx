/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkConeObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkConeObjectRepresentation.h"
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>


namespace igstk
{ 

/** Constructor */
ConeObjectRepresentation::ConeObjectRepresentation():m_StateMachine(this)
{
  igstkLogMacro( DEBUG,  "Constructor called ....\n" );

  // We create the ellipse spatial object
  m_ConeSpatialObject = NULL;
  this->RequestSetSpatialObject( m_ConeSpatialObject );
  m_ConeSource = vtkConeSource::New();
  
  igstkAddInputMacro( ValidConeObject );
  igstkAddInputMacro( NullConeObject  );

  igstkAddStateMacro( NullConeObject  );
  igstkAddStateMacro( ValidConeObject );

  igstkAddTransitionMacro( NullConeObject, NullConeObject, 
                           NullConeObject,  No );
  igstkAddTransitionMacro( NullConeObject, ValidConeObject, 
                           ValidConeObject,  SetConeObject );
  igstkAddTransitionMacro( ValidConeObject, NullConeObject,
                           NullConeObject,  No ); 
  igstkAddTransitionMacro( ValidConeObject, ValidConeObject, 
                           ValidConeObject,  No ); 

  igstkSetInitialStateMacro( NullConeObject );

  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
ConeObjectRepresentation::~ConeObjectRepresentation()  
{
  igstkLogMacro( DEBUG,  "Destructor called ....\n" );
  
  if( m_ConeSource )
    {
    m_ConeSource->Delete();
    m_ConeSource = NULL;
    }
  this->DeleteActors();
}

/** Set the Coneal Spatial Object */
void ConeObjectRepresentation
::RequestSetConeObject( const ConeSpatialObjectType * cone )
{
  igstkLogMacro( DEBUG,  "RequestSetConeObject called ....\n" );

  m_ConeObjectToAdd = cone;
  if( !m_ConeObjectToAdd )
    {
    igstkPushInputMacro( NullConeObject );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidConeObject );
    m_StateMachine.ProcessInputs();
    }
}

/** Set the Cylindrical Spatial Object */
void ConeObjectRepresentation::NoProcessing()
{
  igstkLogMacro( DEBUG,  "NoProcessing called ....\n" );
}


/** Set the Cylindrical Spatial Object */
void ConeObjectRepresentation::SetConeObjectProcessing()
{
  igstkLogMacro( DEBUG,  "SetConeObjectProcessing called ....\n" );

  // We create the ellipse spatial object
  m_ConeSpatialObject = m_ConeObjectToAdd;
  this->RequestSetSpatialObject( m_ConeSpatialObject );

  if( m_ConeSource != NULL )
    {
    m_ConeSource->SetCenter(0, 0, 0);
    m_ConeSource->SetRadius(m_ConeSpatialObject->GetRadius());
    m_ConeSource->SetHeight(m_ConeSpatialObject->GetHeight());
    m_ConeSource->SetResolution(20);
    }
} 

/** Print Self function */
void ConeObjectRepresentation
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if( this->m_ConeSource )
    {
    os << indent << this->m_ConeSource << std::endl;
    }
}

/** Update the visual representation in response to changes in the geometric
 * object */
void ConeObjectRepresentation::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG,  "UpdateRepresentationProcessing called ....\n" );

  m_ConeSource->SetRadius( m_ConeSpatialObject->GetRadius() );
  m_ConeSource->SetHeight( m_ConeSpatialObject->GetHeight() );
}


/** Create the vtk Actors */
void ConeObjectRepresentation::CreateActors()
{
  igstkLogMacro( DEBUG,  "CreateActors called ....\n" );
  
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();
   
  vtkPolyDataMapper *coneMapper = vtkPolyDataMapper::New();
  vtkActor* coneActor = vtkActor::New();

  coneActor->GetProperty()->SetColor(this->GetRed(),
                                         this->GetGreen(),
                                         this->GetBlue()); 
        
  coneActor->GetProperty()->SetOpacity(this->GetOpacity()); 
  coneMapper->SetInput(m_ConeSource->GetOutput());
  coneActor->SetMapper( coneMapper );

  // We align the actor in the Z direction such that the top of
  // the cone is at (0,0,-m_Height/2.0) and the tip at (0,0,0)
  coneActor->SetOrientation(0,-90,0);
  coneActor->SetPosition(0,0,-m_ConeSpatialObject->GetHeight()/2.0);

  // We should check if the actor doesn't exist
  this->AddActor( coneActor );
  coneMapper->Delete();
}

/** Create a copy of the current object representation */
ConeObjectRepresentation::Pointer
ConeObjectRepresentation::Copy() const
{
  igstkLogMacro( DEBUG,  "Copy called ....\n" );
  
  Pointer newOR = ConeObjectRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetConeObject(m_ConeSpatialObject);

  return newOR;
}

} // end namespace igstk
