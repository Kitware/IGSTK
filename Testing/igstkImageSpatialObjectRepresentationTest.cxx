/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSpatialObjectRepresentationTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "igstkConfigure.h"
#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkImageSpatialObject.h"
#include "igstkView2D.h"
#include "igstkFLTKWidget.h"
#include "igstkCTImageReader.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkEvents.h"
#include "igstkAxesObject.h"

namespace igstk
{

namespace ImageSpatialObjectRepresentationTest
{

igstkObserverObjectMacro(CTImage,
    ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)

class ImageRepresentationObserver : public ::itk::Command
{

public:
  
  typedef ImageRepresentationObserver  Self;
  typedef itk::SmartPointer<Self>      Pointer;
  typedef itk::Command                 Superclass;
  itkNewMacro(Self);
  
  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    }
  
  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & event)
    {
    const AxialSliceBoundsEvent * axialEvent = 
          dynamic_cast< const AxialSliceBoundsEvent * >( &event );
    if( axialEvent )
      {
      m_CoronalEventReceived  = false;
      m_SagittalEventReceived = false;
      m_AxialEventReceived    = true;
      EventHelperType::IntegerBoundsType bounds = axialEvent->Get();
      std::cout << "Minimum Slice = " << bounds.minimum << std::endl;
      std::cout << "Maximum Slice = " << bounds.maximum << std::endl;
      return;
      }
        
    const CoronalSliceBoundsEvent * coronalEvent = 
          dynamic_cast< const CoronalSliceBoundsEvent * >( &event );
    if( coronalEvent )
      {
      m_CoronalEventReceived  = true;
      m_SagittalEventReceived = false;
      m_AxialEventReceived    = false;
      EventHelperType::IntegerBoundsType bounds = coronalEvent->Get();
      std::cout << "Minimum Slice = " << bounds.minimum << std::endl;
      std::cout << "Maximum Slice = " << bounds.maximum << std::endl;
      return;
      }
        
    const SagittalSliceBoundsEvent * sagittalEvent = 
          dynamic_cast< const SagittalSliceBoundsEvent * >( &event );
    if( sagittalEvent )
      {
      m_AxialEventReceived    = false;
      m_SagittalEventReceived = true;
      m_CoronalEventReceived  = false;
      EventHelperType::IntegerBoundsType bounds = sagittalEvent->Get();
      std::cout << "Minimum Slice = " << bounds.minimum << std::endl;
      std::cout << "Maximum Slice = " << bounds.maximum << std::endl;
      return;
      }
    } 

  bool GetAxialEventReceived()
    { 
    return m_AxialEventReceived; 
    }
  bool GetSagittalEventReceived()
    { 
    return m_SagittalEventReceived; 
    }
  bool GetCoronalEventReceived()
    { 
    return m_CoronalEventReceived; 
    }

protected:
     
  ImageRepresentationObserver()
    {
    m_AxialEventReceived = false;
    m_CoronalEventReceived = false;
    m_SagittalEventReceived = false;
    }

private:

  bool m_AxialEventReceived;
  bool m_CoronalEventReceived;
  bool m_SagittalEventReceived;
      
};  // end of ImageRepresentationObserver class 

}

}

