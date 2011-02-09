/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAscension3DGTracker.cxx
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

#include "igstkConfigure.h"
#include "igstkAscension3DGTracker.h"
//The ATC3DG.h file is not contained within a namepsace. We will do it here.
//This will only cover the functions, structures and enumerations. There are
//many #define statements that are unaffected by the namespace.
//atc = Ascension Technology Corporation
#ifdef Ascension3DG_MedSafe
namespace atc
{
#include "ATC3DGm.h"
}
#else /* Ascension3DG_MedSafe */
namespace atc
{
#include "ATC3DG.h"
}
#endif /* Ascension3DG_MedSafe */

namespace igstk
{

/** Constructor: Initializes all internal variables. */
 Ascension3DGTracker::Ascension3DGTracker(void)
    :m_StateMachine(this)
{
  this->SetThreadingEnabled( true );

  m_BufferLock = itk::MutexLock::New();
  m_NumberOfTools = 0;

  //states
  igstkAddStateMacro(Idle);
  igstkAddStateMacro(Initialized);
  igstkAddStateMacro(AttemptingToGetSaturation);
  igstkAddStateMacro(AttemptingToGetSensorAttached);
  igstkAddStateMacro(AttemptingToGetTransmitterAttached);
  igstkAddStateMacro(AttemptingToGetSensorInMotionBox);

  //inputs
  igstkAddInputMacro(Initialize);
  igstkAddInputMacro(GetSensorSaturation);
  igstkAddInputMacro(GetSensorAttached);
  igstkAddInputMacro(GetTransmitterAttached);
  igstkAddInputMacro(GetSensorInMotionBox);

  igstkAddTransitionMacro(Idle,
                          Initialize,
                          Initialized,
                          No);
  //If we are idle and any status items are requested, we will stay idle and
  //do nothing. Wen don't have access to this information until the tracker
  //is initialized.
  igstkAddTransitionMacro(Idle,
                           GetSensorSaturation,
                           Idle,
                           No);
  igstkAddTransitionMacro(Idle,
                          GetSensorAttached,
                          Idle,
                          No);
  igstkAddTransitionMacro(Idle,
                          GetTransmitterAttached,
                          Idle,
                          No);
  igstkAddTransitionMacro(Idle,
                          GetSensorInMotionBox,
                          Idle,
                          No);
  igstkAddTransitionMacro(Initialized,
                          Initialize,
                          Initialized,
                          No);  //don't do anything, we are already initialized
  //If we are intiialized we can respond to status requests.
  igstkAddTransitionMacro(Initialized,
                          GetSensorSaturation,
                          Initialized,
                          ReportSensorSaturation);
  igstkAddTransitionMacro(Initialized,
                          GetSensorAttached,
                          Initialized,
                          ReportSensorAttached);
  igstkAddTransitionMacro(Initialized,
                          GetTransmitterAttached,
                          Initialized,
                          ReportTransmitterAttached);
  igstkAddTransitionMacro(Initialized,
                          GetSensorInMotionBox,
                          Initialized,
                          ReportSensorInMotionBox);

  igstkSetInitialStateMacro(Idle);
  m_StateMachine.SetReadyToRun();
                          
}

/** Destructor */
Ascension3DGTracker::~Ascension3DGTracker(void)
{
}


/** Open communication with the tracking device. */
Ascension3DGTracker::ResultType Ascension3DGTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "Ascension3DGTracker::InternalOpen called ...\n");
  
  this->CheckAPIReturnStatus(atc::InitializeBIRDSystem());

  //get the configuration information from the system
  atc::SYSTEM_CONFIGURATION systemConfig;
  this->CheckAPIReturnStatus(atc::GetBIRDSystemConfiguration(&systemConfig));

  //Set the tracking system to return metric data (millimeters).
  int metric = 1;
  this->CheckAPIReturnStatus(atc::SetSystemParameter(atc::METRIC,
    &metric, sizeof(metric)));

  //Go through each sensor the system supports and set what data format
  //it will be returning. Setting the format for sensors that are not
  //attached is fine. If something is attached later at that port it will
  //be ready to go. We will also record whether a sensor is present.
  int sensorID;
  atc::DATA_FORMAT_TYPE formatType = 
    atc::DOUBLE_POSITION_ANGLES_MATRIX_QUATERNION_TIME_Q_BUTTON;
  //clear the members that keep track of the sensors' states
  this->m_SensorSaturated.clear();
  this->m_SensorAttached.clear();
  this->m_SensorInMotionBox.clear();
  for(sensorID = 0; sensorID < systemConfig.numberSensors; ++sensorID)
  {
    this->CheckAPIReturnStatus(atc::SetSensorParameter(sensorID,
      atc::DATA_FORMAT, &formatType, sizeof(formatType)));
    
    //Get the sensor's status
    atc::DEVICE_STATUS status;
    status = atc::GetSensorStatus(sensorID);
    //SATURATED, NOT_ATTACHED, etc. are #defined in ATC3DG.h. This means they not
    //a part of the atc namespace we created when we included ATC3DG.h.
    this->m_SensorSaturated.push_back((status & SATURATED) ? true : false);
    this->m_SensorAttached.push_back((status & NOT_ATTACHED) ? false : true);
    this->m_SensorInMotionBox.push_back((status & OUT_OF_MOTIONBOX) ? false : true);
    this->m_TransmitterAttached = ((status & NO_TRANSMITTER_ATTACHED) ? false : true);
  }

  //We're going to push an input into the state machine here. The syle guide
  //only wants ProcessInputs() to be called from Request methods. Within
  //this class we need to know if the system has been initialized. This
  //method is called in response to Tracker::RequestOpen(), so we are close
  //to a request.
  m_StateMachine.PushInput( m_InitializeInput);
  m_StateMachine.ProcessInputs();

  return SUCCESS;
}

