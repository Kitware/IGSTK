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
#ifndef __igstkObliqueImageSpatialObjectRepresentation_txx
#define __igstkObliqueImageSpatialObjectRepresentation_txx

#include "itkCommand.h"
#include "itkImage.h"
#include "itkMatrix.h"

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
  m_ReslicedImageData  = NULL;

  // Image reslice
  m_ImageReslice = vtkImageReslice::New();


  // Set default values for window and level
  m_Level = 0;
  m_Window = 2000;
  
  igstkAddInputMacro( ValidImageSpatialObject );
  igstkAddInputMacro( NullImageSpatialObject  );

  igstkAddInputMacro( ValidOriginPointOnThePlane );
  igstkAddInputMacro( InValidOriginPointOnThePlane  );

  igstkAddInputMacro( ValidVector1OnThePlane );
  igstkAddInputMacro( InValidVector1OnThePlane  );

  igstkAddInputMacro( ValidVector2OnThePlane );
  igstkAddInputMacro( InValidVector2OnThePlane  );

  igstkAddInputMacro( EmptyImageSpatialObject  );
  igstkAddInputMacro( ConnectVTKPipeline );

  igstkAddInputMacro( Reslice );
   
  igstkAddStateMacro( NullImageSpatialObject );
  igstkAddStateMacro( ValidImageSpatialObject );

  igstkAddStateMacro( NullOriginPointOnThePlane );
  igstkAddStateMacro( ValidOriginPointOnThePlane );
  
  igstkAddStateMacro( NullVector1OnthePlane );
  igstkAddStateMacro( ValidVector1OnThePlane );

  igstkAddStateMacro( NullVector2OnthePlane );
  igstkAddStateMacro( ValidVector2OnThePlane );

  igstkAddTransitionMacro( NullImageSpatialObject, NullImageSpatialObject, 
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, EmptyImageSpatialObject,
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, ValidImageSpatialObject,
                           ValidImageSpatialObject, SetImageSpatialObject );

  igstkAddTransitionMacro( NullOriginPointOnThePlane, InValidOriginPointOnThePlane, 
                           NullOriginPointOnThePlane,  No );
  igstkAddTransitionMacro( NullOriginPointOnThePlane, ValidOriginPointOnThePlane,
                           ValidOriginPointOnThePlane, SetOriginPointOnThePlane );

  igstkAddTransitionMacro( NullImageSpatialObject, InValidOriginPointOnThePlane, 
                           NullImageSpatialObject, No );
  igstkAddTransitionMacro( ValidImageSpatialObject, InValidOriginPointOnThePlane, 
                           ValidImageSpatialObject, No );

  igstkAddTransitionMacro( NullImageSpatialObject, ValidOriginPointOnThePlane, 
                           ValidOriginPointOnThePlane,SetOriginPointOnThePlane );

  igstkAddTransitionMacro( ValidImageSpatialObject, ValidOriginPointOnThePlane,
                           ValidOriginPointOnThePlane, SetOriginPointOnThePlane );

  igstkAddTransitionMacro( ValidOriginPointOnThePlane, ValidVector1OnThePlane, 
                           ValidVector1OnThePlane, SetVector1OnThePlane );

  igstkAddTransitionMacro( ValidVector1OnThePlane, ValidVector2OnThePlane,
                           ValidVector2OnThePlane, SetVector2OnThePlane );

  igstkAddTransitionMacro( ValidVector2OnThePlane, Reslice, ValidVector2OnThePlane , Reslice);
  igstkAddTransitionMacro( ValidImageSpatialObject, Reslice, ValidImageSpatialObject , Reslice);

  igstkAddTransitionMacro( ValidImageSpatialObject, NullImageSpatialObject, 
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, EmptyImageSpatialObject, 
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, ValidImageSpatialObject, 
                           ValidImageSpatialObject,  No ); 

  igstkAddTransitionMacro( NullImageSpatialObject, ConnectVTKPipeline, 
                           NullImageSpatialObject, No );
  igstkAddTransitionMacro( ValidImageSpatialObject, ConnectVTKPipeline, 
                           ValidImageSpatialObject, ConnectVTKPipeline );

  igstkAddTransitionMacro( ValidVector2OnThePlane, ConnectVTKPipeline, 
                           ValidVector2OnThePlane, ConnectVTKPipeline );

 
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
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation\
                        ::DeleteActors called...\n");
   
  this->Superclass::DeleteActors();
  
  m_ImageActor = NULL;

}
 
