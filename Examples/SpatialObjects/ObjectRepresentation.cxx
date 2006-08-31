/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    ObjectRepresentation.cxx
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

// Software Guide : BeginLatex
//
// \index{igstk::BoxObjectRepresentation}
// This example describes how to use the \doxygen{BoxObjectRepresentation}
// to display a \doxygen{BoxObject} in a \doxygen{View3D}
// This example also uses FLTK to create a window, so we include the appropriate header
// files.
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkBoxObjectRepresentation.h"
#include <FL/Fl_Window.h>
#include <igstkView3D.h>
// Software Guide : EndCodeSnippet

int main( int , char *[] )
{
// Software Guide : BeginLatex
//
// Like any applications in IGSTK we first initialize the RealTimeClock.
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
  typedef igstk::BoxObject  ObjectType;
  typedef igstk::BoxObjectRepresentation  ObjectRepresentationType;
  ObjectType::Pointer cube = ObjectType::New();
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// The appropriate object representation for the BoxObject is created
// using standard typedef and smart pointers.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  ObjectRepresentationType::Pointer 
                         cubeRepresentation = ObjectRepresentationType::New();
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// Every ObjectRepresentation have an RGB color and an opacity as rendering
// parameters. These two parameters can be tuned using the SetColor(R,G,B)
// and SetOpacity() functions respectively.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  cubeRepresentation->SetColor( 0.0, 0.0, 1.0 );
  cubeRepresentation->SetOpacity( 1.0 );
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// Then we tell the ObjectRepresentation to get the geometry
// from the BoxSpatialObject. Internally the ObjectRepresentation
// creates VTK actors from the actual geometry of the SpatialObject.
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
  View3DType * view3D = new View3DType(6,6,500,500,"View 3D");

  form->end();
  form->show();
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// We set the current representation of the object to the view 
// using the RequestAddObject() function.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet  
  view3D->RequestAddObject( cubeRepresentation  );
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// We set the refresh rate of the view and we enable interactions.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  view3D->RequestSetRefreshRate( 0.1 );
  view3D->RequestEnableInteractions();
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
// We delete the view3D and form since they are not using
// smart pointers.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  delete view3D;
  delete form;
// Software Guide : EndCodeSnippet
  return EXIT_SUCCESS;
}