/** Request whether the particular sensor (0 based), is attached.
    * An Ascension3DGToolEvent event will be sent. The tracker must have
    * already been initialized with RequestOpen(). It is intended that this
    * method be called on application startup to learn the initial state of
    * the tracker system. After that, an observer can be set up to monitor
    * Ascension3DGToolEvents for changes to the system. */
void Ascension3DGTracker::RequestSensorAttached(int sensorID)
{
  igstkLogMacro( DEBUG, "Ascension3DGTracker::RequestSensorAttached called...\n");
  m_sensorID = sensorID;
  m_StateMachine.PushInput( m_GetSensorAttachedInput );
  m_StateMachine.ProcessInputs();
}

/** Request whether a transmitter, is attached.
    * An Ascension3DGToolEvent event will be sent. The tracker must have
    * already been initialized with RequestOpen(). It is intended that this
    * method be called on application startup to learn the initial state of
    * the tracker system. After that, an observer can be set up to monitor
    * Ascension3DGToolEvents for changes to the system. */
void Ascension3DGTracker::RequestTransmitterAttached()
{
  igstkLogMacro( DEBUG, "Ascension3DGTracker::RequestTransmitterAttached called...\n");
  m_StateMachine.PushInput( m_GetTransmitterAttachedInput );
  m_StateMachine.ProcessInputs();
}

/** Request whether the particular sensor (0 based), is too close to the
    * magnetic transmitter and is saturated. An Ascension3DGToolEvent event
    * will be sent. The tracker must have already been initialized with
    * RequestOpen(). It is intended that this method be called on application
    * startup to learn the initial state of the tracker system. After that,
    * an observer can be set up to monitor Ascension3DGToolEvents for changes
    * to the system. */
void Ascension3DGTracker::RequestSensorSaturated(int sensorID)
{
  igstkLogMacro( DEBUG, "Ascension3DGTracker::RequestSensorSaturated called...\n");
  m_sensorID = sensorID;
  m_StateMachine.PushInput( m_GetSensorSaturationInput );
  m_StateMachine.ProcessInputs();
}

/** Request whether the particular sensor (0 based), is outside of the
    * tracking motion box. An Ascension3DGToolEvent event will be sent. The
    * tracker must have already been initialized with RequestOpen(). It is
    * intended that this method be called on application startup to learn the
    * initial state of the tracker system. After that, an observer can be set
    * up to monitor Ascension3DGToolEvents for changes to the system. */
void Ascension3DGTracker::RequestSensorInMotionBox(int sensorID)
{
  igstkLogMacro( DEBUG, "Ascension3DGTracker::RequestSensorInMotionBox called...\n");
  m_sensorID = sensorID;
  m_StateMachine.PushInput( m_GetSensorInMotionBoxInput );
  m_StateMachine.ProcessInputs();
}

/** Close communication with the tracking device. */
Ascension3DGTracker::ResultType Ascension3DGTracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "Ascension3DGTracker::InternalClose called ...\n");

  this->CheckAPIReturnStatus(atc::CloseBIRDSystem());

  return SUCCESS;
}