/** Set the Image Spatial Object */
template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetImageSpatialObject( const ImageSpatialObjectType * image )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation\
                        ::RequestSetImageSpatialObject called...\n");
  
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

/** Set the origin point on the Oblique plane */
template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetOriginPointOnThePlane( const PointType & point )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::RequestSetOriginPointOnThePlane called...\n");
  
  m_OriginPointOnThePlaneToBeSet = point;

  // check if it is a valid point
  m_StateMachine.PushInput( m_ValidOriginPointOnThePlaneInput );
  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::SetOriginPointOnThePlaneProcessing( )
{
  igstkLogMacro( DEBUG, 
       "igstk::ObliqueImageSpatialObjectRepresentation::SetOriginPointOnThePlaneProcessing called...\n");
  
  m_OriginPointOnThePlane = m_OriginPointOnThePlaneToBeSet;
}

/** Set vector 1 on the Oblique plane */
template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetVector1OnThePlane( const VectorType & vector )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::\
                                 RequestSetVector1OnThePlane called...\n");
  
  m_Vector1OnThePlaneToBeSet = vector;

  // check if it is a valid vector
  m_StateMachine.PushInput( m_ValidVector1OnThePlaneInput );
  m_StateMachine.ProcessInputs();
}

/** Set vector 2 on the Oblique plane */
template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetVector2OnThePlane( const VectorType & vector )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::\
                                 RequestSetVector2OnThePlane called...\n");
  
  m_Vector2OnThePlaneToBeSet = vector;

  // check if it is a valid vector
  m_StateMachine.PushInput( m_ValidVector2OnThePlaneInput );
  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::SetVector1OnThePlaneProcessing( )
{
  igstkLogMacro( DEBUG, 
       "igstk::ObliqueImageSpatialObjectRepresentation::SetVector1OnThePlaneProcessing called...\n");
  
  m_Vector1OnThePlane = m_Vector1OnThePlaneToBeSet;
}


template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::SetVector2OnThePlaneProcessing( )
{
  igstkLogMacro( DEBUG, 
       "igstk::ObliqueImageSpatialObjectRepresentation::SetVector2OnThePlaneProcessing called...\n");
  
  m_Vector2OnThePlane = m_Vector2OnThePlaneToBeSet;
}

