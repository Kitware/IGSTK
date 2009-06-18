/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCrossHairObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkCrossHairObjectRepresentation.h"
#include "igstkEvents.h"

#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>


namespace igstk
{ 

/** Constructor */
CrossHairObjectRepresentation::
CrossHairObjectRepresentation():m_StateMachine(this)
{

  m_LineSourceZ = NULL;
  m_LineSourceY = NULL;
  m_LineSourceX = NULL;
  m_LineProperty = NULL;

  m_ImageBounds[0] = 0;
  m_ImageBounds[1] = 0;
  m_ImageBounds[2] = 0;
  m_ImageBounds[3] = 0;
  m_ImageBounds[4] = 0;
  m_ImageBounds[5] = 0;

  m_LineWidth = 2;

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
CrossHairObjectRepresentation
::~CrossHairObjectRepresentation()  
{
  this->DeleteActors();
  
  if (m_LineProperty != NULL)
    {
    m_LineProperty->Delete();
    m_LineProperty=NULL;
    }

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
}

/** Set the actor's visibility */
void 
CrossHairObjectRepresentation
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

/** Set the line's width */
void 
CrossHairObjectRepresentation
::SetLineWidth(double width)
{
  if( m_LineWidth == width )
    {
    return;
    }
  m_LineWidth = width;

  if ( m_LineProperty != NULL )
    {
    m_LineProperty->SetLineWidth(m_LineWidth);
    }
}

/** Verify time stamp of the attached tool */
bool
CrossHairObjectRepresentation
::VerifyTimeStamp( ) const
{
  igstkLogMacro( DEBUG, 
    "igstk::CrossHairObjectRepresentation::VerifyTimeStamp called...\n");

  if ( !m_CrossHairSpatialObject->IsToolSpatialObjectSet() )
    {
    return true;
    }

  if ( !m_CrossHairSpatialObject->IsInsideBounds() )
    {
    return false;
    }

  // if a tool spatial object is driving the reslicing, compare the 
  // tool spatial object transform with the view render time

  if( this->GetRenderTimeStamp().GetStartTime() >
      this->m_CrossHairSpatialObject->GetToolTransform().GetExpirationTime() )
    {
    // fixme
    double diff = 
    this->GetRenderTimeStamp().GetStartTime() - 
    this->m_CrossHairSpatialObject->GetToolTransform().GetExpirationTime();

    if (diff > 150 )
      {
      return false;
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
void 
CrossHairObjectRepresentation
::RequestSetCrossHairObject( const CrossHairType * crossHair )
{
  m_CrossHairSpatialObjectToAdd = const_cast< CrossHairType*> ( crossHair );

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
void 
CrossHairObjectRepresentation
::NoProcessing()
{
}

/** Set the CrossHair Spatial Object */
void 
CrossHairObjectRepresentation
::SetCrossHairObjectProcessing()
{
  m_CrossHairSpatialObject = m_CrossHairSpatialObjectToAdd;

  m_CrossHairSpatialObject->AddObserver( PointEvent(),
                                         m_CrossHairPositionObserver );

  this->RequestSetSpatialObject( m_CrossHairSpatialObject );

  //get image bounds (use a payloaded event instead)
  
  m_ImageBounds[0]= m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(0);
  m_ImageBounds[1]= m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(1);
  m_ImageBounds[2]= m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(2);
  m_ImageBounds[3]= m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(3);
  m_ImageBounds[4]= m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(4);
  m_ImageBounds[5]= m_CrossHairSpatialObject->GetBoundingBoxDimensionByIndex(5);

} 

void 
CrossHairObjectRepresentation
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** Update the visual representation in response to changes in the geometric
 * object */
void 
CrossHairObjectRepresentation
::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, 
   "CrossHairObjectRepresentation::UpdateRepresentationProcessing called ..\n");

  m_CrossHairPositionObserver->Reset();

  m_CrossHairSpatialObject->RequestGetCrossHairPosition();
  
  if( !m_CrossHairPositionObserver->GotCrossHairPosition() )
  return;

  const PointType& position = 
    m_CrossHairPositionObserver->GetCrossHairPosition();

  m_LineSourceY->SetPoint1( position[0], m_ImageBounds[2], position[2] );
  m_LineSourceY->SetPoint2( position[0], m_ImageBounds[3], position[2] );

  m_LineSourceX->SetPoint1( m_ImageBounds[0], position[1], position[2] );
  m_LineSourceX->SetPoint2( m_ImageBounds[1], position[1], position[2] );

  m_LineSourceZ->SetPoint1( position[0], position[1], m_ImageBounds[4] );
  m_LineSourceZ->SetPoint2( position[0], position[1], m_ImageBounds[5] );
}

/** Create the vtk Actors */
void CrossHairObjectRepresentation
::CreateActors()
{
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  m_LineProperty = vtkProperty::New();
  m_LineProperty->SetAmbient(1);
  m_LineProperty->SetRepresentationToWireframe();
  m_LineProperty->SetInterpolationToFlat();
  m_LineProperty->SetLineWidth(m_LineWidth);
  m_LineProperty->SetColor( this->GetRed(),this->GetGreen(),this->GetBlue() );

  // build x cross hair

  //give any initial value
  m_LineSourceX = vtkLineSource::New();
  m_LineSourceX->SetPoint1( 0, 0, 0 );
  m_LineSourceX->SetPoint2( 0, 1, 0 );

  vtkPolyDataMapper* lineMapperX = vtkPolyDataMapper::New();
  lineMapperX->SetInput( m_LineSourceX->GetOutput() );
  lineMapperX->SetResolveCoincidentTopologyToPolygonOffset();
  lineMapperX->SetResolveCoincidentTopologyPolygonOffsetParameters(10,10);

  vtkActor* lineActorX = vtkActor::New();
  lineActorX->SetMapper (lineMapperX);
  lineActorX->SetProperty(m_LineProperty);

  lineMapperX->Delete();
  this->AddActor( lineActorX );

  // build y cross hair

  //give any initial value
  m_LineSourceY = vtkLineSource::New();
  m_LineSourceY->SetPoint1( 0, 0, 0 );
  m_LineSourceY->SetPoint2( 0, 1, 0 );

  vtkPolyDataMapper* lineMapperY = vtkPolyDataMapper::New();
  lineMapperY->SetInput( m_LineSourceY->GetOutput() );
  lineMapperY->SetResolveCoincidentTopologyToPolygonOffset();
  lineMapperY->SetResolveCoincidentTopologyPolygonOffsetParameters(10,10);

  vtkActor* lineActorY = vtkActor::New();
  lineActorY->SetMapper (lineMapperY);
  lineActorY->SetProperty(m_LineProperty);

  lineMapperY->Delete();
  this->AddActor( lineActorY );

  // build z cross hair

  //give any initial value
  m_LineSourceZ = vtkLineSource::New();
  m_LineSourceZ->SetPoint1( 0, 0, 0 );
  m_LineSourceZ->SetPoint2( 0, 0, 1 );

  vtkPolyDataMapper* lineMapperZ = vtkPolyDataMapper::New();
  lineMapperZ->SetInput( m_LineSourceZ->GetOutput() );
  lineMapperZ->SetResolveCoincidentTopologyToPolygonOffset();
  lineMapperZ->SetResolveCoincidentTopologyPolygonOffsetParameters(10,10);

  vtkActor* lineActorZ = vtkActor::New();
  lineActorZ->SetMapper (lineMapperZ);
  lineActorZ->SetProperty(m_LineProperty);

  lineMapperZ->Delete();
  this->AddActor( lineActorZ );
}

/** Create a copy of the current object representation */
CrossHairObjectRepresentation::Pointer
CrossHairObjectRepresentation::Copy() const
{
  Pointer newOR = CrossHairObjectRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->SetLineWidth(this->GetLineWidth());  
  newOR->RequestSetCrossHairObject(m_CrossHairSpatialObject);

  return newOR;
}


} // end namespace igstk
