/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageResliceObjectRepresentationQtTest2.cxx
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

#include "igstkQTWidget.h"

#include "igstkConfigure.h"
#include "igstkImageResliceObjectRepresentation.h"
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

namespace ImageResliceObjectRepresentationQtTest2
{
igstkObserverObjectMacro(CTImage,
    ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)

igstkObserverMacro( VTKImage, ::igstk::VTKImageModifiedEvent, 
                       ::igstk::EventHelperType::VTKImagePointerType );
}

/** This test demonstrates how to perform orthogonal reslicing using slice number (manual reslicing) */
int igstkImageResliceObjectRepresentationQtTest2( int argc , char * argv [] )
{
  std::cout << " igstkImageResliceObjectRepresentationQtTest2: application started " << std::endl;

  igstk::RealTimeClock::Initialize();

  if( argc < 2 )
    {
    std::cerr << " Missing arguments: " << argv[0] << "CTImage " \
              << std::endl; 
    return EXIT_FAILURE;
    }

  typedef short    PixelType;
  const unsigned int Dimension = 3;

  // the reslicer plane we want to test.
  typedef igstk::ReslicerPlaneSpatialObject            ReslicerPlaneType;

  // we will use an ImageSpatialObject as the reference spatial object for the ReslicerPlane
  typedef igstk::ImageSpatialObject< PixelType, Dimension > 
                                                       ImageSpatialObjectType;
  typedef ImageSpatialObjectType::IndexType::IndexValueType IndexValueType;

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
//  reader->SetLogger( logger );

  //set up CT image observer
  typedef ImageResliceObjectRepresentationQtTest2::CTImageObserver 
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
  typedef ImageResliceObjectRepresentationQtTest2::VTKImageObserver 
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

  // Set a 2D View
  typedef igstk::View2D  View2DType;
  View2DType::Pointer view2D = View2DType::New();
//  view2D->SetLogger( logger );    

  typedef igstk::QTWidget      QTWidgetType;

  // Create an QT minimal GUI
  QApplication app(argc, argv);
  QMainWindow  * qtMainWindow = new QMainWindow();
  qtMainWindow->setFixedSize(512,512);

  typedef igstk::QTWidget      QTWidgetType;

  QTWidgetType * qtWidget2D = 
                      new QTWidgetType();
  qtWidget2D->RequestSetView( view2D );
//  qtWidget2D->SetLogger( logger );
  qtMainWindow->setCentralWidget( qtWidget2D );
  view2D->RequestSetTransformAndParent( identity, worldReference );
  view2D->SetRefreshRate( 40 );

  qtMainWindow->show();

  typedef igstk::ImageResliceObjectRepresentation< ImageSpatialObjectType >
                                        ImageRepresentationType;

  ImageRepresentationType::Pointer  representation =  ImageRepresentationType::New(); 
  //representation->SetLogger( logger );
  representation->SetWindowLevel( 1559, -244 );
  representation->RequestSetImageSpatialObject( imageSpatialObject );

  // Instantiate and use a reslicer plane spatial object
  ReslicerPlaneType::Pointer reslicerPlaneSpatialObject = ReslicerPlaneType::New();
//  reslicerPlaneSpatialObject->SetLogger( logger );

  // Select Orthogonal reslicing mode
  reslicerPlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Orthogonal );

  // Select Axial orientation type
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Axial );

  // Set bounding box provider spatial object to the reslicer plane object
  reslicerPlaneSpatialObject->RequestSetBoundingBoxProviderSpatialObject( imageSpatialObject );

  // Set the reslicer plane spatial object to the representation
  representation->RequestSetReslicePlaneSpatialObject( reslicerPlaneSpatialObject );

  // add the image representation to the view
  view2D->RequestAddObject( representation );

  // a variable to hold image index
  ImageSpatialObjectType::IndexType index;

  // a variable to hold world point coords
  ImageSpatialObjectType::PointType point;

  // auxiliar
  const double *data = NULL;

  // Select axial orientation in the reslicer plane and view
  view2D->RequestSetOrientation( View2DType::Axial );
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Axial );
  
  index[0] = static_cast<IndexValueType>(0.5*(imageExtent[0]+imageExtent[1]));
  index[1] = static_cast<IndexValueType>(0.5*(imageExtent[2]+imageExtent[3]));
  index[2] = static_cast<IndexValueType>(imageExtent[4]);
  
  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );

  data = point.GetVnlVector().data_block();
  reslicerPlaneSpatialObject->RequestSetCursorPosition( data ); 

  view2D->RequestStart();
  view2D->RequestResetCamera();
  qtMainWindow->show();

  for(unsigned int i=(unsigned int)(imageExtent[5]/2); i<=(unsigned int)(3*imageExtent[5]/4); i++)
    {
    index[2] = static_cast<IndexValueType>(i);
    imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
    data = point.GetVnlVector().data_block();
    reslicerPlaneSpatialObject->RequestSetCursorPosition( data );
    std::cout << data[0] << " " << data[1] << " " << data[2] << " axial slice # " << i << std::endl;
    QTest::qWait(10);
    igstk::PulseGenerator::CheckTimeouts();
    }
  view2D->RequestStop();

  /* Change slice orientation to sagittal */
  std::cout << "Sagittal view: " << std::endl;
  view2D->RequestSetOrientation( View2DType::Sagittal );
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Sagittal );

  index[0] = static_cast<IndexValueType>(imageExtent[0]);
  index[1] = static_cast<IndexValueType>(0.5*(imageExtent[2]+imageExtent[3]));
  index[2] = static_cast<IndexValueType>(0.5*(imageExtent[4]+imageExtent[5]));
  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );

  data = point.GetVnlVector().data_block();
  reslicerPlaneSpatialObject->RequestSetCursorPosition( data );

  view2D->RequestStart();
  view2D->RequestResetCamera();

  qtMainWindow->show();

  for(unsigned int i=(unsigned int)(imageExtent[1]/2); i<(unsigned int)(3*imageExtent[1]/4); i++)
    {
    index[0] = static_cast<IndexValueType>(i);
    imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
    const double *data = point.GetVnlVector().data_block();
    reslicerPlaneSpatialObject->RequestSetCursorPosition( data );
    std::cout << data[0] << " " << data[1] << " " << data[2] << " sagittal slice # " << i << std::endl;
    QTest::qWait(10);
    igstk::PulseGenerator::CheckTimeouts();      
    }
  view2D->RequestStop();

  /* Change slice orientation to coronal */
  std::cout << "Coronal view: " << std::endl;
  view2D->RequestSetOrientation( View2DType::Coronal );
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Coronal );

  index[0] = static_cast<IndexValueType>(0.5*(imageExtent[0]+imageExtent[1]));
  index[1] = static_cast<IndexValueType>(imageExtent[2]);
  index[2] = static_cast<IndexValueType>(0.5*(imageExtent[4]+imageExtent[5]));
  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );

  data = point.GetVnlVector().data_block();
  reslicerPlaneSpatialObject->RequestSetCursorPosition( data );

  view2D->RequestStart();
  view2D->RequestResetCamera();

  qtMainWindow->show();

  for(unsigned int i=(unsigned int)(imageExtent[3]/2); i<(unsigned int)(3*imageExtent[3]/4); i++)
    {
    index[1] = static_cast<IndexValueType>(i);
    imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
    const double *data = point.GetVnlVector().data_block();
    reslicerPlaneSpatialObject->RequestSetCursorPosition( data );
    std::cout << data[0] << " " << data[1] << " " << data[2] << " coronal slice # " << i << std::endl;
    QTest::qWait(10);
    igstk::PulseGenerator::CheckTimeouts();
    }
  view2D->RequestStop();

  //Reslice to the center axial slice and take a screenshot.
  //
  view2D->RequestStop();

  view2D->RequestSetOrientation( View2DType::Axial );
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Axial );

  index[0] = static_cast<IndexValueType>(0.5*(imageExtent[0]+imageExtent[1]));
  index[1] = static_cast<IndexValueType>(0.5*(imageExtent[2]+imageExtent[3]));
  index[2] = static_cast<IndexValueType>(0.5*(imageExtent[4]+imageExtent[5]));

  view2D->RequestStart();
  view2D->RequestResetCamera();

  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
  data = point.GetVnlVector().data_block();
  reslicerPlaneSpatialObject->RequestSetCursorPosition( data );

  std::cout << "Saving snapshot to: " << argv[2] << std::endl;
  view2D->RequestSaveScreenShot( argv[2] );
  view2D->RequestStop();

  delete qtWidget2D;
  qtMainWindow->hide();
  delete qtMainWindow;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    std::cout << "Got VTK Logger errors!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
 
  std::cout << "[SUCCESS]" << std::endl;
  return EXIT_SUCCESS;

}

