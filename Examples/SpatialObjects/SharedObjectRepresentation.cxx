/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    SharedObjectRepresentation.cxx
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
// \index{SharedObjectRepresentation}
// This example describes how to share object representations between views.
// We extend the previous example and focus only on the object representation
// sharing. Please refer to the previous example.
//
// Software Guide : EndLatex 

#include "igstkBoxObjectRepresentation.h"
#include <FL/Fl_Window.h>
#include <igstkView3D.h>

int main( int , char *[] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::BoxObject                ObjectType;
  typedef igstk::BoxObjectRepresentation  ObjectRepresentationType;
  ObjectType::Pointer cube = ObjectType::New();

  ObjectRepresentationType::Pointer 
                         cubeRepresentation = ObjectRepresentationType::New();

  cubeRepresentation->SetColor( 0.0, 0.0, 1.0 );
  cubeRepresentation->SetOpacity( 1.0 );

  cubeRepresentation->RequestSetBoxObject(cube);
// Software Guide : BeginLatex
//
// We now add a second View to our window. 
// We have \code{View3D1} and \code{View3D2} as two \doxygen{View3D}s.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  Fl_Window * form = new Fl_Window(512,262,"Sharing Object Representations");

  typedef igstk::View3D  View3DType;
  View3DType * view3D1 = new View3DType(6,6,250,260,"View 3D 1");
  View3DType * view3D2 = new View3DType(260,6,250,260,"View 3D 2");

  form->end();
  form->show();
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// We set the current representation of the object to the first view 
// using the  \code{RequestAddObject()} function.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet  
  view3D1->RequestAddObject( cubeRepresentation  );
 
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// For this example, we create a second object representation
// and we set the color to be red and the opacity to $0.5$.
// We set the same BoxSpatialObject to the geometry.
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  ObjectRepresentationType::Pointer 
                         cubeRepresentation2 = ObjectRepresentationType::New();
  cubeRepresentation2->SetColor( 1.0, 0.0, 0.0 );
  cubeRepresentation2->SetOpacity( 0.5 );

  cubeRepresentation2->RequestSetBoxObject(cube);
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// We then add the newly created representation to the second view. 
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  view3D2->RequestAddObject( cubeRepresentation2  );
// Software Guide : EndCodeSnippet

  view3D1->RequestSetRefreshRate( 0.1 );
  view3D1->RequestEnableInteractions();
  view3D1->RequestStart();
  view3D1->RequestResetCamera();
  
  view3D2->RequestSetRefreshRate( 0.1 );
  view3D2->RequestEnableInteractions();
  view3D2->RequestStart();
  view3D2->RequestResetCamera();

  for(unsigned int i=0;i<100;i++)
    {
    Fl::wait(0.05);
    igstk::PulseGenerator::CheckTimeouts();
    }

// Software Guide : BeginLatex
//
// We then remove the current object representation from
// the second view using the \code{RequestRemoveObject} function.
//
// Software Guide : EndLatex
// Software Guide : BeginCodeSnippet
  view3D2->RequestRemoveObject( cubeRepresentation2 );
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// An important function of the ObjectRepresentation is the 
// \code{Copy()} function which creates a deep copy of the 
// current representation as shown below.
//
// Software Guide : EndLatex
// Software Guide : BeginCodeSnippet
  view3D2->RequestAddObject( cubeRepresentation->Copy() );
// Software Guide : EndCodeSnippet
  view3D2->RequestResetCamera();
  view3D2->Update();

  while(form->visible())
    {
    Fl::wait(0.05);
    igstk::PulseGenerator::CheckTimeouts();
    }

  delete view3D1;
  delete view3D2;
  delete form;

  return EXIT_SUCCESS;
}
