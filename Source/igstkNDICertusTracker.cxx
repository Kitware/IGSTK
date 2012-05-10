/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkNDICertusTracker.cxx
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
// Warning about: identifier was truncated to '255' characters in the
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

//
// These header files can be found in the Utilities/NDICertusTracker subdirectory.
//
#include <math.h>

#include "igstkNDICertusTracker.h"
#include "igstkNDICertusTrackerTool.h"
#include <itksys/SystemTools.hxx>

#include <sstream>
#include <iostream>
#include <fstream>


namespace igstk
{

//Initialize static variables
std::map< unsigned int, std::string> NDICertusTracker::m_ErrorCodeContainer;
bool NDICertusTracker::m_ErrorCodeListCreated = false;

/** Constructor: Initializes all internal variables. */
NDICertusTracker::NDICertusTracker(void):m_StateMachine(this)
{
  // Set the thread
  this->SetThreadingEnabled( true );
  
  // Lock for the data buffer that is used to transfer the
  // transformations from thread that is communicating
  // with the tracker to the main thread.
  m_BufferLock = itk::MutexLock::New();

  // Reset the system variables
  ResetSystemVariables();

  // Load rigid body configuration data
  //LoadRigiBodyCfgFile();        // REMOVED since it was integrated with the trackerController/trackerConfiguration architecture


  // Set TrackerPulseGenerator frequency
  RequestSetFrequency( 30 ); 

}

/** Destructor */
NDICertusTracker::~NDICertusTracker(void)
{
  TransputerShutdownSystem();
}

/** Initialize system data etc. */
bool NDICertusTracker::ResetSystemVariables( void )
{
  igstkLogMacro( DEBUG, "igstk::NDICertusTracker::Initialize called ...\n");

    // Reset of the CERTUS variables
  Status.dtSettings.nMarkers        = 0;
  Status.dtSettings.fFrameFrequency    = 0;
  Status.dtSettings.fMarkerFrequency    = 0;
  Status.dtSettings.nThreshold      = 0;
  Status.dtSettings.nMinimumGain      = 0;
  Status.dtSettings.nStreamData      = 0;
  Status.dtSettings.fDutyCycle      = 0;
  Status.dtSettings.fVoltage        = 0;
  Status.dtSettings.fCollectionTime    = 0;
  Status.dtSettings.fPreTriggerTime    = 0;

  // Initialization of the CERTUS struct
  rigidBodyStatus.lnRigidBodies      = 0;
  for (int i = 0;i<MAX_RIGID_BODIES;i++)
  {
    rigidBodyDescrArray[i].fMax3dError      = 0;
    rigidBodyDescrArray[i].lnFlags        = OPTOTRAK_RETURN_QUATERN_FLAG;
    rigidBodyDescrArray[i].lnMinimumMarkers    = 3;
    rigidBodyDescrArray[i].lnNumberOfMarkers  = 3;
    rigidBodyDescrArray[i].lnRigidId      = i;
    rigidBodyDescrArray[i].lnStartMarker    = 0;
  }

  return SUCCESS;
}


/** Set ini file name*/
void NDICertusTracker::SetIniFileName(std::string strIniFileName)
{
  this->m_strIniFileName = strIniFileName;
}
 
 
/** Create a map data structure containing MTC error code and description */
void NDICertusTracker::CreateErrorCodeList()
{
  ErrorCodeContainerType & ecc = m_ErrorCodeContainer;
}

std::string NDICertusTracker::GetErrorDescription( )
{
  // optional extension: forward error message from device using the NDI API
  /*
  if( OptotrakGetErrorString( (char*)strErrorName.c_str(),
                          MAX_ERROR_STRING_LENGTH + 1 ) == 0 )
  {
    return strErrorName;
  }
  */

  return "NO ERROR";
}


NDICertusTracker::ResultType NDICertusTracker::InternalOpen( void )
{
   igstkLogMacro( DEBUG, "igstk::NDICertusTracker::InternalOpen called ...\n");

   

  /* Initialization involves the following steps
   * 1)  Get configuration            (TransputerDetermineSystemCfg)
   * 2) Load system  of processors          (TrasputerLoadSystem)
   * 3) Initialize system            (TransputerInitializeSystem)
   * 4) LoadCameraParameters          (OptotrackLoadCameraParameters)
   * 5) Update the setup from file        (OptotrackSetupCollectionFromFile)
   * 6) Get Setup                 (OptotrackGetStatus)  
   * 7) Load rigid bodies            (RigidBodyAddFromFile)
   */

   //-------------------------------
   // 1. Determine system configuration
   igstkLogMacro( DEBUG,
      "Determine system configuration...\n");

   if( TransputerDetermineSystemCfg( NULL ) )
    {
        igstkLogMacro( DEBUG,
      "Error in determining the system parameters.\n");
        return FAILURE;
    }
   //--- (end) Determine system configuration


  //----------------------------------------
  // 2. Load the system of processors.      
  if( TransputerLoadSystem("system") != OPTO_NO_ERROR_CODE  )
    {
    igstkLogMacro( CRITICAL, "Error loading the system of processors");
    return FAILURE;
    }
  //--- (end) Load the system of processors.
  
  // Wait one second
  igstk::PulseGenerator::Sleep(1000);

  //----------------------------------------
  /*
     * 3. Communication Initialization
     * Once the system processors have been loaded, the application
     * prepares for communication by initializing the system processors.
     */
  if( TransputerInitializeSystem( OPTO_LOG_ERRORS_FLAG | OPTO_LOG_MESSAGES_FLAG ) != OPTO_NO_ERROR_CODE )
    {
    igstkLogMacro( CRITICAL, "Error initializing CERTUS system of processors");
    return FAILURE;
    }
  //--- (end) Initialize system of processors
  
  //----------------------------------------
  // 4. LoadCameraParameters
  if( OptotrakLoadCameraParameters("standard") != OPTO_NO_ERROR_CODE )
    {
    igstkLogMacro( CRITICAL, "Error loading CERTUS camera parameters");
    return FAILURE;
    }
  //--- (end) LoadCameraParameters
   
  //----------------------------------------
  // 5. Update the setup from file  (if it exists)
    if( itksys::SystemTools::FileExists( m_strIniFileName.c_str() ) )
    {
    if( OptotrakSetupCollectionFromFile((char*)m_strIniFileName.c_str() ) )
    {
      igstkLogMacro( CRITICAL, "Error Setup Collection");
      return FAILURE;
    }
    }else
  {
    igstkLogMacro( CRITICAL, "Error: Ini file does not exist!");
    return FAILURE;
  }
  //--- (end) Update the setup from file  (if exist)
  
  // Wait one second
  igstk::PulseGenerator::Sleep(1000);

  //----------------------------------------
  // 6.  Get Status   
  if( OptotrakGetStatus(&Status.nSensors,
            &Status.nOdaus,
            &Status.nRigidBodies,
            &Status.dtSettings.nMarkers,
                  &Status.dtSettings.fFrameFrequency,
                &Status.dtSettings.fMarkerFrequency,
              &Status.dtSettings.nThreshold,
            &Status.dtSettings.nMinimumGain,
            &Status.dtSettings.nStreamData,
            &Status.dtSettings.fDutyCycle,
            &Status.dtSettings.fVoltage,
            &Status.dtSettings.fCollectionTime,
            &Status.dtSettings.fPreTriggerTime,
            &m_NFlags) )
  {
  igstkLogMacro( CRITICAL, "Error Get Status");
  return FAILURE;
  }
  Status.dtSettings.uFlags = m_NFlags;
  //--- (end) Get Setup 
  
  //----------------------------------------
  // 7. Load rigid bodies
  if (rigidBodyStatus.lnRigidBodies>0)
  {
   for (unsigned int i=0;i<rigidBodyStatus.lnRigidBodies;i++)
  {
    if( RigidBodyAddFromFile(rigidBodyDescrArray[i].lnRigidId,    /* ID associated with this rigid body.*/
                rigidBodyDescrArray[i].lnStartMarker,  /* First marker in the rigid body.*/
                rigidBodyDescrArray[i].szName,        /* RIG file containing rigid body coordinates.*/
                rigidBodyDescrArray[i].lnFlags ) )      /* Flags. */
    {
      igstkLogMacro( CRITICAL, "Error loading rigid body");
      return FAILURE;
    } /* if */
  } /* for */
  }/*  if  */
  else
  {
    igstkLogMacro( CRITICAL, "Error: Rigid body number is set to zero");
    return FAILURE;
  }/* else*/
  //--- (end) Load rigid bodies

  // Wait up one second
  igstk::PulseGenerator::Sleep(1000);

  //---------------------------------------- 
  //  Check Status   
  if( OptotrakGetStatus(         
            &Status.nSensors,
            &Status.nOdaus,
            &Status.nRigidBodies,
            &Status.dtSettings.nMarkers,
                  &Status.dtSettings.fFrameFrequency,
                &Status.dtSettings.fMarkerFrequency,
              &Status.dtSettings.nThreshold,
            &Status.dtSettings.nMinimumGain,
            &Status.dtSettings.nStreamData,
            &Status.dtSettings.fDutyCycle,
            &Status.dtSettings.fVoltage,
            &Status.dtSettings.fCollectionTime,
            &Status.dtSettings.fPreTriggerTime,
            &m_NFlags) )
  {
    igstkLogMacro( CRITICAL, "Error Get Status");
    return FAILURE;
  }
  
  if (Status.nRigidBodies != rigidBodyStatus.lnRigidBodies)
  {
    igstkLogMacro( CRITICAL, "Error Check Status after loading rigid bodies from file");
    return FAILURE;
  }
   //--- (end) Check Status

  // InternalClose();
   return SUCCESS;
}

/** Verify tracker tool information. */
NDICertusTracker::ResultType
NDICertusTracker
::VerifyTrackerToolInformation( const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG,
    "igstk::NDICertusTracker::VerifyTrackerToolInformation called ...\n");

