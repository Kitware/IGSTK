/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkObliqueImageSpatialObjectRepresentation.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkCommand.h"
#include "itkImage.h"

#include "igstkObliqueImageSpatialObjectRepresentation.h"

#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"


#include "igstkEvents.h"

#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"

namespace igstk
{

/** Constructor */

template < class TImageSpatialObject >
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::ObliqueImageSpatialObjectRepresentation():m_StateMachine(this)
{
  // We create the image spatial object
  m_ImageSpatialObject = NULL;

  this->RequestSetSpatialObject( m_ImageSpatialObject );

  // Create classes for displaying images
  m_ImageActor = vtkImageActor::New();
  this->AddActor( m_ImageActor );
  m_MapColors  = vtkImageMapToColors::New();
  m_LUT        = vtkLookupTable::New();
  m_ImageData  = NULL;

  // Image reslice
  m_ImageReslice = vtkImageReslice::New();      


  // Set default values for window and level
  m_Level = 0;
  m_Window = 2000;
  
  igstkAddInputMacro( ValidImageSpatialObject );
  igstkAddInputMacro( NullImageSpatialObject  );
  igstkAddInputMacro( ValidPointOnThePlane );
  igstkAddInputMacro( InValidPointOnThePlane  );
  igstkAddInputMacro( ValidPlaneNormalVector );
  igstkAddInputMacro( InValidPlaneNormalVector  );

  igstkAddInputMacro( EmptyImageSpatialObject  );
  igstkAddInputMacro( ConnectVTKPipeline );

  igstkAddInputMacro( Reslice );
   
  igstkAddStateMacro( NullImageSpatialObject );
  igstkAddStateMacro( ValidImageSpatialObject );

  igstkAddStateMacro( NullPointOnthePlane );
  igstkAddStateMacro( ValidPointOnThePlane );

  igstkAddStateMacro( NullPlaneNormalVector );
  igstkAddStateMacro( ValidPlaneNormalVector );


  igstkAddTransitionMacro( NullImageSpatialObject, NullImageSpatialObject, NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, EmptyImageSpatialObject, NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, ValidImageSpatialObject, ValidImageSpatialObject,  SetImageSpatialObject );

  igstkAddTransitionMacro( NullPointOnthePlane, InValidPointOnThePlane, NullPointOnthePlane,  No );
  igstkAddTransitionMacro( NullPointOnthePlane, ValidPointOnThePlane, ValidPointOnThePlane,  
                                                                               SetPointOnthePlane );

  igstkAddTransitionMacro( NullImageSpatialObject, InValidPointOnThePlane, NullImageSpatialObject, No );
  igstkAddTransitionMacro( ValidImageSpatialObject, InValidPointOnThePlane, ValidImageSpatialObject, No );

   igstkAddTransitionMacro( NullImageSpatialObject, ValidPointOnThePlane, ValidPointOnThePlane,
                                                                                SetPointOnthePlane );
  igstkAddTransitionMacro( ValidImageSpatialObject, ValidPointOnThePlane, ValidPointOnThePlane, 
                                                                                SetPointOnthePlane );


  igstkAddTransitionMacro( NullPlaneNormalVector, InValidPlaneNormalVector, NullPlaneNormalVector,  No );
  igstkAddTransitionMacro( NullPlaneNormalVector, ValidPlaneNormalVector, ValidPlaneNormalVector,  
                                                                               SetPlaneNormalVector );

  igstkAddTransitionMacro( ValidPointOnThePlane, InValidPlaneNormalVector, ValidPointOnThePlane, No );
  igstkAddTransitionMacro( ValidPointOnThePlane, ValidPlaneNormalVector, ValidPlaneNormalVector, 
                                                                               SetPlaneNormalVector );


  igstkAddTransitionMacro( ValidPlaneNormalVector, Reslice, ValidPlaneNormalVector , Reslice);

  igstkAddTransitionMacro( ValidImageSpatialObject, NullImageSpatialObject, NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, EmptyImageSpatialObject, NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, ValidImageSpatialObject, ValidImageSpatialObject,  No ); 

  igstkAddTransitionMacro( NullImageSpatialObject, ConnectVTKPipeline, NullImageSpatialObject, No );
  igstkAddTransitionMacro( ValidImageSpatialObject, ConnectVTKPipeline, ValidImageSpatialObject, ConnectVTKPipeline );
  igstkSetInitialStateMacro( NullImageSpatialObject );

  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
template < class TImageSpatialObject >
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::~ObliqueImageSpatialObjectRepresentation()  
{

  // This deletes also the m_ImageActor
  this->DeleteActors();


  if( m_MapColors )
    {
    m_MapColors->SetLookupTable( NULL );
    m_MapColors->SetInput( NULL );
    m_MapColors->Delete();
    m_MapColors = NULL;
    }

    
  if( m_LUT )
    {
    m_LUT->Delete();
    m_LUT = NULL;
    }
 
  if( m_ImageReslice )
    {
    m_ImageReslice->Delete();
    m_ImageReslice = NULL;
    }
}

/** Overloaded DeleteActor function*/
template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::DeleteActors( )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::DeleteActors called...\n");
  
  this->Superclass::DeleteActors();
  
  m_ImageActor = NULL;

}
 
/** Set the Image Spatial Object */
template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetImageSpatialObject( const ImageSpatialObjectType * image )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::RequestSetImageSpatialObject called...\n");
  
