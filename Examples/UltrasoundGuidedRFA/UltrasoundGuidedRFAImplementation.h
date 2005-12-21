/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    UltrasoundGuidedRFAImplementation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _UltrasoundGuidedRFAImplementation_h
#define _UltrasoundGuidedRFAImplementation_h

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif


#include "igstkSandboxConfigure.h"
#include "UltrasoundGuidedRFA.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkAuroraTracker.h"
#include "igstkFlockOfBirdsTracker.h"
#include "igstkUltrasoundProbeObjectRepresentation.h"
#include "igstkAxesObjectRepresentation.h"

#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace igstk
{


class UltrasoundGuidedRFAImplementation : public UltrasoundGuidedRFA
{
  public:

    typedef itk::Logger              LoggerType; 
    typedef itk::StdStreamLogOutput  LogOutputType;

    typedef igstk::FlockOfBirdsTracker     TrackerType;

#ifdef WIN32
    typedef igstk::SerialCommunicationForWindows  CommunicationType;
#else
    typedef igstk::SerialCommunicationForPosix    CommunicationType;
#endif

  public:

    UltrasoundGuidedRFAImplementation()
      {
      m_Tracker = TrackerType::New();

      m_Logger = LoggerType::New();
      m_LogOutput = LogOutputType::New();
      m_LogFile.open("logUltrasoundGuidedRFA.txt");
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
      //m_Tracker->SetLogger( m_Logger );

      m_Communication = CommunicationType::New();
      //m_Communication->SetLogger( m_Logger );
      m_Communication->SetPortNumber( igstk::SerialCommunication::PortNumber0 );
      m_Communication->SetParity( igstk::SerialCommunication::NoParity );
      m_Communication->SetBaudRate( igstk::SerialCommunication::BaudRate19200 );
      m_Tracker->SetCommunication(m_Communication);

      m_Communication->OpenCommunication();

      /** Tool calibration transform */
      igstk::Transform toolCalibrationTransform;
      igstk::Transform::VectorType translation;
      translation[0] = 0;   // Tip offset
      translation[1] = 0;
      translation[2] = 390;
  
      igstk::Transform::VersorType rotation;
      rotation.SetRotationAroundZ(-3.141597/2.0);
    
      igstk::Transform::VersorType rotation2;
      rotation2.SetRotationAroundY(-3.141597/2.0);
      
      //igstk::Transform::VersorType rotation3;
      //rotation3.SetRotationAroundY(3.141597/2.0);
 
      rotation = rotation2*rotation;
   
      translation = rotation.Transform(translation);

      toolCalibrationTransform.SetTranslationAndRotation(translation,rotation,0.0001,100000);
      m_Tracker->SetToolCalibrationTransform( 0, 0, toolCalibrationTransform);
/*
      igstk::Transform toolCalibrationTransform;
      igstk::Transform::VectorType translation;
      translation[0] = 0;   // Tip offset
      translation[1] = -390/2.0;
      translation[2] = 0;
  
      igstk::Transform::VersorType rotation;
      rotation.SetRotationAroundZ(3.141597/2.0);
    
      igstk::Transform::VersorType rotation2;
      rotation2.SetRotationAroundY(3.141597/2.0);

      igstk::Transform::VersorType rotation3;
      rotation3.SetRotationAroundX(3.141597/2.0);
      rotation = rotation2*rotation*rotation3;
   
      translation = rotation.Transform(translation);

      toolCalibrationTransform.SetTranslationAndRotation(translation,rotation,0.0001,100000);
      m_Tracker->SetToolCalibrationTransform( 0, 0, toolCalibrationTransform);
*/

      /** Tool calibration transform */
      igstk::Transform patientTransform;
      igstk::Transform::VectorType translationP;
      translationP[0] = 0;   // Tip offset
      translationP[1] = 0;
      translationP[2] = 0;

      igstk::Transform::VersorType rotationP;
      rotationP.SetRotationAroundX(3.141597/2.0);
    
      igstk::Transform::VersorType rotation2P;
      rotation2P.SetRotationAroundY(-3.141597/2.0);
      rotationP = rotation2P*rotationP;

      patientTransform.SetTranslationAndRotation(translationP,rotationP,0.0001,100000);
      m_Tracker->SetPatientTransform(patientTransform);

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

    ~UltrasoundGuidedRFAImplementation()
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
    
    void AddProbe( igstk::UltrasoundProbeObjectRepresentation * cylinderRepresentation )
      {
       this->Display3D->RequestAddObject(       cylinderRepresentation->Copy() );
       this->DisplayAxial->RequestAddObject(    cylinderRepresentation->Copy() );
       this->DisplayCoronal->RequestAddObject(  cylinderRepresentation->Copy() );
       this->DisplaySagittal->RequestAddObject( cylinderRepresentation->Copy() );
      }

    void AddAxes( igstk::AxesObjectRepresentation * cylinderRepresentation )
      {
       this->Display3D->RequestAddObject(       cylinderRepresentation->Copy() );
       this->DisplayAxial->RequestAddObject(    cylinderRepresentation->Copy() );
       this->DisplayCoronal->RequestAddObject(  cylinderRepresentation->Copy() );
       this->DisplaySagittal->RequestAddObject( cylinderRepresentation->Copy() );
      }

    void AttachObjectToTrack( igstk::SpatialObject * objectToTrack )
      {
      const unsigned int toolPort = 0;
      const unsigned int toolNumber = 0;
      m_Tracker->AttachObjectToTrackerTool( toolPort, toolNumber, objectToTrack );
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