/** Put the tracking device into tracking mode. */
Ascension3DGTracker::ResultType 
Ascension3DGTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, 
                "Ascension3DGTracker::InternalStartTracking called ...\n");

  //We will turn on the first attached magnetic transmitter we see. To do this,
  //we first get the tracker board's configuration to see how many possible
  //transmitters there are. The should be a good enough method as there are
  //currently no 3D Guidance trackers that support more than 1 transmitter.
  atc::BOARD_CONFIGURATION boardConfig;
  this->CheckAPIReturnStatus(GetBoardConfiguration(0,&boardConfig));

  //go through all the possible magnetic transmitters and find the
  //first attached one
  short selectID = Ascension3DGTracker::TRANSMITTER_OFF;
  int i=0;
  bool found = false;
  while((i < boardConfig.numberTransmitters) && (found == false))
    {
    //get the transmitter configuration
    atc::TRANSMITTER_CONFIGURATION transConfig;
    this->CheckAPIReturnStatus(GetTransmitterConfiguration(i,&transConfig));
    if(transConfig.attached)
      {
      selectID = i;
      found = true;
      }
    ++i;
    }

  //if no magnetic transmitters were attached, selectID will still be TRANSMITTER_OFF
  this->CheckAPIReturnStatus(atc::SetSystemParameter(atc::SELECT_TRANSMITTER,
    &selectID, sizeof(selectID)));

  return SUCCESS;
}

/** Take the tracking device out of tracking mode. */
Ascension3DGTracker::ResultType 
Ascension3DGTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG,
                "Ascension3DGTracker::InternalStopTracking called ...\n");

  //turn the magnetic transmitter off
  short selectID = Ascension3DGTracker::TRANSMITTER_OFF;
  this->CheckAPIReturnStatus(atc::SetSystemParameter(atc::SELECT_TRANSMITTER,
    &selectID, sizeof(selectID)));

  return SUCCESS;
}

/** Reset the tracking device to put it back to its original state. */
Ascension3DGTracker::ResultType Ascension3DGTracker::InternalReset( void )
{
  return SUCCESS;
}


/** Update the status and the transforms for all TrackerTools. */
Ascension3DGTracker::ResultType Ascension3DGTracker::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG, 
                 "Ascension3DGTracker::InternalUpdateStatus called ...\n");

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
    // decide "tool visibility"
    if (this->m_ToolStatusContainer[inputItr->first] != TOOL_AVAILABLE)
      {
      igstkLogMacro( INFO, 
            "igstk::Ascension3DGTracker::InternalUpdateStatus: " <<
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

    const double normsquared =
        (inputItr->second)[3]*(inputItr->second)[3] +
        (inputItr->second)[4]*(inputItr->second)[4] +
        (inputItr->second)[5]*(inputItr->second)[5] +
        (inputItr->second)[6]*(inputItr->second)[6];

    // don't allow null quaternions
    if (normsquared < 1e-6)
      {
        rotation.Set(0.0, 0.0, 0.0, 1.0);
        igstkLogMacro( WARNING, 
            "igstk::Ascension3DGTracker::InternUpdateStatus: bad "
            "quaternion, norm = " << sqrt(normsquared) << "\n");
      }
    else
      {
        rotation.Set((inputItr->second)[3],
                     (inputItr->second)[4],
                     (inputItr->second)[5],
                     (inputItr->second)[6]);
      }

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
    }

  m_BufferLock->Unlock();
  return SUCCESS;
}

/** Update the status and the transforms. 
  * This function is called by a separate thread.*/
