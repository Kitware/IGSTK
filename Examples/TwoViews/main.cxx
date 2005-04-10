/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    main.cxx
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
#pragma warning( disable : 4284 )
#endif

#include "TwoViews.h"
#include "igstkScene.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkMouseTracker.h"
#include "vtkInteractorObserver.h"

int main(int , char** )
{ 
  TwoViews* m_GUI = new TwoViews();
  m_GUI->Window->show();

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

  // Add the ellipsoid to the scene
  scene->RequestAddObject(m_GUI->Display1,ellipsoidRepresentation);

  // Add the cylinder to the scene
  scene->RequestAddObject(m_GUI->Display1,cylinderRepresentation);

  // Add another Object representations to the second display
  scene->RequestAddObject(m_GUI->Display2,ellipsoidRepresentation->Copy());
  scene->RequestAddObject(m_GUI->Display2,cylinderRepresentation->Copy());

  m_GUI->Display2->RequestResetCamera();
  m_GUI->Display2->Update();

  // Enable interactions
  m_GUI->Display2->RequestEnableInteractions();

  m_GUI->Display1->RequestResetCamera();
  m_GUI->Display1->Update();

  // Enable interactions
  m_GUI->Display1->RequestEnableInteractions();

  m_GUI->MouseTrackerBox->SetObjectToTrack(ellipsoid);
  m_GUI->MouseTrackerBox->SetView(m_GUI->Display1);
  m_GUI->MouseTrackerBox->SetView2(m_GUI->Display2);
   
  scene->RequestRemoveObject(m_GUI->Display1,cylinderRepresentation);
  m_GUI->Display1->Update();

  m_GUI->Display1->RequestSetRefreshRate( 30 ); // 30 Hz
  m_GUI->Display2->RequestSetRefreshRate( 30 ); // 30 Hz

  m_GUI->Display1->Start();
  m_GUI->Display2->Start();

  Fl::run();

  delete m_GUI;

  return EXIT_SUCCESS;
}