  m_ImageSpatialObjectToAdd = image;

  if( !m_ImageSpatialObjectToAdd )
    {
    m_StateMachine.PushInput( m_NullImageSpatialObjectInput );
    }
  else 
    {
    if( m_ImageSpatialObjectToAdd->IsEmpty() )
      {
      m_StateMachine.PushInput( m_EmptyImageSpatialObjectInput );
      }
    else
      {
      m_StateMachine.PushInput( m_ValidImageSpatialObjectInput );
      }
    }
  
  m_StateMachine.ProcessInputs();
}

/** Set the Point on the Oblique plane */
template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetPointOnthePlane( const PointType & point )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::RequestSetPointOnthePlane called...\n");
  
  m_PointOnthePlaneToBeSet = point;

  // check if it is a valid point
  m_StateMachine.PushInput( m_ValidPointOnThePlaneInput );
  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::SetPointOnthePlaneProcessing( )
{
  igstkLogMacro( DEBUG, 
       "igstk::ObliqueImageSpatialObjectRepresentation::SetPointOnthePlaneProcessing called...\n");
  
  m_PointOnthePlane = m_PointOnthePlaneToBeSet;
}

/** Set the Normal Vector of the Oblique plane */
template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetPlaneNormalVector( const VectorType & vector )
{
  igstkLogMacro( DEBUG, 
           "igstk::ObliqueImageSpatialObjectRepresentation::RequestSetPlaneNormalVector called...\n");
  
  m_PlaneNormalVectorToBeSet = vector;

  // Check if it is valid
  m_StateMachine.PushInput( m_ValidPlaneNormalVectorInput );
  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::SetPlaneNormalVectorProcessing( )
{
  igstkLogMacro( DEBUG, 
       "igstk::ObliqueImageSpatialObjectRepresentation::SetPlaneNormalVectorProcessing called...\n");
  
  m_PlaneNormalVector = m_PlaneNormalVectorToBeSet;
}

/** Request reslicing */
template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestReslice(  )
{
  igstkLogMacro( DEBUG, 
           "igstk::ObliqueImageSpatialObjectRepresentation::RequestReslice called...\n");
  
  m_StateMachine.PushInput( m_ResliceInput );
  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::ResliceProcessing(  )
{
  igstkLogMacro( DEBUG, 
           "igstk::ObliqueImageSpatialObjectRepresentation::ResliceProcessing called...\n");

  std::cout << "Image Data after before reslicing " << std::endl;

  m_ImageReslice->SetInput ( m_ImageData );
 
  // Set the reslicing plane axes 
  vtkMatrix4x4         * resliceAxes;

  resliceAxes = vtkMatrix4x4::New();

  resliceAxes->Identity();

  // Set the x-axis 
  resliceAxes->SetElement( 0, 0, 1.0);
  resliceAxes->SetElement( 1, 0, 0.0);
  resliceAxes->SetElement( 2, 0, 0.0);
  
  // Set the y-axis
  resliceAxes->SetElement( 0, 0, 0.0);
  resliceAxes->SetElement( 1, 0, 1.0);
  resliceAxes->SetElement( 2, 0, 0.0);
 
  // Set the z-axis
  resliceAxes->SetElement( 0, 0, 0.0);
  resliceAxes->SetElement( 1, 0, 0.0);
  resliceAxes->SetElement( 2, 0, 1.0);
 
  // Set the origin 
  resliceAxes->SetElement( 0, 0, 128.0);
  resliceAxes->SetElement( 1, 0, 128.0);
  resliceAxes->SetElement( 2, 0, 2.0);

  m_ImageReslice->SetResliceAxes( resliceAxes );

 
  // Set the spacing 
  double spacing[3];
  m_ImageData->GetSpacing( spacing );
  m_ImageReslice->SetOutputSpacing( spacing[0],spacing[1],spacing[2] );

  // Set the output extent

  int ext[6];
  m_ImageData->GetExtent( ext );

  m_ImageReslice->SetOutputExtent( ext[0], ext[1], ext[2], 
                                     ext[3], ext[4], ext[5]);
  m_ImageReslice->Update();

  m_ImageData = m_ImageReslice->GetOutput();   

  std::cout << "Image data after reslicing " << std::endl;

  m_ImageData->Print( std::cout );
  resliceAxes->Delete(); 
}

template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::SetWindowLevel( double window, double level )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::SetWindowLevel called...\n");

  m_Window = window;
  m_Level = level;

  m_LUT->SetTableRange ( (m_Level - m_Window/2.0), (m_Level + m_Window/2.0) );
}

/** Null Operation for a State Machine Transition */
template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::NoProcessing()
{
}

/** Set nhe Image Spatial Object */
template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::SetImageSpatialObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::SetImageSpatialObjectProcessing called...\n");

  // We create the image spatial object
  m_ImageSpatialObject = m_ImageSpatialObjectToAdd;

  this->RequestSetSpatialObject( m_ImageSpatialObject );
  
  // This method gets a VTK image data from the private method of the
  // ImageSpatialObject and stores it in the representation by invoking the
  // private SetImage method.
  this->ConnectImage();

  m_MapColors->SetInput( m_ImageData );

  m_ImageActor->SetInput( m_MapColors->GetOutput() );
}


