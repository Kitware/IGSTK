/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    main.cxx
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
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
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
#include "igstkRealTimeClock.h"

#include "vtkInteractorObserver.h"
#include "igstkView3D.h"

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace 
{

/** This method creates a random versor. */
igstk::Transform::VersorType GetRandomVersor()
{
  igstk::Transform::VersorType rotation;
  double x = static_cast<double>(rand())/static_cast<double>(RAND_MAX);
  double y = static_cast<double>(rand())/static_cast<double>(RAND_MAX);
  double z = static_cast<double>(rand())/static_cast<double>(RAND_MAX);
  double w = static_cast<double>(rand())/static_cast<double>(RAND_MAX);
  rotation.Set(x, y, z, w);

  return rotation;
}

/** This method creates a random versor whose values are "near" a
 *  supplied versor.
 */
igstk::Transform::VersorType GetRandomVersorNear( 
                                     const igstk::Transform::VersorType& versorIn,
                                     double lambda)
{

  igstk::Transform::VersorType rotation;
  double x = lambda * static_cast<double>(rand())/static_cast<double>(RAND_MAX)
             + versorIn.GetX();
  double y = lambda * static_cast<double>(rand())/static_cast<double>(RAND_MAX)
             + versorIn.GetY();
  double z = lambda * static_cast<double>(rand())/static_cast<double>(RAND_MAX)
             + versorIn.GetZ();
  double w = lambda * static_cast<double>(rand())/static_cast<double>(RAND_MAX)
             + versorIn.GetW();
  rotation.Set(x, y, z, w);

  return rotation;

}

/** This method creates a random versor whose change from versorIn1 is about
 *  the same as the change from versorIn0 to versorIn1.
 */
igstk::Transform::VersorType GetRandomVersorNear( 
                                     const igstk::Transform::VersorType& versorIn0,
                                     const igstk::Transform::VersorType& versorIn1,
                                     double lambda)
{

  double x = lambda * static_cast<double>(rand())/static_cast<double>(RAND_MAX) +
             (versorIn1.GetX() - versorIn0.GetX()) +
             versorIn1.GetX();
  double y = lambda * static_cast<double>(rand())/static_cast<double>(RAND_MAX) +
             (versorIn1.GetY() - versorIn0.GetY()) +
             versorIn1.GetY();
  double z = lambda * static_cast<double>(rand())/static_cast<double>(RAND_MAX) +
             (versorIn1.GetZ() - versorIn0.GetZ()) +
             versorIn1.GetZ();
  double w = lambda * static_cast<double>(rand())/static_cast<double>(RAND_MAX) +
             (versorIn1.GetW() - versorIn0.GetW()) +
             versorIn1.GetW();

  igstk::Transform::VersorType rotation;
  rotation.Set(x, y, z, w);

  return rotation;
}

} // anonymous namespace

