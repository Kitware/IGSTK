/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUltrasoundProbeObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkUltrasoundProbeObjectRepresentation.h"

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkSphere.h>
#include <vtkImplicitBoolean.h>
#include <vtkSampleFunction.h>
#include <vtkMarchingContourFilter.h>
#include <vtkPlane.h>
#include <vtkCylinder.h>
#include <vtkSphereSource.h>
#include <vtkBox.h>

namespace igstk
{ 

/** Constructor */
UltrasoundProbeObjectRepresentation
::UltrasoundProbeObjectRepresentation():m_StateMachine(this)
{
  // We create the ellipse spatial object
  m_UltrasoundProbeSpatialObject = NULL;
  this->RequestSetSpatialObject( m_UltrasoundProbeSpatialObject );
  
  igstkAddInputMacro( ValidUltrasoundProbeObject );
  igstkAddInputMacro( NullUltrasoundProbeObject  );

  igstkAddStateMacro( NullUltrasoundProbeObject   );
  igstkAddStateMacro( ValidUltrasoundProbeObject  );

  igstkAddTransitionMacro( NullUltrasoundProbeObject, 
                           NullUltrasoundProbeObject, 
                           NullUltrasoundProbeObject,  No );
  igstkAddTransitionMacro( NullUltrasoundProbeObject, 
                           ValidUltrasoundProbeObject, 
                           ValidUltrasoundProbeObject,  
                           SetUltrasoundProbeObject );
  igstkAddTransitionMacro( ValidUltrasoundProbeObject, 
                           NullUltrasoundProbeObject, 
                           NullUltrasoundProbeObject,  No ); 
  igstkAddTransitionMacro( ValidUltrasoundProbeObject, 
                           ValidUltrasoundProbeObject, 
                           ValidUltrasoundProbeObject,  No ); 

  igstkSetInitialStateMacro( NullUltrasoundProbeObject );

  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
UltrasoundProbeObjectRepresentation::~UltrasoundProbeObjectRepresentation()  
{
  this->DeleteActors();
}

/** Set the UltrasoundProbeal Spatial Object */
void UltrasoundProbeObjectRepresentation
::RequestSetUltrasoundProbeObject( const UltrasoundProbeSpatialObjectType * 
                                                            UltrasoundProbe )
{
  m_UltrasoundProbeObjectToAdd = UltrasoundProbe;
  if( !m_UltrasoundProbeObjectToAdd )
    {
    m_StateMachine.PushInput( m_NullUltrasoundProbeObjectInput );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    m_StateMachine.PushInput( m_ValidUltrasoundProbeObjectInput );
    m_StateMachine.ProcessInputs();
    }
}

/** Set the UltrasoundProbe Spatial Object */
void UltrasoundProbeObjectRepresentation::NoProcessing()
{
}

/** Set the UltrasoundProbe Spatial Object */
void UltrasoundProbeObjectRepresentation::SetUltrasoundProbeObjectProcessing()
{
  m_UltrasoundProbeSpatialObject = m_UltrasoundProbeObjectToAdd;
  this->RequestSetSpatialObject( m_UltrasoundProbeSpatialObject );
} 

/** Print Self function */
void UltrasoundProbeObjectRepresentation
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** Update the visual representation in response to changes in the geometric
 * object */
void UltrasoundProbeObjectRepresentation::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "UpdateRepresentationProcessing called ....\n");
}