Ascension3DGTracker::ResultType 
Ascension3DGTracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "Ascension3DGTracker::InternalThreadedUpdateStatus "
          "called ...\n");

  m_BufferLock->Lock();

  typedef TrackerToolTransformContainerType::const_iterator  InputConstIterator;
  InputConstIterator inputItr = this->m_ToolTransformBuffer.begin();
  InputConstIterator inputEnd = this->m_ToolTransformBuffer.end();

  //The correct way to get data from all sensors is to call GetSyncronousRecord and
  //pass a sensorID of ALL_SENSORS (defined in ATC3DG.h). This requires passing a
  //buffer n times as large as the data format requires, where n is the number of
  //possible sensors in the system, not the number of attached sensors.
  atc::SYSTEM_CONFIGURATION sysConfig;
  this->CheckAPIReturnStatus(atc::GetBIRDSystemConfiguration(&sysConfig));

  typedef atc::DOUBLE_POSITION_ANGLES_MATRIX_QUATERNION_TIME_Q_BUTTON_RECORD RecordType;
  RecordType *record = new RecordType[sysConfig.numberSensors];
  this->CheckAPIReturnStatus(atc::GetSynchronousRecord(ALL_SENSORS, record,
    sysConfig.numberSensors * sizeof(RecordType)));

  int sensorIndex = 0;
  while( inputItr != inputEnd )
  {
    //set the tool's flag to unavailable
    this->m_ToolStatusContainer[inputItr->first] = Ascension3DGTracker::TOOL_UNAVAILABLE;
    //get the 0 based sensor id based on the tool's name
    unsigned short sensorID = this->ConvertToolNameToSensorID(inputItr->first);

    //For each sensor, we need to get its status. This will tell us if the sensor
    //is attached, if it is saturated from being too close to the magnetic transmitter,
    //and if there are any other errors in the tracking system.
    atc::DEVICE_STATUS status = atc::GetSensorStatus(sensorID);
    //SATURATED, NOT_ATTACHED, etc. are all #defined in ATC3DG.h. This means
    //they are not part of the atc namespace we created.
    bool saturated, attached, inMotionBox;
    bool transmitterRunning, transmitterAttached, globalError;
    saturated = status & SATURATED;
    attached = !(status & NOT_ATTACHED);
    inMotionBox = !(status & OUT_OF_MOTIONBOX);
    transmitterRunning = !(status & NO_TRANSMITTER_RUNNING);
    transmitterAttached = !(status & NO_TRANSMITTER_ATTACHED);
    globalError = status & GLOBAL_ERROR;

    std::vector< double > transform(7);  //3 positions, 4 quaternions
    transform[0] = record[sensorIndex].x;
    transform[1] = record[sensorIndex].y;
    transform[2] = record[sensorIndex].z;

    // Ascension quaternion: q0, q1, q2, and q3 where q0
    // is the scaler component
    // itk versor: void Set( T x, T y, T z, T w );
    transform[3] = record[sensorIndex].q[1];
    transform[4] = record[sensorIndex].q[2];
    transform[5] = record[sensorIndex].q[3];
    transform[6] = -record[sensorIndex].q[0];

    this->m_ToolTransformBuffer[ inputItr->first ] = transform;
    //The tool will only be considered available if a transmitter is attached
    //and running, the sensor is attached, it is within the motion box, and it
    //is not saturated.
    if((!transmitterAttached) || (!transmitterRunning) || (!attached) ||
      (!inMotionBox) || saturated)
      {
      this->m_ToolStatusContainer[ inputItr->first ] = TOOL_UNAVAILABLE;
      }
    else
      {
      this->m_ToolStatusContainer[ inputItr->first ] = TOOL_AVAILABLE;
      }

    /*Invoke events based on what the sensor status is now and what it was last time.
    * Need to compare to what it was last time so we do not send multiple events for
    * the same occurance.
    */
    if(saturated && (!this->m_SensorSaturated[sensorID]))
      {
      //sensor just became saturated
      this->InvokeSensorToolEvent(inputItr->first, sensorID,
        Ascension3DGToolEventStruct::TOOL_SATURATED);
      }
    else if((!saturated) && this->m_SensorSaturated[sensorID])
      {
      //sensor just came out of saturation
      this->InvokeSensorToolEvent(inputItr->first, sensorID,
        Ascension3DGToolEventStruct::TOOL_OUT_OF_SATURATION);
      }
    if((!attached) && this->m_SensorAttached[sensorID])
      {
      //sensor just became disconnected
      this->InvokeSensorToolEvent(inputItr->first, sensorID,
        Ascension3DGToolEventStruct::TOOL_DISCONNECTED);
      }
    else if(attached && (!this->m_SensorAttached[sensorID]))
      {
      //sensor just became attached
      this->InvokeSensorToolEvent(inputItr->first, sensorID,
        Ascension3DGToolEventStruct::TOOL_CONNECTED);
      }
    if((!inMotionBox) && (this->m_SensorInMotionBox[sensorID]))
      {
      //sensor just moved out of the motion box
      this->InvokeSensorToolEvent(inputItr->first, sensorID,
        Ascension3DGToolEventStruct::TOOL_OUT_OF_MOTION_BOX);
      } 
    else if(inMotionBox && (!this->m_SensorInMotionBox[sensorID]))
      {
      //sensor just moved back into the motion box
      this->InvokeSensorToolEvent(inputItr->first, sensorID,
        Ascension3DGToolEventStruct::TOOL_IN_MOTION_BOX);
      }
    if((!transmitterAttached) && this->m_TransmitterAttached)
      {
      //transmitter was just detached
      this->InvokeSensorToolEvent(inputItr->first, sensorID,
        Ascension3DGToolEventStruct::TRANSMITTER_DETACHED);
      }
    else if(transmitterAttached && (!this->m_TransmitterAttached))
      {
      //transmitter was just attached
      this->InvokeSensorToolEvent(inputItr->first, sensorID,
        Ascension3DGToolEventStruct::TRANSMITTER_ATTACHED);
      }
    //assign the current states to the member variables
    this->m_SensorSaturated[sensorID] = saturated;
    this->m_SensorAttached[sensorID] = attached;
    this->m_SensorInMotionBox[sensorID] = inMotionBox;
    this->m_TransmitterAttached = transmitterAttached;

    ++inputItr;
    ++sensorIndex;
  } 

  delete [] record;

  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Enable all tool ports that are occupied. */
