/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    ObjectRepresentation.cxx
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

// Software Guide : BeginLatex
//
// \index{igstk::BoxObjectRepresentation}
// This example describes how to use the \doxygen{BoxObjectRepresentation}
// to display a \doxygen{BoxObject} in a \doxygen{View3D}.
// 
// This example also uses the Fast Light GUI Toolkit (FLTK) to create a window, 
// therefore we include the appropriate header files.
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkBoxObjectRepresentation.h"
#include <FL/Fl_Window.H>
#include <igstkView3D.h>
#include <igstkFLTKWidget.h>
// Software Guide : EndCodeSnippet

int main( int , char *[] )
{
// Software Guide : BeginLatex
//
// Like any applications in IGSTK we first initialize the RealTimeClock to ensure
// proper synchronization between the different components.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
 igstk::RealTimeClock::Initialize();
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// Then we create a cube of size $10mm$ using the \doxygen{BoxObject}
// class.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::BoxObject                ObjectType;
  typedef igstk::BoxObjectRepresentation  ObjectRepresentationType;
  ObjectType::Pointer cube = ObjectType::New();
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// The appropriate object representation for the \doxygen{BoxObject} is created
// using standard typedef and smart pointers.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  ObjectRepresentationType::Pointer 
                         cubeRepresentation = ObjectRepresentationType::New();
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// Every ObjectRepresentation has a color and an opacity as rendering
// parameters. These two parameters can be tuned using 
// the \code{SetColor(R,G,B)} and \code{SetOpacity()} functions respectively.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  cubeRepresentation->SetColor( 0.0, 0.0, 1.0 );
  cubeRepresentation->SetOpacity( 1.0 );
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// Then set the BoxSpatialObject geometry to the ObjectRepresentation.
// Internally the ObjectRepresentation creates VTK actors 
// from the actual geometry of the \doxygen{SpatialObject}.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  cubeRepresentation->RequestSetBoxObject(cube);
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// We then define the GUI window and the view.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  Fl_Window * form = new Fl_Window(512,512,"Displaying my first object");

  typedef igstk::View3D  View3DType;

  // Create an FLTK minimal GUI
  typedef igstk::FLTKWidget      FLTKWidgetType;

  View3DType::Pointer view3D = View3DType::New();

  // instantiate FLTK widget 
  FLTKWidgetType * fltkWidget3D = 
                      new FLTKWidgetType( 6,6,500,500,"3D View");
  fltkWidget3D->RequestSetView( view3D );

  form->end();
  form->show();
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// We set the current representation of the object to the view 
// using the  \code{RequestAddObject()} function.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet  
  view3D->RequestAddObject( cubeRepresentation  );
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// We set the refresh rate of the view. The refresh rate defines
// when the view should be rendered. Note that a faster refresh rate
// might be requested but might not be achieved, depending on the capabilities
// of the system.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  view3D->SetRefreshRate( 0.1 );
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// A spatial relationship must be established between the View and the Object
// to be visualized. In this case we define it as an identity transform, which
// means that both of them refer to the same coordinate system.
//
// Software Guide : EndLatex 
// Software Guide : BeginCodeSnippet
  igstk::Transform transform;
  transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  cube->RequestSetTransformAndParent( transform, view3D );
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// Finally we request the view to start rendering the scene.
//
// Software Guide : EndLatex 
// Software Guide : BeginCodeSnippet
  view3D->RequestStart();
// Software Guide : EndCodeSnippet
  view3D->RequestResetCamera();

// Software Guide : BeginLatex
//
// We then refresh the display until the window is closed.
// Note that we use the internal timer from FLTK to make sure the view has
// time to be rendered. The \code{PulseGenerator::CheckTimeouts()}
// function is the heart of the system which tells the views to refresh itself.
//
// Software Guide : EndLatex 
// Software Guide : BeginCodeSnippet
  while(form->visible())
    {
    Fl::wait(0.05);
    igstk::PulseGenerator::CheckTimeouts();
    view3D->RequestResetCamera();
    }

// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// At the end, we delete the fltkWidget3D and form since they are not using
// smart pointers.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  delete fltkWidget3D;
  delete form;
// Software Guide : EndCodeSnippet
  return EXIT_SUCCESS;
}
