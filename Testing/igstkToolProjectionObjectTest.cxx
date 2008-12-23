/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolProjectionObjectTest.cxx
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
//Warning about: identifier was truncated to '255' characters in the 
//debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkToolProjectionObject.h"
#include "igstkToolProjectionRepresentation.h"
#include "igstkImageSpatialObject.h"
#include "igstkCTImageReader.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkAxesObject.h"
#include "igstkEvents.h"
#include "igstkTransform.h"
#include "igstkVTKLoggerOutput.h"

namespace ToolProjectionObjectTest
{
igstkObserverObjectMacro(CTImage,
    ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)

igstkObserverMacro( VTKImage, ::igstk::VTKImageModifiedEvent, 
                       ::igstk::EventHelperType::VTKImagePointerType );
}


int igstkToolProjectionObjectTest( int argc, char * argv[] )
{
  if( argc < 2 )
    {
    std::cerr << " Missing arguments: " << argv[0] << "CTImage " << std::endl; 
    return EXIT_FAILURE;
    }
  typedef short    PixelType;
  const unsigned int Dimension = 3;

  typedef igstk::ImageSpatialObject<PixelType,Dimension> ImageSpatialObjectType;

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
  typedef ToolProjectionObjectTest::CTImageObserver 
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
  imageSpatialObject->RequestSetTransformAndParent( identity, worldReference );  
 
  typedef igstk::ToolProjectionObject                ObjectType;
  typedef igstk::ToolProjectionRepresentation  RepresentationType;

  ObjectType::Pointer toolProjectionObject = ObjectType::New();

  RepresentationType::Pointer toolProjectionObjectRepresentation = RepresentationType::New();

 toolProjectionObjectRepresentation->RequestSetToolProjectionObject( toolProjectionObject );

 //Set the reslicing plane
 
 typedef ::igstk::ReslicerPlaneSpatialObject               ReslicerPlaneType;
 ReslicerPlaneType::Pointer reslicerPlaneSpatialObject = ReslicerPlaneType::New();
  reslicerPlaneSpatialObject->SetLogger( logger );

  // Select Orthogonal reslicing mode
  reslicerPlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Orthogonal );

  // Select axial orientation
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Axial );

  // Set bounding box provider spatial object
  reslicerPlaneSpatialObject->RequestSetBoundingBoxProviderSpatialObject( imageSpatialObject );

  toolProjectionObjectRepresentation->RequestSetReslicePlaneSpatialObject( reslicerPlaneSpatialObject );

  bool TestPassed = true;

  // Test Property
  std::cout << "Testing Property : ";
  toolProjectionObjectRepresentation->SetColor(0.1,0.2,0.3);
  toolProjectionObjectRepresentation->SetOpacity(0.4);
  if(fabs(toolProjectionObjectRepresentation->GetRed()-0.1)>0.00001)
    {
    std::cerr << "GetRed() [FAILED]" << std::endl;
    TestPassed= false;
    }
  if(fabs(toolProjectionObjectRepresentation->GetGreen()-0.2)>0.00001)
    {
    std::cerr << "GetGreen()[FAILED]" << std::endl;
    TestPassed = false;
    }
  if(fabs(toolProjectionObjectRepresentation->GetBlue()-0.3)>0.00001)
    {
    std::cerr << "GetBlue() [FAILED]" << std::endl;
    TestPassed = false;
    }
  if(fabs(toolProjectionObjectRepresentation->GetOpacity()-0.4)>0.00001)
    {
    std::cerr << "GetOpacity() [FAILED]" << std::endl;
    TestPassed = false;
    }

  toolProjectionObjectRepresentation->Print( std::cout );
  toolProjectionObject->Print( std::cout );

  toolProjectionObjectRepresentation->GetNameOfClass();
  toolProjectionObject->GetNameOfClass();
 
  if( TestPassed )
    {
    return EXIT_SUCCESS;
    } 
  else
    {
    return EXIT_FAILURE;
    }
}