void Ascension3DGTracker::EnableToolPorts()
{
  igstkLogMacro( DEBUG, "Ascension3DGTracker::EnableToolPorts called...\n");
}

/** Disable all tool ports. */
void Ascension3DGTracker::DisableToolPorts( void )
{
  igstkLogMacro( DEBUG, "Ascension3DGTracker::DisableToolPorts called...\n");
}

/** Verify tracker tool information */
Ascension3DGTracker::ResultType
Ascension3DGTracker
::VerifyTrackerToolInformation( const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG, 
    "Ascension3DGTracker::VerifyTrackerToolInformation called...\n");
  int sensorID = ConvertToolNameToSensorID(trackerTool->GetTrackerToolIdentifier());
  //Check that the sensor ID is valid. Should be >= 0 and < the number of sensors
  //the tracking system can support.
  atc::SYSTEM_CONFIGURATION systemConfig;
  this->CheckAPIReturnStatus(atc::GetBIRDSystemConfiguration(&systemConfig));
  
  Ascension3DGTracker::ResultType result = FAILURE;
  if(sensorID >= 0 && sensorID < systemConfig.numberSensors)
    result = SUCCESS;  
  return result;
}

/** Verify the specified tracker measurement frequency. */
Ascension3DGTracker::ResultType
Ascension3DGTracker
::ValidateSpecifiedFrequency( double frequencyInHz )
{
  igstkLogMacro(DEBUG,
    "Ascension3DGTracker::ValidateSpecifiedFrequency called...\n");
  //Different Ascension 3D Guidance systems have different minimum and maximum
  //measurement frequencies. The ATC3DG API does not provide a method for determining
  //the minimum or maximum.
  return SUCCESS;
}

/** Remove tracker tool from internal containers */
Ascension3DGTracker::ResultType
Ascension3DGTracker
::RemoveTrackerToolFromInternalDataContainers( 
  const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG, 
    "Ascension3DGTracker::RemoveTrackerToolFromInternalDataContainers"
    " called...\n");

  //GetTrackerToolIdentifier is actually returning a const std:string, not
  //a const std::string&. This can be assigned to a const std::string&.
  //If ToolTracker is changed, our temporary will go away.
  const std::string &trackerToolIdentifier = 
    trackerTool->GetTrackerToolIdentifier();

  // remove the tool from the Transform buffer container
  this->m_ToolTransformBuffer.erase( trackerToolIdentifier );
  this->m_ToolStatusContainer.erase( trackerToolIdentifier );

  return SUCCESS;
}

Ascension3DGTracker::ResultType
Ascension3DGTracker::
AddTrackerToolToInternalDataContainers( const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG,
      "igstk::Ascension3DGTracker::AddTrackerToolToInternalDataContainers "
      "called ...\n");

  if ( trackerTool == NULL )
    {
    return FAILURE;
    }

  //GetTrackerToolIdentifier is actually returning a const std:string, not
  //a const std::string&. This can be assigned to a const std::string&.
  //If ToolTracker is changed, our temporary will go away.
  const std::string &trackerToolIdentifier =
      trackerTool->GetTrackerToolIdentifier();

  std::vector< double > transform;
  transform.push_back( 1.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );

  this->m_ToolTransformBuffer[ trackerToolIdentifier ] = transform;
  this->m_ToolStatusContainer[ trackerToolIdentifier ] = TOOL_UNAVAILABLE;

  m_NumberOfTools ++;

  return SUCCESS;
}


