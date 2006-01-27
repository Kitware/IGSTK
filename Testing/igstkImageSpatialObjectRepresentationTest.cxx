/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSpatialObjectRepresentationTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkImageSpatialObject.h"
#include "igstkView2D.h"
#include "igstkCTImageReader.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkEvents.h"

namespace igstk
{

  namespace ImageSpatialObjectRepresentationTest
  {
    class ImageRepresentationObserver : public ::itk::Command
    {
    public:
      typedef ImageRepresentationObserver  Self;
      typedef itk::SmartPointer<Self>      Pointer;
      typedef itk::Command                 Superclass;
      itkNewMacro(Self);
      void Execute(const itk::Object *caller, const itk::EventObject & event)
      {
      }
      void Execute(itk::Object *caller, const itk::EventObject & event)
      {
        const AxialSliceBoundsEvent * axialEvent = 
          dynamic_cast< const AxialSliceBoundsEvent * >( &event );
        if( axialEvent )
          {
          m_EventReceived = true;
          EventHelperType::IntegerBoundsType bounds = axialEvent->Get();
          std::cout << "Minimum Slice = " << bounds.minimum << std::endl;
          std::cout << "Maximum Slice = " << bounds.maximum << std::endl;
          }
      } 
     bool GetEventReceived()
        { 
        return m_EventReceived; 
        }
    protected:
      ImageRepresentationObserver()
      {
      m_EventReceived = false;
      }
    private:
      bool m_EventReceived;
      
    };  // end of ImageRepresentationObserver class 

  }
}



int igstkImageSpatialObjectRepresentationTest( int argc , char * argv [] )
{


  typedef short    PixelType;
  const unsigned int Dimension = 3;

  typedef igstk::ImageSpatialObject< 
                                PixelType, 
                                Dimension 
                                       > ImageSpatialObjectType;
  
  typedef igstk::ImageSpatialObjectRepresentation< 
                                  ImageSpatialObjectType 
                                                      >   RepresentationType;

  RepresentationType::Pointer  representation = RepresentationType::New();

  typedef itk::Logger              LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  // Instantiate a reader
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

  // Test error case
  representation->RequestSetImageSpatialObject( NULL );

  // Test correct case
  ImageSpatialObjectType::Pointer imageSpatialObject = ImageSpatialObjectType::New();
  representation->RequestSetImageSpatialObject( imageSpatialObject );

  // Exercise the TypeMacro() which defines the GetNameOfClass()
  std::string name = representation->GetNameOfClass();

  std::cout << "Name of class = " << name << std::endl;
  
  representation->RequestSetSliceNumber( 10 );

  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(532,532,"CT Read View Test");
    
  typedef igstk::View2D  View2DType;

  View2DType * view2D = new View2DType( 10,10,512,512,"2D View");

  form->end();
  form->show();
   
  // Test all possible orientations 
  representation->RequestSetOrientation( RepresentationType::Sagittal );
  representation->RequestSetOrientation( RepresentationType::Coronal );
  representation->RequestSetOrientation( RepresentationType::Axial );

  representation->RequestSetSliceNumber( 10 );

  typedef igstk::ImageSpatialObjectRepresentationTest::ImageRepresentationObserver 
                                                                        ObserverType;

  ObserverType::Pointer observer = ObserverType::New();

  representation->AddObserver( igstk::AxialSliceBoundsEvent(), observer );
  
  representation->RequestSetImageSpatialObject( reader->GetOutput() );

  // Test all possible orientations 
  representation->RequestSetOrientation( RepresentationType::Sagittal );
  representation->RequestSetOrientation( RepresentationType::Coronal );
  representation->RequestSetOrientation( RepresentationType::Axial );

  representation->RequestSetSliceNumber( 3 );

  representation->RequestGetSliceNumberBounds();
  
  if( observer->GetEventReceived() )
    {
    std::cout << "Event Received successfuly" << std::endl;
    }
  else
    {
    std::cerr << "Error AxialSliceBoundsEvent() was not received ";
    std::cerr << "after invoking RequestGetSliceNumberBounds()." << std::endl;
    return EXIT_FAILURE;
    }
    
  // Do manual redraws
  for(unsigned int i=0; i<5; i++)
    {
    view2D->Update();  // schedule redraw of the view
    Fl::check();       // trigger FLTK redraws
    igstk::PulseGenerator::CheckTimeouts();
    std::cout << "i= " << i << std::endl;
    }

  // Do manual redraws for each orientation while changing slice numbers
  {
  representation->RequestSetOrientation( RepresentationType::Axial );
  for(unsigned int i=0; i<5; i++)
    {
    representation->RequestSetSliceNumber( i );
    view2D->Update();  // schedule redraw of the view
    Fl::check();       // trigger FLTK redraws
    igstk::PulseGenerator::CheckTimeouts();
    std::cout << "i= " << i << std::endl;
    }
  }

  {
  representation->RequestSetOrientation( RepresentationType::Sagittal );
  for(unsigned int i=0; i<10; i++)
    {
    representation->RequestSetSliceNumber( i );
    view2D->Update();  // schedule redraw of the view
    Fl::check();       // trigger FLTK redraws
    igstk::PulseGenerator::CheckTimeouts();
    std::cout << "i= " << i << std::endl;
    }
  }

  {
  representation->RequestSetOrientation( RepresentationType::Coronal );
  for(unsigned int i=0; i<10; i++)
    {
    representation->RequestSetSliceNumber( i );
    view2D->Update();  // schedule redraw of the view
    Fl::check();       // trigger FLTK redraws
    igstk::PulseGenerator::CheckTimeouts();
    std::cout << "i= " << i << std::endl;
    }
  }


  // On purpose request non-existing slices. 
  // The requests should be ignored by the state machine.
  {
  representation->RequestSetOrientation( RepresentationType::Axial );
  for(unsigned int i=5; i<10; i++)
    {
    representation->RequestSetSliceNumber( i );
    view2D->Update();  // schedule redraw of the view
    Fl::check();       // trigger FLTK redraws
    igstk::PulseGenerator::CheckTimeouts();
    std::cout << "i= " << i << std::endl;
    }
  }


  delete view2D;
  delete form;
 

  return EXIT_SUCCESS;
}

