/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    FourViewsAndTrackingImplementation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __FourViewsAndTrackingImplementation_h
#define __FourViewsAndTrackingImplementation_h

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif


// BeginLatex
//
// This example illustrates how to create a typical four-quadrant view
// application using IGSTK.  This is a very common configuration for the user
// interface of image guided surgery applications. The four quadrant views
// include one three dimensional view, one axial, one coronal and a Sagittal
// one. The application illustrates how the objects that represent the geometry
// of the scene are instantiated only once in a centralized way, while their
// representation objects are instantiated one per every view. More precisely,
// if we display a Cylinder in the four views, the geometric specification of
// the Cylinder is created only once, but the representation classes that
// render that cylinder in the view are instantiated four times, one per each
// view.
// 
// EndLatex

// BeginCodeSnippet
#include "FourViewsAndTrackingGUI.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkAuroraTracker.h"
#include "igstkView2D.h"
#include "igstkView3D.h"
// EndCodeSnippet

#include "igstkSerialCommunication.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace igstk
{

class FourViewsAndTrackingImplementation : public FourViewsAndTrackingGUI
{

public:

  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput               LogOutputType;
  typedef igstk::AuroraTracker                  TrackerType;
  typedef igstk::AuroraTrackerTool              TrackerToolType;

  typedef igstk::SerialCommunication            CommunicationType;

public:

  FourViewsAndTrackingImplementation()
    {
    this->m_Display3D = ViewType3D::New();
    this->m_DisplayAxial = ViewType2D::New();
    this->m_DisplayCoronal = ViewType2D::New();
    this->m_DisplaySagittal = ViewType2D::New();

    this->m_DisplayAxial->RequestSetOrientation( ViewType2D::Axial );
    this->m_DisplaySagittal->RequestSetOrientation( ViewType2D::Sagittal );
    this->m_DisplayCoronal->RequestSetOrientation( ViewType2D::Coronal );

    this->Display3DWidget->RequestSetView( this->m_Display3D );
    this->DisplayAxialWidget->RequestSetView( this->m_DisplayAxial );
    this->DisplayCoronalWidget->RequestSetView( this->m_DisplayCoronal );
    this->DisplaySagittalWidget->RequestSetView( this->m_DisplaySagittal );

    m_Tracker = TrackerType::New();
    m_TrackerTool = TrackerToolType::New();
    m_Logger = LoggerType::New();
    m_LogOutput = LogOutputType::New();
    m_LogFile.open("logFourViewsAndTracking.txt");
    if( !m_LogFile.fail() )
      {
      m_LogOutput->SetStream( m_LogFile );
      }
    else
      {
      std::cerr << "Problem opening Log file, using cerr instead " << std::endl;
      m_LogOutput->SetStream( std::cerr );
      }
    m_Logger->AddLogOutput( m_LogOutput );

    // add stdout for debug purposes
    LogOutputType::Pointer coutLogOutput = LogOutputType::New();
    coutLogOutput->SetStream( std::cout );
    m_Logger->AddLogOutput( coutLogOutput );

    m_Logger->SetPriorityLevel( LoggerType::DEBUG );
    m_Tracker->SetLogger( m_Logger );

    m_Communication = CommunicationType::New();
    m_Communication->SetLogger( m_Logger );
    m_Communication->SetPortNumber( igstk::SerialCommunication::PortNumber0 );
    m_Communication->SetParity( igstk::SerialCommunication::NoParity );
    m_Communication->SetBaudRate( igstk::SerialCommunication::BaudRate115200 );
    m_Communication->SetDataBits( igstk::SerialCommunication::DataBits8 );
    m_Communication->SetStopBits( igstk::SerialCommunication::StopBits1 );
    m_Communication->SetHardwareHandshake( 
                                  igstk::SerialCommunication::HandshakeOff );
    m_Tracker->SetCommunication(m_Communication);

    m_Communication->OpenCommunication();

    const unsigned int toolPort = 0;
    m_TrackerTool->RequestSetPortNumber( toolPort );
    m_TrackerTool->RequestConfigure();
    m_TrackerTool->RequestAttachToTracker( m_Tracker );

    m_Tracker->RequestOpen();

    // Set up the four quadrant views
    this->m_Display3D->RequestResetCamera();
    this->Display3DWidget->RequestEnableInteractions();
    this->m_Display3D->SetRefreshRate( 60 ); // 60 Hz

    this->m_DisplayAxial->RequestResetCamera();
    this->DisplayAxialWidget->RequestEnableInteractions();
    this->m_DisplayAxial->SetRefreshRate( 60 ); // 60 Hz

    this->m_DisplayCoronal->RequestResetCamera();
    this->DisplayCoronalWidget->RequestEnableInteractions();
    this->m_DisplayCoronal->SetRefreshRate( 60 ); // 60 Hz

    this->m_DisplaySagittal->RequestResetCamera();
    this->DisplaySagittalWidget->RequestEnableInteractions();
    this->m_DisplaySagittal->SetRefreshRate( 60 ); // 60 Hz
      
    m_Tracking = false;
    }

  ~FourViewsAndTrackingImplementation()
    {
    m_Tracker->RequestReset();
    m_Tracker->RequestStopTracking();
    m_Tracker->RequestClose();
    }
    
  void EnableTracking()
    {
    m_Tracking = true;
    m_Tracker->RequestStartTracking();
    }
    
  void DisableTracking()
    {
    m_Tracker->RequestReset();
    m_Tracker->RequestStopTracking();
    m_Tracking = false;
    }
    
  void AddCylinder( igstk::CylinderObjectRepresentation * 
                                                      cylinderRepresentation )
    {
    this->m_Display3D->RequestAddObject(       cylinderRepresentation->Copy() );
    this->m_DisplayAxial->RequestAddObject(    cylinderRepresentation->Copy() );
    this->m_DisplayCoronal->RequestAddObject(  cylinderRepresentation->Copy() );
    this->m_DisplaySagittal->RequestAddObject( cylinderRepresentation->Copy() );
    }
    
  void AddEllipsoid( igstk::EllipsoidObjectRepresentation * 
                                                     ellipsoidRepresentation )
    {
    this->m_Display3D->RequestAddObject(      ellipsoidRepresentation->Copy() );
    this->m_DisplayAxial->RequestAddObject(   ellipsoidRepresentation->Copy() );
    this->m_DisplayCoronal->RequestAddObject( ellipsoidRepresentation->Copy() );
    this->m_DisplaySagittal->RequestAddObject(ellipsoidRepresentation->Copy() );
    }

  void AttachObjectToTrack( igstk::SpatialObject * objectToTrack )
    {
    igstk::Transform transform;
    transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
    objectToTrack->RequestSetTransformAndParent( transform, m_TrackerTool );
    }

  void ResetCameras()
    {
    this->m_Display3D->RequestResetCamera();
    this->m_DisplayAxial->RequestResetCamera();
    this->m_DisplayCoronal->RequestResetCamera();
    this->m_DisplaySagittal->RequestResetCamera();
    }

  void StartViews()
    {
    this->m_Display3D->RequestStart();
    this->m_DisplayAxial->RequestStart();
    this->m_DisplayCoronal->RequestStart();
    this->m_DisplaySagittal->RequestStart();
    }

  void StopViews()
    {
    this->m_Display3D->RequestStop();
    this->m_DisplayAxial->RequestStop();
    this->m_DisplayCoronal->RequestStop();
    this->m_DisplaySagittal->RequestStop();
    }


  void ConnectViewsToSpatialObjectParent( igstk::SpatialObject* so )
    {
    igstk::Transform trans;
    trans.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

    this->m_Display3D->RequestSetTransformAndParent( trans, so );
    this->m_DisplayAxial->RequestSetTransformAndParent( trans, so );
    this->m_DisplayCoronal->RequestSetTransformAndParent( trans, so );
    this->m_DisplaySagittal->RequestSetTransformAndParent( trans, so );
    }

private:
  typedef igstk::View2D ViewType2D;
  typedef igstk::View3D ViewType3D;

  LoggerType::Pointer             m_Logger;
  LogOutputType::Pointer          m_LogOutput;
  TrackerType::Pointer            m_Tracker;
  TrackerToolType::Pointer        m_TrackerTool;

  CommunicationType::Pointer m_Communication;

  bool                    m_Tracking;
    
  std::ofstream           m_LogFile;

  ViewType2D::Pointer m_DisplayAxial;
  ViewType2D::Pointer m_DisplayCoronal;
  ViewType2D::Pointer m_DisplaySagittal;
  ViewType3D::Pointer m_Display3D;
};

} // end namespace igstk

#endif
