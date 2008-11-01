/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolProjectionRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkToolProjectionRepresentation.h"
#include "igstkEvents.h"

#include <vtkActor.h>
#include <vtkAxisActor2D.h>
#include <vtkProperty.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkTubeFilter.h>
#include <vtkPlaneSource.h>



namespace igstk
{ 

/** Constructor */
ToolProjectionRepresentation
::ToolProjectionRepresentation():m_StateMachine(this)
{

  m_LineSource = NULL;
  m_Tuber = NULL;
  m_LineMapper = NULL;

  m_ToolProjectionSpatialObject = NULL;

  m_VTKPlaneObserver = VTKPlaneObserver::New();

  m_PlaneNormal.Fill(0.0);

  this->RequestSetSpatialObject( m_ToolProjectionSpatialObject );
  
  igstkAddInputMacro( ValidToolProjectionObject );
  igstkAddInputMacro( NullToolProjectionObject  );
  igstkAddInputMacro( ValidReslicePlaneSpatialObject );
  igstkAddInputMacro( InValidReslicePlaneSpatialObject );

  igstkAddStateMacro( NullToolProjectionObject  );
  igstkAddStateMacro( ValidToolProjectionObject );
  igstkAddStateMacro( ValidReslicePlaneSpatialObject );

  //from NullToolProjectionObject

  igstkAddTransitionMacro( NullToolProjectionObject, ValidToolProjectionObject, 
                           ValidToolProjectionObject,  SetToolProjectionObject );

  igstkAddTransitionMacro( NullToolProjectionObject, NullToolProjectionObject, 
                           NullToolProjectionObject,  No );

  igstkAddTransitionMacro( NullToolProjectionObject, ValidReslicePlaneSpatialObject, 
                           NullToolProjectionObject,  No );

  igstkAddTransitionMacro( NullToolProjectionObject, InValidReslicePlaneSpatialObject, 
                           NullToolProjectionObject,  No );

  //from ValidToolProjectionObject

  igstkAddTransitionMacro( ValidToolProjectionObject, ValidReslicePlaneSpatialObject, 
                           ValidReslicePlaneSpatialObject,  SetReslicePlaneSpatialObject );

  igstkAddTransitionMacro( ValidToolProjectionObject, NullToolProjectionObject, 
                           NullToolProjectionObject,  No ); 

  igstkAddTransitionMacro( ValidToolProjectionObject, ValidToolProjectionObject, 
                           ValidToolProjectionObject,  No );

  igstkAddTransitionMacro( ValidToolProjectionObject, InValidReslicePlaneSpatialObject, 
                           ValidToolProjectionObject,  No );


  //from ValidReslicePlaneSpatialObject

  igstkAddTransitionMacro( ValidReslicePlaneSpatialObject, ValidToolProjectionObject, 
                           ValidReslicePlaneSpatialObject,  No );

  igstkAddTransitionMacro( ValidReslicePlaneSpatialObject, NullToolProjectionObject, 
                           ValidReslicePlaneSpatialObject,  No ); 

  igstkAddTransitionMacro( ValidReslicePlaneSpatialObject, ValidReslicePlaneSpatialObject, 
                           ValidReslicePlaneSpatialObject,  No );

  igstkAddTransitionMacro( ValidReslicePlaneSpatialObject, InValidReslicePlaneSpatialObject, 
                           ValidReslicePlaneSpatialObject,  No );

  m_StateMachine.SelectInitialState( m_NullToolProjectionObjectState );

  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
ToolProjectionRepresentation
::~ToolProjectionRepresentation()  
{
  this->DeleteActors();

  if (m_LineSource != NULL)
  {
    m_LineSource->Delete();
    m_LineSource=NULL;
  }

  if (m_Tuber != NULL)
  {
    m_Tuber->Delete();
    m_Tuber=NULL;
  }

  if (m_LineMapper != NULL)
  {
    m_LineMapper->Delete();
    m_LineMapper=NULL;
  }

}


/** Request to Set the ToolProjectionSpatial Object */
void ToolProjectionRepresentation
::RequestSetToolProjectionObject( const ToolProjectionSpatialObjectType * ToolProjection )
{
  m_ToolProjectionObjectToAdd = ToolProjection;
  if( !m_ToolProjectionObjectToAdd )
    {
    m_StateMachine.PushInput( m_NullToolProjectionObjectInput );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    m_StateMachine.PushInput( m_ValidToolProjectionObjectInput );
    m_StateMachine.ProcessInputs();
    }
}

/** Set the ToolProjection Spatial Object */
void ToolProjectionRepresentation
::NoProcessing()
{
}

/** Set the ToolProjection Spatial Object */
void ToolProjectionRepresentation
::SetToolProjectionObjectProcessing()
{
  m_ToolProjectionSpatialObject = m_ToolProjectionObjectToAdd;
  this->RequestSetSpatialObject( m_ToolProjectionSpatialObject );
} 

/** Requests to set the ReslicePlaneSpatialObject*/
void 
ToolProjectionRepresentation
::RequestSetReslicePlaneSpatialObject( const ReslicerPlaneType *
reslicePlaneSpatialObject )
{  
  igstkLogMacro( DEBUG,"igstk::ToolProjectionRepresentation\
                       ::RequestSetReslicePlaneSpatialObject called...\n");

  m_ReslicePlaneSpatialObjectToBeSet = const_cast< ReslicerPlaneType* >(reslicePlaneSpatialObject);

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

/** Sets the ReslicePlaneSpatialObject*/
void 
ToolProjectionRepresentation
::SetReslicePlaneSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ToolProjectionRepresentation\
                       ::SetReslicePlaneSpatialObjectProcessing called...\n");

  m_ReslicePlaneSpatialObject = m_ReslicePlaneSpatialObjectToBeSet;

  m_ReslicePlaneSpatialObject->RequestComputeReslicingPlane();
}

/** Print Self function */
void ToolProjectionRepresentation
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** Update the visual representation in response to changes in the geometric
 * object */
void ToolProjectionRepresentation
::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "UpdateRepresentationProcessing called ....\n");

