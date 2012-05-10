/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: MultiTrackerLogger.h,v $
  Language:  C++
  Date:      $Date: 2011-03-17 20:12:26 $
  Version:   $Revision: 1.00 $

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __MultiTrackerLogger_h
#define __MultiTrackerLogger_h

// BeginLatex
//
// This application is used to log the tracker measurements using either one
// or two connected tracker tools into a text file.
// We'll need an initialized tracker, and two tracker tool objects, so that
// we can measure the position and orientation of a tool w.r.t. the tracker,
// or an other reference tool. As a first step, we define an abstract
// 'TrackerInitializer' class, to define a device independent initialization
// scheme. Later we'll specialize this class to implement the device-dependent
// initializer classes.
//
// This is the definition of the class 'TrackerInitializer':
//
// EndLatex

// BeginCodeSnippet
#include <list>
#include <string>

#include "igstkTrackerTool.h"

using namespace std;

class TrackerInitializer {
public:
  TrackerInitializer()
    {
    }

  ~TrackerInitializer();

  virtual igstk::Tracker::Pointer CreateTracker() = 0;
  virtual igstk::TrackerTool::Pointer CreateTrackerTool( int id ) = 0;

  static std::list<std::string> GetAvailableTrackerTypes() {
    m_TrackerTypes.clear();
    // register the available tracker types

#ifdef USE_EASYTRACK
    m_TrackerTypes.push_back( "EasyTrack500" );
#endif
#ifdef USE_POLARISCLASSIC
    m_TrackerTypes.push_back( "PolarisClassic" );
#endif
#ifdef USE_AXIOS3D
    m_TrackerTypes.push_back( "Axios3D" );
#endif
    return m_TrackerTypes;
    }

  static TrackerInitializer *CreateTrackerInitializer( std::string type );
protected:
  static std::list<std::string> m_TrackerTypes;
  static const unsigned int m_Tool = 0;
  static const unsigned int m_DRF = 1;
};
// EndCodeSnippet

// BeginLatex
// In order to use the Atracsys EasyTrack 500 device, we create a specialization
// of the TrackerInitializer class, using the EasyTrack500 IGSTK implementation.
// In the Logger application, we've used several CMake macro definitons to turn
// the specific tracker implementations on and off.
// EndLatex

// BeginCodeSnippet
#ifdef USE_EASYTRACK
#include "igstkAtracsysEasyTrack.h"
#include "igstkAtracsysEasyTrackTool.h"

class EasyTrackInitializer : public TrackerInitializer
{
public:
  EasyTrackInitializer() : TrackerInitializer() {}
  ~EasyTrackInitializer() {}

  igstk::Tracker::Pointer CreateTracker()
    {
    return igstk::AtracsysEasyTrack::New().GetPointer();
    }

  igstk::TrackerTool::Pointer CreateTrackerTool( int id )
    {
    switch( id ){
      case m_Tool:
        {
        igstk::AtracsysEasyTrackTool::Pointer tool =
                                            igstk::AtracsysEasyTrackTool::New();
        // pointer tool attached to port 0
        tool->RequestSetPortNumber(0);
        return tool.GetPointer();
        }
      case m_DRF:
        {
        igstk::AtracsysEasyTrackTool::Pointer tool =
                                            igstk::AtracsysEasyTrackTool::New();
        tool->RequestSetPortNumber(1);
          return tool.GetPointer();
        }
      }

    return NULL;
  }
};

#endif
// EndCodeSnippet


// BeginLatex
// In order to use the old Polaris Tracker (1996), we create a specialization of
// the TrackerInitializer class, using implementation.
// EndLatex

// BeginCodeSnippet
#ifdef USE_POLARISCLASSIC
#include "igstkSerialCommunication.h"
#include "igstkPolarisClassicTracker.h"
#include "igstkPolarisTrackerTool.h"

class PolarisClassicInitializer : public TrackerInitializer
{
public:
  PolarisClassicInitializer() : TrackerInitializer() {}
  ~PolarisClassicInitializer() {}

  igstk::Tracker::Pointer CreateTracker()
  {
    // create the communication object
    igstk::SerialCommunication::Pointer  serialComm =
                                          igstk::SerialCommunication::New();

    serialComm->SetPortNumber( igstk::SerialCommunication::PortNumber3 );
    serialComm->SetParity( igstk::SerialCommunication::NoParity );
    serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate115200 );
    serialComm->SetDataBits( igstk::SerialCommunication::DataBits8 );
    serialComm->SetStopBits( igstk::SerialCommunication::StopBits1 );
    serialComm->SetHardwareHandshake(igstk::SerialCommunication::HandshakeOff);
    serialComm->SetTimeoutPeriod(100);