int igstkImageSpatialObjectRepresentationTest( int , char * argv[] )
{
  igstk::RealTimeClock::Initialize();

  typedef short    PixelType;
  const unsigned int Dimension = 3;

  typedef igstk::ImageSpatialObject<PixelType,Dimension> 
                                                       ImageSpatialObjectType;
  
  typedef igstk::ImageSpatialObjectRepresentation<ImageSpatialObjectType>   
                                                           RepresentationType;

  RepresentationType::Pointer  representation = RepresentationType::New();

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( LoggerType::CRITICAL );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput = 
                                            igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);// redirect messages from VTK 
                                     // OutputWindow -> logger

  igstk::AxesObject::Pointer worldReference = igstk::AxesObject::New();

  worldReference->SetLogger( logger );

  // Instantiate a reader
  //
  typedef igstk::CTImageReader         ReaderType;
  ReaderType::Pointer   reader = ReaderType::New();
  reader->SetLogger( logger );
  typedef igstk::ImageSpatialObjectRepresentationTest::CTImageObserver 
                                                        CTImageObserverType;
  CTImageObserverType::Pointer ctImageObserver = CTImageObserverType::New(); 
  reader->AddObserver(::igstk::CTImageReader::ImageModifiedEvent(),
                            ctImageObserver);


  /* Read in a DICOM series */
  std::cout << "Reading CT image : " << argv[1] << std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );
 
  reader->RequestReadImage();
  representation->SetLogger( logger );

  reader->RequestGetImage();
  reader->Print(std::cout);

  if(!ctImageObserver->GotCTImage())
    {
    std::cout << "No CTImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

  // Test error case
  representation->RequestSetImageSpatialObject( NULL );

  // Test correct case
  ImageSpatialObjectType::Pointer imageSpatialObject = 
                                                ImageSpatialObjectType::New();
  representation->RequestSetImageSpatialObject( imageSpatialObject );

  imageSpatialObject->RequestGetImageExtent();// code coverage

  // Exercise the TypeMacro() which defines the GetNameOfClass()
  std::string name = representation->GetNameOfClass();

  std::cout << "Name of class = " << name << std::endl;
  
  representation->RequestSetSliceNumber( 10 );

  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(532,532,"CT Read View Test");
    
  typedef igstk::View2D  View2DType;
  // Create an FLTK minimal GUI
  typedef igstk::FLTKWidget      WidgetType;

  View2DType::Pointer view2D = View2DType::New();

  // instantiate FLTK widget 
  WidgetType * widget2D = new WidgetType( 10,10,512,512,"2D View");
  widget2D->RequestSetView( view2D );
  widget2D->SetLogger( logger );

  view2D->SetLogger( logger );

  form->end();
  form->show();
   
  // Test all possible orientations 
  representation->RequestSetOrientation( RepresentationType::Sagittal );
  representation->RequestSetOrientation( RepresentationType::Coronal );
  representation->RequestSetOrientation( RepresentationType::Axial );

  representation->RequestSetSliceNumber( 10 );

  representation->SetOpacity( 0.3 );

  typedef 
    igstk::ImageSpatialObjectRepresentationTest::ImageRepresentationObserver 
                                                                ObserverType;

  ObserverType::Pointer observer = ObserverType::New();

  representation->AddObserver( igstk::AxialSliceBoundsEvent(),    observer );
  representation->AddObserver( igstk::CoronalSliceBoundsEvent(),  observer );
  representation->AddObserver( igstk::SagittalSliceBoundsEvent(), observer );

  imageSpatialObject = ctImageObserver->GetCTImage();
  imageSpatialObject->RequestGetImageExtent(); // code coverage
  imageSpatialObject->SetLogger( logger );

  igstk::Transform  transform;
  transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
  imageSpatialObject->RequestSetTransformAndParent( transform, worldReference );


  representation->RequestSetImageSpatialObject( imageSpatialObject );

  // Test Get Bounds for Axial orientation 
  representation->RequestSetOrientation( RepresentationType::Axial );
  igstk::PulseGenerator::CheckTimeouts();
  
  representation->RequestSetSliceNumber( 3 );
  igstk::PulseGenerator::CheckTimeouts();
  
  representation->RequestGetSliceNumberBounds();
  igstk::PulseGenerator::CheckTimeouts();
  
  if( observer->GetAxialEventReceived() )
    {
    std::cout << "Axial Event Received successfuly" << std::endl;
    }
  else
    {
    std::cerr << "Error AxialSliceBoundsEvent() was not received ";
    std::cerr << "after invoking RequestGetSliceNumberBounds()." << std::endl;
    return EXIT_FAILURE;
    }
   
  // Test Get Bounds for Coronal orientation 
  representation->RequestSetOrientation( RepresentationType::Coronal );
  igstk::PulseGenerator::CheckTimeouts();

  representation->RequestSetSliceNumber( 3 );
  igstk::PulseGenerator::CheckTimeouts();

  representation->RequestGetSliceNumberBounds();
  igstk::PulseGenerator::CheckTimeouts();

  
  if( observer->GetCoronalEventReceived() )
    {
    std::cout << "Coronal Event Received successfuly" << std::endl;
    }
  else
    {
    std::cerr << "Error CoronalSliceBoundsEvent() was not received ";
    std::cerr << "after invoking RequestGetSliceNumberBounds()." << std::endl;
    return EXIT_FAILURE;
    }

  // Test Get Bounds for Sagittal orientation 
  representation->RequestSetOrientation( RepresentationType::Sagittal );
  igstk::PulseGenerator::CheckTimeouts();

  representation->RequestSetSliceNumber( 3 );
  igstk::PulseGenerator::CheckTimeouts();

  representation->RequestGetSliceNumberBounds();
  igstk::PulseGenerator::CheckTimeouts();

  if( observer->GetSagittalEventReceived() )
    {
    std::cout << "Sagittal Event Received successfuly" << std::endl;
    }
  else
    {
    std::cerr << "Error SagittalSliceBoundsEvent() was not received ";
    std::cerr << "after invoking RequestGetSliceNumberBounds()." << std::endl;
    return EXIT_FAILURE;
    }

  view2D->RequestAddObject( representation );

  view2D->RequestSetTransformAndParent( transform, worldReference );
  
  // Set and initialize the pulse generator of the view 
  view2D->SetRefreshRate( 30 );
  view2D->RequestStart();
 

  // Do manual redraws for each orientation while changing slice numbers
    {
    std::cout << " Axial View " << std::endl;
    view2D->RequestSetOrientation( igstk::View2D::Axial );
    view2D->RequestResetCamera();
    representation->RequestSetOrientation( RepresentationType::Axial );
    igstk::PulseGenerator::CheckTimeouts();

    for(unsigned int i=0; i<5; i++)
      {
      representation->RequestSetSliceNumber( i );
      Fl::wait(0.01);
      igstk::PulseGenerator::CheckTimeouts();
      Fl::check();       // trigger FLTK redraws
      std::cout << "Slice i= " << i << std::endl;
      }
    }

    {
    std::cout << " Sagittal View " << std::endl;
    representation->RequestSetOrientation( RepresentationType::Sagittal );
    view2D->RequestSetOrientation( igstk::View2D::Sagittal );
    view2D->RequestResetCamera();
    for(unsigned int i=0; i<10; i++)
      {
      representation->RequestSetSliceNumber( i );
      Fl::wait(0.01);
      igstk::PulseGenerator::CheckTimeouts();
      Fl::check();       // trigger FLTK redraws
      std::cout << "i= " << i << std::endl;
      }
    }

    {
    std::cout << " Coronal View " << std::endl;
    representation->RequestSetOrientation( RepresentationType::Coronal );
    view2D->RequestSetOrientation( igstk::View2D::Coronal );
    view2D->RequestResetCamera();
    
    for(unsigned int i=0; i<511; i++)
      {
      representation->RequestSetSliceNumber( i );
      Fl::wait(0.01);
      igstk::PulseGenerator::CheckTimeouts();
      Fl::check();       // trigger FLTK redraws
      std::cout << "i= " << i << std::endl;
      }
    
    }
    
  // On purpose request non-existing slices. 
  // The requests should be ignored by the state machine.
    {
    representation->RequestSetOrientation( RepresentationType::Axial );
    view2D->RequestSetOrientation( igstk::View2D::Axial );
    view2D->RequestResetCamera();
   
    for(unsigned int i=5; i<10; i++)
      {
      representation->RequestSetSliceNumber( i );
      Fl::wait(0.01);
      igstk::PulseGenerator::CheckTimeouts();
      Fl::check();       // trigger FLTK redraws
      std::cout << "i= " << i << std::endl;
      }
    }

  // Exercising the physical point to index function
    {
    ImageSpatialObjectType::IndexType            index;
    ImageSpatialObjectType::ContinuousIndexType  cindex;
    ImageSpatialObjectType::PointType            p;
    for ( double z = 0.0; z< 100; z += 10.0 )
      {
      p[0] = cos( z ) * z;
      p[1] = sin( z ) * z;
      p[2] = z;
      if ( !imageSpatialObject->IsInside( p ) )
        {
        std::cout<< "Point outside the image region!" << std::endl; 
        }
        
      imageSpatialObject->TransformPhysicalPointToIndex( p, index );
      imageSpatialObject->TransformPhysicalPointToContinuousIndex( p, cindex );
      std::cout<< "Index           : " << index  << std::endl; 
      std::cout<< "Continuous Index: " << cindex << std::endl;
            
      }

    }


  delete widget2D;
  delete form;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
 
  return EXIT_SUCCESS;
}
