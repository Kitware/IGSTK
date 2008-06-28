/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerToolReferenceTest.cxx
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
#pragma warning( disable : 4786 )
#endif

#include <iostream>

#include "igstkAxesObject.h"
#include "igstkAxesObjectRepresentation.h"
#include "igstkBoxObject.h"
#include "igstkBoxObjectRepresentation.h"
#include "igstkCylinderObject.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkView3D.h"
#include "igstkDefaultWidget.h"
#include "igstkRealTimeClock.h"
#include "igstkSimulatedTrackerTool.h"
#include "igstkCircularSimulatedTracker.h"
#include "igstkTransformObserver.h"

int igstkTrackerToolReferenceTest( int , char * [] )
{
  igstk::RealTimeClock::Initialize();

  //
  // Create a Axes object to provide a visual reference
  //
  typedef igstk::AxesObject    AxesObjectType;
  AxesObjectType::Pointer axesObject = AxesObjectType::New();
  axesObject->SetSize(3.0, 3.0, 3.0);

  typedef igstk::AxesObjectRepresentation  RepresentationType;
  RepresentationType::Pointer axesRepresentation = RepresentationType::New();

  axesRepresentation->RequestSetAxesObject( axesObject );

  typedef igstk::View3D  View3DType;

  View3DType::Pointer view3D = View3DType::New();
    
  typedef igstk::DefaultWidget      WidgetType;


  // Create an minimal GUI
  WidgetType * widget = new WidgetType( 512, 512 );
  widget->RequestSetView( view3D );


  typedef igstk::CircularSimulatedTracker     TrackerType;
  typedef igstk::SimulatedTrackerTool         TrackerToolType;

  TrackerType::Pointer      tracker     = TrackerType::New();

  const double speedInDegreesPerSecond = 36.0;
  const double radiusInMillimeters = 2.0;

  tracker->RequestOpen();
  tracker->SetRadius( radiusInMillimeters );
  tracker->SetAngularSpeed( speedInDegreesPerSecond );

  TrackerToolType::Pointer  trackerTool = TrackerToolType::New();

  trackerTool->RequestSetName("Circle1");
  trackerTool->RequestConfigure();
  trackerTool->RequestAttachToTracker( tracker );

  TrackerToolType::Pointer  referenceTrackerTool = TrackerToolType::New();

  referenceTrackerTool->RequestSetName("Reference");
  referenceTrackerTool->RequestConfigure();
  referenceTrackerTool->RequestAttachToTracker( tracker );

  typedef igstk::BoxObject                ToolObjectType;
  typedef igstk::BoxObjectRepresentation  ToolRepresentationType;

  ToolObjectType::Pointer toolObject = ToolObjectType::New();
  toolObject->SetSize( 1.0, 1.0, 1.0 );

  ToolRepresentationType::Pointer 
                toolRepresentation = ToolRepresentationType::New();
  toolRepresentation->RequestSetBoxObject( toolObject );
  toolRepresentation->SetColor( 1.0, 0.5, 0.5 );

  ToolObjectType::Pointer referenceToolObject = ToolObjectType::New();
  referenceToolObject->SetSize( 1.0, 1.0, 1.0 );

  ToolRepresentationType::Pointer 
                referenceToolRepresentation = ToolRepresentationType::New();
  referenceToolRepresentation->RequestSetBoxObject( referenceToolObject );
  referenceToolRepresentation->SetColor( 0.5, 1.0, 0.5 );


  typedef igstk::CylinderObject                TargetObjectType;
  typedef igstk::CylinderObjectRepresentation  TargetRepresentationType;

  TargetObjectType::Pointer targetObject = TargetObjectType::New();
  targetObject->SetRadius( 0.1 );
  targetObject->SetHeight( 2.0 );

  TargetRepresentationType::Pointer 
              targetRepresentation = TargetRepresentationType::New();
  targetRepresentation->RequestSetCylinderObject( targetObject );
  targetRepresentation->SetColor( 0.5, 0.5, 1.0 );

  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  igstk::Transform cylinderTransform;
  igstk::Transform::VectorType    translation;
  igstk::Transform::VersorType    rotation;
  translation[0] =    0;
  translation[1] =    0;
  translation[2] = -1.0;
  rotation.Set(0.0, 0.0, 0.0, 1.0);
  const double transformUncertainty = 1.0;
  cylinderTransform.SetTranslation(
                            translation,
                            transformUncertainty,
                            igstk::TimeStamp::GetLongestPossibleTime() );

  // Connect the objects in the scene to a coordinate reference system.
  tracker->RequestSetTransformAndParent( identity, axesObject );
  toolObject->RequestSetTransformAndParent( identity, trackerTool );
  referenceToolObject->RequestSetTransformAndParent( identity,
                                                  referenceTrackerTool );
  targetObject->RequestSetTransformAndParent( cylinderTransform, axesObject );
  view3D->RequestSetTransformAndParent( identity, axesObject );

  // Create calibration transforms
  igstk::Transform calibrationToolTransform;
  translation[0] =    0;
  translation[1] =    0;
  translation[2] = -2.0;
  rotation.Set(0.0, 0.0, 0.0, 1.0);
  calibrationToolTransform.SetTranslation(
                                 translation,
                                 transformUncertainty,
                                 igstk::TimeStamp::GetLongestPossibleTime() );
  trackerTool->SetCalibrationTransform( calibrationToolTransform );

  igstk::Transform calibrationReferenceToolTransform;
  translation[0] =    0;
  translation[1] =    0;
  translation[2] =  2.0;
  calibrationToolTransform.SetTranslation(
                                  translation,
                                  transformUncertainty,
                                  igstk::TimeStamp::GetLongestPossibleTime() );
  referenceTrackerTool->SetCalibrationTransform( 
                                        calibrationReferenceToolTransform );

  view3D->SetRefreshRate( 30 );
  view3D->SetRendererBackgroundColor( 0.8, 0.8, 0.9 );
  view3D->SetCameraPosition( 10.0, 5.0, 3.0 );
  view3D->SetCameraFocalPoint( 0.0, 0.0, 0.0 );
  view3D->SetCameraViewUp( 0, 0, 1.0 );

  view3D->RequestAddObject( axesRepresentation );
  view3D->RequestAddObject( toolRepresentation );
  view3D->RequestAddObject( referenceToolRepresentation );
  view3D->RequestAddObject( targetRepresentation );

  typedef igstk::TransformObserver ObserverType;
  ObserverType::Pointer transformObserver = ObserverType::New();

  transformObserver->ObserveTransformEventsFrom( trackerTool );

  typedef igstk::CoordinateSystem   ReferenceSystemType;

  view3D->RequestStart();
  tracker->RequestStartTracking();

  std::cout << "Transforms with respect to the tracker." << std::endl;

  for( unsigned int i = 0; i < 30; i++ )
    {
    igstk::PulseGenerator::Sleep(50);
    igstk::PulseGenerator::CheckTimeouts();

    transformObserver->Clear();
    trackerTool->RequestComputeTransformTo( tracker );
    if( transformObserver->GotTransform() )
      {
      igstk::TransformObserver::PayloadType payload = 
        transformObserver->GetTransformBetweenCoordinateSystems();
      const ReferenceSystemType * source = payload.GetSource();
      const ReferenceSystemType * destination = payload.GetDestination();
      std::cout << source << std::endl;
      std::cout << destination << std::endl;
      igstk::Transform transform = transformObserver->GetTransform();
      if ( transform.IsValidNow() )
        {
        std::cout << transform << std::endl;
        }
      }
    }

  view3D->RequestStop();
  tracker->RequestStopTracking();

  std::cout << "Transforms with respect to the reference tracker tool" 
            << std::endl;

  tracker->RequestStartTracking();
  view3D->RequestStart();

  for( unsigned int i = 0; i < 30; i++ )
    {
    igstk::PulseGenerator::Sleep(50);
    igstk::PulseGenerator::CheckTimeouts();

    transformObserver->Clear();
    trackerTool->RequestComputeTransformTo( referenceTrackerTool );
    if( transformObserver->GotTransform() )
      {
      igstk::TransformObserver::PayloadType payload = 
        transformObserver->GetTransformBetweenCoordinateSystems();
      const ReferenceSystemType * source = payload.GetSource();
      const ReferenceSystemType * destination = payload.GetDestination();
      std::cout << source << std::endl;
      std::cout << destination << std::endl;
      igstk::Transform transform = transformObserver->GetTransform();
      if ( transform.IsValidNow() )
        {
        std::cout << transform << std::endl;
        }
      }

    }


  view3D->RequestStop();
  tracker->RequestStopTracking();

  std::cout << "--- Connect the Reference Tool Hierarchy --- " << std::endl;

  //
  // Now connect the reference tool in the hierarchy
  //
  tracker->RequestSetReferenceTool( referenceTrackerTool );
  referenceTrackerTool->RequestSetTransformAndParent( identity, axesObject );

  std::cout << "Transforms with respect to the tracker." << std::endl;

  tracker->RequestStartTracking();
  view3D->RequestStart();

  for( unsigned int i = 0; i < 30; i++ )
    {
    igstk::PulseGenerator::Sleep(50);
    igstk::PulseGenerator::CheckTimeouts();

    transformObserver->Clear();
    trackerTool->RequestComputeTransformTo( tracker );
    if( transformObserver->GotTransform() )
      {
      igstk::TransformObserver::PayloadType payload = 
        transformObserver->GetTransformBetweenCoordinateSystems();
      const ReferenceSystemType * source = payload.GetSource();
      const ReferenceSystemType * destination = payload.GetDestination();
      std::cout << source << std::endl;
      std::cout << destination << std::endl;
      igstk::Transform transform = transformObserver->GetTransform();
      if ( transform.IsValidNow() )
        {
        std::cout << transform << std::endl;
        }
      }
    }

  view3D->RequestStop();
  tracker->RequestStopTracking();

  std::cout << "Transforms with respect to the reference tracker tool" 
           << std::endl;

  tracker->RequestStartTracking();
  view3D->RequestStart();

  for( unsigned int i = 0; i < 30; i++ )
    {
    igstk::PulseGenerator::Sleep(50);
    igstk::PulseGenerator::CheckTimeouts();

    transformObserver->Clear();
    trackerTool->RequestComputeTransformTo( referenceTrackerTool );
    if( transformObserver->GotTransform() )
      {
      igstk::TransformObserver::PayloadType payload = 
        transformObserver->GetTransformBetweenCoordinateSystems();
      const ReferenceSystemType * source = payload.GetSource();
      const ReferenceSystemType * destination = payload.GetDestination();
      std::cout << source << std::endl;
      std::cout << destination << std::endl;
      igstk::Transform transform = transformObserver->GetTransform();
      if ( transform.IsValidNow() )
        {
        std::cout << transform << std::endl;
        }
      }
    }
    
  view3D->RequestStop();
  tracker->RequestStopTracking();

  tracker->RequestClose();

  delete widget;

  return EXIT_SUCCESS;
}
