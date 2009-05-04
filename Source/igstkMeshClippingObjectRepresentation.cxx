/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMeshClippingObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkMeshClippingObjectRepresentation_cxx
#define __igstkMeshClippingObjectRepresentation_cxx

#include "igstkMeshClippingObjectRepresentation.h"
#include "igstkEvents.h"

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkIdList.h>
#include <vtkPolyDataMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPlane.h>
#include <vtkClipPolyData.h>
#include <vtkProperty.h>


namespace igstk
{ 

/** Constructor */

MeshClippingObjectRepresentation
::MeshClippingObjectRepresentation():m_StateMachine(this)
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

  m_LineWidth = 1.0;

  m_Clipper = vtkClipPolyData::New();
  m_ReslicerPlaneCenterObserver = ReslicerPlaneCenterObserver::New();
  m_ReslicerPlaneNormalObserver = ReslicerPlaneNormalObserver::New();
  
  m_ClipProperty = vtkProperty::New();
  m_ClipProperty->SetAmbient(1);
  //m_ClipProperty->SetRepresentationToWireframe();
  m_ClipProperty->SetInterpolationToFlat();
  m_ClipProperty->SetLineWidth(m_LineWidth);

} 

/** Destructor */
MeshClippingObjectRepresentation
::~MeshClippingObjectRepresentation()  
{
  // This must be called in order to avoid memory leaks.
  this->DeleteActors();
  
  m_Plane->Delete();
  m_Clipper->Delete();
  m_ClipProperty->Delete();
}


/** Set the Meshal Spatial Object */
void 
MeshClippingObjectRepresentation
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
void 
MeshClippingObjectRepresentation
::NoProcessing()
{
}


/** Set the Cylindrical Spatial Object */
void 
MeshClippingObjectRepresentation
::SetMeshObjectProcessing()
{
  // We create the ellipse spatial object
  m_MeshObject = m_MeshObjectToBeSet;
  this->RequestSetSpatialObject( m_MeshObject );
} 

void 
MeshClippingObjectRepresentation
::RequestSetReslicePlaneSpatialObject( const ReslicerPlaneType * reslicerPlane )
{  
  igstkLogMacro( DEBUG,"igstk::MeshResliceObjectRepresentation\
                       ::RequestSetReslicePlaneSpatialObject called...\n");

  m_ReslicePlaneSpatialObjectToBeSet = 
                               const_cast< ReslicerPlaneType* >(reslicerPlane);

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

void 
MeshClippingObjectRepresentation
::SetReslicePlaneSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::MeshResliceObjectRepresentation\
                       ::SetReslicePlaneSpatialObjectProcessing called...\n");

  m_ReslicePlaneSpatialObject = m_ReslicePlaneSpatialObjectToBeSet;

  m_ReslicePlaneSpatialObject->AddObserver( ReslicerPlaneType::ReslicerPlaneCenterEvent(),
                                            m_ReslicerPlaneCenterObserver );

  m_ReslicePlaneSpatialObject->AddObserver( ReslicerPlaneType::ReslicerPlaneNormalEvent(),
                                            m_ReslicerPlaneNormalObserver );

  m_ReslicePlaneSpatialObject->RequestComputeReslicingPlane();  
}

/** Print Self function */

void MeshClippingObjectRepresentation
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


/** Update the visual representation in response to changes in the geometric
 *  object */

void MeshClippingObjectRepresentation
::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "UpdateRepresentationProcessing called ....\n");

  m_ReslicePlaneSpatialObject->RequestComputeReslicingPlane();

  m_ReslicerPlaneCenterObserver->Reset();
  m_ReslicerPlaneNormalObserver->Reset();
  m_ReslicePlaneSpatialObject->RequestGetReslicingPlaneParameters();
  
  ReslicerPlaneType::VectorType center;
  if( m_ReslicerPlaneCenterObserver->GotReslicerPlaneCenter() )
    {
      center = m_ReslicerPlaneCenterObserver->GetReslicerPlaneCenter();     
    }
  else
    return;

  ReslicerPlaneType::VectorType normal;
  if( m_ReslicerPlaneNormalObserver->GotReslicerPlaneNormal() )
    {
      normal = m_ReslicerPlaneNormalObserver->GetReslicerPlaneNormal();
    }
  else
    return;

  m_Plane->SetNormal( normal[0], normal[1], normal[2] );
  m_Plane->SetOrigin( center[0], center[1], center[2] );
}

