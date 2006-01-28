/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    FourViewsAndTrackingImplementation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _FourViewsAndTrackingImplementation_h
#define _FourViewsAndTrackingImplementation_h

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
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
#include "igstkSandboxConfigure.h"
#include "FourViewsAndTrackingGUI.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkAuroraTracker.h"
// EndCodeSnippet

#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace igstk
{


class FourViewsAndTrackingImplementation : public FourViewsAndTrackingGUI
{
  public:

    typedef itk::Logger              LoggerType; 
    typedef itk::StdStreamLogOutput  LogOutputType;

    typedef igstk::AuroraTracker     TrackerType;

#ifdef WIN32
    typedef igstk::SerialCommunicationForWindows  CommunicationType;
#else
    typedef igstk::SerialCommunicationForPosix    CommunicationType;
#endif

  public:

    FourViewsAndTrackingImplementation()
      {
      m_Tracker = TrackerType::New();

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
      m_Communication->SetBaudRate( igstk::SerialCommunication::BaudRate9600 );
      m_Communication->SetDataBits( igstk::SerialCommunication::DataBits8 );
      m_Communication->SetStopBits( igstk::SerialCommunication::StopBits1 );
      m_Communication->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff );
      m_Tracker->SetCommunication(m_Communication);

      m_Communication->OpenCommunication();

      m_Tracker->Open();
      m_Tracker->Initialize();

      // Set up the four quadrant views
      this->Display3D->RequestResetCamera();
      this->Display3D->Update();
      this->Display3D->RequestEnableInteractions();
      this->Display3D->RequestSetRefreshRate( 60 ); // 60 Hz
      this->Display3D->RequestStart();

      this->DisplayAxial->RequestResetCamera();
      this->DisplayAxial->Update();
      this->DisplayAxial->RequestEnableInteractions();
      this->DisplayAxial->RequestSetRefreshRate( 60 ); // 60 Hz
      this->DisplayAxial->RequestStart();

      this->DisplayCoronal->RequestResetCamera();
      this->DisplayCoronal->Update();
      this->DisplayCoronal->RequestEnableInteractions();
      this->DisplayCoronal->RequestSetRefreshRate( 60 ); // 60 Hz
      this->DisplayCoronal->RequestStart();

      this->DisplaySagittal->RequestResetCamera();
      this->DisplaySagittal->Update();
      this->DisplaySagittal->RequestEnableInteractions();
      this->DisplaySagittal->RequestSetRefreshRate( 60 ); // 60 Hz
      this->DisplaySagittal->RequestStart();
      
      m_Tracking = false;
      }

    ~FourViewsAndTrackingImplementation()
      {
      m_Tracker->Reset();
      m_Tracker->StopTracking();
      m_Tracker->Close();
      }
    
    void EnableTracking()
      {
      m_Tracking = true;
      m_Tracker->StartTracking();
      }
    
    void DisableTracking()
      {
      m_Tracker->Reset();
      m_Tracker->StopTracking();
      m_Tracking = false;
      }
    
    void AddCylinder( igstk::CylinderObjectRepresentation * cylinderRepresentation )
      {
       this->Display3D->RequestAddObject(       cylinderRepresentation->Copy() );
       this->DisplayAxial->RequestAddObject(    cylinderRepresentation->Copy() );
       this->DisplayCoronal->RequestAddObject(  cylinderRepresentation->Copy() );
       this->DisplaySagittal->RequestAddObject( cylinderRepresentation->Copy() );
      }
    
    void AddEllipsoid( igstk::EllipsoidObjectRepresentation * ellipsoidRepresentation )
      {
      this->Display3D->RequestAddObject(       ellipsoidRepresentation->Copy() );
      this->DisplayAxial->RequestAddObject(    ellipsoidRepresentation->Copy() );
      this->DisplayCoronal->RequestAddObject(  ellipsoidRepresentation->Copy() );
      this->DisplaySagittal->RequestAddObject( ellipsoidRepresentation->Copy() );
      }

    void AttachObjectToTrack( igstk::SpatialObject * objectToTrack )
      {
      const unsigned int toolPort = 0;
      const unsigned int toolNumber = 0;
      m_Tracker->AttachObjectToTrackerTool( toolPort, toolNumber, objectToTrack );
      }

    void ResetCameras()
      {
      this->Display3D->RequestResetCamera();
      this->DisplayAxial->RequestResetCamera();
      this->DisplayCoronal->RequestResetCamera();
      this->DisplaySagittal->RequestResetCamera();

      this->Display3D->Update();
      this->DisplayAxial->Update();
      this->DisplayCoronal->Update();
      this->DisplaySagittal->Update();
      }

  private:

    LoggerType::Pointer     m_Logger;
    LogOutputType::Pointer  m_LogOutput;
    TrackerType::Pointer    m_Tracker;

    CommunicationType::Pointer m_Communication;

    bool                    m_Tracking;
    
    std::ofstream           m_LogFile;
};


} // end of namespace

#endif

