/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCrossHairRepresentation.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkCrossHairRepresentation.h"
#include "igstkEvents.h"

#include <vtkActor.h>
#include <vtkAxisActor2D.h>
#include <vtkProperty.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkTubeFilter.h>



namespace igstk
{ 

/** Constructor */
template < class TImageSpatialObject >
CrossHairRepresentation< TImageSpatialObject >::
CrossHairRepresentation():m_StateMachine(this)
{

  m_AxialLineSource = NULL;
  m_SagittalLineSource = NULL;
  m_CoronalLineSource = NULL;

  m_AxialTuber = NULL;
  m_SagittalTuber = NULL;
  m_CoronalTuber = NULL;

  m_AxialLineMapper = NULL;
  m_SagittalLineMapper = NULL;
  m_CoronalLineMapper = NULL;

  m_ImageBounds[0] = 0;
  m_ImageBounds[1] = 0;
  m_ImageBounds[2] = 0;
  m_ImageBounds[3] = 0;
  m_ImageBounds[4] = 0;
  m_ImageBounds[5] = 0;

  m_CrossHairSpatialObject = NULL;
  this->RequestSetSpatialObject( m_CrossHairSpatialObject );
  
  igstkAddInputMacro( ValidCrossHairObject );
  igstkAddInputMacro( NullCrossHairObject  );
  igstkAddInputMacro( ValidReslicePlaneSpatialObject );
  igstkAddInputMacro( InValidReslicePlaneSpatialObject );

  igstkAddStateMacro( NullCrossHairObject  );
  igstkAddStateMacro( ValidCrossHairObject );
  igstkAddStateMacro( ValidReslicePlaneSpatialObject );

  //from NullCrossHairObject

  igstkAddTransitionMacro( NullCrossHairObject, ValidCrossHairObject, 
                           ValidCrossHairObject,  SetCrossHairObject );

  igstkAddTransitionMacro( NullCrossHairObject, NullCrossHairObject, 
                           NullCrossHairObject,  No );

  igstkAddTransitionMacro( NullCrossHairObject, ValidReslicePlaneSpatialObject, 
                           NullCrossHairObject,  No );

  igstkAddTransitionMacro( NullCrossHairObject, InValidReslicePlaneSpatialObject, 
                           NullCrossHairObject,  No );

  //from ValidCrossHairObject

  igstkAddTransitionMacro( ValidCrossHairObject, ValidReslicePlaneSpatialObject, 
                           ValidReslicePlaneSpatialObject,  SetReslicePlaneSpatialObject );

  igstkAddTransitionMacro( ValidCrossHairObject, NullCrossHairObject, 
                           NullCrossHairObject,  No ); 

  igstkAddTransitionMacro( ValidCrossHairObject, ValidCrossHairObject, 
                           ValidCrossHairObject,  No );

  igstkAddTransitionMacro( ValidCrossHairObject, InValidReslicePlaneSpatialObject, 
                           ValidCrossHairObject,  No );


  //from ValidReslicePlaneSpatialObject

  igstkAddTransitionMacro( ValidReslicePlaneSpatialObject, ValidCrossHairObject, 
                           ValidReslicePlaneSpatialObject,  No );

  igstkAddTransitionMacro( ValidReslicePlaneSpatialObject, NullCrossHairObject, 
                           ValidReslicePlaneSpatialObject,  No ); 

  igstkAddTransitionMacro( ValidReslicePlaneSpatialObject, ValidReslicePlaneSpatialObject, 
                           ValidReslicePlaneSpatialObject,  No );

  igstkAddTransitionMacro( ValidReslicePlaneSpatialObject, InValidReslicePlaneSpatialObject, 
                           ValidReslicePlaneSpatialObject,  No );

  m_StateMachine.SelectInitialState( m_NullCrossHairObjectState );

  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
template < class TImageSpatialObject >
CrossHairRepresentation< TImageSpatialObject >
::~CrossHairRepresentation()  
{
  this->DeleteActors();

  if (m_AxialLineSource != NULL)
  {
    m_AxialLineSource->Delete();
    m_AxialLineSource=NULL;
  }

  if (m_SagittalLineSource != NULL)
  {
    m_SagittalLineSource->Delete();
    m_SagittalLineSource=NULL;
  }

  if (m_CoronalLineSource != NULL)
  {
    m_CoronalLineSource->Delete();
    m_CoronalLineSource=NULL;
  }

  if (m_AxialTuber != NULL)
  {
    m_AxialTuber->Delete();
    m_AxialTuber=NULL;
  }

  if (m_SagittalTuber != NULL)
  {
    m_SagittalTuber->Delete();
    m_SagittalTuber=NULL;
  }

  if (m_CoronalTuber != NULL)
  {
    m_CoronalTuber->Delete();
    m_CoronalTuber=NULL;
  }

  if (m_AxialLineMapper != NULL)
  {
    m_AxialLineMapper->Delete();
    m_AxialLineMapper=NULL;
  }

  if (m_SagittalLineMapper != NULL)
  {
    m_SagittalLineMapper->Delete();
    m_SagittalLineMapper=NULL;
  }

  if (m_CoronalLineMapper != NULL)
  {
    m_CoronalLineMapper->Delete();
    m_CoronalLineMapper=NULL;
  }

}

/** Verify time stamp of the attached tool*/
template < class TImageSpatialObject >
bool
CrossHairRepresentation < TImageSpatialObject >
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

/** Request to Set the CrossHairSpatial Object */
template < class TImageSpatialObject >
void CrossHairRepresentation< TImageSpatialObject >
::RequestSetCrossHairObject( const CrossHairSpatialObjectType * CrossHair )
{
  m_CrossHairObjectToAdd = CrossHair;
  if( !m_CrossHairObjectToAdd )
    {
    m_StateMachine.PushInput( m_NullCrossHairObjectInput );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    m_StateMachine.PushInput( m_ValidCrossHairObjectInput );
    m_StateMachine.ProcessInputs();
    }
}

/** Set the CrossHair Spatial Object */
template < class TImageSpatialObject >
void CrossHairRepresentation< TImageSpatialObject >
::NoProcessing()
{
}

/** Set the CrossHair Spatial Object */
template < class TImageSpatialObject >
void CrossHairRepresentation< TImageSpatialObject >
::SetCrossHairObjectProcessing()
{
  m_CrossHairSpatialObject = m_CrossHairObjectToAdd;
  this->RequestSetSpatialObject( m_CrossHairSpatialObject );
} 

/** Requests to set the ReslicePlaneSpatialObject*/
template < class TImageSpatialObject >
void 
CrossHairRepresentation< TImageSpatialObject >
::RequestSetReslicePlaneSpatialObject( const ReslicePlaneSpatialObjectType *
reslicePlaneSpatialObject )
{  
  igstkLogMacro( DEBUG,"igstk::CrossHairRepresentation\
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

/** Sets the ReslicePlaneSpatialObject*/
template < class TImageSpatialObject >
void 
CrossHairRepresentation< TImageSpatialObject >
::SetReslicePlaneSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::CrossHairRepresentation\
                       ::SetReslicePlaneSpatialObjectProcessing called...\n");

  m_ReslicePlaneSpatialObject = m_ReslicePlaneSpatialObjectToBeSet;

  m_ReslicePlaneSpatialObject->RequestComputeReslicingPlane();  

  /** 
   * Request information about the slice bounds. The answer will be
   * received in the form of event. This will be used to set the 
   * size of the cross hairs
   */
 
  ImageBoundsObserver::Pointer boundsObs = ImageBoundsObserver::New();

  unsigned int obsID;
  obsID = m_ReslicePlaneSpatialObject->AddObserver( igstk::ImageBoundsEvent(), boundsObs );

  m_ReslicePlaneSpatialObject->RequestGetImageBounds();

  if( boundsObs->GotImageBounds() )
    {
    m_ImageBounds[0] = boundsObs->GetImageBounds().xmin;
    m_ImageBounds[1] = boundsObs->GetImageBounds().xmax;
    m_ImageBounds[2] = boundsObs->GetImageBounds().ymin;
    m_ImageBounds[3] = boundsObs->GetImageBounds().ymax;
    m_ImageBounds[4] = boundsObs->GetImageBounds().zmin;
    m_ImageBounds[5] = boundsObs->GetImageBounds().zmax;
    }

  m_ReslicePlaneSpatialObject->RemoveObserver(obsID);

  // build axial cross hair
  m_AxialLineSource = vtkLineSource::New();
  m_AxialLineSource->SetPoint1( 0, 0, 0 );
  m_AxialLineSource->SetPoint2( 1, 1, 1 );

  m_AxialTuber = vtkTubeFilter::New();
  m_AxialTuber->SetInput ( m_AxialLineSource->GetOutput() );
  m_AxialTuber->SetRadius (1);
  m_AxialTuber->SetNumberOfSides(3);

  m_AxialLineMapper = vtkPolyDataMapper::New();
  m_AxialLineMapper->SetInput( m_AxialTuber->GetOutput() );

 // build sagittal cross hair
  m_SagittalLineSource = vtkLineSource::New();
  m_SagittalLineSource->SetPoint1( 0, 0, 0 );
  m_SagittalLineSource->SetPoint2( 1, 1, 1 );

  m_SagittalTuber = vtkTubeFilter::New();
  m_SagittalTuber->SetInput ( m_SagittalLineSource->GetOutput() );
  m_SagittalTuber->SetRadius (1);
  m_SagittalTuber->SetNumberOfSides(3);

  m_SagittalLineMapper = vtkPolyDataMapper::New();
  m_SagittalLineMapper->SetInput( m_SagittalTuber->GetOutput() );

 // build coronal cross hair
  m_CoronalLineSource = vtkLineSource::New();
  m_CoronalLineSource->SetPoint1( 0, 0, 0 );
  m_CoronalLineSource->SetPoint2( 1, 1, 1 );

  m_CoronalTuber = vtkTubeFilter::New();
  m_CoronalTuber->SetInput ( m_CoronalLineSource->GetOutput() );
  m_CoronalTuber->SetRadius (1);
  m_CoronalTuber->SetNumberOfSides(3);

  m_CoronalLineMapper = vtkPolyDataMapper::New();
  m_CoronalLineMapper->SetInput( m_CoronalTuber->GetOutput() );

}

/** Print Self function */
template < class TImageSpatialObject >
void CrossHairRepresentation< TImageSpatialObject >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** Update the visual representation in response to changes in the geometric
 * object */
template < class TImageSpatialObject >
void CrossHairRepresentation< TImageSpatialObject >
::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "CrossHairRepresentation::UpdateRepresentationProcessing called ....\n");

   m_ReslicePlaneSpatialObject->RequestComputeReslicingPlane();  

  double point1[3];
  point1[0] = 0.0;
  point1[1] = 0.0;
  point1[2] = 0.0;

  /** 
   * Request information about the tool position. The answer will be
   * received in the form of event. This will be used to set the 
   * position of the cross hairs
   */
 
  ToolPositionObserver::Pointer positionObs = ToolPositionObserver::New();

  unsigned int obsID;
  obsID = m_ReslicePlaneSpatialObject->AddObserver( igstk::PointEvent(), positionObs );

  m_ReslicePlaneSpatialObject->RequestGetToolPosition();

  if( positionObs->GotToolPosition() )
    {
        point1[0] =  positionObs->GetToolPosition()[0];
        point1[1] =  positionObs->GetToolPosition()[1];
        point1[2] =  positionObs->GetToolPosition()[2];
    }

  m_ReslicePlaneSpatialObject->RemoveObserver(obsID);

  m_SagittalLineSource->SetPoint1( point1[0], m_ImageBounds[2], point1[2] );
  m_SagittalLineSource->SetPoint2( point1[0], m_ImageBounds[3], point1[2] );
  m_CoronalLineSource->SetPoint1( m_ImageBounds[0], point1[1], point1[2] );
  m_CoronalLineSource->SetPoint2( m_ImageBounds[1], point1[1], point1[2] );
  m_AxialLineSource->SetPoint1( point1[0], point1[1], m_ImageBounds[4] );
  m_AxialLineSource->SetPoint2( point1[0], point1[1], m_ImageBounds[5] );

}

/** Sets actors visibility */
template < class TImageSpatialObject >
void CrossHairRepresentation< TImageSpatialObject >
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

/** Create the vtk Actors */
template < class TImageSpatialObject >
void CrossHairRepresentation< TImageSpatialObject >
::CreateActors()
{
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  vtkActor* axialLineActor = vtkActor::New();
  axialLineActor->SetMapper (m_AxialLineMapper);

  vtkActor* sagittalLineActor = vtkActor::New();
  sagittalLineActor->SetMapper (m_SagittalLineMapper);

  vtkActor* coronalLineActor = vtkActor::New();
  coronalLineActor->SetMapper (m_CoronalLineMapper);

  // set their color according to the orientation
  axialLineActor->GetProperty()->SetColor( 1,0,0 );
  sagittalLineActor->GetProperty()->SetColor( 0,1,0 );
  coronalLineActor->GetProperty()->SetColor( 0,0,1 );

  this->AddActor( axialLineActor );
  this->AddActor( sagittalLineActor );
  this->AddActor( coronalLineActor );

}

/** Create a copy of the current object representation */
template < class TImageSpatialObject >
typename CrossHairRepresentation< TImageSpatialObject >::Pointer
CrossHairRepresentation< TImageSpatialObject >
::Copy() const
{
  Pointer newOR = CrossHairRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetCrossHairObject(m_CrossHairSpatialObject);

  return newOR;
}


} // end namespace igstk
