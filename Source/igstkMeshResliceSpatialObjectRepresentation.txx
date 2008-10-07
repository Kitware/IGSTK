/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMeshResliceSpatialObjectRepresentation.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkMeshResliceSpatialObjectRepresentation.h"
#include "igstkEvents.h"

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkIdList.h>
#include <vtkDataSetMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPlane.h>
#include <vtkCutter.h>
#include <vtkTubeFilter.h>


namespace igstk
{ 

/** Constructor */
template < class TImageSpatialObject >
MeshResliceSpatialObjectRepresentation< TImageSpatialObject >
::MeshResliceSpatialObjectRepresentation():m_StateMachine(this)
{
  // We create the ellipse spatial object
  m_MeshObject = NULL;
  
  this->RequestSetSpatialObject( m_MeshObject );
  
  igstkAddInputMacro( ValidMeshObject );
  igstkAddInputMacro( NullMeshObject );

  igstkAddInputMacro( ValidReslicePlaneSpatialObject );
  igstkAddInputMacro( InValidReslicePlaneSpatialObject );

  igstkAddStateMacro( NullMeshObject );
  igstkAddStateMacro( ValidMeshObject );
  igstkAddStateMacro( ValidReslicePlaneSpatialObject );

  // from NullMeshObject 

  igstkAddTransitionMacro( NullMeshObject, ValidMeshObject, 
                           ValidMeshObject,  SetMeshObject );

  igstkAddTransitionMacro( NullMeshObject, NullMeshObject, 
                           NullMeshObject,  No );

  igstkAddTransitionMacro( NullMeshObject, ValidReslicePlaneSpatialObject, 
                           NullMeshObject,  No );

  igstkAddTransitionMacro( NullMeshObject, InValidReslicePlaneSpatialObject, 
                           NullMeshObject,  No );

  // from ValidMeshObject 

  igstkAddTransitionMacro( ValidMeshObject, ValidReslicePlaneSpatialObject, 
                           ValidReslicePlaneSpatialObject,  SetReslicePlaneSpatialObject );

  igstkAddTransitionMacro( ValidMeshObject, InValidReslicePlaneSpatialObject, 
                           ValidMeshObject,  No );

  igstkAddTransitionMacro( ValidMeshObject, NullMeshObject, 
                           NullMeshObject,  No ); 

  igstkAddTransitionMacro( ValidMeshObject, ValidMeshObject, 
                           ValidMeshObject,  No );

  // from ValidReslicePlaneSpatialObject 

  igstkAddTransitionMacro( ValidReslicePlaneSpatialObject, ValidReslicePlaneSpatialObject, 
                           ValidReslicePlaneSpatialObject,  No );

  igstkAddTransitionMacro( ValidReslicePlaneSpatialObject, InValidReslicePlaneSpatialObject, 
                           ValidReslicePlaneSpatialObject,  No );

  igstkAddTransitionMacro( ValidReslicePlaneSpatialObject, NullMeshObject, 
                           ValidReslicePlaneSpatialObject,  No ); 

  igstkAddTransitionMacro( ValidReslicePlaneSpatialObject, ValidMeshObject, 
                           ValidReslicePlaneSpatialObject,  No );


  igstkSetInitialStateMacro( NullMeshObject );

  m_StateMachine.SetReadyToRun();

  m_Plane = vtkPlane::New();
  m_Plane->SetOrigin(0,0,0);
  m_Plane->SetNormal(1,0,0);

  m_Cutter = vtkCutter::New();
} 

/** Destructor */
template < class TImageSpatialObject >
MeshResliceSpatialObjectRepresentation< TImageSpatialObject >
::~MeshResliceSpatialObjectRepresentation()  
{
  // This must be called in order to avoid memory leaks.
  this->DeleteActors();
  
  if ( m_Plane )
    {
    m_Plane->Delete();
    m_Plane = NULL;
    }

  if ( m_Cutter )
    {
    m_Cutter->Delete();
    m_Cutter = NULL;
    }
}


/** Set the Meshal Spatial Object */
template < class TImageSpatialObject >
void MeshResliceSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetMeshObject( const MeshObjectType * Mesh )
{
  m_MeshObjectToBeSet = Mesh;
  if( !m_MeshObjectToBeSet )
    {
    igstkPushInputMacro( NullMeshObject );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidMeshObject );
    m_StateMachine.ProcessInputs();
    }
}


