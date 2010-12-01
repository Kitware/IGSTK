/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVascularNetworkObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkVascularNetworkObjectRepresentation.h"

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkFloatArray.h>
#include <vtkSphereSource.h>
#include <vtkPolyLine.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkCleanPolyData.h>
#include <vtkTubeFilter.h>

namespace igstk
{ 

/** Constructor */
VascularNetworkObjectRepresentation
::VascularNetworkObjectRepresentation():m_StateMachine(this)
{
  m_VascularNetworkObject = NULL;
  this->RequestSetSpatialObject( m_VascularNetworkObject );
  
  igstkAddInputMacro( ValidVascularNetworkObject );
  igstkAddInputMacro( NullVascularNetworkObject );
  igstkAddInputMacro( VesselReceived );
  igstkAddInputMacro( VesselNotFound );

  igstkAddStateMacro( NullVascularNetworkObject );
  igstkAddStateMacro( ValidVascularNetworkObject );
  igstkAddStateMacro( AttemptingToGetVessel );

  // FIXME : Include Vessel dialog in the Transitions
  igstkAddTransitionMacro( NullVascularNetworkObject, 
                           NullVascularNetworkObject, 
                           NullVascularNetworkObject,  
                           No );
  igstkAddTransitionMacro( NullVascularNetworkObject, 
                           ValidVascularNetworkObject, 
                           ValidVascularNetworkObject,  
                           SetVascularNetworkObject );
  igstkAddTransitionMacro( ValidVascularNetworkObject,
                           NullVascularNetworkObject,
                           NullVascularNetworkObject,
                           No ); 
  igstkAddTransitionMacro( ValidVascularNetworkObject,
                           ValidVascularNetworkObject, 
                           ValidVascularNetworkObject,
                           No ); 
  igstkSetInitialStateMacro( NullVascularNetworkObject );

  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
VascularNetworkObjectRepresentation::~VascularNetworkObjectRepresentation()  
{
  // This must be called in order to avoid Memory Leaks.
  this->DeleteActors();
}

/** Set the VasvualNetwork Spatial Object */
void VascularNetworkObjectRepresentation
::RequestSetVascularNetworkObject( const VascularNetworkObjectType * network )
{
  m_VascularNetworkObjectToAdd = network;
  if( !m_VascularNetworkObjectToAdd )
    {
    igstkPushInputMacro( NullVascularNetworkObject );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidVascularNetworkObject );
    m_StateMachine.ProcessInputs();
    }
}

/** Null operation for a State Machine transition */
void VascularNetworkObjectRepresentation::NoProcessing()
{
}

/** Set the Vascular Network Object */
void VascularNetworkObjectRepresentation
::SetVascularNetworkObjectProcessing()
{
  // We create the vascular network spatial object
  m_VascularNetworkObject = m_VascularNetworkObjectToAdd;

  // Connect the method that enables the observer in the 
  // transduction macro
  //this->ObserveVesselReceivedInput( m_VascularNetworkObject );

  this->RequestSetSpatialObject( m_VascularNetworkObject );
} 

/** Print Self function */
void VascularNetworkObjectRepresentation
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << "VascularNetworkObject " << m_VascularNetworkObject.GetPointer() 
     << std::endl;
  Superclass::PrintSelf(os, indent);
}

/** Update the visual representation in response to changes in the geometric
 * object */
void VascularNetworkObjectRepresentation::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "UpdateRepresentationProcessing called ....\n");
}

/** Create the vtk Actors */
void VascularNetworkObjectRepresentation::CreateActors()
{
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  unsigned int nTubes = m_VascularNetworkObject->GetNumberOfVessels();

  for(unsigned int ntube = 0; ntube < nTubes; ntube++)
    {
    VascularNetworkObjectType * network =
      const_cast< VascularNetworkObjectType * >( 
        m_VascularNetworkObject.GetPointer() );

    VesselObserver::Pointer vesselObserver = VesselObserver::New();

    network->AddObserver(
            igstk::VesselObjectModifiedEvent(),
            vesselObserver);

    network->RequestGetVessel(ntube);
    
    if(vesselObserver->GotVessel())
      {
      m_VesselToBeSet = vesselObserver->GetVessel();
      this->CreateActorsForOneVesselProcessing();
      }
    }

}

