/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSceneTest.cxx
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
#include "igstkScene.h"
#include "igstkView3D.h"
#include "igstkEllipsoidObjectRepresentation.h"

int igstkSceneTest( int, char * [] )
{

  try
    {
    typedef igstk::Scene  SceneType;
    SceneType::Pointer scene = SceneType::New();

    typedef igstk::EllipsoidObjectRepresentation  ObjectType;
    ObjectType::Pointer ellipsoid = ObjectType::New();
     
    // Create an FLTK minimal GUI
    Fl_Window * form = new Fl_Window(600,300,"View3D Test");
   
    igstk::View3D* view = new igstk::View3D(0,0,0,0);

    form->end();

    // End of the GUI creation

    form->show();
    
    view->RequestResetCamera();
      
    unsigned int i=0;
    

    // Add Object
    std::cout << "Testing RequestAddObject: ";
    scene->RequestAddObject(view,ellipsoid); 
    std::cout << "[PASSED]" << std::endl;
 
    for(i=0; i<50; i++)
      {
      view->Update();  // schedule redraw of the view
      Fl::check();     // trigger FLTK redraws
      }



    // Remove object
    std::cout << "Testing RequestRemoveObject: ";
    scene->RequestRemoveObject(view,ellipsoid); 
    std::cout << "[PASSED]" << std::endl;

    for(i=0; i<50; i++)
      {
      view->Update();  // schedule redraw of the view
      Fl::check();     // trigger FLTK redraws
      }

    // Testing the igstkSpatialObject
    std::cout << "Testing SpatialObject: ";
    igstk::SpatialObject::Pointer object = igstk::SpatialObject::New();

    double validityTimeInMilliseconds = 2000.0;
    igstk::Transform transform;
    igstk::Transform::VectorType translation;
    translation[0] = 0;
    translation[1] = 1;
    translation[2] = 2;
    transform.SetTranslation( translation, validityTimeInMilliseconds );
    object->SetTransform( transform );

    object->GetOffset();
    object->GetMatrix();
    object->GetNameOfClass();
    std::cout << "[PASSED]" << std::endl;

    // Testing Removing all objects, first add an object
    scene->RequestAddObject(view,ellipsoid); 
    std::cout << "Testing Removing all object: ";

    for(i=0; i<50; i++)
      {
      view->Update();  // schedule redraw of the view
      Fl::check();     // trigger FLTK redraws
      }

    //... now remove all
    scene->RequestRemoveAllObjects(); 

    for(i=0; i<50; i++)
      {
      view->Update();  // schedule redraw of the view
      Fl::check();     // trigger FLTK redraws
      }

    std::cout << "[PASSED]" << std::endl;


    // Testing printself
    scene->Print(std::cout);
    scene->GetNameOfClass();

    } 
  catch(...)
    {
    std::cerr << "Exception catched !!!" << std::endl;
    return EXIT_FAILURE;
    }


  return EXIT_SUCCESS;
}


