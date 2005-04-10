/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkViewTest.cxx
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
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include "igstkView2D.h"
#include "igstkView3D.h"
#include "igstkEvents.h"
#include "igstkScene.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"

namespace ViewTest
{
  class ViewObserver : public ::itk::Command 
  {
  public:
    typedef  ViewObserver   Self;
    typedef  ::itk::Command    Superclass;
    typedef  ::itk::SmartPointer<Self>  Pointer;
    itkNewMacro( Self );
  protected:
    ViewObserver() 
      {
      m_PulseCounter = 0;
      m_Form = 0;
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
        
    void Execute(itk::Object *caller, const itk::EventObject & event)
      {
        
      ::igstk::View * view = 
        dynamic_cast< ::igstk::View * >( caller );
      
      if( ::igstk::RefreshEvent().CheckEvent( &event ) )
        {
        m_PulseCounter++;

        if( m_PulseCounter > 20 )
          {
          view->Stop();
          if( m_Form )
            {
            m_Form->hide();
            }
          return;
          }
        }
      }
  private:
    unsigned long       m_PulseCounter;
    Fl_Window *         m_Form;
  };



}


int igstkViewTest( int, char * [] )
{

  typedef igstk::View2D  View2DType;
  typedef igstk::View3D  View3DType;

  try
    {
    // Create the scene
    igstk::Scene::Pointer scene = igstk::Scene::New();

    // Create the ellipsoid 
    igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
    ellipsoid->SetRadius(1,1,1);
    
    // Create the ellipsoid representation
    igstk::EllipsoidObjectRepresentation::Pointer ellipsoidRepresentation = igstk::EllipsoidObjectRepresentation::New();
    ellipsoidRepresentation->RequestSetEllipsoidObject( ellipsoid );
    ellipsoidRepresentation->SetColor(0.0,1.0,0.0);
    ellipsoidRepresentation->SetOpacity(1.0);

    // Create the cylinder 
    igstk::CylinderObject::Pointer cylinder = igstk::CylinderObject::New();
    cylinder->SetRadius(0.1);
    cylinder->SetHeight(3);

    // Create the cylinder representation
    igstk::CylinderObjectRepresentation::Pointer cylinderRepresentation = igstk::CylinderObjectRepresentation::New();
    cylinderRepresentation->RequestSetCylinderObject( cylinder );
    cylinderRepresentation->SetColor(1.0,0.0,0.0);
    cylinderRepresentation->SetOpacity(1.0);

    // Create an FLTK minimal GUI
    Fl_Window * form = new Fl_Window(600,300,"View3D Test");
    
    View2DType * view2D = new View2DType( 10,10,280,280,"2D View");
    View3DType * view3D = new View3DType(310,10,280,280,"3D View");

    form->end();
    // End of the GUI creation

    form->show();
    
    view2D->RequestResetCamera();
    view2D->RequestEnableInteractions();
    
    view3D->RequestResetCamera();
    view3D->RequestEnableInteractions();
    
 
    // Add the ellipsoid to the scene
    scene->RequestAddObject(view2D,ellipsoidRepresentation);
    
    // Add the cylinder to the scene
    scene->RequestAddObject(view3D,cylinderRepresentation);

    
    // Do manual redraws
    for(unsigned int i=0; i<100; i++)
      {
      view2D->Update();  // schedule redraw of the view
      view3D->Update();  // schedule redraw of the view
      Fl::check();       // trigger FLTK redraws
      }



    view2D->RequestDisableInteractions();
    view3D->RequestDisableInteractions();

    // Remove the ellipsoid from the scene
    scene->RequestRemoveObject(view2D,ellipsoidRepresentation);
    
    // Remove the cylinder from the scene
    scene->RequestRemoveObject(view3D,cylinderRepresentation);

    // Exercise error conditions.
    //
    // Attempt to add an object with null pointer
    scene->RequestAddObject(view3D,0);
    // Attempt to add a view with null pointer
    scene->RequestAddObject(0,cylinderRepresentation);
    // Attempt to add view and object with null pointers
    scene->RequestAddObject(0,0);

    // Exercise error conditions.
    //
    // Attempt to remove an object with null pointer
    scene->RequestRemoveObject(view3D,0);
    // Attempt to remove a view with null pointer
    scene->RequestRemoveObject(0,cylinderRepresentation);
    // Attempt to remove view and object with null pointers
    scene->RequestRemoveObject(0,0);


    // Do automatic redraws using the internal PulseGenerator
    scene->RequestAddObject(view2D,ellipsoidRepresentation);
    scene->RequestAddObject(view3D,cylinderRepresentation);
    typedef ViewTest::ViewObserver ObserverType;
    ObserverType::Pointer viewObserver = ObserverType::New();
    view3D->AddObserver( ::igstk::RefreshEvent(), viewObserver );
    viewObserver->SetForm( form );

    view3D->Start();

    Fl::run();

    // at this point the observer should have hide the form

    delete view2D;
    delete view3D;
    delete form;
    }
  catch(...)
    {
    std::cerr << "Exception catched !!" << std::endl;
    return EXIT_FAILURE;
    }





  return EXIT_SUCCESS;
}


