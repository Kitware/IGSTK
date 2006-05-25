/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkObliqueContourMeshObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkObliqueContourMeshObjectRepresentation.h"
#include "igstkEvents.h"

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkIdList.h>
#include <vtkDataSetMapper.h>
#include <vtkUnstructuredGrid.h>


namespace igstk
{ 

/** Constructor */
ObliqueContourMeshObjectRepresentation
::ObliqueContourMeshObjectRepresentation():m_StateMachine(this)
{
  // We create the ellipse spatial object
  m_MeshObject = NULL;

  this->RequestSetSpatialObject( m_MeshObject );
  
  igstkAddInputMacro( ValidMeshObject );
  igstkAddInputMacro( NullMeshObject );
  igstkAddInputMacro( ValidOriginPointOnThePlane );
  igstkAddInputMacro( InValidOriginPointOnThePlane  );
  igstkAddInputMacro( ValidVector1OnThePlane );
  igstkAddInputMacro( InValidVector1OnThePlane  );
  igstkAddInputMacro( ValidVector2OnThePlane );
  igstkAddInputMacro( InValidVector2OnThePlane  );
  igstkAddInputMacro( Reslice );
   
  igstkAddStateMacro( NullOriginPointOnThePlane );
  igstkAddStateMacro( ValidOriginPointOnThePlane );
  igstkAddStateMacro( NullVector1OnthePlane );
  igstkAddStateMacro( ValidVector1OnThePlane );
  igstkAddStateMacro( NullVector2OnthePlane );
  igstkAddStateMacro( ValidVector2OnThePlane );
  igstkAddStateMacro( NullMeshObject );
  igstkAddStateMacro( ValidMeshObject );

  // NullMeshObject 
  igstkAddTransitionMacro( NullMeshObject, NullMeshObject, 
                           NullMeshObject,  No );
  igstkAddTransitionMacro( NullMeshObject, ValidMeshObject, 
                           ValidMeshObject,  SetMeshObject );

  // ValidMeshObject 
  igstkAddTransitionMacro( ValidMeshObject, NullMeshObject, 
                           NullMeshObject,  No ); 
  igstkAddTransitionMacro( ValidMeshObject, ValidMeshObject, 
                           ValidMeshObject,  No );
  igstkAddTransitionMacro( ValidMeshObject, ValidMeshObject, 
                           ValidMeshObject,  No );

  igstkAddTransitionMacro( NullOriginPointOnThePlane, 
                           InValidOriginPointOnThePlane, 
                           NullOriginPointOnThePlane,  No );
  igstkAddTransitionMacro( NullOriginPointOnThePlane, 
                           ValidOriginPointOnThePlane,
                           ValidOriginPointOnThePlane, 
                           SetOriginPointOnThePlane );

  igstkAddTransitionMacro( NullMeshObject, 
                           InValidOriginPointOnThePlane, 
                           NullMeshObject, No );
  igstkAddTransitionMacro( ValidMeshObject, 
                           InValidOriginPointOnThePlane, 
                           ValidMeshObject, No );

  igstkAddTransitionMacro( NullMeshObject, ValidOriginPointOnThePlane, 
                           ValidOriginPointOnThePlane,
                           SetOriginPointOnThePlane );

  igstkAddTransitionMacro( ValidMeshObject,ValidOriginPointOnThePlane,
                           ValidOriginPointOnThePlane, 
                           SetOriginPointOnThePlane );

  igstkAddTransitionMacro( ValidOriginPointOnThePlane, ValidVector1OnThePlane, 
                           ValidVector1OnThePlane, SetVector1OnThePlane );

  igstkAddTransitionMacro( ValidVector1OnThePlane, ValidVector2OnThePlane,
                           ValidVector2OnThePlane, SetVector2OnThePlane );

  igstkAddTransitionMacro( ValidVector2OnThePlane, Reslice, 
                           ValidMeshObject , Reslice);
  igstkAddTransitionMacro( ValidMeshObject, Reslice, 
                           ValidMeshObject , Reslice);

  igstkAddTransitionMacro( ValidMeshObject, NullMeshObject, 
                           NullMeshObject,  No ); 
  igstkAddTransitionMacro( ValidMeshObject, ValidMeshObject, 
                           ValidMeshObject,  No ); 

  igstkSetInitialStateMacro( NullMeshObject );

  m_StateMachine.SetReadyToRun();

  m_Plane = vtkPlane::New();
  m_Cutter = vtkCutter::New();
} 

/** Destructor */
ObliqueContourMeshObjectRepresentation::
~ObliqueContourMeshObjectRepresentation()  
{
  // This must be called in order to avoid memory leaks.
  this->DeleteActors();
  m_Plane->Delete();
  m_Cutter->Delete();
}


/** Set the Meshal Spatial Object */
void ObliqueContourMeshObjectRepresentation
::RequestSetMeshObject( const MeshObjectType * Mesh )
{
  m_MeshObjectToAdd = Mesh;
  if( !m_MeshObjectToAdd )
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

/** Set the origin point on the Oblique plane */
void 
ObliqueContourMeshObjectRepresentation
::RequestSetOriginPointOnThePlane( const PointType & point )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueContourMeshObjectRepresentation\
                        ::RequestSetOriginPointOnThePlane called...\n");
  
  m_OriginPointOnThePlaneToBeSet = point;

  // check if it is a valid point
  m_StateMachine.PushInput( m_ValidOriginPointOnThePlaneInput );
  m_StateMachine.ProcessInputs();
}

void 
ObliqueContourMeshObjectRepresentation
::SetOriginPointOnThePlaneProcessing( )
{
  igstkLogMacro( DEBUG, 
       "igstk::ObliqueContourMeshObjectRepresentation\
       ::SetOriginPointOnThePlaneProcessing called...\n");
  
  m_OriginPointOnThePlane = m_OriginPointOnThePlaneToBeSet;
}

/** Set vector 1 on the Oblique plane */
void 
ObliqueContourMeshObjectRepresentation
::RequestSetVector1OnThePlane( const VectorType & vector )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueContourMeshObjectRepresentation::\
                                 RequestSetVector1OnThePlane called...\n");
  
