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

        if( m_PulseCounter > 10 )
          {
          if( m_View )
            {
            m_View->RequestStop();
            } 
          else
            {
            std::cerr << "View pointer is NULL " << std::endl;
            }
          if( m_Form )
            {
            m_Form->hide();
            }
          *m_End = true;
          return;
          }
        }
      }
  private:
    unsigned long       m_PulseCounter;
    Fl_Window          *m_Form;
    ::igstk::View      *m_View;
    bool *              m_End;
  };



}


int igstkViewTest( int, char * [] )
{

  typedef igstk::View2D  View2DType;
  typedef igstk::View3D  View3DType;

  bool bEnd = false;

  try
    {

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
    Fl_Window * form = new Fl_Window(601,301,"View3D Test");
    
    View2DType * view2D = new View2DType( 10,10,280,280,"2D View");
    View3DType * view3D = new View3DType(310,10,280,280,"3D View");

    // Exercise GetNameOfClass() method
    std::cout << view2D->View2DType::Superclass::GetNameOfClass() << std::endl;
    std::cout << view3D->GetNameOfClass() << std::endl;
    std::cout << view3D->GetNameOfClass() << std::endl;
      
    form->end();
    // End of the GUI creation

    form->show();
    
    view2D->RequestResetCamera();
    view2D->RequestEnableInteractions();
    
    view3D->RequestResetCamera();
    view3D->RequestEnableInteractions();
    
 
    // Add the ellipsoid to the view
    view2D->RequestAddObject( ellipsoidRepresentation );
    
    // Add the cylinder to the view
    view3D->RequestAddObject( cylinderRepresentation );
    
    // Do manual redraws
    for(unsigned int i=0; i<10; i++)
      {
      view2D->Update();  // schedule redraw of the view
      view3D->Update();  // schedule redraw of the view
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
    // Attempt to add an object with null pointer
    view3D->RequestAddObject( 0 );

    // Exercise error conditions.
    //
    // Attempt to remove an object with null pointer
    view3D->RequestRemoveObject( 0 );

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

    // Exercise and test the PrintSelf() methods
    view2D->PrintSelf(std::cout, 0);
    view3D->PrintSelf(std::cout, 0);

    view2D->RequestStart();
    view3D->RequestStart();

    std::cout << *view2D << std::endl;
    std::cout << *view3D << std::endl;

    view2D->RequestSetRefreshRate( 30 );
    view3D->RequestSetRefreshRate( 10 );


    while(1)
      {
      Fl::wait(0.001);
      igstk::PulseGenerator::CheckTimeouts();
      if( bEnd )
        {
        break;
        }
      }


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





  return EXIT_SUCCESS;
}


