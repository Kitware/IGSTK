/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMeshObjectTest2.cxx
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
// Warning about: identifier was truncated to '255' characters in the 
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkMeshObject.h"
#include "igstkMeshObjectRepresentation.h"
#include "igstkMeshReader.h"

#include "igstkConfigure.h"
#include "igstkView2D.h"
#include "igstkFLTKWidget.h"
#include "igstkRealTimeClock.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkDefaultWidget.h"

namespace igstk
{

  namespace VisibilityObjectTest
  {

    class ViewObserver : public ::itk::Command 
    {
    public:

      typedef  ViewObserver               Self;
      typedef  ::itk::Command             Superclass;
      typedef  ::itk::SmartPointer<Self>  Pointer;
      itkNewMacro( Self );

    protected:

      ViewObserver() 
      {
        m_PulseCounter = 0;
        m_NumberOfPulsesToStop = 50;
        m_View = 0;
        m_End = NULL;
      }
    public:

      void Reset()
      {
        if( m_End )
        {
          *m_End = false;
        }
      }
      void SetEndFlag( bool * end )
      {
        m_End = end;
      }

      void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
      {
        std::cerr << "Execute( const * ) should not be called" << std::endl;
      }

      void SetView( ::igstk::View * view )
      {
        m_View = view;
        if( m_View )
        {
          m_View->AddObserver( ::igstk::RefreshEvent(), this );
        }
        m_PulseCounter = 0;
      }

      void SetNumberOfPulsesToStop( unsigned long number )
      {
        m_NumberOfPulsesToStop = number;
        m_PulseCounter = 0;
      }

      void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & event)
      {
        if( ::igstk::RefreshEvent().CheckEvent( &event ) )
        {
          m_PulseCounter++;

          if( m_PulseCounter > m_NumberOfPulsesToStop )
          {
            *m_End = true;
            return;
          }
          else
          {
            *m_End = false;
          }
        }
      }

    private:

      unsigned long              m_PulseCounter;
      unsigned long              m_NumberOfPulsesToStop;
      ::igstk::View::Pointer     m_View;
      bool *                     m_End;
    };

  } // end of VisibilityObjectTest namespace

} // end namespace igstk

igstkObserverObjectMacro( MeshObject, 
                         igstk::MeshReader::MeshModifiedEvent,
                         igstk::MeshObject);

int igstkMeshObjectTest2( int argc, char * argv [] )
{
  if( argc < 3 )
  {
    std::cerr << "Usage: " << argv[0] 
    << "Mesh file "
    << "Screen save"
    << std::endl;
    return EXIT_FAILURE;
  }

  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput               LogOutputType;

  typedef igstk::MeshReader                     ReaderType;
  typedef igstk::MeshObject                     ObjectType;
  typedef igstk::MeshObjectRepresentation       RepresentationType;
  
  typedef igstk::View2D                         View2DType;
  typedef igstk::DefaultWidget                  WidgetType;

  igstk::RealTimeClock::Initialize();

  // logger object created for logging mouse activities
  LoggerType::Pointer      logger = LoggerType::New();
  LogOutputType::Pointer   logOutput= LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( LoggerType::DEBUG );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput = 
                                                igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  

  ObjectType::Pointer            spatialObject = ObjectType::New();
  RepresentationType::Pointer    representation = RepresentationType::New();
  representation->SetLogger( logger );
  spatialObject->SetLogger( logger );

  ReaderType::Pointer  reader = ReaderType::New();
  reader->SetLogger( logger );

  std::string filename = argv[1];
  reader->RequestSetFileName( filename );
  reader->RequestReadObject();
  
  // Attach an observer
  MeshObjectObserver::Pointer observer = MeshObjectObserver::New();
 
  reader->AddObserver(igstk::MeshReader::MeshModifiedEvent(),observer);

  try
    {
    reader->RequestGetOutput();
    }
  catch( ... )
    {
    std::cerr << "ERROR: An exception was thrown while reading the MeshObject"
      << std::endl;
    std::cerr << "This should not have happened. The StateMachine should have" 
      << std::endl;
    std::cerr << "caught that exception and converted it into a SM Input " 
      << std::endl;
    return EXIT_FAILURE;
    }

  if(!observer->GotMeshObject())
    {
    std::cout << "No MeshObject!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  
  spatialObject = observer->GetMeshObject();
  
  representation->RequestSetMeshObject( spatialObject );
   representation->SetColor(1,0,0);

  Fl_Window * form = new Fl_Window(280,280,"Mesh read & display tests");
  // Setup the Widget and View
  WidgetType * widget = new WidgetType( 280, 280 );
  View2DType::Pointer view = View2DType::New();
  widget->RequestSetView( view );

  form->end();
  form->show();


  view->SetLogger( logger );
  view->RequestAddObject( representation );

  igstk::Transform transform;
  transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
  view->RequestSetTransformAndParent( transform, spatialObject );

  // Reseting the camera after reading the image is more effective
  view->RequestResetCamera();

  // Configuring the view refresh rate
  view->SetRefreshRate( 40 );

  bool bEnd = false;
  
  typedef ::igstk::VisibilityObjectTest::ViewObserver  ViewObserverType;
  ViewObserverType::Pointer viewObserver = ViewObserverType::New();

  viewObserver->SetView( view );
  viewObserver->SetEndFlag( &bEnd );

  viewObserver->SetNumberOfPulsesToStop( 50 );
  viewObserver->Reset();
  view->RequestStart();

  // Check for FLTK events before we start checking 
  // IGSTK events. This should allow the widget to 
  // be displayed first.
  while( !bEnd )
  {
    igstk::PulseGenerator::Sleep(20);
    igstk::PulseGenerator::CheckTimeouts();
  }
  
  view->RequestStop();

  // Testing UpdateRepresentationFromGeometry. Changing the Spatial Object
  // geometrical parameters should trigger an update in the representation
  // class.
  std::cout << "Testing set properties : ";
  representation->SetColor(0.9,0.7,0.1);
  representation->SetOpacity(0.8);


  viewObserver->SetNumberOfPulsesToStop( 50 );
  viewObserver->Reset();
  view->RequestStart();
  std::string screenShotFileName = argv[2];

  while( !bEnd )
  {
    igstk::PulseGenerator::Sleep(20);
    igstk::PulseGenerator::CheckTimeouts();
  }

  view->RequestStop();
  igstk::PulseGenerator::Sleep(1);
  view->RequestSaveScreenShot( screenShotFileName );
  viewObserver->SetView(NULL);

  std::cout << representation << std::endl;
  std::cout << spatialObject << std::endl;

  delete widget;
  delete form;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
