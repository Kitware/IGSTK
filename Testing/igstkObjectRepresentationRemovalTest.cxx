/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkObjectRepresentationRemovalTest.cxx
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
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif


#include "igstkEllipsoidObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkView3D.h"
#include "igstkDefaultWidget.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

int igstkObjectRepresentationRemovalTest( int , char* [] )
{
  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput               LogOutputType;
  typedef igstk::EllipsoidObjectRepresentation  ObjectRepresentationType;
  typedef igstk::EllipsoidObject                ObjectType;
  typedef igstk::View3D                         View3DType;
  typedef igstk::DefaultWidget                  WidgetType;

  // Make a logger object
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  std::ofstream logFile("igstkObjectRepresentationRemovalTestLog.txt");
  logOutput->SetStream( logFile );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  // Make the spatial object
  ObjectType::Pointer ellipsoidObject1 = ObjectType::New();
  ellipsoidObject1->SetLogger( logger );
  
  // Make a spatial object representation
  ObjectRepresentationType::Pointer 
                    ellipsoidRepresentation1 = ObjectRepresentationType::New();
  ellipsoidRepresentation1->SetLogger( logger );
  ellipsoidRepresentation1->RequestSetEllipsoidObject(ellipsoidObject1);
  ellipsoidRepresentation1->SetColor( 0.0, 0.0, 1.0 );
  ellipsoidRepresentation1->SetOpacity( 0.4 );
     
  // Make a view and set the logger
  View3DType::Pointer view3D = View3DType::New();
  view3D->SetLogger( logger );
 
  // Create an minimal GUI
  WidgetType * widget = new WidgetType( 512, 512 );
  widget->RequestSetView( view3D );

  // Add the object we want to display.
  view3D->RequestAddObject( ellipsoidRepresentation1  );

  // Setup the coordinate system graph
  //
  //          View
  //            |
  //        Ellipsoid
  //
  igstk::Transform identityTransform;
  identityTransform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
  ellipsoidObject1->RequestSetTransformAndParent( identityTransform, view3D );

  // Reset the camera so that it shows the objects in the scene
  view3D->RequestResetCamera();

  // Start the pulse generator in the view.
  view3D->RequestStart();

  // The following code tests the cleanup of the observers created in the
  // locally scoped object representation.

    { // <- Start a local scope

      //    Make a locally scoped representation that share the spatial object
      //    we created above.
      ObjectRepresentationType::Pointer 
                        ellipsoidRepresentation2 = 
                                            ObjectRepresentationType::New();
      ellipsoidRepresentation2->RequestSetEllipsoidObject(ellipsoidObject1);
      ellipsoidRepresentation2->SetColor( 1.0, 0.0, 0.0 );
      ellipsoidRepresentation2->SetOpacity( 0.4 );

      // Add the local representation to the view.
      view3D->RequestAddObject( ellipsoidRepresentation2 );

      // Render for a bit.
      for(int i = 0; i < 10; i++)
        {
        igstk::PulseGenerator::Sleep(50);
        igstk::PulseGenerator::CheckTimeouts();
        }

      // Remove the locally scoped object
      view3D->RequestRemoveObject( ellipsoidRepresentation2 );

    } // <- End a local scope - ellipsoidRepresentation2 should be deleted here
      //    and its observers should be cleaned up.

  // Render some more.
  for(int i = 0; i < 10; i++)
    {
    igstk::PulseGenerator::Sleep(50);
    igstk::PulseGenerator::CheckTimeouts();
    }

  delete widget;

  // The goal is to exit without a crash.
  return EXIT_SUCCESS;
}
