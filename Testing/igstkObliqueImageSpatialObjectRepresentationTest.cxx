/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkObliqueImageSpatialObjectRepresentationTest.cxx
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

#include "igstkObliqueImageSpatialObjectRepresentation.h"
#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkImageSpatialObject.h"
#include "igstkView2D.h"
#include "igstkFLTKWidget.h"
#include "igstkCTImageReader.h"

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkEvents.h"

namespace igstk
{
namespace ObliqueImageSpatialObjectRepresentationTest
{
igstkObserverObjectMacro(CTImage,
    ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)
}
}

int igstkObliqueImageSpatialObjectRepresentationTest( 
                                                 int argc , char * argv [] )
{

  igstk::RealTimeClock::Initialize();

  typedef short    PixelType;
  const unsigned int Dimension = 3;

  typedef igstk::ImageSpatialObject< 
                                PixelType, 
                                Dimension> ImageSpatialObjectType;
  
  typedef igstk::ObliqueImageSpatialObjectRepresentation< 
                                  ImageSpatialObjectType 
                                                      >   RepresentationType;

  RepresentationType::Pointer  representation = RepresentationType::New();

  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  //
  typedef igstk::CTImageReader         ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();

  reader->SetLogger( logger );

  /* Read in a DICOM series */
  std::cout << "Reading CT image : " << argv[1] << std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );
 
  reader->RequestReadImage();

  representation->SetLogger( logger );

  // Attach an observer
  typedef igstk::ObliqueImageSpatialObjectRepresentationTest::CTImageObserver 
                                                          CTImageObserverType;

  CTImageObserverType::Pointer ctImageObserver = CTImageObserverType::New();

  reader->AddObserver(::igstk::CTImageReader::ImageModifiedEvent(),
                               ctImageObserver);

  reader->RequestGetImage();

  if(!ctImageObserver->GotCTImage())
    {
    std::cout << "No CTImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

  representation->RequestSetImageSpatialObject( ctImageObserver->GetCTImage() );

  // Exercise the TypeMacro() which defines the GetNameOfClass()
  std::string name = representation->GetNameOfClass();

  std::cout << "Name of class = " << name << std::endl;
  
  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(532,532,"CT Read View Test");
    
  typedef igstk::View2D  View2DType;
  // Create an FLTK minimal GUI
  typedef igstk::FLTKWidget      FLTKWidgetType;

  View2DType::Pointer view2D = View2DType::New();
  // instantiate FLTK widget 
  FLTKWidgetType * fltkWidget2D = 
                      new FLTKWidgetType( 10,10,280,280,"2D View");
  fltkWidget2D->RequestSetView( view2D );
  fltkWidget2D->SetLogger( logger );

  form->end();
  form->show();
  
  // Set oblique plane parameters
 
  typedef RepresentationType::PointType  PointType;
  typedef RepresentationType::VectorType VectorType;

  PointType  point;

  point[0] = 18.0;
  point[1] = -171.0;
  point[2] = -186.0;

  representation->RequestSetOriginPointOnThePlane( point );
  
  VectorType  vector1;

  vector1[0] = 2.0;
  vector1[1] = 0.0;
  vector1[2] = 0.0;

  representation->RequestSetVector1OnThePlane( vector1 );
  
  VectorType  vector2;

  vector2[0] = 0.0;
  vector2[1] = 4.0;
  vector2[2] = 0.0;

  representation->RequestSetVector2OnThePlane( vector2 );
  
  representation->RequestReslice();  

  view2D->RequestAddObject( representation );
  view2D->RequestResetCamera();

  // Set the refresh rate and start the pulse generator
  view2D->SetRefreshRate( 30 );
  view2D->RequestStart();

  for(unsigned int i=0; i<10; i++) 
    {
    std::cout << "Refresh=" << i << std::endl;
    Fl::wait( 0.01 );
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check(); 
    }
  
  view2D->RequestStop();

  delete fltkWidget2D;
  delete form;

  return EXIT_SUCCESS;
}
