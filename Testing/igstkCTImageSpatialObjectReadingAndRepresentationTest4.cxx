/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCTImageSpatialObjectReadingAndRepresentationTest4.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkCTImageReader.h"
#include "igstkCTImageSpatialObjectRepresentation.h"
#include "igstkViewNew3D.h"
#include "igstkFLTKWidget.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
// FIXCS #include "igstkWorldCoordinateReferenceSystemObject.h"
#include "igstkBoxObject.h"
#include "igstkBoxObjectRepresentation.h"
#include "igstkAxesObjectRepresentation.h"

namespace Helper
{
igstkObserverObjectMacro(CTImage,
  ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)
}

int main( int argc, char* argv[] )
{

  igstk::RealTimeClock::Initialize();

  if( argc < 2 )
    {
    std::cerr << "Usage: " << argv[0] << "  CTImageInput " << std::endl;
    return EXIT_FAILURE;
    }
  
  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput = igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  

  /* FIXCS 
  typedef igstk::WorldCoordinateReferenceSystemObject  WorldReferenceSystemType;

  WorldReferenceSystemType::Pointer worldReference = WorldReferenceSystemType::New();
  worldReference->SetSize(10,10,10);
  */

  typedef igstk::AxesObjectRepresentation  AxesRepresentationType;
  AxesRepresentationType::Pointer AxesRepresentation = AxesRepresentationType::New();
  // FIXCS AxesRepresentation->RequestSetAxesObject( worldReference );

  typedef igstk::CTImageReader         ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();

  std::cout << "Reading CT image : " << argv[1] << std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );  
  
  typedef Helper::CTImageObserver CTImageObserverType;

  CTImageObserverType::Pointer ctImageObserver = CTImageObserverType::New();

  reader->AddObserver(::igstk::CTImageReader::ImageModifiedEvent(), ctImageObserver);

  typedef igstk::ViewNew3D  View3DType;
  View3DType::Pointer view = View3DType::New();

  typedef igstk::FLTKWidget      FLTKWidgetType;

  Fl_Window * form = new Fl_Window(820,820,"CT Read View Test");
  FLTKWidgetType * fltkWidget = new FLTKWidgetType(10,10,800,800,"View 3D");

  fltkWidget->RequestSetView( view );


  form->end();
  form->show();

  reader->RequestReadImage();
  reader->RequestGetImage();

  if( !ctImageObserver->GotCTImage() )
    {
    std::cerr << "The image was expected to be received" << std::endl;
    std::cerr << " but the payload event did not arrive." << std::endl;
    return EXIT_FAILURE;
    }
     
  typedef igstk::CTImageSpatialObject  CTImageType;
  typedef CTImageType::Pointer         CTImagePointer;

  CTImagePointer ctImage = ctImageObserver->GetCTImage();

  if( ctImage->IsEmpty() )
    {
    std::cerr << "The image was empty." << std::endl;
    return EXIT_FAILURE;
    }

  ctImage->SetLogger( logger );

  igstk::Transform transform;
  transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  // FIXCS ctImage->RequestAttachToSpatialObjectParent( worldReference );
  // FIXCS ctImage->RequestSetTransformToSpatialObjectParent( transform );

  typedef igstk::CTImageSpatialObjectRepresentation RepresentationType;
  RepresentationType::Pointer imageRepresentation = RepresentationType::New();
  imageRepresentation->SetLogger( logger );
  imageRepresentation->RequestSetImageSpatialObject( ctImage );


  
  typedef igstk::BoxObject BoxObjectType;

  BoxObjectType::Pointer  boxObject = BoxObjectType::New();

  boxObject->SetSize( 10, 10, 10 );
  // FIXCS boxObject->RequestAttachToSpatialObjectParent( worldReference );
  transform.SetToIdentity( 1000 ); // 1 second
  // FIXCS boxObject->RequestSetTransformToSpatialObjectParent( transform );
 
  typedef igstk::BoxObjectRepresentation BoxRepresentationType;
  BoxRepresentationType::Pointer boxObjectRepresentation = BoxRepresentationType::New();

  boxObjectRepresentation->SetLogger( logger );
  boxObjectRepresentation->RequestSetBoxObject( boxObject );
  boxObjectRepresentation->SetColor( 1.0, 1.0, 1.0 );
  boxObjectRepresentation->SetOpacity( 1.0 );

  view->RequestAddObject( imageRepresentation );
  view->RequestAddObject( boxObjectRepresentation );
  // FIXCS view->RequestAddObject( AxesRepresentation );

  view->SetRefreshRate( 30 );
  view->SetRendererBackgroundColor( 0.8, 0.8, 0.9 );
  view->SetCameraPosition( 1000.0, 500.0, 250.0 );
  view->SetFocalPoint( 0.0, 0.0, 0.0 );
  view->SetCameraViewUp( 0, 0, 1.0 );

  // this will indirectly call CreateActors() 
  // FIXCS view->RequestAddObject( AxesRepresentation );
  view->RequestResetCamera();
  view->RequestStart();


  for( unsigned int i=0; i < 100; i++)
    {
    Fl::wait( 0.01 );
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check();       // trigger FLTK redraws
    }

  delete fltkWidget;
  delete form;
 
  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}
