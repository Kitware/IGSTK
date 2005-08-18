/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMeshObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkMeshObjectRepresentation.h"
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include "igstkEvents.h"

#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkIdList.h>
#include <vtkDataSetMapper.h>
#include <vtkUnstructuredGrid.h>

namespace igstk
{ 

/** Constructor */
MeshObjectRepresentation::MeshObjectRepresentation():m_StateMachine(this)
{
  // We create the ellipse spatial object
  m_MeshObject = NULL;
  this->RequestSetSpatialObject( m_MeshObject );
  
  m_StateMachine.AddInput( m_ValidMeshObjectInput,  "ValidMeshObjectInput" );
  m_StateMachine.AddInput( m_NullMeshObjectInput,   "NullMeshObjectInput"  );

  m_StateMachine.AddState( m_NullMeshObjectState,  "NullMeshObjectState"     );
  m_StateMachine.AddState( m_ValidMeshObjectState, "ValidMeshObjectState"     );

  const ActionType NoAction = 0;

  m_StateMachine.AddTransition( m_NullMeshObjectState, m_NullMeshObjectInput, m_NullMeshObjectState,  NoAction );
  m_StateMachine.AddTransition( m_NullMeshObjectState, m_ValidMeshObjectInput, m_ValidMeshObjectState,  & MeshObjectRepresentation::SetMeshObject );
  m_StateMachine.AddTransition( m_ValidMeshObjectState, m_NullMeshObjectInput, m_NullMeshObjectState,  NoAction ); 
  m_StateMachine.AddTransition( m_ValidMeshObjectState, m_ValidMeshObjectInput, m_ValidMeshObjectState,  NoAction ); 

  m_StateMachine.SelectInitialState( m_NullMeshObjectState );

  m_StateMachine.SetReadyToRun();


} 

/** Destructor */
MeshObjectRepresentation::~MeshObjectRepresentation()  
{
}


/** Set the Meshal Spatial Object */
void MeshObjectRepresentation::RequestSetMeshObject( const MeshObjectType * Mesh )
{
  m_MeshObjectToAdd = Mesh;
  if( !m_MeshObjectToAdd )
    {
    m_StateMachine.PushInput( m_NullMeshObjectInput );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    m_StateMachine.PushInput( m_ValidMeshObjectInput );
    m_StateMachine.ProcessInputs();
    }
}


/** Set the Cylindrical Spatial Object */
void MeshObjectRepresentation::SetMeshObject()
{
  // We create the ellipse spatial object
  m_MeshObject = m_MeshObjectToAdd;
  this->RequestSetSpatialObject( m_MeshObject );
} 


/** Print Self function */
void MeshObjectRepresentation::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


/** Update the visual representation in response to changes in the geometric
 *  object */
void MeshObjectRepresentation::UpdateRepresentation()
{
}

/** Create the vtk Actors */
void MeshObjectRepresentation::CreateActors()
{
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  vtkUnstructuredGrid* polyData = vtkUnstructuredGrid::New();
  vtkPoints* polyPoints = vtkPoints::New();

  MeshObjectType::PointsContainerPointer points = m_MeshObject->GetPoints();
  MeshObjectType::PointsContainer::const_iterator it = points->begin();

  unsigned long nPoints = points->size();
  unsigned long id = 0;
  polyPoints->SetNumberOfPoints(nPoints);

  for(;it != points->end();it++)
    {
    polyPoints->SetPoint((*it).first,(*it).second[0],(*it).second[1],(*it).second[2]);
    }    

  MeshObjectType::CellsContainer* cells = m_MeshObject->GetCells();
  MeshObjectType::CellsContainer::ConstIterator it_cells = cells->Begin();
  unsigned long nCells = cells->size();
    
  polyData->Allocate(nCells);
    
  for(;it_cells != cells->End();it_cells++)
    {
    vtkIdList* pts = vtkIdList::New();     
    MeshObjectType::CellTraits::PointIdConstIterator itptids = (*it_cells)->Value()->GetPointIds();
    unsigned int id =0;
    unsigned long ptsSize = (*it_cells)->Value()->GetNumberOfPoints();
    pts->SetNumberOfIds(ptsSize);
    while(itptids != (*it_cells)->Value()->PointIdsEnd())
      {
      pts->SetId(id,*itptids);
      itptids++;
      id++;
      }

    switch(ptsSize)
      {
      case 2: polyData->InsertNextCell(VTK_LINE,pts);
      break;
      case 3: polyData->InsertNextCell(VTK_TRIANGLE,pts);
      break;
      case 4: polyData->InsertNextCell(VTK_TETRA,pts);
      break;
      default:
      std::cout << "MeshObjectRepresentation: Don't know how to represent cells of size " << ptsSize << std::endl;      
      }
    pts->Delete();
    }
  
  vtkDataSetMapper *pointMapper = vtkDataSetMapper::New();
  vtkActor* meshActor = vtkActor::New();

  meshActor->GetProperty()->SetColor(this->GetRed(),
                                     this->GetGreen(),
                                     this->GetBlue()); 
    
  polyData->SetPoints(polyPoints);
  pointMapper->SetInput(polyData);
  meshActor->SetMapper(pointMapper);
 
  this->AddActor( meshActor );

  polyPoints->Delete();
  polyData->Delete();
  pointMapper->Delete();
}

/** Create a copy of the current object representation */
MeshObjectRepresentation::Pointer
MeshObjectRepresentation::Copy() const
{
  Pointer newOR = MeshObjectRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetMeshObject(m_MeshObject);

  return newOR;
}


} // end namespace igstk

