/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageReslicePlaneSpatialObjectTest.cxx
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
#include "igstkImageReslicePlaneSpatialObject.h"
#include "igstkAxesObject.h"
#include "igstkImageSpatialObject.h"
#include "igstkCTImageReader.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkEvents.h"
#include "igstkCylinderObject.h"
#include "igstkTransform.h"
#include "vtkPlaneSource.h"

namespace ImageReslicePlaneSpatialObjectTest
{
igstkObserverObjectMacro(CTImage,
    ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)
}

int igstkImageReslicePlaneSpatialObjectTest( int argc , char * argv [] )
{
  igstk::RealTimeClock::Initialize();


  if( argc < 2 )
    {
    std::cerr << " Missing arguments: " << argv[0]  \
              << "\t Input image" << std::endl; 
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
  AxesObjectType::Pointer axesObject = AxesObjectType::New();
  axesObject->SetSize(3.0, 3.0, 3.0);

  // Define identity transform
  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  typedef igstk::CTImageReader         ReaderType;
  ReaderType::Pointer   reader = ReaderType::New();
  reader->SetLogger( logger );

  //set up CT image observer
  typedef ImageReslicePlaneSpatialObjectTest::CTImageObserver 
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

  //Connect the image spatial object to the reference coordinate system
  imageSpatialObject->RequestSetTransformAndParent( identity, axesObject );
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
  translation[0] =    0;
  translation[1] =    0;
  translation[2] = -1.0;
  rotation.Set(0.0, 0.0, 0.0, 1.0);
  const double transformUncertainty = 1.0;
  toolTransform.SetTranslation(
                          translation,
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );
  toolSpatialObject->RequestSetTransformAndParent( toolTransform, axesObject );
  planeSpatialObject->RequestSetToolSpatialObject( toolSpatialObject );

  vtkPlaneSource * plane = planeSpatialObject->GetReslicingPlane();

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
 
  return EXIT_SUCCESS;
}

