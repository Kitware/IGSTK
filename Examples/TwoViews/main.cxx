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
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkTubeObject.h"
#include "igstkMeshObject.h"
#include "igstkTubeObjectRepresentation.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkMeshObjectRepresentation.h"
#include "igstkMouseTracker.h"
#include "vtkInteractorObserver.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

int main(int , char** )
{ 
  TwoViews* m_GUI = new TwoViews();

  m_GUI->MainWindow->show();

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

  // Add the ellipsoid to the view
  m_GUI->Display1->RequestAddObject( ellipsoidRepresentation );

  // Add the cylinder to the view
  m_GUI->Display1->RequestAddObject( cylinderRepresentation );

  // Create the tube 
  igstk::TubeObject::Pointer tube = igstk::TubeObject::New();
  igstk::TubeObject::PointType p1;
  p1.SetPosition(5,5,5);
  p1.SetRadius(1);
  tube->AddPoint(p1);
  igstk::TubeObject::PointType p2;
  p2.SetPosition(10,10,10);
  p2.SetRadius(2);
  tube->AddPoint(p2);

  // Create the cylinder representation
  igstk::TubeObjectRepresentation::Pointer tubeRepresentation = igstk::TubeObjectRepresentation::New();
  tubeRepresentation->RequestSetTubeObject( tube );
  tubeRepresentation->SetColor(0,0,1.0);
  m_GUI->Display1->RequestAddObject( tubeRepresentation );

  // Create the mesh 
  igstk::MeshObject::Pointer mesh = igstk::MeshObject::New();
  mesh->AddPoint(0,0,0,0);
  mesh->AddPoint(1,9,0,0);
  mesh->AddPoint(2,9,9,0);
  mesh->AddPoint(3,0,0,9);
  mesh->AddTetrahedronCell(0,0,1,2,3);

  // Create the cylinder representation
  igstk::MeshObjectRepresentation::Pointer meshRepresentation = igstk::MeshObjectRepresentation::New();
  meshRepresentation->RequestSetMeshObject( mesh );
  meshRepresentation->SetColor(1,0,0);
  m_GUI->Display1->RequestAddObject( meshRepresentation );


  // Add another Object representations to the second display
  m_GUI->Display2->RequestAddObject( ellipsoidRepresentation->Copy() );
  m_GUI->Display2->RequestAddObject( cylinderRepresentation->Copy() );

  m_GUI->Display2->RequestResetCamera();
  m_GUI->Display2->Update();

  // Enable interactions
  m_GUI->Display2->RequestEnableInteractions();

  m_GUI->Display1->RequestResetCamera();
  m_GUI->Display1->Update();

  // Enable interactions
  m_GUI->Display1->RequestEnableInteractions();

   // Create a tracker
  igstk::MouseTracker::Pointer tracker = igstk::MouseTracker::New();

  // Initialize the tracker
  tracker->Open();
  tracker->Initialize();
  tracker->SetScaleFactor( 100.0 );

  const unsigned int toolPort = 0;
  const unsigned int toolNumber = 0;
  tracker->AttachObjectToTrackerTool( toolPort, toolNumber, ellipsoid );

  m_GUI->SetTracker( tracker );

  
  m_GUI->Display1->RequestRemoveObject( cylinderRepresentation );
  m_GUI->Display1->Update();

  // Setup the logging system
  itk::Logger::Pointer logger = itk::Logger::New();
  itk::StdStreamLogOutput::Pointer logOutput = itk::StdStreamLogOutput::New();
  itk::StdStreamLogOutput::Pointer fileOutput = itk::StdStreamLogOutput::New();
  
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  std::ofstream ofs( "logTwoViews.txt" );
  fileOutput->SetStream( ofs );
  logger->AddLogOutput( logOutput );
 
  m_GUI->Display1->SetLogger( logger ); 
  m_GUI->Display2->SetLogger( logger ); 
  tracker->SetLogger( logger );

  m_GUI->Display1->RequestSetRefreshRate( 30 ); // 30 Hz
  m_GUI->Display2->RequestSetRefreshRate( 30 ); // 30 Hz

  m_GUI->Display1->RequestStart();
  m_GUI->Display2->RequestStart();

  Fl::run();

  tracker->StopTracking();
  tracker->Close();

  delete m_GUI;

  ofs.close();

  return EXIT_SUCCESS;
}