/**Retrieve the 0 based sensor id value given the tool's name. */
unsigned short Ascension3DGTracker::ConvertToolNameToSensorID(const std::string &name)
{
  //The string will just be the number of the sensor, "0", "1", "2", etc.
  unsigned short id = atoi(name.c_str());
  return id;
}

/** Retrieve the tool's name given the 0 based sensor id. */
std::string Ascension3DGTracker::ConvertSensorIDToToolName(unsigned short id)
{
  std::stringstream name;
  name << id;
  return std::string(name.str());
}

/** Print Self function */
void Ascension3DGTracker::PrintSelf( std::ostream& os, 
                                     itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** Check the status code returned from the API call. Invoke an event if necessary.
 *  Returns the status it was sent. */
int Ascension3DGTracker::CheckAPIReturnStatus(int status)
{
  if(status != atc::BIRD_ERROR_SUCCESS)
  {
    char buffer[512];
    atc::GetErrorText(status,buffer,sizeof(buffer),atc::SIMPLE_MESSAGE);
    std::cout<<"Error: "<<buffer<<std::endl;
    //create an Acsension3DGEventErrorType object and fill it in
    Ascension3DGEventErrorType payload;
    payload.ErrorString = buffer;
    payload.AscensionErrorCode = status;
    Ascension3DGErrorEvent errorEvent;
    errorEvent.Set(payload);
    this->InvokeEvent(errorEvent);
  }
  return status;
}


void Ascension3DGTracker::InvokeSensorToolEvent(std::string sensorName, int sensorID,
    Ascension3DGToolEventStruct::ToolEvents eventType)
{
  Ascension3DGToolEvent toolEvent;
  Ascension3DGToolEventStruct payload;
  payload.EventType = eventType;
  payload.ToolIdentifier = sensorName;
  payload.toolID = sensorID;
  toolEvent.Set(payload);
  this->InvokeEvent(toolEvent);
}


void Ascension3DGTracker::ReportSensorSaturationProcessing()
{
  //If the sensorID is valid, call the function that invokes the tool event.
  if(m_sensorID >= 0 && m_sensorID < m_SensorSaturated.size())
  {
    InvokeSensorToolEvent(ConvertSensorIDToToolName(m_sensorID), m_sensorID, 
      (this->m_SensorSaturated[m_sensorID] ? 
      Ascension3DGToolEventStruct::TOOL_SATURATED : 
    Ascension3DGToolEventStruct::TOOL_OUT_OF_SATURATION));
  }
}

void Ascension3DGTracker::ReportSensorAttachedProcessing()
{
  //If the sensorID is valid, call the function that invokes the tool event.
  if(m_sensorID >= 0 && m_sensorID < m_SensorAttached.size())
  {
    InvokeSensorToolEvent(ConvertSensorIDToToolName(m_sensorID), m_sensorID, 
      (this->m_SensorAttached[m_sensorID] ?
      Ascension3DGToolEventStruct::TOOL_CONNECTED :
    Ascension3DGToolEventStruct::TOOL_DISCONNECTED));
  }
}
 
void Ascension3DGTracker::ReportTransmitterAttachedProcessing()
{
  //We will call the InvokeSensorToolEvent function even though we
  //are dealing with information for the transmitter.
  InvokeSensorToolEvent("transmitter", 0,
    (this->m_TransmitterAttached ?
    Ascension3DGToolEventStruct::TRANSMITTER_ATTACHED :
    Ascension3DGToolEventStruct::TRANSMITTER_DETACHED));
}

void Ascension3DGTracker::ReportSensorInMotionBoxProcessing()
{
  //If the sensorID is valid, call the function that invokes the tool event.
  if(m_sensorID >= 0 && m_sensorID < m_SensorInMotionBox.size())
    {
    InvokeSensorToolEvent(ConvertSensorIDToToolName(m_sensorID), m_sensorID, 
      (this->m_SensorInMotionBox[m_sensorID] ?
      Ascension3DGToolEventStruct::TOOL_IN_MOTION_BOX :
    Ascension3DGToolEventStruct::TOOL_OUT_OF_MOTION_BOX));
    }
}

} // end of namespace igstk