int main(int , char** )
{ 
  igstk::RealTimeClock::Initialize();

  TwoViews* m_GUI = new TwoViews();

  /**
   *  Coordinate systems:
   *       
   *                        Mesh
   *                          | 
   *              ------------------------
   *              |           |           |
   *          Ellipsoid     Tube       Display1
   *              |
   *        -----------
   *       |           |
   *    Display2    Cylinder 
   *
   *  Display1 looks at the scene from with respect to the coordinates 
   *  of the mesh since the transform from Display1 to Mesh is identity.
   *
   *  Display2 looks at the scene with respect to the ellipsoid.
   *
   *  While the ellipsoid rotates, Display1 shows the movement with respect
   *  to the mesh. Display2 shows the scene with respect to the ellipsoid,
   *  so as the ellipsoid rotates with respect to the mesh, the mesh appears
   *  to move in Display2. The rotation of the ellipsoid is visible through
   *  the movement of the cylinder which is attached to the ellipsoid.
   *
   */

  // Create the ellipsoid 
  igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
  ellipsoid->SetRadius(1,1,1);
  
  // Create the ellipsoid representation
  igstk::EllipsoidObjectRepresentation::Pointer ellipsoidRepresentation = 
                                 igstk::EllipsoidObjectRepresentation::New();
  ellipsoidRepresentation->RequestSetEllipsoidObject( ellipsoid );
  ellipsoidRepresentation->SetColor(0.0,1.0,0.0);
  ellipsoidRepresentation->SetOpacity(1.0);

  // Create the cylinder 
  igstk::CylinderObject::Pointer cylinder = igstk::CylinderObject::New();
  cylinder->SetRadius(0.1);
  cylinder->SetHeight(9);

  igstk::Transform cylinderTransform;
  cylinderTransform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
  // cylinder is relative to the ellipsoid.
  cylinder->RequestSetTransformAndParent( cylinderTransform, ellipsoid.GetPointer() );

  // Create the cylinder representation
  igstk::CylinderObjectRepresentation::Pointer cylinderRepresentation = 
                                  igstk::CylinderObjectRepresentation::New();
  cylinderRepresentation->RequestSetCylinderObject( cylinder );
  cylinderRepresentation->SetColor(1.0,1.0,0.0);
  cylinderRepresentation->SetOpacity(1.0);

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
  igstk::TubeObjectRepresentation::Pointer tubeRepresentation = 
                                     igstk::TubeObjectRepresentation::New();
  tubeRepresentation->RequestSetTubeObject( tube );
  tubeRepresentation->SetColor(0,0,1.0);

  // Create the mesh 
  igstk::MeshObject::Pointer mesh = igstk::MeshObject::New();
  mesh->AddPoint(0,0,0,0);
  mesh->AddPoint(1,9,0,0);
  mesh->AddPoint(2,9,9,0);
  mesh->AddPoint(3,0,0,9);
  mesh->AddTetrahedronCell(0,0,1,2,3);

  igstk::Transform ellipsoidToMeshTransform;
  ellipsoidToMeshTransform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  // Ellipsoid is relative to the mesh.
  ellipsoid->RequestSetTransformAndParent( ellipsoidToMeshTransform, mesh.GetPointer() );
  
  igstk::Transform tubeTransform;
  tubeTransform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  // Tube is relative to the mesh
  tube->RequestSetTransformAndParent( tubeTransform, mesh.GetPointer() );

  // Create the cylinder representation
  igstk::MeshObjectRepresentation::Pointer meshRepresentation = 
                                      igstk::MeshObjectRepresentation::New();
  meshRepresentation->RequestSetMeshObject( mesh );
  meshRepresentation->SetColor(1,0,0);

  // instantiate a 3D view 
  typedef igstk::View3D        View3DType;
  View3DType::Pointer view3D = View3DType::New();
  View3DType::Pointer view3D2 = View3DType::New();

  m_GUI->Display1->RequestSetView( view3D );
  m_GUI->Display2->RequestSetView( view3D2 );

  // show() can be called after the RequestSetView() invocations.
  m_GUI->MainWindow->show();

  igstk::Transform displayTransform;
  displayTransform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  view3D->RequestSetTransformAndParent( displayTransform, mesh.GetPointer() );
  view3D2->RequestSetTransformAndParent( displayTransform, ellipsoid.GetPointer() );

  // Add another Object representations to the second display
  view3D2->RequestAddObject( ellipsoidRepresentation->Copy() );
  view3D2->RequestAddObject( cylinderRepresentation->Copy() );
  view3D2->RequestAddObject( meshRepresentation->Copy() );

  view3D->RequestAddObject( ellipsoidRepresentation );
  view3D->RequestAddObject( cylinderRepresentation );
  view3D->RequestAddObject( tubeRepresentation );
  view3D->RequestAddObject( meshRepresentation );

  view3D2->RequestResetCamera();
  view3D->RequestResetCamera();

  // Create a tracker
  igstk::MouseTracker::Pointer tracker = igstk::MouseTracker::New();

  // Initialize the tracker
  tracker->RequestOpen();
  tracker->RequestInitialize();
  tracker->SetScaleFactor( 100.0 );

  const unsigned int toolPort = 0;
  const unsigned int toolNumber = 0;
  tracker->AttachObjectToTrackerTool( toolPort, toolNumber, ellipsoid );

  m_GUI->SetTracker( tracker );
  
  // Setup the logging system
  typedef igstk::Object::LoggerType             LoggerType;

  LoggerType::Pointer logger = LoggerType::New();
  itk::StdStreamLogOutput::Pointer logOutput = itk::StdStreamLogOutput::New();
  itk::StdStreamLogOutput::Pointer fileOutput = itk::StdStreamLogOutput::New();
  
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );

  std::ofstream ofs( "logTwoViews.txt" );
  fileOutput->SetStream( ofs );
  logger->AddLogOutput( logOutput );
 
  view3D->SetLogger( logger ); 
  view3D2->SetLogger( logger ); 
  tracker->SetLogger( logger );

  view3D->SetRefreshRate( 30 ); // 30 Hz
  view3D2->SetRefreshRate( 30 ); // 30 Hz

  view3D->RequestStart();
  view3D2->RequestStart();

  typedef igstk::Transform::VersorType  VersorType;
  VersorType versor0 = GetRandomVersorNear( 
                                    ellipsoidToMeshTransform.GetRotation(),
                                    0.05 );
  VersorType versor1 = ellipsoidToMeshTransform.GetRotation();

  while(1)
    {
    Fl::wait( 0.01 );
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check();   // trigger FLTK redraws

    igstk::Transform::VersorType versor2 = GetRandomVersorNear( versor0, 
                                                                versor1, 
                                                                0.01 );
    // Transform should not expire before the next render.
    ellipsoidToMeshTransform.SetRotation( versor2,
                                          1e-5,
                                          60 ); 

    ellipsoid->RequestSetTransformAndParent( ellipsoidToMeshTransform, 
                                             mesh.GetPointer()          );
    
    versor0 = versor1;
    versor1 = versor2;
    }

  view3D->RequestStop();
  view3D2->RequestStop();

  tracker->RequestStopTracking();
  tracker->RequestClose();

  delete m_GUI;

  ofs.close();

  return EXIT_SUCCESS;
}