  m_Vector1OnThePlaneToBeSet = vector;

  // check if it is a valid vector
  m_StateMachine.PushInput( m_ValidVector1OnThePlaneInput );
  m_StateMachine.ProcessInputs();
}

/** Set vector 2 on the Oblique plane */
void 
ObliqueContourMeshObjectRepresentation
::RequestSetVector2OnThePlane( const VectorType & vector )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueContourMeshObjectRepresentation::\
                                 RequestSetVector2OnThePlane called...\n");
  
  m_Vector2OnThePlaneToBeSet = vector;

  // check if it is a valid vector
  m_StateMachine.PushInput( m_ValidVector2OnThePlaneInput );
  m_StateMachine.ProcessInputs();
}

void 
ObliqueContourMeshObjectRepresentation
::SetVector1OnThePlaneProcessing( )
{
  igstkLogMacro( DEBUG, 
       "igstk::ObliqueContourMeshObjectRepresentation\
       ::SetVector1OnThePlaneProcessing called...\n");
  
  m_Vector1OnThePlane = m_Vector1OnThePlaneToBeSet;
}

void 
ObliqueContourMeshObjectRepresentation
::SetVector2OnThePlaneProcessing( )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueContourMeshObjectRepresentation\
                        ::SetVector2OnThePlaneProcessing called...\n");
  
  m_Vector2OnThePlane = m_Vector2OnThePlaneToBeSet;
}

/** Request reslicing */
void 
ObliqueContourMeshObjectRepresentation
::RequestReslice(  )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueContourMeshObjectRepresentation\
                         ::RequestReslice called...\n");
  
  m_StateMachine.PushInput( m_ResliceInput );
  m_StateMachine.ProcessInputs();
}

/** Reslice processing */
void ObliqueContourMeshObjectRepresentation::ResliceProcessing()
{
  m_Plane->SetOrigin(m_OriginPointOnThePlane[0],
                     m_OriginPointOnThePlane[1],
                     m_OriginPointOnThePlane[2]);

  VectorType v3 = itk::CrossProduct(m_Vector1OnThePlane,m_Vector2OnThePlane);

  m_Plane->SetNormal(v3[0],v3[1],v3[2]);

  m_Cutter->Update();
}


/** Null operation for a State Machine transition */
void ObliqueContourMeshObjectRepresentation::NoProcessing()
{
}


/** Set the Cylindrical Spatial Object */
void ObliqueContourMeshObjectRepresentation::SetMeshObjectProcessing()
{
  // We create the ellipse spatial object
  m_MeshObject = m_MeshObjectToAdd;
  this->RequestSetSpatialObject( m_MeshObject );
} 


/** Print Self function */
void ObliqueContourMeshObjectRepresentation
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


/** Update the visual representation in response to changes in the geometric
 *  object */
void ObliqueContourMeshObjectRepresentation::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "UpdateRepresentationProcessing called ....\n");
  this->RequestVerifyTimeStamp();
}

/** Create the vtk Actors */
void ObliqueContourMeshObjectRepresentation::CreateActors()
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
        igstkLogMacro( CRITICAL, "ObliqueContourMeshObjectRepresentation: "
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
  pointMapper->SetInput(m_Cutter->GetOutput());

  meshActor->SetMapper(pointMapper);

  this->AddActor( meshActor );

  polyPoints->Delete();
  polyData->Delete();
  pointMapper->Delete();
}

/** Create a copy of the current object representation */
ObliqueContourMeshObjectRepresentation::Pointer
ObliqueContourMeshObjectRepresentation::Copy() const
{
  Pointer newOR = ObliqueContourMeshObjectRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetMeshObject(m_MeshObject);

  return newOR;
}

} // end namespace igstk
