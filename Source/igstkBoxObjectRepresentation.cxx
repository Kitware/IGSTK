/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkBoxObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkBoxObjectRepresentation.h"
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>


namespace igstk
{ 

/** Constructor */
BoxObjectRepresentation::BoxObjectRepresentation():m_StateMachine(this)
{
  // We create the ellipse spatial object
  m_BoxSpatialObject = NULL;
  this->RequestSetSpatialObject( m_BoxSpatialObject );
  m_BoxSource = vtkCubeSource::New();
  
  igstkAddInputMacro( ValidBoxObject );
  igstkAddInputMacro( NullBoxObject  );

  igstkAddStateMacro( NullBoxObject  );
  igstkAddStateMacro( ValidBoxObject );

  igstkAddTransitionMacro( NullBoxObject, NullBoxObject, NullBoxObject,  No );
  igstkAddTransitionMacro( NullBoxObject, ValidBoxObject, ValidBoxObject,  SetBoxObject );
  igstkAddTransitionMacro( ValidBoxObject, NullBoxObject, NullBoxObject,  No ); 
  igstkAddTransitionMacro( ValidBoxObject, ValidBoxObject, ValidBoxObject,  No ); 

  igstkSetInitialStateMacro( NullBoxObject );

  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
BoxObjectRepresentation::~BoxObjectRepresentation()  
{
  if( m_BoxSource )
    {
    m_BoxSource->Delete();
    m_BoxSource = NULL;
    }
  this->DeleteActors();
}

/** Set the Box Spatial Object */
void BoxObjectRepresentation::RequestSetBoxObject( const BoxSpatialObjectType * box )
{
  m_BoxObjectToAdd = box;
  if( !m_BoxObjectToAdd )
    {
    igstkPushInputMacro( NullBoxObject );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidBoxObject );
    m_StateMachine.ProcessInputs();
    }
}


/** No Processing */
void BoxObjectRepresentation::NoProcessing()
{
}


/** Set the Box Spatial Object */
void BoxObjectRepresentation::SetBoxObjectProcessing()
{
  // We create the ellipse spatial object
  m_BoxSpatialObject = m_BoxObjectToAdd;
  this->RequestSetSpatialObject( m_BoxSpatialObject );

  if( m_BoxSource != NULL )
    {
    m_BoxSource->SetCenter(0, 0, 0);
    m_BoxSource->SetXLength(m_BoxSpatialObject->GetSizeX());
    m_BoxSource->SetYLength(m_BoxSpatialObject->GetSizeY());
    m_BoxSource->SetZLength(m_BoxSpatialObject->GetSizeZ());
    }
} 

/** Print Self function */
void BoxObjectRepresentation::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if( this->m_BoxSource )
    {
    os << indent << this->m_BoxSource << std::endl;
    }
}


/** Update the visual representation in response to changes in the geometric
 * object */
void BoxObjectRepresentation::UpdateRepresentationProcessing()
{
  m_BoxSource->SetXLength(m_BoxSpatialObject->GetSizeX());
  m_BoxSource->SetYLength(m_BoxSpatialObject->GetSizeY());
  m_BoxSource->SetZLength(m_BoxSpatialObject->GetSizeZ());
}


/** Create the vtk Actors */
void BoxObjectRepresentation::CreateActors()
{
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();
   
  vtkPolyDataMapper *boxMapper = vtkPolyDataMapper::New();
  vtkActor* boxActor = vtkActor::New();

  boxActor->GetProperty()->SetColor(this->GetRed(),
                                         this->GetGreen(),
                                         this->GetBlue()); 
        
  boxActor->GetProperty()->SetOpacity(this->GetOpacity()); 
  boxMapper->SetInput(m_BoxSource->GetOutput());
  boxActor->SetMapper( boxMapper );

  // We should check if the actor doesn't exist
  this->AddActor( boxActor );
  boxMapper->Delete();
}

/** Create a copy of the current object representation */
BoxObjectRepresentation::Pointer
BoxObjectRepresentation::Copy() const
{
  Pointer newOR = BoxObjectRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetBoxObject(m_BoxSpatialObject);

  return newOR;
}

} // end namespace igstk

