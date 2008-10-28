/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCrossHairRepresentation.cxx
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
CrossHairRepresentation::CrossHairRepresentation():m_StateMachine(this)
{

  m_LineSourceZ = NULL;
  m_LineSourceY = NULL;
  m_LineSourceX = NULL;

  m_TuberX = NULL;
  m_TuberY = NULL;
  m_TuberZ = NULL;

  m_LineMapperX = NULL;
  m_LineMapperY = NULL;
  m_LineMapperZ = NULL;

  m_ImageBounds[0] = 0;
  m_ImageBounds[1] = 0;
  m_ImageBounds[2] = 0;
  m_ImageBounds[3] = 0;
  m_ImageBounds[4] = 0;
  m_ImageBounds[5] = 0;

  m_CrossHairPositionObserver = CrossHairPositionObserver::New();

  m_CrossHairSpatialObject = NULL;
  this->RequestSetSpatialObject( m_CrossHairSpatialObject );

  //List of states
  igstkAddStateMacro( NullCrossHairObject  );
  igstkAddStateMacro( ValidCrossHairObject );

  // List of state machine inputs
  igstkAddInputMacro( ValidCrossHairObject );
  igstkAddInputMacro( NullCrossHairObject  );

  //from NullCrossHairObject

  igstkAddTransitionMacro( NullCrossHairObject, ValidCrossHairObject, 
                           ValidCrossHairObject,  SetCrossHairObject );

  igstkAddTransitionMacro( NullCrossHairObject, NullCrossHairObject, 
                           NullCrossHairObject,  No );

  //from ValidCrossHairObject

  igstkAddTransitionMacro( ValidCrossHairObject, NullCrossHairObject, 
                           NullCrossHairObject,  No ); 

  igstkAddTransitionMacro( ValidCrossHairObject, ValidCrossHairObject, 
                           ValidCrossHairObject,  No );

  m_StateMachine.SelectInitialState( m_NullCrossHairObjectState );

  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
CrossHairRepresentation
::~CrossHairRepresentation()  
{
  this->DeleteActors();

  if (m_LineSourceZ != NULL)
  {
    m_LineSourceZ->Delete();
    m_LineSourceZ=NULL;
  }

  if (m_LineSourceY != NULL)
  {
    m_LineSourceY->Delete();
    m_LineSourceY=NULL;
  }

  if (m_LineSourceX != NULL)
  {
    m_LineSourceX->Delete();
    m_LineSourceX=NULL;
  }

  if (m_TuberX != NULL)
  {
    m_TuberX->Delete();
    m_TuberX=NULL;
  }

  if (m_TuberY != NULL)
  {
    m_TuberY->Delete();
    m_TuberY=NULL;
  }

  if (m_TuberZ != NULL)
  {
    m_TuberZ->Delete();
    m_TuberZ=NULL;
  }

  if (m_LineMapperX != NULL)
  {
    m_LineMapperX->Delete();
    m_LineMapperX=NULL;
  }

  if (m_LineMapperY != NULL)
  {
    m_LineMapperY->Delete();
    m_LineMapperY=NULL;
  }

  if (m_LineMapperZ != NULL)
  {
    m_LineMapperZ->Delete();
    m_LineMapperZ=NULL;
  }

}

/** Verify time stamp of the attached tool*/
/*
template < class TImageSpatialObject >
bool
CrossHairRepresentation < TImageSpatialObject >
::VerifyTimeStamp( ) const
{
  igstkLogMacro( DEBUG, 
    "igstk::ImageResliceSpatialObjectRepresentation::VerifyTimeStamp called...\n");

  if( this->m_CrossHairSpatialObject.IsNull() )
    {
    return false;
    }

  // if a tool spatial object is driving the reslicing, compare the 
  //   tool spatial object transform with the view render time
  if( this->m_CrossHairSpatialObject->->IsToolSpatialObjectSet())
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
}*/

/** Request to Set the CrossHairSpatial Object */
void CrossHairRepresentation
::RequestSetCrossHairObject( const CrossHairSpatialObjectType * CrossHair )
{
  m_CrossHairSpatialObjectToAdd = const_cast< CrossHairSpatialObjectType*> ( CrossHair );

  if( !m_CrossHairSpatialObjectToAdd )
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
void CrossHairRepresentation
::NoProcessing()
{
}

/** Set the CrossHair Spatial Object */
void CrossHairRepresentation
::SetCrossHairObjectProcessing()
{
  m_CrossHairSpatialObject = m_CrossHairSpatialObjectToAdd;

  m_CrossHairSpatialObject->AddObserver( PointEvent(), m_CrossHairPositionObserver );

  this->RequestSetSpatialObject( m_CrossHairSpatialObject );

  //get image bounds (use a payloaded event instead)
  
  m_ImageBounds[0] = m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(0);
  m_ImageBounds[1] = m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(1);
  m_ImageBounds[2] = m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(2);
  m_ImageBounds[3] = m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(3);
  m_ImageBounds[4] = m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(4);
  m_ImageBounds[5] = m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(5);

    // build axial cross hair
  m_LineSourceZ = vtkLineSource::New();
  m_LineSourceZ->SetPoint1( 0, 0, 0 );
  m_LineSourceZ->SetPoint2( 1, 1, 1 );

  m_TuberX = vtkTubeFilter::New();
  m_TuberX->SetInput ( m_LineSourceZ->GetOutput() );
  m_TuberX->SetRadius (1);
  m_TuberX->SetNumberOfSides(3);

  m_LineMapperX = vtkPolyDataMapper::New();
  m_LineMapperX->SetInput( m_TuberX->GetOutput() );

 // build sagittal cross hair
  m_LineSourceY = vtkLineSource::New();
  m_LineSourceY->SetPoint1( 0, 0, 0 );
  m_LineSourceY->SetPoint2( 1, 1, 1 );

  m_TuberY = vtkTubeFilter::New();
  m_TuberY->SetInput ( m_LineSourceY->GetOutput() );
  m_TuberY->SetRadius (1);
  m_TuberY->SetNumberOfSides(3);

  m_LineMapperY = vtkPolyDataMapper::New();
  m_LineMapperY->SetInput( m_TuberY->GetOutput() );

 // build coronal cross hair
  m_LineSourceX = vtkLineSource::New();
  m_LineSourceX->SetPoint1( 0, 0, 0 );
  m_LineSourceX->SetPoint2( 1, 1, 1 );

  m_TuberZ = vtkTubeFilter::New();
  m_TuberZ->SetInput ( m_LineSourceX->GetOutput() );
  m_TuberZ->SetRadius (1);
  m_TuberZ->SetNumberOfSides(3);

  m_LineMapperZ = vtkPolyDataMapper::New();
  m_LineMapperZ->SetInput( m_TuberZ->GetOutput() );

} 

/** Requests to set the ReslicePlaneSpatialObject*/
/*
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
*/

/** Sets the ReslicePlaneSpatialObject*/
/*
template < class TImageSpatialObject >
void 
CrossHairRepresentation< TImageSpatialObject >
::SetReslicePlaneSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::CrossHairRepresentation\
                       ::SetReslicePlaneSpatialObjectProcessing called...\n");

  m_ReslicePlaneSpatialObject = m_ReslicePlaneSpatialObjectToBeSet;

  m_ReslicePlaneSpatialObject->RequestComputeReslicingPlane();  
 
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
  m_LineSourceZ = vtkLineSource::New();
  m_LineSourceZ->SetPoint1( 0, 0, 0 );
  m_LineSourceZ->SetPoint2( 1, 1, 1 );

  m_TuberX = vtkTubeFilter::New();
  m_TuberX->SetInput ( m_LineSourceZ->GetOutput() );
  m_TuberX->SetRadius (1);
  m_TuberX->SetNumberOfSides(3);

  m_LineMapperX = vtkPolyDataMapper::New();
  m_LineMapperX->SetInput( m_TuberX->GetOutput() );

 // build sagittal cross hair
  m_LineSourceY = vtkLineSource::New();
  m_LineSourceY->SetPoint1( 0, 0, 0 );
  m_LineSourceY->SetPoint2( 1, 1, 1 );

  m_TuberY = vtkTubeFilter::New();
  m_TuberY->SetInput ( m_LineSourceY->GetOutput() );
  m_TuberY->SetRadius (1);
  m_TuberY->SetNumberOfSides(3);

  m_LineMapperY = vtkPolyDataMapper::New();
  m_LineMapperY->SetInput( m_TuberY->GetOutput() );

 // build coronal cross hair
  m_LineSourceX = vtkLineSource::New();
  m_LineSourceX->SetPoint1( 0, 0, 0 );
  m_LineSourceX->SetPoint2( 1, 1, 1 );

  m_TuberZ = vtkTubeFilter::New();
  m_TuberZ->SetInput ( m_LineSourceX->GetOutput() );
  m_TuberZ->SetRadius (1);
  m_TuberZ->SetNumberOfSides(3);

  m_LineMapperZ = vtkPolyDataMapper::New();
  m_LineMapperZ->SetInput( m_TuberZ->GetOutput() );

}
*/
/** Print Self function */
void CrossHairRepresentation
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** Update the visual representation in response to changes in the geometric
 * object */
void CrossHairRepresentation
::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "CrossHairRepresentation::UpdateRepresentationProcessing called ....\n");

  m_CrossHairPositionObserver->Reset();

  m_CrossHairSpatialObject->RequestGetCrossHairPosition();
  
  if( m_CrossHairPositionObserver->GotCrossHairPosition() )
    {
      const PointType& position = m_CrossHairPositionObserver->GetCrossHairPosition();

      m_LineSourceY->SetPoint1( position[0], m_ImageBounds[2], position[2] );
      m_LineSourceY->SetPoint2( position[0], m_ImageBounds[3], position[2] );

      m_LineSourceX->SetPoint1( m_ImageBounds[0], position[1], position[2] );
      m_LineSourceX->SetPoint2( m_ImageBounds[1], position[1], position[2] );

      m_LineSourceZ->SetPoint1( position[0], position[1], m_ImageBounds[4] );
      m_LineSourceZ->SetPoint2( position[0], position[1], m_ImageBounds[5] );
    }
}

/** Sets actors visibility */
void CrossHairRepresentation
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
void CrossHairRepresentation
::CreateActors()
{
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  vtkActor* lineActorX = vtkActor::New();
  lineActorX->SetMapper (m_LineMapperX);

  vtkActor* lineActorY = vtkActor::New();
  lineActorY->SetMapper (m_LineMapperY);

  vtkActor* lineActorZ = vtkActor::New();
  lineActorZ->SetMapper (m_LineMapperZ);

  ColorScalarType r = this->GetRed();
  ColorScalarType g = this->GetGreen();
  ColorScalarType b = this->GetBlue();

  lineActorX->GetProperty()->SetColor( r,g,b );
  lineActorY->GetProperty()->SetColor( r,g,b );
  lineActorZ->GetProperty()->SetColor( r,g,b );

  this->AddActor( lineActorX );
  this->AddActor( lineActorY );
  this->AddActor( lineActorZ );
}

/** Create a copy of the current object representation */
CrossHairRepresentation::Pointer
CrossHairRepresentation::Copy() const
{
  Pointer newOR = CrossHairRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetCrossHairObject(m_CrossHairSpatialObject);

  return newOR;
}


} // end namespace igstk
