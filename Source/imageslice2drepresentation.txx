#ifndef CXIMAGESLICE2DREPRESENTATION_TXX_
#define CXIMAGESLICE2DREPRESENTATION_TXX_

#include "imageslice2drepresentation.h"
#include "cxcommon.h"

#include <igstkEvents.h>

#include <itkCommand.h>
#include <itkImage.h>
#include <itkCastImageFilter.h>
#include <itkImageFileReader.h>

#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkImageData.h>
#include <vtkCallbackCommand.h>

namespace cx
{
template <class TImageSpatialObject> void 
ImageSlice2dRepresentation<TImageSpatialObject>
::CursoringCallback(vtkObject * caller, unsigned long eventId, void *clientData, 
                    void *callData)
{
  ImageSlice2dRepresentation<TImageSpatialObject> *owning =
    reinterpret_cast<ImageSlice2dRepresentation<TImageSpatialObject> *>(clientData);
  vtkImagePlaneWidget2D *imagePlane = dynamic_cast<vtkImagePlaneWidget2D *>(caller);
  if(imagePlane == NULL)
  {
    return;
  }
  if((eventId == vtkCommand::InteractionEvent) || (eventId == vtkCommand::StartInteractionEvent))
  {
    switch(imagePlane->GetState())
    {
    case vtkImagePlaneWidget2D::WINDOWLEVELLING:
    {      
      double windowLevel[2];
      imagePlane->GetWindowLevel(windowLevel);
      
      WindowLevelType theWindowLevel;
      theWindowLevel.window = windowLevel[0];
      theWindowLevel.level = windowLevel[1];
      WindowLevelEvent event;
      event.Set(theWindowLevel);
      owning->InvokeEvent(event);
      break;
    }
    case vtkImagePlaneWidget2D::CURSORING:
    {
      double x, y, z;
      imagePlane->GetCurrentWorldCursorPosition(x, y, z);
     
      igstk::EventHelperType::PointType point;
      point.SetElement(0, x);
      point.SetElement(1, y);
      point.SetElement(2, z);
      CursorEvent event;
      event.Set(point);
      owning->InvokeEvent(event);
      break;
    }
    }
  }
}
template < class TImageSpatialObject >
ImageSlice2dRepresentation< TImageSpatialObject >
::ImageSlice2dRepresentation() : 
    m_StateMachine(this),
    m_ImageSpatialObject(NULL),
    m_ImageSpatialObjectToAdd(NULL),
    m_ImageData(NULL),
    m_ImagePlane(vtkImagePlaneWidget2D::New()),
    m_Orientation(Axial),
    m_VTKImageObserver(VTKImageObserver::New())
{
  this->RequestSetSpatialObject( m_ImageSpatialObject );
  
  vtkCallbackCommand *cbc = vtkCallbackCommand::New();
  cbc->SetCallback(ImageSlice2dRepresentation::CursoringCallback);
  cbc->SetClientData(this);
  m_ImagePlane->AddObserver(vtkCommand::InteractionEvent, cbc);
  m_ImagePlane->AddObserver(vtkCommand::StartInteractionEvent, cbc);
  cbc->Delete();
  
  igstkAddInputMacro( ValidImageSpatialObject );
  igstkAddInputMacro( NullImageSpatialObject  );
  igstkAddInputMacro( EmptyImageSpatialObject  );
  igstkAddInputMacro( SetSliceNumber  );
  igstkAddInputMacro( ValidSliceNumber  );
  igstkAddInputMacro( InvalidSliceNumber  );
  igstkAddInputMacro( ValidOrientation  );
  igstkAddInputMacro( RequestSliceNumberBounds);
  igstkAddInputMacro( ConnectVTKPipeline );

  igstkAddStateMacro( NullImageSpatialObject );
  igstkAddStateMacro( ValidImageSpatialObject );
  igstkAddStateMacro( ValidImageOrientation );
  igstkAddStateMacro( ValidSliceNumber );
  igstkAddStateMacro( AttemptingToSetSliceNumber );

  igstkAddTransitionMacro( NullImageSpatialObject, NullImageSpatialObject,
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, EmptyImageSpatialObject,
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, ValidImageSpatialObject,
                           ValidImageSpatialObject,  SetImageSpatialObject );
  igstkAddTransitionMacro( NullImageSpatialObject, SetSliceNumber,
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, ValidSliceNumber,
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, InvalidSliceNumber,
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, ValidOrientation,
                           NullImageSpatialObject,  No );

  igstkAddTransitionMacro( ValidImageSpatialObject, NullImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, EmptyImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, ValidImageSpatialObject,
                           ValidImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, SetSliceNumber,
                           ValidImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, ValidSliceNumber,
                           ValidImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, InvalidSliceNumber,
                           ValidImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, ValidOrientation,
                           ValidImageOrientation,  SetOrientation ); 

  igstkAddTransitionMacro( ValidImageOrientation, NullImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageOrientation, EmptyImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageOrientation, ValidImageSpatialObject,
                           ValidImageSpatialObject,  SetImageSpatialObject );
  igstkAddTransitionMacro( ValidImageOrientation, SetSliceNumber,
                       AttemptingToSetSliceNumber,  AttemptSetSliceNumber ); 
  igstkAddTransitionMacro( ValidImageOrientation, ValidSliceNumber,
                           ValidSliceNumber,  SetSliceNumber ); 
  igstkAddTransitionMacro( ValidImageOrientation, InvalidSliceNumber,
                           ValidImageOrientation,  No ); 
  igstkAddTransitionMacro( ValidImageOrientation, ValidOrientation,
                           ValidImageOrientation,  SetOrientation ); 

  igstkAddTransitionMacro( ValidSliceNumber, NullImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidSliceNumber, EmptyImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidSliceNumber, ValidImageSpatialObject,
                           ValidImageSpatialObject,  SetImageSpatialObject );
  igstkAddTransitionMacro( ValidSliceNumber, SetSliceNumber,
                        AttemptingToSetSliceNumber,  AttemptSetSliceNumber );
  igstkAddTransitionMacro( ValidSliceNumber, ValidSliceNumber,
                           ValidSliceNumber,  SetSliceNumber ); 
  igstkAddTransitionMacro( ValidSliceNumber, InvalidSliceNumber,
                           ValidImageOrientation,  No ); 
  igstkAddTransitionMacro( ValidSliceNumber, ValidOrientation,
                           ValidImageOrientation,  SetOrientation ); 

  igstkAddTransitionMacro( AttemptingToSetSliceNumber, NullImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( AttemptingToSetSliceNumber, EmptyImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( AttemptingToSetSliceNumber, ValidImageSpatialObject,
                           ValidImageSpatialObject,  SetImageSpatialObject );
  igstkAddTransitionMacro( AttemptingToSetSliceNumber, SetSliceNumber,
                        AttemptingToSetSliceNumber,  AttemptSetSliceNumber );
  igstkAddTransitionMacro( AttemptingToSetSliceNumber, ValidSliceNumber,
                           ValidSliceNumber,  SetSliceNumber ); 
  igstkAddTransitionMacro( AttemptingToSetSliceNumber, InvalidSliceNumber,
                           ValidImageOrientation,  No ); 
  igstkAddTransitionMacro( AttemptingToSetSliceNumber, ValidOrientation,
                           ValidImageOrientation,  SetOrientation ); 

  igstkAddTransitionMacro( ValidImageSpatialObject, RequestSliceNumberBounds,
                           ValidImageSpatialObject, ReportSliceNumberBounds );
  igstkAddTransitionMacro( ValidSliceNumber, RequestSliceNumberBounds,
                           ValidSliceNumber, ReportSliceNumberBounds );
  igstkAddTransitionMacro( ValidImageOrientation, RequestSliceNumberBounds,
                           ValidImageOrientation, ReportSliceNumberBounds );

  igstkAddTransitionMacro( NullImageSpatialObject, ConnectVTKPipeline,
                           NullImageSpatialObject, No );
  igstkAddTransitionMacro( ValidImageSpatialObject, ConnectVTKPipeline,
                           ValidImageSpatialObject, ConnectVTKPipeline );
  igstkAddTransitionMacro( ValidImageOrientation, ConnectVTKPipeline,
                           ValidImageOrientation, ConnectVTKPipeline );
  igstkAddTransitionMacro( ValidSliceNumber, ConnectVTKPipeline, 
                           ValidSliceNumber, ConnectVTKPipeline );

  igstkSetInitialStateMacro( NullImageSpatialObject );

  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
template < class TImageSpatialObject >
ImageSlice2dRepresentation< TImageSpatialObject >
::~ImageSlice2dRepresentation()  
{
  std::cout << "ImageSlice2dRepresentation destructor " << m_Orientation << std::endl;
  m_ImagePlane->Delete();
  // This deletes also the m_ImageActor
  this->DeleteActors();
}

/** Overloaded DeleteActor function */
template < class TImageSpatialObject >
void 
ImageSlice2dRepresentation< TImageSpatialObject >
::DeleteActors( )
{
  igstkLogMacro( DEBUG, "igstk::ImageSlice2dRepresentation\
                        ::DeleteActors called...\n");
  
  this->Superclass::DeleteActors();
}
 
/** Set the Image Spatial Object */
template < class TImageSpatialObject >
void 
ImageSlice2dRepresentation< TImageSpatialObject >
::RequestSetImageSpatialObject( const ImageSpatialObjectType * image )
{
  igstkLogMacro( DEBUG, "igstk::ImageSlice2dRepresentation\
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


template < class TImageSpatialObject >
void 
ImageSlice2dRepresentation< TImageSpatialObject >
::RequestSetOrientation( OrientationType orientation )
{
  igstkLogMacro( DEBUG, "igstk::ImageSlice2dRepresentation\
                        ::RequestSetOrientation called...\n");
  
  m_OrientationToBeSet = orientation;

  m_StateMachine.PushInput( m_ValidOrientationInput );
  m_StateMachine.ProcessInputs();
}


template < class TImageSpatialObject >
void 
ImageSlice2dRepresentation< TImageSpatialObject >
::SetOrientationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImageSlice2dRepresentation\
                        ::SetOrientationProcessing called...\n");
  m_Orientation = m_OrientationToBeSet;

  //this->m_ImagePlane->SetMarginSizeX(0.0);
  //this->m_ImagePlane->SetMarginSizeY(0.0);
  
  switch(m_Orientation)
  {
  case Axial:
      m_ImagePlane->SetPlaneOrientationToZAxes();
      break;
  case Coronal:
      m_ImagePlane->SetPlaneOrientationToYAxes();
      break;
  case Sagittal:
      m_ImagePlane->SetPlaneOrientationToXAxes();
      break;
  default:
      std::cout << "Invalid m_Orientation: " << m_Orientation << std::endl;
      break;
  }
  std::cout << "Orientation is now " << m_Orientation << std::endl;
}
  

template < class TImageSpatialObject >
void 
ImageSlice2dRepresentation< TImageSpatialObject >
::RequestSetSliceNumber( SliceNumberType slice )
{

  igstkLogMacro( DEBUG, "igstk::ImageSlice2dRepresentation\
                        ::RequestSetSliceNumber called...\n");

  m_SliceNumberToBeSet = slice;

  m_StateMachine.PushInput( m_SetSliceNumberInput );
  m_StateMachine.ProcessInputs();

}


template < class TImageSpatialObject >
void 
ImageSlice2dRepresentation< TImageSpatialObject >
::AttemptSetSliceNumberProcessing()
{

  igstkLogMacro( DEBUG, "igstk::ImageSlice2dRepresentation\
                        ::AttemptSetSliceNumberProcessing called...\n");
  if( m_ImagePlane )
    {

    SliceNumberType minSlice = 0;
    SliceNumberType maxSlice = 0;
    
    int ext[6];

    m_ImageData->Update();
    m_ImageData->GetExtent( ext );

    switch( m_Orientation )
      {
      case Axial:
        minSlice = ext[4];
        maxSlice = ext[5];
        break;
      case Sagittal:
        minSlice = ext[0];
        maxSlice = ext[1];
        break;
      case Coronal:
        minSlice = ext[2];
        maxSlice = ext[3];
        break;
      }

    if( m_SliceNumberToBeSet >= minSlice && m_SliceNumberToBeSet <= maxSlice )
      {
      m_StateMachine.PushInput( m_ValidSliceNumberInput );
      }
    else
      {
      m_StateMachine.PushInput( m_InvalidSliceNumberInput );
      }

    m_StateMachine.ProcessInputs();
    }
}

template < class TImageSpatialObject >
void 
ImageSlice2dRepresentation< TImageSpatialObject >
::SetSliceNumberProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImageSlice2dRepresentation\
                        ::SetSliceNumber called...\n");

  m_SliceNumber = m_SliceNumberToBeSet;
  m_ImagePlane->SetSliceIndex(m_SliceNumber);
}


template < class TImageSpatialObject >
void 
ImageSlice2dRepresentation< TImageSpatialObject >
::SetWindowLevel( double window, double level )
{
  igstkLogMacro( DEBUG, "igstk::ImageSlice2dRepresentation\
                        ::SetWindowLevel called...\n");

  m_ImagePlane->SetWindowLevel(window, level);
}

/** Null Operation for a State Machine Transition */
template < class TImageSpatialObject >
void 
ImageSlice2dRepresentation< TImageSpatialObject >
::NoProcessing()
{
}

/** Set the Image Spatial Object */
template < class TImageSpatialObject >
void 
ImageSlice2dRepresentation< TImageSpatialObject >
::SetImageSpatialObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImageSlice2dRepresentation\
                        ::SetImageSpatialObjectProcessing called...\n");

  // We create the image spatial object
  m_ImageSpatialObject = m_ImageSpatialObjectToAdd;

  m_ImageSpatialObject->AddObserver( igstk::VTKImageModifiedEvent(), 
                                     m_VTKImageObserver );

  this->RequestSetSpatialObject( m_ImageSpatialObject );
  
  // This method gets a VTK image data from the private method of the
  // ImageSpatialObject and stores it in the representation by invoking the
  // private SetImage method.
  //
  // 
  this->m_VTKImageObserver->Reset();

  this->m_ImageSpatialObject->RequestGetVTKImage();

  if( this->m_VTKImageObserver->GotVTKImage() ) 
    {
    this->m_ImageData = this->m_VTKImageObserver->GetVTKImage();
    if( this->m_ImageData )
      {
          this->m_ImageData->Update();          
          this->m_ImagePlane->SetInput( this->m_ImageData);
      }
    }
}


/** Print Self function */
template < class TImageSpatialObject >
void
ImageSlice2dRepresentation< TImageSpatialObject >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


/** Update the visual representation in response to changes in the geometric
 * object */
template < class TImageSpatialObject >
void
ImageSlice2dRepresentation< TImageSpatialObject >
::UpdateRepresentationProcessing()
{
    //igstkLogMacro( DEBUG, "igstk::ImageSlice2dRepresentation \
        //                 ::UpdateRepresentationProcessing called...\n");
  if( m_ImageData )
    {
        m_ImagePlane->UpdatePlacement();
    }
}


/** Create the vtk Actors */
template < class TImageSpatialObject >
void
ImageSlice2dRepresentation< TImageSpatialObject >
::CreateActors()
{
  igstkLogMacro( DEBUG, "igstk::ImageSlice2dRepresentation\
                        ::CreateActors called...\n");

  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  igstkPushInputMacro( ConnectVTKPipeline );
  m_StateMachine.ProcessInputs(); 
}

/** Create a copy of the current object representation */
template < class TImageSpatialObject >
typename ImageSlice2dRepresentation< TImageSpatialObject >::Pointer
ImageSlice2dRepresentation< TImageSpatialObject >
::Copy() const
{
  igstkLogMacro( DEBUG, "igstk::ImageSlice2dRepresentation\
                        ::Copy called...\n");

  Pointer newOR = ImageSlice2dRepresentation::New();
  newOR->SetColor( this->GetRed(), this->GetGreen(), this->GetBlue() );
  newOR->SetOpacity( this->GetOpacity() );
  newOR->RequestSetImageSpatialObject( m_ImageSpatialObject );

  return newOR;
}

 
template < class TImageSpatialObject >
void
ImageSlice2dRepresentation< TImageSpatialObject >
::SetImage( const vtkImageData * image )
{
  igstkLogMacro( DEBUG, "igstk::ImageSlice2dRepresentation\
                        ::SetImage called...\n");

  // This const_cast<> is needed here due to the lack of 
  // const-correctness in VTK 
  m_ImageData = const_cast< vtkImageData *>( image );
}


template < class TImageSpatialObject >
void
ImageSlice2dRepresentation< TImageSpatialObject >
::ReportSliceNumberBoundsProcessing() 
{
  int ext[6];

  m_ImageData->Update();
  m_ImageData->GetExtent( ext );

  igstk::EventHelperType::IntegerBoundsType bounds;

  switch( m_Orientation )
    {
    case Axial:
      {
      bounds.minimum = ext[4];
      bounds.maximum = ext[5];
      igstk::AxialSliceBoundsEvent event;
      event.Set( bounds );
      this->InvokeEvent( event );
      break;
      }
    case Sagittal:
      {
      bounds.minimum = ext[0];
      bounds.maximum = ext[1];
      igstk::SagittalSliceBoundsEvent event;
      event.Set( bounds );
      this->InvokeEvent( event );
      break;
      }
    case Coronal:
      {
      bounds.minimum = ext[2];
      bounds.maximum = ext[3];
      igstk::CoronalSliceBoundsEvent event;
      event.Set( bounds );
      this->InvokeEvent( event );
      break;
      }
    }
}

template < class TImageSpatialObject >
void
ImageSlice2dRepresentation< TImageSpatialObject >
::RequestGetSliceNumberBounds() 
{
  m_StateMachine.PushInput( m_RequestSliceNumberBoundsInput );
  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void
ImageSlice2dRepresentation< TImageSpatialObject >
::ConnectVTKPipelineProcessing() 
{
  this->m_ImagePlane->RestrictPlaneToVolumeOn();
    this->m_ImagePlane->DisplayTextOff();
    this->m_ImagePlane->On();
    this->m_ImagePlane->InteractionOff();
    this->m_ImagePlane->InteractionOn();
}

/** Set the opacity */
template < class TImageSpatialObject >
void
ImageSlice2dRepresentation< TImageSpatialObject >
::SetOpacity(float alpha)
{
}

}

#endif