/** Create the vtk Actors */
void UltrasoundProbeObjectRepresentation::CreateActors()
{
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();
   
  vtkPolyDataMapper *tipMapper = vtkPolyDataMapper::New();
  vtkActor* tipActor = vtkActor::New();

  // Create the tip
  vtkCylinder* cylinder = vtkCylinder::New();
  cylinder->SetRadius(40);
  cylinder->SetCenter(0,0,-40);

  vtkPlane* plane = vtkPlane::New();
  plane->SetOrigin(0,0,-40);
  plane->SetNormal(0,0,-1);

  vtkImplicitBoolean* tip = vtkImplicitBoolean::New();
  tip->SetOperationTypeToIntersection();
  tip->AddFunction(cylinder);
  tip->AddFunction(plane);

  vtkSampleFunction* sample = vtkSampleFunction::New();
  sample->SetImplicitFunction(tip);
  sample->SetModelBounds(-42,42,-12.5,12.5,-42,42);
  sample->SetSampleDimensions(60,60,60);
  sample->ComputeNormalsOff();
  sample->CappingOn();
   
  vtkMarchingContourFilter* surface = vtkMarchingContourFilter::New();
  surface->SetInput((vtkDataObject*)(sample->GetOutput()));
  surface->SetValue(0,0.0);
  surface->ComputeScalarsOff();

  tipActor->GetProperty()->SetColor(1.0,1.0,1.0); 
  tipActor->GetProperty()->SetOpacity(this->GetOpacity()); 
    
  tipMapper->SetInput(surface->GetOutput());
  tipActor->SetMapper( tipMapper );

  this->AddActor( tipActor );
  tipMapper->Delete();
  tip->Delete();
  cylinder->Delete();
  sample->Delete();
  plane->Delete();
  surface->Delete();

  // Create the black band
  vtkPolyDataMapper *tipBlackMapper = vtkPolyDataMapper::New();
  vtkActor* tipBlackActor = vtkActor::New();

  vtkCylinder* cylinderTip = vtkCylinder::New();
  cylinderTip->SetRadius(40.8);
  cylinderTip->SetCenter(0,0,-40);

  vtkPlane* planeBlack = vtkPlane::New();
  planeBlack->SetOrigin(0,0,-30);
  planeBlack->SetNormal(0,0,-1);

  vtkImplicitBoolean* tipBlack = vtkImplicitBoolean::New();
  tipBlack->SetOperationTypeToIntersection();
  tipBlack->AddFunction(cylinderTip);
  tipBlack->AddFunction(planeBlack);

  vtkSampleFunction* sampleBlack = vtkSampleFunction::New();
  sampleBlack->SetImplicitFunction(tipBlack);
  sampleBlack->SetModelBounds(-42,42,-8.0,8.0,-42,42);
  sampleBlack->SetSampleDimensions(60,60,60);
  sampleBlack->ComputeNormalsOff();
   
  vtkMarchingContourFilter* surfaceBlack = vtkMarchingContourFilter::New();
  surfaceBlack->SetInput((vtkDataObject*)(sampleBlack->GetOutput()));
  surfaceBlack->SetValue(0,0.0);
  surfaceBlack->ComputeScalarsOff();

  tipBlackActor->GetProperty()->SetColor(0.0,0.0,0.0); 
  tipBlackActor->GetProperty()->SetOpacity(this->GetOpacity()); 
    
  tipBlackMapper->SetInput(surfaceBlack->GetOutput());
  tipBlackActor->SetMapper( tipBlackMapper );

  this->AddActor( tipBlackActor );
  tipBlackMapper->Delete();
  cylinderTip->Delete();
  planeBlack->Delete();
  tipBlack->Delete();
  sampleBlack->Delete();
  surfaceBlack->Delete();

  // Now creates the handle
  vtkPolyDataMapper *handleMapper = vtkPolyDataMapper::New();
  vtkActor* handleActor = vtkActor::New();

  vtkBox* boxHandle = vtkBox::New();
  boxHandle->SetXMin(-39,-12.5,-130);
  boxHandle->SetXMax(39,12.5,-40);

  vtkCylinder* cylinderHandle1 = vtkCylinder::New();
  cylinderHandle1->SetRadius(40);
  cylinderHandle1->SetCenter(-60,0,-85);

  vtkCylinder* cylinderHandle2 = vtkCylinder::New();
  cylinderHandle2->SetRadius(40);
  cylinderHandle2->SetCenter(60,0,-85);

  vtkImplicitBoolean* handle = vtkImplicitBoolean::New();
  handle->SetOperationTypeToDifference();
  handle->AddFunction(boxHandle);
  handle->AddFunction(cylinderHandle1);
  handle->AddFunction(cylinderHandle2);

  vtkSampleFunction* sampleHandle = vtkSampleFunction::New();
  sampleHandle->SetImplicitFunction(handle);
  sampleHandle->SetModelBounds(-42,42,-40,40,-200,-10);
  sampleHandle->SetSampleDimensions(60,60,60);
  sampleHandle->ComputeNormalsOn();
  sampleHandle->CappingOn();
   
  vtkMarchingContourFilter* surfaceHandle = vtkMarchingContourFilter::New();
  surfaceHandle->SetInput((vtkDataObject*)(sampleHandle->GetOutput()));
  surfaceHandle->SetValue(0,0.0);
  surfaceHandle->ComputeScalarsOff();

  handleActor->GetProperty()->SetColor(1.0,1.0,1.0); 
  handleActor->GetProperty()->SetOpacity(this->GetOpacity()); 
    
  handleMapper->SetInput(surfaceHandle->GetOutput());
  handleActor->SetMapper( handleMapper );

  this->AddActor( handleActor );
  handleMapper->Delete();
  boxHandle->Delete();
  cylinderHandle1->Delete();
  cylinderHandle2->Delete();
  handle->Delete();
  sampleHandle->Delete();
  surfaceHandle->Delete();
}

/** Create a copy of the current object representation */
UltrasoundProbeObjectRepresentation::Pointer
UltrasoundProbeObjectRepresentation::Copy() const
{
  Pointer newOR = UltrasoundProbeObjectRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetUltrasoundProbeObject(m_UltrasoundProbeSpatialObject);

  return newOR;
}


} // end namespace igstk