/** Create the vtk Actors */

void MeshClippingObjectRepresentation
::CreateActors()
{
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  vtkPolyData* polyData = vtkPolyData::New();
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
        igstkLogMacro( CRITICAL, "MeshResliceObjectRepresentation: "
            << "Don't know how to represent cells of size "
            << ptsSize << " \n" );
      }
    pts->Delete();
    }

  polyData->SetPoints(polyPoints);

  m_Clipper->SetInput(polyData);
  m_Clipper->SetClipFunction(m_Plane);
  m_Clipper->InsideOutOn();

  vtkActor* clipActor = vtkActor::New();
  vtkPolyDataMapper* clipMapper = vtkPolyDataMapper::New();
  //clipMapper->SetResolveCoincidentTopologyToPolygonOffset();
  //clipMapper->SetResolveCoincidentTopologyPolygonOffsetParameters(10,10);

  m_ClipProperty->SetColor(this->GetRed(),
                              this->GetGreen(),
                              this->GetBlue());

  //m_ClipProperty->SetRepresentationToWireframe();
  clipMapper->SetInput(m_Clipper->GetOutput());
  clipActor->SetMapper(clipMapper);
  clipActor->SetProperty(m_ClipProperty);

  clipMapper->Delete();

  this->AddActor( clipActor );

  polyPoints->Delete();
  polyData->Delete();

}

/** Create a copy of the current object representation */

MeshClippingObjectRepresentation::Pointer
MeshClippingObjectRepresentation::Copy() const
{
  Pointer newOR = MeshClippingObjectRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->SetLineWidth(this->GetLineWidth());
  newOR->RequestSetMeshObject(m_MeshObject);
  newOR->RequestSetReslicePlaneSpatialObject(m_ReslicePlaneSpatialObject);

  return newOR;
}

/** Set the line width */

void MeshClippingObjectRepresentation
::SetLineWidth(double width)
{
  if( this->m_LineWidth == width )
    {
    return;
    }
  this->m_LineWidth = width;

  m_ClipProperty->SetLineWidth(m_LineWidth);
}

/** Set the actor´s visibility */

void MeshClippingObjectRepresentation
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

/** Verify time stamp of the attached tool*/

bool
MeshClippingObjectRepresentation
::VerifyTimeStamp( ) const
{
  igstkLogMacro( DEBUG, 
    "igstk::MeshResliceObjectRepresentation::VerifyTimeStamp called...\n");

  if( this->m_ReslicePlaneSpatialObject.IsNull() )
    {
    return false;
    }

  /* if a tool spatial object is driving the reslicing, compare the 
     tool spatial object transform with the view render time*/
  if( this->m_ReslicePlaneSpatialObject->IsToolSpatialObjectSet())
    {
    if( this->GetRenderTimeStamp().GetExpirationTime() <
      this->m_ReslicePlaneSpatialObject->GetToolTransform().GetStartTime() ||
      this->GetRenderTimeStamp().GetStartTime() >
      this->m_ReslicePlaneSpatialObject->GetToolTransform().GetExpirationTime() )
      {
        // fixme
        double diff = 
          this->GetRenderTimeStamp().GetStartTime() - this->m_ReslicePlaneSpatialObject->GetToolTransform().GetExpirationTime();

        if (diff > 250 )
        {
          //std::cout << diff << std::endl;
          return false;
        }
        else
          return true;
      }
    else
      {
      return true;
      }
    }
  else
    {
    return true;
    }
}

} // end namespace igstk


#endif

