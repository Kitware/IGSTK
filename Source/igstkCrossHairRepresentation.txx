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

  m_HorizontalLineSource = NULL;
  m_HorizontalTuber = NULL;
  m_HorizontalLineMapper = NULL;

  m_VerticalLineSource = NULL;
  m_VerticalTuber = NULL;
  m_VerticalLineMapper = NULL;

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

  if (m_VerticalLineSource != NULL)
  {
    m_VerticalLineSource->Delete();
    m_VerticalLineSource=NULL;
  }

  if (m_VerticalTuber != NULL)
  {
    m_VerticalTuber->Delete();
    m_VerticalTuber=NULL;
  }

  if (m_VerticalLineMapper != NULL)
  {
    m_VerticalLineMapper->Delete();
    m_VerticalLineMapper=NULL;
  }

  if (m_HorizontalLineSource != NULL)
  {
    m_HorizontalLineSource->Delete();
    m_HorizontalLineSource=NULL;
  }

  if (m_HorizontalTuber != NULL)
  {
    m_HorizontalTuber->Delete();
    m_HorizontalTuber=NULL;
  }

  if (m_HorizontalLineMapper != NULL)
  {
    m_HorizontalLineMapper->Delete();
    m_HorizontalLineMapper=NULL;
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

  // build vertical cross hair
  m_VerticalLineSource = vtkLineSource::New();
  m_VerticalLineSource->SetPoint1( 0, 0, 0 );
  m_VerticalLineSource->SetPoint2( 1, 1, 1 );

  m_VerticalTuber = vtkTubeFilter::New();
  m_VerticalTuber->SetInput ( m_VerticalLineSource->GetOutput() );
  m_VerticalTuber->SetRadius (1);
  m_VerticalTuber->SetNumberOfSides(3);

  m_VerticalLineMapper = vtkPolyDataMapper::New();
  m_VerticalLineMapper->SetInput( m_VerticalTuber->GetOutput() );

    // build horizontal cross hair
  m_HorizontalLineSource = vtkLineSource::New();
  m_HorizontalLineSource->SetPoint1( 0, 0, 0 );
  m_HorizontalLineSource->SetPoint2( 1, 1, 1 );

  m_HorizontalTuber = vtkTubeFilter::New();
  m_HorizontalTuber->SetInput ( m_HorizontalLineSource->GetOutput() );
  m_HorizontalTuber->SetRadius (1);
  m_HorizontalTuber->SetNumberOfSides(5);

  m_HorizontalLineMapper = vtkPolyDataMapper::New();
  m_HorizontalLineMapper->SetInput( m_HorizontalTuber->GetOutput() );

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

  switch ( m_ReslicePlaneSpatialObject->GetOrientationType() )
  {
    case ReslicePlaneSpatialObjectType::Axial:
      m_VerticalLineSource->SetPoint1( point1[0], m_ImageBounds[2], point1[2] );
      m_VerticalLineSource->SetPoint2( point1[0], m_ImageBounds[3], point1[2] );
      m_HorizontalLineSource->SetPoint1( m_ImageBounds[0], point1[1], point1[2] );
      m_HorizontalLineSource->SetPoint2( m_ImageBounds[1], point1[1], point1[2] );
      break;
    case ReslicePlaneSpatialObjectType::Sagittal:
      m_VerticalLineSource->SetPoint1( point1[0], point1[1], m_ImageBounds[4] );
      m_VerticalLineSource->SetPoint2( point1[0], point1[1], m_ImageBounds[5] );
      m_HorizontalLineSource->SetPoint1( point1[0], m_ImageBounds[2], point1[2] );
      m_HorizontalLineSource->SetPoint2( point1[0], m_ImageBounds[3], point1[2] );
      break;
    case ReslicePlaneSpatialObjectType::Coronal:
      m_VerticalLineSource->SetPoint1( point1[0], point1[1], m_ImageBounds[4] );
      m_VerticalLineSource->SetPoint2( point1[0], point1[1], m_ImageBounds[5] );
      m_HorizontalLineSource->SetPoint1( m_ImageBounds[0], point1[1], point1[2] );
      m_HorizontalLineSource->SetPoint2( m_ImageBounds[1], point1[1], point1[2] );
      break;
  }
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

  vtkActor* verticalLineActor = vtkActor::New();
  verticalLineActor->SetMapper (m_VerticalLineMapper);

  vtkActor* horizontalLineActor = vtkActor::New();
  horizontalLineActor->SetMapper (m_HorizontalLineMapper);

   // set their color according to the orientation
  switch ( m_ReslicePlaneSpatialObject->GetOrientationType() )
  {
    case ReslicePlaneSpatialObjectType::Axial:
    {
    verticalLineActor->GetProperty()->SetColor( 0,1,0 );
    horizontalLineActor->GetProperty()->SetColor( 0,0,1 );
    break;
    }
    case ReslicePlaneSpatialObjectType::Sagittal:
    {
    verticalLineActor->GetProperty()->SetColor( 0,0,1 );
    horizontalLineActor->GetProperty()->SetColor( 1,0,0 );
    break;
    }
    case ReslicePlaneSpatialObjectType::Coronal:
    {
    verticalLineActor->GetProperty()->SetColor( 0,1,0 );
    horizontalLineActor->GetProperty()->SetColor( 1,0,0 );
    break;
    }
    default:
    {
    std::cerr << "Invalid orientaiton" << std::endl;
    break;
    }
  }

  this->AddActor( verticalLineActor );

  this->AddActor( horizontalLineActor );

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