  // check if trackertool object is generated
  typedef igstk::NDICertusTrackerTool              NDICertusTrackerToolType;

  TrackerToolType * trackerToolNonConst =
          const_cast<TrackerToolType *>( trackerTool );

  NDICertusTrackerToolType * certusTrackerTool  =
        dynamic_cast< NDICertusTrackerToolType *> ( trackerToolNonConst );

  if ( certusTrackerTool == NULL )
    {
    return FAILURE;
    }

  return SUCCESS;
}

/** Detach System. */
NDICertusTracker::ResultType NDICertusTracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "igstk::NDICertusTracker::InternalClose called ...\n");

   //------------------------------------------------
   //Shutdown the processors message passing system.
   
  if( TransputerShutdownSystem() )
    {
        return FAILURE;
    }
   //--- (end) Shutdown

  return SUCCESS;
}

/** Put the tracking device into tracking mode. */
NDICertusTracker::ResultType NDICertusTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG,
    "igstk::NDICertusTracker::InternalStartTracking called ...\n");
   if( OptotrakActivateMarkers() ) // activate markers
    {
      return FAILURE;
    } /* if */

  // give the device time to activate the markers
  igstk::PulseGenerator::Sleep(1000);

  return SUCCESS;
}

/** Take the tracking device out of tracking mode. */
NDICertusTracker::ResultType NDICertusTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG,
    "igstk::NDICertusTracker::InternalStopTracking called ...\n");

  if( OptotrakDeActivateMarkers() ) // Deactivate markers
  {
    return FAILURE;
  } /* if */

  // give the device time to deactivate the markers
  igstk::PulseGenerator::Sleep(1000);

  return SUCCESS;
}