/** Null operation for a State Machine transition */
template < class TImageSpatialObject >
void MeshResliceSpatialObjectRepresentation< TImageSpatialObject >
::NoProcessing()
{
}


/** Set the Cylindrical Spatial Object */
template < class TImageSpatialObject >
void MeshResliceSpatialObjectRepresentation< TImageSpatialObject >
::SetMeshObjectProcessing()
{
  // We create the ellipse spatial object
  m_MeshObject = m_MeshObjectToBeSet;
  this->RequestSetSpatialObject( m_MeshObject );
} 

template < class TImageSpatialObject >
void 
MeshResliceSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetReslicePlaneSpatialObject( const ReslicePlaneSpatialObjectType *
reslicePlaneSpatialObject )
{  
  igstkLogMacro( DEBUG,"igstk::MeshResliceSpatialObjectRepresentation\
                       ::RequestSetReslicePlaneSpatialObject called...\n");

  m_ReslicePlaneSpatialObjectToBeSet = const_cast< ReslicePlaneSpatialObjectType
*>(reslicePlaneSpatialObject);

  if( !m_ReslicePlaneSpatialObjectToBeSet )
    {
    m_StateMachine.PushInput( m_InValidReslicePlaneSpatialObjectInput );
    }
  else
    {
    m_StateMachine.PushInput( m_ValidReslicePlaneSpatialObjectInput );
    }

  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
MeshResliceSpatialObjectRepresentation< TImageSpatialObject >
::SetReslicePlaneSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::MeshResliceSpatialObjectRepresentation\
                       ::SetReslicePlaneSpatialObjectProcessing called...\n");

  m_ReslicePlaneSpatialObject = m_ReslicePlaneSpatialObjectToBeSet;

  m_ReslicePlaneSpatialObject->RequestComputeReslicingPlane();  
}

/** Print Self function */
template < class TImageSpatialObject >
void MeshResliceSpatialObjectRepresentation< TImageSpatialObject >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


/** Update the visual representation in response to changes in the geometric
 *  object */
template < class TImageSpatialObject >
void MeshResliceSpatialObjectRepresentation< TImageSpatialObject >
::UpdateRepresentationProcessing()
{
igstkLogMacro( DEBUG, "UpdateRepresentationProcessing called ....\n");

  m_ReslicePlaneSpatialObject->RequestComputeReslicingPlane();


  vtkPlaneSource* plane = m_ReslicePlaneSpatialObject->GetReslicingPlane();

  double *normal = plane->GetNormal();

  double *center = plane->GetCenter();

  m_Plane->SetNormal( normal[0], normal[1], normal[2] );
  m_Plane->SetOrigin( center[0], center[1], center[2] );

}