/** Request reslicing */
template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestReslice(  )
{
  igstkLogMacro( DEBUG, 
           "igstk::ObliqueImageSpatialObjectRepresentation\
           ::RequestReslice called...\n");
  
  m_StateMachine.PushInput( m_ResliceInput );
  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::ResliceProcessing(  )
{
  igstkLogMacro( DEBUG, 
           "igstk::ObliqueImageSpatialObjectRepresentation\
           ::ResliceProcessing called...\n");

  std::cout << "Image Data before reslicing " << std::endl;
  m_ImageData->Print( std::cout );

  m_ImageReslice->SetInput ( m_ImageData );
  
  // Set the reslice axes origin
  m_ImageReslice->SetResliceAxesOrigin( this->m_OriginPointOnThePlane[0], 
                                        this->m_OriginPointOnThePlane[1], 
                                        this->m_OriginPointOnThePlane[2] );
  VectorType v1;
  VectorType v2;
  VectorType v3;  
   
  // Check if vector v1 and v2 are orthogonal

  v1 = this->m_Vector1OnThePlane;
  v2 = this->m_Vector2OnThePlane;
  
  double dot_product;
  dot_product = v1*v2;

  if ( dot_product != 0.0 )
    {
    // FIXME: need to handle this situation too
    std::cerr<< "The two vectors are not perpendicular" << std::endl;        
    }

  // Generate vector v3 
  v3 = itk::CrossProduct( v1, v2 ); 
  
  VectorType v1Normalized;
  VectorType v2Normalized;
  VectorType v3Normalized;
  
  v1Normalized = v1/v1.GetNorm();
  v2Normalized = v2/v2.GetNorm();
  v3Normalized = v3/v3.GetNorm();

  std::cout << "V1:" << v1Normalized[0] << "," << v1Normalized[1] \
                    << "," << v1Normalized[2] << std::endl;
  std::cout << "V2:" << v2Normalized[0] << "," << v2Normalized[1] \
                    << "," << v2Normalized[2] << std::endl;
  std::cout << "V3:" << v3Normalized[0] << "," << v3Normalized[1] \
                    << "," << v3Normalized[2] << std::endl;
                
  // set the reslice axes 
  m_ImageReslice->SetResliceAxesDirectionCosines( 
                 v1Normalized[0], v1Normalized[1], v1Normalized[2],
                 v2Normalized[0], v2Normalized[1], v2Normalized[2],
                 v3Normalized[0], v3Normalized[1], v3Normalized[2]
               );
                  
  // set the output dimensionality to 2
  m_ImageReslice->SetOutputDimensionality(2);

  // Set the output spacing 
  double spacing[3];
  m_ImageData->GetSpacing( spacing );
  m_ImageReslice->SetOutputSpacing( spacing[0],spacing[1], 1.0 );

  // Set the output extent
  int ext[6];
  m_ImageData->GetExtent( ext );
  m_ImageReslice->SetOutputExtent( ext[0], ext[1], ext[2], 
                                     ext[3], 0, 0);
  // Set the output origin
//  m_ImageReslice->SetOutputOrigin( 0.0, 0.0, 0.0 );

  double origin[6];
  m_ImageData->GetOrigin( origin );
  m_ImageReslice->SetOutputOrigin( origin );
  
  m_ImageReslice->Update();
  m_ImageReslice->Print( std::cout );

  m_ReslicedImageData = m_ImageReslice->GetOutput();   
  std::cout << "Image data after reslicing " << std::endl;
  m_ReslicedImageData->Print( std::cout );
 
}

template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::SetWindowLevel( double window, double level )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation\
                        ::SetWindowLevel called...\n");

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
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation\
                         ::SetImageSpatialObjectProcessing called...\n");

  // We create the image spatial object
  m_ImageSpatialObject = m_ImageSpatialObjectToAdd;

  this->RequestSetSpatialObject( m_ImageSpatialObject );
  
  // This method gets a VTK image data from the private method of the
  // ImageSpatialObject and stores it in the representation by invoking the
  // private SetImage method.
  this->ConnectImage();

  m_MapColors->SetInput( m_ImageData ) ;

  m_ImageActor->SetInput( m_MapColors->GetOutput() );
}


/** Print Self function */
template < class TImageSpatialObject >
void
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Origin Point on the plane";
  os << indent << this->m_OriginPointOnThePlane << std::endl;
  os << indent << "Vector 1 on the plane";
  os << indent << this->m_Vector1OnThePlane << std::endl;
  os << indent << "Vector 2 on the plane";
  os << indent << this->m_Vector2OnThePlane << std::endl;
}


/** Update the visual representation in response to changes in the geometric
 * object */
template < class TImageSpatialObject >
void
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::UpdateRepresentationProcessing()
{
   igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::\
                         UpdateRepresentationProcessing called...\n");
   if( m_ReslicedImageData )
     {
     m_MapColors->SetInput( m_ReslicedImageData );
     }
}


/** Create the vtk Actors */
template < class TImageSpatialObject >
void
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::CreateActors()
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation\
                        ::CreateActors called...\n");

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
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation\
                        ::Copy called...\n");

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
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation\
                        ::SetImage called...\n");

  // This const_cast<> is needed here due to 
  // the lack of const-correctness in VTK 
  m_ImageData = const_cast< vtkImageData *>( image );
}

template < class TImageSpatialObject >
void
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::ConnectImage()
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation\
                        ::ConnectImage called...\n");

  typedef Friends::ObliqueImageSpatialObjectRepresentationToImageSpatialObject  
                                                                   HelperType;
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
  m_MapColors->SetInput( m_ReslicedImageData );
  m_ImageActor->SetInput( m_MapColors->GetOutput() );
  m_ImageActor->InterpolateOn();
}

} // end namespace igstk

#endif
