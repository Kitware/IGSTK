/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUltrasoundImageSimulatorTest.cxx
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
//  Warning about: identifier was truncated to '255' characters
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkEvents.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkObject.h"
#include "igstkMRImageReader.h"
#include "igstkUltrasoundImageSimulator.h"
#include "igstkUSImageObjectRepresentation.h"
#include "igstkView2D.h"
#include "igstkFLTKWidget.h"

namespace UltrasoundImageSimulatorTest
{
igstkObserverObjectMacro(MRImage,
    ::igstk::MRImageReader::ImageModifiedEvent,::igstk::MRImageSpatialObject)

// Simulate an US from the MR image
typedef igstk::UltrasoundImageSimulator<igstk::MRImageSpatialObject>
                                                              USSimulatorType;
typedef USSimulatorType::ImageModifiedEvent        USImageModifiedEventType;

igstkObserverObjectMacro(SimulatedUSImage,
                         USImageModifiedEventType,
                         igstk::USImageObject)
}

int igstkUltrasoundImageSimulatorTest( int argc, char * argv[] )
{
  if(argc<3)
    {
    std::cout << "Usage = " << argv[0] << " directory"
              << argv[1] << "output image" << std::endl;
    return EXIT_FAILURE;
    }

  igstk::MRImageReader::Pointer MRReader = igstk::MRImageReader::New();
  MRReader->RequestSetDirectory(argv[1]);
  MRReader->RequestReadImage();

  typedef UltrasoundImageSimulatorTest::MRImageObserver
                                                        MRImageObserverType;
  MRImageObserverType::Pointer mrImageObserver = MRImageObserverType::New();

  MRReader->AddObserver(::igstk::MRImageReader::ImageModifiedEvent(),
                            mrImageObserver);

  MRReader->RequestGetImage();

  if( !mrImageObserver->GotMRImage() )
    {
    std::cout << "No MRImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

  // Simulate an US from the MR image
  typedef UltrasoundImageSimulatorTest::USSimulatorType USSimulatorType;
  USSimulatorType::Pointer usSimulator = USSimulatorType::New();

  usSimulator->RequestSetImageGeometricModel(mrImageObserver->GetMRImage());

  typedef igstk::Transform TransformType;
  TransformType usTransform;
  TransformType::VectorType translation;
  translation.Fill( 0 );
  translation[0] = 3;
  translation[1] = 2;
  translation[2] = 1;
  usTransform.SetTranslation( translation, 0, 10000 );

  usSimulator->RequestSetTransform(usTransform);
  usSimulator->RequestReslice();

  usSimulator->Print( std::cout );

  typedef UltrasoundImageSimulatorTest::SimulatedUSImageObserver
                                                     SimulatedUSImageObserver;
  SimulatedUSImageObserver::Pointer usImageObserver
                                            = SimulatedUSImageObserver::New();
  usSimulator->AddObserver(USSimulatorType::ImageModifiedEvent(),
                             usImageObserver);

  usSimulator->RequestGetImage();

  if(!usImageObserver->GotSimulatedUSImage())
    {
    std::cout << "No USImage!" << std::endl;
    return EXIT_FAILURE;
    }

  igstk::USImageObject::Pointer usImage =
                                       usImageObserver->GetSimulatedUSImage();

  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(532,532,"UltrasoundImageSimulatorTest");

  typedef igstk::View2D          View2DType;
  typedef igstk::FLTKWidget      FLTKWidgetType;

  View2DType::Pointer view2D = View2DType::New();

  // instantiate FLTK widget
  FLTKWidgetType * fltkWidget2D =
                    new FLTKWidgetType( 10,10,512,512,"2D View");
  fltkWidget2D->RequestSetView( view2D );


  form->end();
  form->show();

  // Add the image object representation to the view
  typedef igstk::USImageObjectRepresentation ImageRepresentationType;
  ImageRepresentationType::Pointer imageRepresentation
                                            = ImageRepresentationType::New();

  view2D->RequestAddObject( imageRepresentation );

  imageRepresentation->SetWindowLevel( 255 / 2.0, 255 / 2.0 );
  imageRepresentation->RequestSetImageSpatialObject( usImage );

  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
  usImage->RequestSetTransformAndParent( identity, view2D );

  imageRepresentation->RequestSetOrientation( ImageRepresentationType::Axial );

  view2D->RequestSetOrientation( igstk::View2D::Axial );

  imageRepresentation->RequestSetSliceNumber( 0 );

  view2D->RequestResetCamera();
  view2D->SetRefreshRate( 30 );
  view2D->RequestStart();

  Fl::wait(1.0);
  // Check FLTK events first to get the widget displayed before the view.
  Fl::check();   
  igstk::PulseGenerator::CheckTimeouts();

  // Save screenshots in a file
  std::string filename;
  filename = argv[2];
  std::cout << "Saving a screen shot in file:" << argv[2] << std::endl;
  view2D->RequestSaveScreenShot( filename );

  delete fltkWidget2D;
  delete form;

  return EXIT_SUCCESS;
}
