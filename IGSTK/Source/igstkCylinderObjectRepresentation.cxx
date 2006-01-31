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
  
  igstkAddInputMacro( ValidCylinderObject );
  igstkAddInputMacro( NullCylinderObject  );

  igstkAddStateMacro( NullCylinderObject  );
  igstkAddStateMacro( ValidCylinderObject );

  igstkAddTransitionMacro( NullCylinderObject, NullCylinderObject, NullCylinderObject,  No );
  igstkAddTransitionMacro( NullCylinderObject, ValidCylinderObject, ValidCylinderObject,  SetCylinderObject );
  igstkAddTransitionMacro( ValidCylinderObject, NullCylinderObject, NullCylinderObject,  No ); 
  igstkAddTransitionMacro( ValidCylinderObject, ValidCylinderObject, ValidCylinderObject,  No ); 

  igstkSetInitialStateMacro( NullCylinderObject );

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
  this->DeleteActors();
}




/** Set the Cylinderal Spatial Object */
void CylinderObjectRepresentation::RequestSetCylinderObject( const CylinderSpatialObjectType * cylinder )
{
  m_CylinderObjectToAdd = cylinder;
  if( !m_CylinderObjectToAdd )
    {
    igstkPushInputMacro( NullCylinderObject );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidCylinderObject );
    m_StateMachine.ProcessInputs();
    }


}



/** Set the Cylindrical Spatial Object */
void CylinderObjectRepresentation::NoProcessing()
{
}


/** Set the Cylindrical Spatial Object */
void CylinderObjectRepresentation::SetCylinderObjectProcessing()
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
void CylinderObjectRepresentation::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "UpdateRepresentationProcessing called ....\n");
  this->RequestVerifyTimeStamp();

  m_CylinderSource->SetRadius( m_CylinderSpatialObject->GetRadius() );
  m_CylinderSource->SetHeight( m_CylinderSpatialObject->GetHeight() );
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

  // We align the actor in the Z direction such that the top of
  // the cylinder is at (0,0,-m_Height) and the tip at (0,0,0)
  cylinderActor->SetOrientation(90,0,0);
  cylinderActor->SetPosition(0,0,m_CylinderSpatialObject->GetHeight()/2.0);

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