/** Create the vtk Actors for one vessel */
void VascularNetworkObjectRepresentation::CreateActorsForOneVesselProcessing()
{
  const VesselObjectType * vessel = m_VesselToBeSet;

  if(vessel->GetNumberOfPoints() < 2)
    {
    igstkLogMacro( CRITICAL, "Not enough points to render a tube.\n" );
    return;
    }
      
  double spacing[3];
  spacing[0] = vessel->GetSpacing()[0];
  spacing[1] = vessel->GetSpacing()[1];
  spacing[2] = vessel->GetSpacing()[2];
  
  //Step 1: copy skeleton points from a vessel into vtkPoints
  //vtkPoints assumes a triplet is coming so use pointer arithmetic
  //to jump to the next spot in a multidimensional array
  unsigned int nPoints = vessel->GetNumberOfPoints();

  vtkPoints* vPoints = vtkPoints::New();
  vPoints->SetNumberOfPoints(nPoints);
  vtkFloatArray* vScalars = vtkFloatArray::New();
  vScalars->SetNumberOfTuples(nPoints);
  vtkFloatArray* vColorScalars = vtkFloatArray::New();
  vColorScalars->SetNumberOfTuples(nPoints);
  vtkFloatArray* vVectors = vtkFloatArray::New();
  vVectors->SetNumberOfTuples(3*nPoints);
  vVectors->SetNumberOfComponents(3);

  const VesselObjectType::PointType* pt = vessel->GetPoint(0); 
  vtkSphereSource * sphereSource1 = vtkSphereSource::New();
  sphereSource1->SetCenter((float)(pt->GetPosition()[0]*spacing[0]),
                           (float)(pt->GetPosition()[1]*spacing[1]),
                           (float)(pt->GetPosition()[2]*spacing[2]));
  sphereSource1->SetRadius(pt->GetRadius()*0.95*spacing[0]);
  vtkPolyDataMapper *sphereMapper1 = vtkPolyDataMapper::New();
  sphereMapper1->SetInput(sphereSource1->GetOutput());
  
  vtkActor* sphere1 = vtkActor::New();
  sphere1->SetMapper(sphereMapper1);
  
  sphere1->GetProperty()->SetColor(this->GetRed(),
                                  this->GetGreen(),
                                  this->GetBlue());
        
  sphere1->GetProperty()->SetOpacity(this->GetOpacity());
  this->AddActor( sphere1 );

  sphereMapper1->Delete();
  sphereSource1->Delete();

  for( unsigned int i=0; i < nPoints; i++ )
    {
    const VesselObjectType::PointType* pt = vessel->GetPoint(i); 
    vPoints->SetPoint(i, (float)(pt->GetPosition()[0]*spacing[0]),
                         (float)(pt->GetPosition()[1]*spacing[1]),
                         (float)(pt->GetPosition()[2]*spacing[2]));
    vScalars->SetTuple1(i,pt->GetRadius()*0.95*spacing[0]);
    vVectors->SetTuple3(i,pt->GetRadius()*0.95*spacing[0],0,0);
    }  

  pt = vessel->GetPoint(nPoints-1); 
  vtkSphereSource * sphereSource2 = vtkSphereSource::New();
  sphereSource2->SetCenter((float)(pt->GetPosition()[0]*spacing[0]),
                           (float)(pt->GetPosition()[1]*spacing[1]),
                           (float)(pt->GetPosition()[2]*spacing[2]));
  sphereSource2->SetRadius(pt->GetRadius()*0.95*spacing[0]);
  
  vtkPolyDataMapper *sphereMapper2 = vtkPolyDataMapper::New();
  sphereMapper2->SetInput(sphereSource2->GetOutput());
  
  vtkActor* sphere2 = vtkActor::New();
  sphere2->SetMapper(sphereMapper2);
 
  sphere2->GetProperty()->SetColor(this->GetRed(),
                                  this->GetGreen(),
                                  this->GetBlue());
  sphere2->GetProperty()->SetOpacity(this->GetOpacity());

  this->AddActor( sphere2 );

  sphereMapper2->Delete();
  sphereSource2->Delete();
  
  //Step 2: create a point id list (for a polyline this is just linear)
  vtkIdType* pntIds = new vtkIdType[nPoints];
  for (unsigned int j = 0; j < nPoints; j++)
    {
    pntIds[j] = j;
    }
        
  //Step3: create a polyline from the points and pt id list
  vtkPolyLine* vPLine = vtkPolyLine::New();
  vPLine->Initialize(nPoints, pntIds, vPoints);

  //Step 4: convert this to a cellarray (required for input to polydata)
  vtkCellArray* vCA = vtkCellArray::New();
  vCA->InsertNextCell(vPLine); 

  //Step 5: create a scalar array that indicates the radius at each
  //skeleton point. Vtk's way of setting radius is screwy: it fails if every
  //point has the same radius. It also uses a minimum radius  (called radius)
  //and a max radius (defined by scale factor). In order to get this to work, 
  //you need to find the min and max of your vessel radii--if the same, later
  //set a constant radius in the tube filter. If not the same, you need to 
  //define the min radius and the ratio max/min. If you send these params,
  //the tube will end up with proper radius settings. Weird.

  //Step 6: convert to polydata (required for input to anything else)
  vtkPolyData* vPData = vtkPolyData::New();
  vPData->SetLines(vCA);
  vPData->SetPoints(vPoints);
  vtkFloatingPointType range[2];
  bool use_scalars = false;
  float min_scalar, max_scalar;
  vScalars->GetRange(range);
  min_scalar = range[0];
  max_scalar = range[1];
  if (min_scalar <= 0.0001) 
    {
    min_scalar = 0.0001;
    }
  if(max_scalar < min_scalar) 
    {
    max_scalar = min_scalar;
    }
  
  use_scalars = true;
  vPData->GetPointData()->SetScalars(vScalars);
  vPData->GetPointData()->SetVectors(vVectors); 

  //Step 7: remove any duplicate points from polydata. The tube filter
  //fails if any duplicates are present
  vtkCleanPolyData* vClean = vtkCleanPolyData::New();
  vClean->SetInput(vPData);

  //Step 8: make tubes. The number of sides per tube is set by nsides.
  //Even an nsides of 3 looks surprisingly good.
  vtkTubeFilter* vTFilter = vtkTubeFilter::New();
  vTFilter->SetNumberOfSides(5);
  vTFilter->SetInput(vClean->GetOutput());
  vTFilter->CappingOff();

  vTFilter->SetRadius(min_scalar);   //this call sets min rad.
  vTFilter->SetRadiusFactor(max_scalar/min_scalar); //sets max rad.
  vTFilter->SetVaryRadiusToVaryRadiusByScalar();

  //Step 9: create a mapper of the tube
  vtkPolyDataMapper* vMapper = vtkPolyDataMapper::New();
  vMapper->SetInput(vTFilter->GetOutput());
      
  vMapper->ScalarVisibilityOff();    //interpret scalars as color command
   
  //Step 10: Add the mapper to the actor. You can now delete everything.
  //A matrix for the actor, colors, opacities, etc can be set by
  //the caller before or after this function is called.
  vtkActor* TubeActor = vtkActor::New();
  TubeActor->SetMapper(vMapper);

  TubeActor->GetProperty()->SetColor(this->GetRed(),
                                    this->GetGreen(),
                                    this->GetBlue());

  TubeActor->GetProperty()->SetOpacity(this->GetOpacity());

  this->AddActor( TubeActor );

  vPoints->Delete();
  delete [] pntIds;
  vScalars->Delete();
  vPLine->Delete();
  vClean->Delete();
  vCA->Delete();
  vPData->Delete();
  vTFilter->Delete();
  vMapper->Delete();
  vColorScalars->Delete();
  vVectors->Delete();
}

/** Create a copy of the current object representation */
VascularNetworkObjectRepresentation::Pointer
VascularNetworkObjectRepresentation::Copy() const
{
  Pointer newOR = VascularNetworkObjectRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetVascularNetworkObject(m_VascularNetworkObject);

  return newOR;
}


} // end namespace igstk
