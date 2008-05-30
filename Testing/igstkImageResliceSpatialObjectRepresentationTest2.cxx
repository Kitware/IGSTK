/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageResliceSpatialObjectRepresentationTest2.cxx
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

// QT header files
#include <QApplication>
#include <QMainWindow>
#include <QtTest/QTest>

#include "igstkConfigure.h"
#include "igstkImageResliceSpatialObjectRepresentation.h"
#include "igstkAxesObject.h"
#include "igstkImageSpatialObject.h"
#include "igstkCTImageReader.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkEvents.h"
#include "igstkView2D.h"
#include "igstkQTWidget.h"

namespace ImageResliceSpatialObjectRepresentationTest2
{
igstkObserverObjectMacro(CTImage,
    ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)

igstkObserverMacro( VTKImage, ::igstk::VTKImageModifiedEvent, 
                       ::igstk::EventHelperType::VTKImagePointerType );
}

/** This test demonstrates how to perform orthogonal reslicing using slice number (manual reslicing) */
int igstkImageResliceSpatialObjectRepresentationTest2( int argc , char * argv [] )
{
  igstk::RealTimeClock::Initialize();


  if( argc < 2 )
    {
    std::cerr << " Missing arguments: " << argv[0] << "CTImage " \
              << std::endl; 
    return EXIT_FAILURE;
    }
  typedef short    PixelType;
  const unsigned int Dimension = 3;

  typedef igstk::ImageSpatialObject<PixelType,Dimension> 
                                                       ImageSpatialObjectType;
  
  typedef igstk::ImageReslicePlaneSpatialObject<ImageSpatialObjectType>
                                                       ResliceSpatialObjectType;

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( LoggerType::DEBUG );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput = 
                                            igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);// redirect messages from VTK 
                                     // OutputWindow -> logger

  // Create Axes object to act as a reference coordinate system
  typedef igstk::AxesObject    AxesObjectType;
  AxesObjectType::Pointer worldReference = AxesObjectType::New();
  worldReference->SetSize(3.0, 3.0, 3.0);

  // Define identity transform
  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  typedef igstk::CTImageReader         ReaderType;
  ReaderType::Pointer   reader = ReaderType::New();
  reader->SetLogger( logger );

  //set up CT image observer
  typedef ImageResliceSpatialObjectRepresentationTest2::CTImageObserver 
                                                        CTImageObserverType;
  CTImageObserverType::Pointer ctImageObserver = CTImageObserverType::New(); 
  reader->AddObserver(::igstk::CTImageReader::ImageModifiedEvent(),
                            ctImageObserver);

  /* Read in a DICOM series */
  std::cout << "Reading CT image : " << argv[1] << std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );
  reader->RequestReadImage();
  reader->RequestGetImage();
 
  if(!ctImageObserver->GotCTImage())
    {
    std::cout << "No CTImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

  // Set input image spatial object
  ImageSpatialObjectType::Pointer imageSpatialObject = ImageSpatialObjectType::New(); 
  imageSpatialObject = ctImageObserver->GetCTImage();

  //Determine the image parameters
  //first access the VTK image data
  typedef ImageResliceSpatialObjectRepresentationTest2::VTKImageObserver 
                                                        VTKImageObserverType;
  
  VTKImageObserverType::Pointer vtkImageObserver = VTKImageObserverType::New();

  imageSpatialObject->AddObserver( ::igstk::VTKImageModifiedEvent(), 
                                     vtkImageObserver );
  vtkImageObserver->Reset();
  imageSpatialObject->RequestGetVTKImage();

  if( !vtkImageObserver->GotVTKImage() ) 
    {
    std::cout << "No VTKImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

  vtkImageData * imageData = vtkImageObserver->GetVTKImage();

  int imageExtent[6];
  imageData->GetWholeExtent( imageExtent );
  std::cout << "Image extent: " << "(" << imageExtent[0] << "," 
                                << imageExtent[1] << ","
                                << imageExtent[2] << ","
                                << imageExtent[3] << ","
                                << imageExtent[4] << ","
                                << imageExtent[5] << ")" << std::endl;

  //Connect the image spatial object to the reference coordinate system
  imageSpatialObject->RequestSetTransformAndParent( identity, worldReference );

  typedef igstk::ImageResliceSpatialObjectRepresentation< ImageSpatialObjectType >
                                        RepresentationType;

  RepresentationType::Pointer  representation =  RepresentationType::New(); 
  representation->SetLogger( logger );
  representation->SetWindowLevel( 1559, -244 );
  representation->RequestSetImageSpatialObject( imageSpatialObject );
  //View
  typedef igstk::View2D  View2DType;
  View2DType::Pointer view2D = View2DType::New();
  view2D->SetLogger( logger );
    
  view2D->RequestResetCamera();
  view2D->RequestAddObject( representation );
   
  // Create an QT minimal GUI
  QApplication app(argc, argv);
  QMainWindow  * qtMainWindow = new QMainWindow();
  qtMainWindow->setFixedSize(512,512);

  typedef igstk::QTWidget      QTWidgetType;

  QTWidgetType * qtWidget2D = 
                      new QTWidgetType();
  qtWidget2D->RequestSetView( view2D );
  qtWidget2D->SetLogger( logger );
  qtMainWindow->setCentralWidget( qtWidget2D );
  view2D->RequestSetTransformAndParent( identity, worldReference );
  view2D->SetRefreshRate( 40 );

  //Instantiate and use reslice plane spatial object
  std::cout << "Attach a reslice plane spatial object ....." << std::endl;
  ResliceSpatialObjectType::Pointer planeSpatialObject = ResliceSpatialObjectType::New();
  planeSpatialObject->SetLogger( logger );

   // Select Orthogonal reslicing mode
  planeSpatialObject->RequestSetReslicingMode( 
           igstk::ImageReslicePlaneSpatialObject<ImageSpatialObjectType>::Orthogonal );

  // Select axial orientation
  planeSpatialObject->RequestSetOrientationType(
           igstk::ImageReslicePlaneSpatialObject<ImageSpatialObjectType>::Axial );
  planeSpatialObject->RequestSetImageSpatialObject( imageSpatialObject );

  typedef ResliceSpatialObjectType::SliceNumberType SliceNumberType;

  planeSpatialObject->RequestSetSliceNumber( imageExtent[4] );

 //Set to the reslice plane to the representation
  representation->RequestSetReslicePlaneSpatialObject( planeSpatialObject );

  view2D->RequestStart();
  view2D->RequestResetCamera();
  qtMainWindow->show();
  for(unsigned int i=0; i<imageExtent[5]; i++)
      {
      planeSpatialObject->RequestSetSliceNumber( i );
      QTest::qWait(10);
      igstk::PulseGenerator::CheckTimeouts();
      std::cout << "Axial slice number: " << i << std::endl;
      }
  view2D->RequestStop();

  /* Change slice orientation to sagittal */

  std::cout << "Sagittal view: " << std::endl;
  planeSpatialObject->RequestSetOrientationType(
           igstk::ImageReslicePlaneSpatialObject<ImageSpatialObjectType>::Sagittal );

  planeSpatialObject->RequestSetSliceNumber( imageExtent[0] );

  view2D->RequestStart();
  view2D->RequestResetCamera();
  qtMainWindow->show();
  for(unsigned int i=0; i<imageExtent[1]; i++)
      {
      planeSpatialObject->RequestSetSliceNumber( i );
      QTest::qWait(10);
      igstk::PulseGenerator::CheckTimeouts();
      std::cout << "Sagittal slice number: " << i << std::endl;
      }
  view2D->RequestStop();

  /* Change slice orientation to coronal */
  
  std::cout << "Coronal view: " << std::endl;
  planeSpatialObject->RequestSetOrientationType(
           igstk::ImageReslicePlaneSpatialObject<ImageSpatialObjectType>::Coronal );
  planeSpatialObject->RequestSetSliceNumber( imageExtent[2] );  

  view2D->RequestStart();
  view2D->RequestResetCamera();
  qtMainWindow->show();
  for(unsigned int i=0; i<imageExtent[3]; i++)
      {
      planeSpatialObject->RequestSetSliceNumber( i );
      QTest::qWait(10);
      igstk::PulseGenerator::CheckTimeouts();
      std::cout << "Sagittal slice number: " << i << std::endl;
      }
  view2D->RequestStop();

  delete qtWidget2D;
  delete qtMainWindow;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
 
  return EXIT_SUCCESS;
}