/** Create the vtk Actors */
template < class TImageSpatialObject >
void MeshResliceSpatialObjectRepresentation< TImageSpatialObject >
::CreateActors()
{
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  vtkUnstructuredGrid* polyData = vtkUnstructuredGrid::New();
  vtkPoints* polyPoints = vtkPoints::New();

  MeshObjectType::PointsContainerPointer points = m_MeshObject->GetPoints();
  MeshObjectType::PointsContainer::const_iterator it = points->begin();

  unsigned long nPoints = points->size();

  polyPoints->SetNumberOfPoints(nPoints);

  for(;it != points->end();it++)
    {
    polyPoints->SetPoint((*it).first,
                        (*it).second[0],(*it).second[1],(*it).second[2]);
    }

  MeshObjectType::CellsContainer* cells = m_MeshObject->GetCells();
  MeshObjectType::CellsContainer::ConstIterator it_cells = cells->Begin();
  unsigned long nCells = cells->size();
    
  polyData->Allocate(nCells);
    
  for(;it_cells != cells->End();it_cells++)
    {
    vtkIdList* pts = vtkIdList::New();
    MeshObjectType::CellTraits::PointIdConstIterator itptids 
                                      = (*it_cells)->Value()->GetPointIds();
    unsigned int id =0;
    const unsigned long ptsSize = (*it_cells)->Value()->GetNumberOfPoints();
    pts->SetNumberOfIds(ptsSize);
    while(itptids != (*it_cells)->Value()->PointIdsEnd())
      {
      pts->SetId(id,*itptids);
      itptids++;
      id++;
      }

    switch( ptsSize )
      {
      case 2: 
        polyData->InsertNextCell( VTK_LINE, pts );
        break;
      case 3: 
        polyData->InsertNextCell( VTK_TRIANGLE, pts );
        break;
      case 4: 
        polyData->InsertNextCell( VTK_TETRA, pts );
        break;
      default:
        igstkLogMacro( CRITICAL, "MeshResliceSpatialObjectRepresentation: "
            << "Don't know how to represent cells of size "
            << ptsSize << " \n" );
      }
    pts->Delete();
    }
  
  vtkDataSetMapper *pointMapper = vtkDataSetMapper::New();
  m_Cutter->SetInput(polyData);
  m_Cutter->SetCutFunction(m_Plane);

  vtkActor* meshActor = vtkActor::New();

  meshActor->GetProperty()->SetColor(this->GetRed(),
                                     this->GetGreen(),
                                     this->GetBlue());
  polyData->SetPoints(polyPoints);

  m_Tuber = vtkTubeFilter::New();
  m_Tuber->SetInput ( m_Cutter->GetOutput() );
  m_Tuber->SetRadius (0.5);
  m_Tuber->SetNumberOfSides(5);

  pointMapper->SetInput(m_Tuber->GetOutput());

  meshActor->SetMapper(pointMapper);

  this->AddActor( meshActor );

  polyPoints->Delete();
  polyData->Delete();
  pointMapper->Delete();
}

/** Create a copy of the current object representation */
template < class TImageSpatialObject >
typename MeshResliceSpatialObjectRepresentation< TImageSpatialObject >::Pointer
MeshResliceSpatialObjectRepresentation< TImageSpatialObject >::Copy() const
{
  Pointer newOR = MeshResliceSpatialObjectRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->SetLineWidth(this->GetLineWidth());
  newOR->RequestSetMeshObject(m_MeshObject);

  return newOR;
}

/** Set the line width */
template < class TImageSpatialObject >
void MeshResliceSpatialObjectRepresentation< TImageSpatialObject >
::SetLineWidth(LineWidthType width)
{
  if( this->m_LineWidth == width )
    {
    return;
    }
  this->m_LineWidth = width;

  // Update all the actors
  ActorsListType::iterator it = this->m_Actors.begin();
  while(it != this->m_Actors.end())
    {
    vtkActor * va = dynamic_cast< vtkActor * >( *it );
    if( va )
      {
      va->GetProperty()->SetLineWidth(this->m_LineWidth);
      }
    it++;
    }
}

/** Set the actor�s visibility */
template < class TImageSpatialObject >
void MeshResliceSpatialObjectRepresentation< TImageSpatialObject >
::SetVisibility(bool visibility)
{
  // Update all the actors
  ActorsListType::iterator it = this->m_Actors.begin();
  while(it != this->m_Actors.end())
    {
    vtkActor * va = dynamic_cast< vtkActor * >( *it );
    if( va )
      {
      va->SetVisibility(visibility);
      }
    it++;
    }
}

template < class TImageSpatialObject >
void 
MeshResliceSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetPlaneNormal(Transform::VectorType &normal)
{
  m_Plane->SetNormal( normal[0], normal[1], normal[2] );
}

template < class TImageSpatialObject >
void 
MeshResliceSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetPlaneCenter(Transform::VectorType &center)
{
  m_Plane->SetOrigin( center[0], center[1], center[2] );
}


} // end namespace igstk