  //m_ReslicePlaneSpatialObject->RequestComputeReslicingPlane();

  if ( m_ReslicePlaneSpatialObject->IsToolSpatialObjectSet() )
  {
    igstk::Transform toolTransform = m_ReslicePlaneSpatialObject->GetToolTransform();
    VectorType point1 = toolTransform.GetTranslation();
    VectorType point2;
    point2.Fill(0);
    igstk::Transform::VersorType rotation = toolTransform.GetRotation();
    VectorType toolAxis;
    toolAxis[0] = 1;
    toolAxis[1] = 0;
    toolAxis[2] = 0;
    toolAxis = rotation.Transform(toolAxis);

    unsigned int planeObsID = 
        m_ReslicePlaneSpatialObject->AddObserver( VTKPlaneModifiedEvent(),
                                        m_VTKPlaneObserver );
    
    m_VTKPlaneObserver->Reset();

    m_ReslicePlaneSpatialObject->RequestGetVTKPlane();
    
    if( m_VTKPlaneObserver->GotVTKPlane() )
    {
        this->SetPlane( m_VTKPlaneObserver->GetVTKPlane() );        

        VectorType toolProy = itk::CrossProduct( m_PlaneNormal, itk::CrossProduct(toolAxis, m_PlaneNormal) );

        point2 = point1 + toolProy*this->m_ToolProjectionSpatialObject->GetSize();
    }

    m_ReslicePlaneSpatialObject->RemoveObserver( planeObsID );

    if ( (point2-point1).GetNorm() > 0.1 )
    {
      m_LineSource->SetPoint1( point1[0], point1[1], point1[2] );
      m_LineSource->SetPoint2( point2[0], point2[1], point2[2] );
    }
  }
}

/** Sets actors visibility */
void ToolProjectionRepresentation
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
ToolProjectionRepresentation
::VerifyTimeStamp( ) const
{
  igstkLogMacro( DEBUG, 
    "igstk::ImageResliceSpatialObjectRepresentation::VerifyTimeStamp called...\n");

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

void
ToolProjectionRepresentation
::SetPlane( const vtkPlaneSource * plane )
{
  igstkLogMacro( DEBUG, "igstk::ToolProjectionRepresentation\
                        ::SetPlane called...\n");

  // This const_cast<> is needed here due to 
  // the lack of const-correctness in VTK 
  //m_PlaneSource = 
  vtkPlaneSource* auxPlane = const_cast< vtkPlaneSource *>( plane );

  double* normal = auxPlane->GetNormal();

  m_PlaneNormal[0] = normal[0];
  m_PlaneNormal[1] = normal[1];
  m_PlaneNormal[2] = normal[2];
}

/** Create the vtk Actors */
void ToolProjectionRepresentation
::CreateActors()
{
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  // build projection line
  m_LineSource = vtkLineSource::New();    
  m_LineSource->SetPoint1( 0, 0, 0 );
  m_LineSource->SetPoint2( 1, 1, 1 );

  m_Tuber = vtkTubeFilter::New();
  m_Tuber->SetInput ( m_LineSource->GetOutput() );
  m_Tuber->SetRadius (1);
  m_Tuber->SetNumberOfSides(3);

  m_LineMapper = vtkPolyDataMapper::New();
  m_LineMapper->SetInput ( m_Tuber->GetOutput() );

  vtkActor* lineActor = vtkActor::New();
  lineActor->SetMapper (m_LineMapper);

  lineActor->GetProperty()->SetColor( this->GetRed(),this->GetGreen(),this->GetBlue() );

  this->AddActor( lineActor );
}

/** Create a copy of the current object representation */
ToolProjectionRepresentation::Pointer
ToolProjectionRepresentation
::Copy() const
{
  Pointer newOR = ToolProjectionRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetToolProjectionObject(m_ToolProjectionSpatialObject);

  return newOR;
}


} // end namespace igstk