/** Print Self function */
template < class TImageSpatialObject >
void
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  std::cout << indent << "Point on the plane" << this->m_PointOnthePlane << std::endl;
  std::cout << indent << "Plane normal vector" << this->m_PlaneNormalVector << std::endl;
}


/** Update the visual representation in response to changes in the geometric
 * object */
template < class TImageSpatialObject >
void
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::UpdateRepresentationProcessing()
{    
   igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::UpdateRepresentationProcessing called...\n");
   if( m_ImageData )
     {
     m_MapColors->SetInput( m_ImageData );
     }
}


/** Create the vtk Actors */
template < class TImageSpatialObject >
void
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::CreateActors()
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::CreateActors called...\n");

  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  m_ImageActor = vtkImageActor::New();
  this->AddActor( m_ImageActor );
    
  m_LUT->SetTableRange ( (m_Level - m_Window/2.0), (m_Level + m_Window/2.0) );
  m_LUT->SetSaturationRange (0, 0);
  m_LUT->SetHueRange (0, 0);
  m_LUT->SetValueRange (0, 1);
  m_LUT->SetRampToLinear();

  m_MapColors->SetLookupTable( m_LUT );

  igstkPushInputMacro( ConnectVTKPipeline );
  m_StateMachine.ProcessInputs(); 

}

/** Create a copy of the current object representation */
template < class TImageSpatialObject >
typename ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >::Pointer
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::Copy() const
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::Copy called...\n");

  Pointer newOR = ObliqueImageSpatialObjectRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetImageSpatialObject(m_ImageSpatialObject);

  return newOR;
}

  
template < class TImageSpatialObject >
void
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::SetImage( const vtkImageData * image )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::SetImage called...\n");

  // This const_cast<> is needed here due to the lack of const-correctness in VTK 
  m_ImageData = const_cast< vtkImageData *>( image );
}



template < class TImageSpatialObject >
void
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::ConnectImage()
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::ConnectImage called...\n");

  typedef Friends::ObliqueImageSpatialObjectRepresentationToImageSpatialObject  HelperType;
  HelperType::ConnectImage( m_ImageSpatialObject.GetPointer(), this );
  if( m_ImageData )
    {
    m_ImageData->Update();
    }
}


template < class TImageSpatialObject >
void
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::ConnectVTKPipelineProcessing() 
{
  m_MapColors->SetInput( m_ImageData );
  m_ImageActor->SetInput( m_MapColors->GetOutput() );
  m_ImageActor->InterpolateOn();
}


} // end namespace igstk

