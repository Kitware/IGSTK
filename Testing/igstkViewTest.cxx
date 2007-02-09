/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkViewTest.cxx
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
//  Warning about: identifier was truncated to '255' characters in the 
//  debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include "igstkView2D.h"
#include "igstkView3D.h"
#include "igstkEvents.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkVTKLoggerOutput.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace ViewTest
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
    m_Form = 0;
    m_View = 0;
    }
public:

  void SetForm( Fl_Window * form )
    {
    m_Form = form;
    }
  void Execute(const itk::Object *caller, const itk::EventObject & event)
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
    }

  void SetEndFlag( bool * end )
    {
    m_End = end;
    }

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    if( ::igstk::RefreshEvent().CheckEvent( &event ) )
      {
      m_PulseCounter++;

      if( m_PulseCounter > 20 )
        {
        if( m_View )
          {
          m_View->RequestStop();
          } 
        else
          {
          std::cerr << "View pointer is NULL " << std::endl;
          }
        *m_End = true;
        return;
        }
      }
    }
private:
  
  unsigned long       m_PulseCounter;
  Fl_Window *         m_Form;
  ::igstk::View *     m_View;
  bool *              m_End;

};

}

int igstkViewTest( int, char * [] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::View2D  View2DType;
  typedef igstk::View3D  View3DType;

  bool bEnd = false;

  typedef itk::Logger              LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput = 
                                                igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from 
                                       // VTK OutputWindow -> logger


  try
    {
    // Create the ellipsoid 
    igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
    ellipsoid->SetRadius(0.1,0.1,0.1);
    
    // Create the ellipsoid representation
    igstk::EllipsoidObjectRepresentation::Pointer ellipsoidRepresentation =
                             igstk::EllipsoidObjectRepresentation::New();

    ellipsoidRepresentation->RequestSetEllipsoidObject( ellipsoid );
    ellipsoidRepresentation->SetColor(0.0,1.0,0.0);
    ellipsoidRepresentation->SetOpacity(1.0);

    // Create the cylinder 
    igstk::CylinderObject::Pointer cylinder = igstk::CylinderObject::New();
    cylinder->SetRadius(0.1);
    cylinder->SetHeight(0.5);

    // Create the cylinder representation
    igstk::CylinderObjectRepresentation::Pointer cylinderRepresentation =
                              igstk::CylinderObjectRepresentation::New();

    cylinderRepresentation->RequestSetCylinderObject( cylinder );
    cylinderRepresentation->SetColor(1.0,0.0,0.0);
    cylinderRepresentation->SetOpacity(1.0);

    const double validityTimeInMilliseconds = 1e300; // 100 seconds
    igstk::Transform transform;
    igstk::Transform::VectorType translation;
    translation[0] = 0;
    translation[1] = 0;
    translation[2] = 0;
    igstk::Transform::VersorType rotation;
    rotation.Set( 0.0, 0.0, 0.0, 1.0 );
    igstk::Transform::ErrorType errorValue = 10; // 10 millimeters

    transform.SetTranslationAndRotation( 
        translation, rotation, errorValue, validityTimeInMilliseconds );

    ellipsoid->RequestSetTransform( transform );


    translation[1] = -0.25;  // translate the cylinder along Y
    translation[2] = -2.00;  // translate the cylinder along Z
    rotation.Set( 0.7071, 0.0, 0.0, 0.7071 );

    transform.SetTranslationAndRotation( 
        translation, rotation, errorValue, validityTimeInMilliseconds );

    cylinder->RequestSetTransform( transform );

    cylinderRepresentation->SetLogger( logger );
  
    // Create an FLTK minimal GUI
    Fl_Window * form = new Fl_Window(601,301,"View Test");
    
    View2DType * view2D = new View2DType( 10,10,280,280,"2D View");
    View3DType * view3D = new View3DType(310,10,280,280,"3D View");

    // Exercise GetNameOfClass() method
    std::cout << view2D->View2DType::Superclass::GetNameOfClass() << std::endl;
    std::cout << view3D->GetNameOfClass() << std::endl;
    std::cout << view3D->GetNameOfClass() << std::endl;
      
    form->end();
    // End of the GUI creation

    form->show();

    // Exercice some view functions
    view2D->Initialize();
    view2D->Enable();
    view2D->Render();
    view2D->Update();
    
    view2D->RequestResetCamera();
    view2D->RequestEnableInteractions();
    
    view3D->RequestResetCamera();
    view3D->RequestEnableInteractions();
    
    // Add the ellipsoid to the view
    view2D->RequestAddObject( ellipsoidRepresentation );
    
    // Add the cylinder to the view
    view3D->RequestAddObject( cylinderRepresentation );
    
    // Exercise error conditions.
    //
    // Attempt to add an object that already exists.
    view3D->RequestAddObject( ellipsoidRepresentation );


    // Set the refresh rate and start 
    // the pulse generators of the views.
    view2D->RequestSetRefreshRate( 30 );
    view3D->RequestSetRefreshRate( 10 );

    view2D->RequestStart();
    view3D->RequestStart();

    // Do manual redraws
    for(unsigned int i=0; i<10; i++)
      {
      Fl::wait(0.01);
      igstk::PulseGenerator::CheckTimeouts();
      Fl::check();       // trigger FLTK redraws
      }

    view2D->RequestDisableInteractions();
    view3D->RequestDisableInteractions();

    // Remove the ellipsoid from the view
    view2D->RequestRemoveObject( ellipsoidRepresentation );
    
    // Remove the cylinder from the view
    view3D->RequestRemoveObject( cylinderRepresentation );

    // Exercise error conditions.
    //
    // Attempt to remove an object that already was removed
    view3D->RequestRemoveObject( ellipsoidRepresentation );


    // Exercise error conditions.
    //
    // Attempt to add an object with null pointer
    view3D->RequestAddObject( 0 );
    
    // Exercise error conditions.
    //
    // Attempt to remove an object with null pointer
    view3D->RequestRemoveObject( 0 );

    // Exercise error conditions.
    //
    // Attempt to add an annotation with a null pointer
    view3D->RequestAddAnnotation2D( 0 );
 
    // Do automatic redraws using the internal PulseGenerator
    view2D->RequestAddObject( ellipsoidRepresentation );
    view3D->RequestAddObject( cylinderRepresentation );
    typedef ViewTest::ViewObserver ObserverType;
    ObserverType::Pointer viewObserver = ObserverType::New();
    
    viewObserver->SetView( view3D );
    viewObserver->SetForm( form );
    viewObserver->SetEndFlag( &bEnd );

    // Exercise the code for resizing the window
    form->resize(100, 100, 600, 300);

    // Exercise and test the Print() methods
    view2D->Print( std::cout, 0 );
    view3D->Print( std::cout, 0 );

    std::cout << *view2D << std::endl;
    std::cout << *view3D << std::endl;

    while(1)
      {
      Fl::wait(0.01);
      igstk::PulseGenerator::CheckTimeouts();
      if( bEnd )
        {
        break;
        }
      }

    // Exercise the screenshot option with a valid filename
    view2D->RequestSaveScreenShot("igstkViewTestScreenshot1.png");
    
    // Exercise the screenshot option with an valid filename
    view2D->RequestSaveScreenShot("igstkViewTestScreenshot1.jpg");

    // at this point the observer should have hid the form

    delete view2D;
    delete view3D;
    delete form;
    }
  catch(...)
    {
    std::cerr << "Exception catched !!" << std::endl;
    return EXIT_FAILURE;
    }

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
 
  return EXIT_SUCCESS;
}