/** Reset the tracking device to put it back to its original state. */
NDICertusTracker::ResultType NDICertusTracker::InternalReset( void )
{
  igstkLogMacro( DEBUG, "igstk::NDICertusTracker::InternalReset called ...\n");


  return SUCCESS;
}

/** Update the status and the transforms for all TrackerTools. */
NDICertusTracker::ResultType NDICertusTracker::InternalUpdateStatus()
{
  
  igstkLogMacro( DEBUG,
    "igstk::NDICertusTracker::InternalUpdateStatus called ...\n");

  // This method and the InternalThreadedUpdateStatus are both called
  // continuously in the Tracking state.  This method is called from
  // the main thread, while InternalThreadedUpdateStatus is called
  // from the thread that actually communicates with the device.
  // A shared memory buffer is used to transfer information between
  // the threads, and it must be locked when either thread is
  // accessing it.
  m_BufferLock->Lock();

  typedef TrackerToolTransformContainerType::const_iterator  InputConstIterator;

  InputConstIterator inputItr = this->m_ToolTransformBuffer.begin();
  InputConstIterator inputEnd = this->m_ToolTransformBuffer.end();

  TrackerToolsContainerType trackerToolContainer =
    this->GetTrackerToolContainer();


  while( inputItr != inputEnd )
    {

    // only report tools that are in view
    if (! this->m_ToolStatusContainer[inputItr->first])
      {
      igstkLogMacro( DEBUG, "igstk::CertusTracker::InternalUpdateStatus: " <<
                     "tool " << inputItr->first << " is not in view\n");

      // report to the tracker tool that the tracker is not available
      this->ReportTrackingToolNotAvailable(
        trackerToolContainer[inputItr->first]);

      ++inputItr;
      continue;
      }
    // report to the tracker tool that the tracker is Visible
    this->ReportTrackingToolVisible(trackerToolContainer[inputItr->first]);

    // create the transform
    TransformType transform;

    typedef TransformType::VectorType TranslationType;
    TranslationType translation;

    translation[0] = (inputItr->second)[0];
    translation[1] = (inputItr->second)[1];
    translation[2] = (inputItr->second)[2];

    typedef TransformType::VersorType RotationType;
    RotationType rotation;
  
    rotation.Set( (inputItr->second)[3],
                   (inputItr->second)[4],
                   (inputItr->second)[5],
                   (inputItr->second)[6]);

    // report error value
    // Get error value from the tracker. TODO
    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = 0.0;

    transform.SetToIdentity(this->GetValidityTime());
    transform.SetTranslationAndRotation(translation, rotation, errorValue,
                                        this->GetValidityTime());
    // set the raw transform
    this->SetTrackerToolRawTransform(
      trackerToolContainer[inputItr->first], transform );
  
    this->SetTrackerToolTransformUpdate(
      trackerToolContainer[inputItr->first], true );

    ++inputItr;
  
    }// while

  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Update the shared memory buffer and the transforms.  This function
 *  is called by the thread that communicates with the tracker while
 *  the tracker is in the Tracking state. */
NDICertusTracker::ResultType NDICertusTracker::InternalThreadedUpdateStatus( void )
{
   

  igstkLogMacro( DEBUG, "igstk::NDICertusTracker::InternalThreadedUpdateStatus "
                 "called ...\n");

  // Send the commands to the device that will get the transforms
  // for all of the tools.
  // Lock the buffer that this method shares with InternalUpdateStatus
  m_BufferLock->Lock();
  
  // Get Rigid Body Transform
  if (DataGetLatestTransforms( &m_UFrameNumber, &m_UElements, &m_UFlags, &RigidBodyData ) )
  {
    igstkLogMacro( CRITICAL, "Error getting rigid body transforms");
    
    m_BufferLock->Unlock();
    return FAILURE;
  }

  for(int uRigidCnt = 0; uRigidCnt < m_UElements; ++uRigidCnt )
  {
      std::vector < double > transform;

      // Check if a Tracker tool is added with this rigidBody type
    //
    typedef TrackerToolTransformContainerType::iterator InputIterator;

    InputIterator rigidBodyItr =
      m_ToolTransformBuffer.find( rigidBodyDescrArray[uRigidCnt].szName );

    //Check if tool is visible
    if( (RigidBodyData.pRigidData[uRigidCnt].flags != OPTOTRAK_UNDETERMINED_FLAG) && (RigidBodyData.pRigidData[uRigidCnt].flags != OPTOTRAK_RIGID_ERR_MKR_SPREAD) 
    &&( RigidBodyData.pRigidData[uRigidCnt].transformation.quaternion.translation.x>(-10000000) ) ) // This is terrible I know, but OPTOTRAK_UNDETERMINED_FLAG does not seem to work properly; basically when a tool is not visible Certus system returns a very negative constant...
    {
      double translation[3];
      //the first three are translation
      translation[0] = RigidBodyData.pRigidData[uRigidCnt].transformation.quaternion.translation.x;
      translation[1] = RigidBodyData.pRigidData[uRigidCnt].transformation.quaternion.translation.y;
      translation[2] = RigidBodyData.pRigidData[uRigidCnt].transformation.quaternion.translation.z;

      transform.push_back( translation[0] );
      transform.push_back( translation[1] );
      transform.push_back( translation[2] );

      //the next four are quaternion
      double quaternion[4];
      quaternion[0] = RigidBodyData.pRigidData[uRigidCnt].transformation.quaternion.rotation.qx;
      quaternion[1] = RigidBodyData.pRigidData[uRigidCnt].transformation.quaternion.rotation.qy;
      quaternion[2] = RigidBodyData.pRigidData[uRigidCnt].transformation.quaternion.rotation.qz;
      quaternion[3] = RigidBodyData.pRigidData[uRigidCnt].transformation.quaternion.rotation.q0;
      
      transform.push_back( quaternion[0] );
      transform.push_back( quaternion[1] );
      transform.push_back( quaternion[2] );
      transform.push_back( quaternion[3] );


      if( rigidBodyItr != this->m_ToolTransformBuffer.end() )
      {
        this->m_ToolTransformBuffer[ rigidBodyDescrArray[uRigidCnt].szName ] = transform;
        this->m_ToolStatusContainer[ rigidBodyDescrArray[uRigidCnt].szName ] = 1;
      }

    }
    else //Tool not visible
    {
        transform.push_back( 0.0 );
        transform.push_back( 0.0 );
        transform.push_back( 0.0 );
        transform.push_back( 0.0 );
        transform.push_back( 0.0 );
        transform.push_back( 0.0 );
        transform.push_back( 1.0 );

      if( rigidBodyItr != this->m_ToolTransformBuffer.end() )
      {
        this->m_ToolTransformBuffer[ rigidBodyDescrArray[uRigidCnt].szName ] = transform;
        this->m_ToolStatusContainer[ rigidBodyDescrArray[uRigidCnt].szName ] = 0;
      }
      
    }
  }

  // unlock the buffer
  m_BufferLock->Unlock();

  return SUCCESS;
}

NDICertusTracker::ResultType
NDICertusTracker::
AddTrackerToolToInternalDataContainers( const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG,
    "igstk::NDICertusTracker::AddTrackerToolFromInternalDataContainers "
                 "called ...\n");

  if ( trackerTool == NULL )
    {
    return FAILURE;
    }

  const std::string trackerToolIdentifier =
                    trackerTool->GetTrackerToolIdentifier();

  std::vector< double > transform;
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 1.0 );

  this->m_ToolTransformBuffer[ trackerToolIdentifier ] = transform;
  
  this->m_ToolStatusContainer[ trackerToolIdentifier ] = 0;

  return SUCCESS;
}


NDICertusTracker::ResultType
NDICertusTracker::
RemoveTrackerToolFromInternalDataContainers
( const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG,
    "igstk::NDICertusTracker::RemoveTrackerToolFromInternalDataContainers "
                 "called ...\n");

  const std::string trackerToolIdentifier =
                      trackerTool->GetTrackerToolIdentifier();

  // remove the tool from the Transform buffer container
  this->m_ToolTransformBuffer.erase( trackerToolIdentifier );
  this->m_ToolStatusContainer.erase( trackerToolIdentifier );

  return SUCCESS;
}

/** Print Self function */
void NDICertusTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);


  os << indent << "Number of tools: " << this->m_NumberOfTools << std::endl;
          
}

int NDICertusTracker::SetCoordinateFrame(int rigidBodyID, int flag)
{
  return(RigidBodyChangeFOR(rigidBodyID,flag));
}
        
} // end of namespace igstk