    std::cout << "PortNumber: " << serialComm->GetPortNumber() << std::endl;
    std::cout << "Parity: " << serialComm->GetParity() << std::endl;
    std::cout << "BaudRate: " << serialComm->GetBaudRate() << std::endl;
    std::cout << "DataBits: " << serialComm->GetDataBits() << std::endl;
    std::cout << "StopBits: " << serialComm->GetStopBits() << std::endl;
    std::cout << "HardwareHandshake: "
        << serialComm->GetHardwareHandshake() << std::endl;
    std::cout << "TimeoutPeriod: "
        << serialComm->GetTimeoutPeriod() << std::endl;

    serialComm->SetCaptureFileName(
                  "RecordedStreamBySerialCommunicationTest.txt" );
    serialComm->SetCapture( true );
    if( serialComm->GetCapture() != true )
    {
    std::cout << "Set/GetCapture() failed" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    }
    std::cout << "CaptureFileName: "
        << serialComm->GetCaptureFileName() << std::endl;

    serialComm->OpenCommunication();

    igstk::PolarisClassicTracker::Pointer tracker =
                                      igstk::PolarisClassicTracker::New();
    tracker->SetCommunication(serialComm);

    return tracker.GetPointer();
    }

  igstk::TrackerTool::Pointer CreateTrackerTool( int id )
    {
    switch( id ){
      case m_Tool:
        {
        igstk::PolarisTrackerTool::Pointer tool =
                                          igstk::PolarisTrackerTool::New();
        tool->RequestSelectWiredTrackerTool();
        // default port number is 0
        tool->RequestSetPortNumber(0);
        return tool.GetPointer();
        }
      case m_DRF:
        {
        igstk::PolarisTrackerTool::Pointer tool =
                                           igstk::PolarisTrackerTool::New();
        tool->RequestSelectWiredTrackerTool();
        tool->RequestSetPortNumber(1);
        return tool.GetPointer();
        }
      }
    return NULL;
    }
};

#endif
// EndCodeSnippet

// BeginLatex
// Specialization for the Axios3D Tracker:
// EndLatex

// BeginCodeSnippet
#ifdef USE_AXIOS3D
#include "igstkAxios3DTracker.h"
#include "igstkAxios3DTrackerTool.h"

class Axios3DInitializer : public TrackerInitializer
{
public:
  Axios3DInitializer() : TrackerInitializer() {}
  ~Axios3DInitializer() {}

  igstk::Tracker::Pointer CreateTracker()
    {
    igstk::Axios3DTracker::Pointer tracker = igstk::Axios3DTracker::New();
    // place locator configuration files into the same directory as the
    // MultiTrackerLogger executable
    tracker->RequestLoadLocatorsXML("DRF.xml");
    tracker->RequestLoadLocatorsXML("Probe.xml");
    tracker->setVirtualMode(false);

    return tracker.GetPointer();
    }

  igstk::TrackerTool::Pointer CreateTrackerTool( int id )
    {
    switch( id ){
      case m_Tool:
        {
        igstk::Axios3DTrackerTool::Pointer tool =
                                              igstk::Axios3DTrackerTool::New();
        tool->RequestSetMarkerName("Probe");
        return tool.GetPointer();
        }
      case m_DRF:
        {
        igstk::Axios3DTrackerTool::Pointer tool =
                                              igstk::Axios3DTrackerTool::New();
        tool->RequestSetMarkerName("DRF");
        return tool.GetPointer();
        }
      }
    return NULL;
    }
};

#endif
// EndCodeSnippet

// BeginLatex
// We've adapted the "Factory" design pattern to allow the creation of the
// device specific TrackerInitializer classes using a string as the tracker
// identifier.
// EndLatex

// BeginCodeSnippet
TrackerInitializer *TrackerInitializer::CreateTrackerInitializer(
                                                          std::string type )
{
#ifdef USE_EASYTRACK
  if( type == "EasyTrack500" ){
    return new EasyTrackInitializer();
    }
#endif
#ifdef USE_POLARISCLASSIC
  if( type == "PolarisClassic" ){
    return new PolarisClassicInitializer();
    }
#endif
#ifdef USE_AXIOS3D
  if( type == "Axios3D" ){
    return new Axios3DInitializer();
    }
#endif

  return NULL;
}
// EndCodeSnippet
#endif
