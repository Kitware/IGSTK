/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageResliceSpatialObjectRepresentationTest.cxx
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
#include "igstkCylinderObject.h"
#include "igstkTransform.h"
#include "igstkView2D.h"
#include "igstkQTWidget.h"

namespace ImageResliceSpatialObjectRepresentationTest
{
igstkObserverObjectMacro(CTImage,
    ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)

igstkObserverMacro( VTKImage, ::igstk::VTKImageModifiedEvent, 
                       ::igstk::EventHelperType::VTKImagePointerType );
}

int igstkImageResliceSpatialObjectRepresentationTest( int argc , char * argv [] )
{
  igstk::RealTimeClock::Initialize();


  if( argc < 3 )
    {
    std::cerr << " Missing arguments: " << argv[0] << "CTImage " \
              << "Output image file for a screenshot" << std::endl; 
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
  typedef ImageResliceSpatialObjectRepresentationTest::CTImageObserver 
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
  typedef ImageResliceSpatialObjectRepresentationTest::VTKImageObserver 
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

  /* Compute image bounds */
  double imageSpacing[3];
  imageData->GetSpacing( imageSpacing );
  std::cout << "Image spacing: " << "(" << imageSpacing[0] << "," 
                                 << imageSpacing[1] << ","
                                 << imageSpacing[2] << ")" << std::endl;
  double imageOrigin[3];
  imageData->GetOrigin( imageOrigin );
  std::cout << "Image origin: " << "(" << imageOrigin[0] << "," 
                                 << imageOrigin[1] << ","
                                 << imageOrigin[2] << ")" << std::endl;

  int imageExtent[6];
  imageData->GetWholeExtent( imageExtent );
  std::cout << "Image extent: " << "(" << imageExtent[0] << "," 
                                << imageExtent[1] << ","
                                << imageExtent[2] << ","
                                << imageExtent[3] << ","
                                << imageExtent[4] << ","
                                << imageExtent[5] << ")" << std::endl;

  double bounds[] = { imageOrigin[0] + imageSpacing[0]*imageExtent[0], //xmin
                       imageOrigin[0] + imageSpacing[0]*imageExtent[1], //xmax
                       imageOrigin[1] + imageSpacing[1]*imageExtent[2], //ymin
                       imageOrigin[1] + imageSpacing[1]*imageExtent[3], //ymax
                       imageOrigin[2] + imageSpacing[2]*imageExtent[4], //zmin
                       imageOrigin[2] + imageSpacing[2]*imageExtent[5]};//zmax

  for ( unsigned int i = 0; i <= 4; i += 2 ) // reverse bounds if necessary
      {
      if ( bounds[i] > bounds[i+1] )
        {
        double t = bounds[i+1];
        bounds[i+1] = bounds[i];
        bounds[i] = t;
        }
      }

  std::cout << "Image bounds: " << "(" << bounds[0] << "," 
                                << bounds[1] << ","
                                << bounds[2] << ","
                                << bounds[3] << ","
                                << bounds[4] << ","
                                << bounds[5] << ")" << std::endl;

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

  view2D->RequestStart();

  view2D->RequestResetCamera();

  qtMainWindow->show();

  for( unsigned int i=0; i < 100; i++)
    {
    QTest::qWait(10);
    igstk::PulseGenerator::CheckTimeouts();
    }

  //Request refreshing stop to take a screenshot
  view2D->RequestStop();
      
  /* Save screenshots in a file */
  std::string filename;
  filename = argv[2]; 
  std::cout << "Saving a screen shot in file:" << argv[2] << std::endl;
  view2D->RequestSaveScreenShot( filename );

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

  // Set input tool spatial object
  typedef igstk::CylinderObject                           ToolSpatialObjectType;
  ToolSpatialObjectType::Pointer toolSpatialObject = ToolSpatialObjectType::New();  
  toolSpatialObject->SetRadius( 0.1 );
  toolSpatialObject->SetHeight( 2.0 );

  // set tool transform
  igstk::Transform toolTransform;
  igstk::Transform::VectorType    translation;
  igstk::Transform::VersorType    rotation;
  translation[0] =    0.5 * (bounds[0] + bounds[1] );
  translation[1] =    0.5 * (bounds[2] + bounds[3] );
  translation[2] =  bounds[4];
  rotation.Set(0.0, 0.0, 0.0, 1.0);
  const double transformUncertainty = 1.0;
  toolTransform.SetTranslation(
                          translation,
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );
  toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );
  planeSpatialObject->RequestSetToolSpatialObject( toolSpatialObject );

 //Set to the reslice plane to the representation
  representation->RequestSetReslicePlaneSpatialObject( planeSpatialObject );

  view2D->RequestStart();
  view2D->RequestResetCamera();
  qtMainWindow->show();
  //Iteratively change the tool transform to reslice
  for(unsigned int i=0; i<imageExtent[5]; i++)
      {
      translation[0] =    0.5 * (bounds[0] + bounds[1] );
      translation[1] =    0.5 * (bounds[2] + bounds[3] );
      translation[2] =    bounds[4] + i*imageSpacing[2];
      toolTransform.SetTranslation(
                          translation,
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );
      toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );
      QTest::qWait(10);
      igstk::PulseGenerator::CheckTimeouts();
      }
  view2D->RequestStop();

  /* Change slice orientation to sagittal */
  std::cout << "Sagittal view: " << std::endl;
  planeSpatialObject->RequestSetOrientationType(
           igstk::ImageReslicePlaneSpatialObject<ImageSpatialObjectType>::Sagittal );

  translation[0] =    bounds[0];
  translation[1] =    0.5 * (bounds[2] + bounds[3] );
  translation[2] =    0.5 * (bounds[4] + bounds[5] );
  rotation.Set(0.0, 0.0, 0.0, 1.0);
  toolTransform.SetTranslation(
                          translation,
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );
  toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );

  view2D->RequestStart();
  view2D->RequestResetCamera();
  qtMainWindow->show();
  //Iteratively change the tool transf1rm to reslice
  for(unsigned int i=0; i<imageExtent[1]; i++)
      {
      translation[0] =    bounds[0] + i*imageSpacing[0];
      translation[1] =    0.5 * (bounds[2] + bounds[3] );
      translation[2] =    0.5 * (bounds[4] + bounds[5] );
      toolTransform.SetTranslation(
                          translation,
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );
      toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );
      QTest::qWait(10);
      igstk::PulseGenerator::CheckTimeouts();
      }
  view2D->RequestStop();

  /* Change slice orientation to sagittal */
  std::cout << "Coronal view: " << std::endl;
  planeSpatialObject->RequestSetOrientationType(
           igstk::ImageReslicePlaneSpatialObject<ImageSpatialObjectType>::Coronal );

  translation[0] =    0.5 * (bounds[0] + bounds[1] );
  translation[1] =    bounds[2];
  translation[2] =    0.5 * (bounds[4] + bounds[5] );
  rotation.Set(0.0, 0.0, 0.0, 1.0);
  toolTransform.SetTranslation(
                          translation,
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );
  toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );

  view2D->RequestStart();
  view2D->RequestResetCamera();
  qtMainWindow->show();
  //Iteratively change the tool transf1rm to reslice
  for(unsigned int i=0; i<imageExtent[3]; i++)
      {
      translation[0] =    0.5 * (bounds[0] + bounds[1] );
      translation[1] =    bounds[2] + i*imageSpacing[1];
      translation[2] =    0.5 * (bounds[4] + bounds[5] );
      toolTransform.SetTranslation(
                          translation,
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );
      toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );
      QTest::qWait(10);
      igstk::PulseGenerator::CheckTimeouts();
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

