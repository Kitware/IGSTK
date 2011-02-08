/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAscension3DGTracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAscension3DGTracker_h
#define __igstkAscension3DGTracker_h

#include "igstkAscension3DGTrackerTool.h"
#include "igstkTracker.h"

namespace igstk
{
/** Structure holding the Ascension 3DG error type*/
struct Ascension3DGEventErrorType {
  /** String describing the error, as returned from the Ascension tracker.*/
  EventHelperType::StringType ErrorString;

  /** Error code returned from the tracker. See the ATC3DG.h file.*/
  int AscensionErrorCode;
};

/** Structure holding a notification event. These can be used to monitor 
 * the state of the tracker.
 */
struct Ascension3DGToolEventStruct {
  enum ToolEvents{
  TOOL_DISCONNECTED,  /**a tool that was connected has been disconnected*/
  TOOL_CONNECTED,  /**a tool that was disconnected has been reconnected*/
  TOOL_SATURATED,  /**a sensor is too close to the transmitter and has saturated*/
  TOOL_OUT_OF_SATURATION,  /**a sensor that was saturated is no longer saturated*/
  TOOL_OUT_OF_MOTION_BOX,  /**a sensor that was within the motion box is now out*/
  TOOL_IN_MOTION_BOX,  /**a sensor that was out of the motion box is now back in*/
  TRANSMITTER_DETACHED,  /**a transmitter that was attached has been disconnected*/
  TRANSMITTER_ATTACHED,  /**a transmitter that was diconnected was attached*/
  };

  Ascension3DGToolEventStruct(){};

  /** a text descriptor of the tool the notificatrion is for*/
  EventHelperType::StringType ToolIdentifier;

  /**the id number of the tool the notification is for*/
  int toolID;

    /**the type of notification*/
  ToolEvents EventType;
};

  igstkLoadedEventMacro( Ascension3DGErrorEvent, IGSTKErrorEvent, 
    Ascension3DGEventErrorType);
  igstkLoadedEventMacro( Ascension3DGToolEvent, IGSTKEvent,
    Ascension3DGToolEventStruct);

/** \class Ascension3DGTracker
 *
 * \brief Provides support for the Ascension 3D Guidance trackers.
 *
 * This class provides an interface to Ascension Technology 
 * Corporation's 3D Guidance family of magnetic trackers.
 *
 * Events are provided that can be used to monitor the status of
 * the tracker. These include notifications when sensors are attached
 * or detached, when a sensor is too close or too far away from the
 * magnetic transmitter, and if the magnetic transmitter is unplugged.
 * At startup the state of the system can be querried with the Request
 * methods. After the initial state is learned, changes can be monitored
 * by setting up an observer for Ascension3DGToolEvent events.
 *
 * \ingroup Tracker
 *
 */

class Ascension3DGTracker : public Tracker
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( Ascension3DGTracker, Tracker )

  /** Get the number of tools that have been detected. */
  igstkGetMacro( NumberOfTools, unsigned int )

  /** Request whether the particular sensor (0 based), is attached.
    * An Ascension3DGToolEvent event will be sent. The tracker must have
    * already been initialized with RequestOpen(). It is intended that this
    * method be called on application startup to learn the initial state of
    * the tracker system. After that, an observer can be set up to monitor
    * Ascension3DGToolEvents for changes to the system. */
  void RequestSensorAttached(int sensorID);

  /** Request whether a transmitter, is attached.
    * An Ascension3DGToolEvent event will be sent. The tracker must have
    * already been initialized with RequestOpen(). It is intended that this
    * method be called on application startup to learn the initial state of
    * the tracker system. After that, an observer can be set up to monitor
    * Ascension3DGToolEvents for changes to the system. */
  void RequestTransmitterAttached();

  /** Request whether the particular sensor (0 based), is too close to the
    * magnetic transmitter and is saturated. An Ascension3DGToolEvent event
    * will be sent. The tracker must have already been initialized with
    * RequestOpen(). It is intended that this method be called on application
    * startup to learn the initial state of the tracker system. After that,
    * an observer can be set up to monitor Ascension3DGToolEvents for changes
    * to the system. */
  void RequestSensorSaturated(int sensorID);

  /** Request whether the particular sensor (0 based), is outside of the
    * tracking motion box. An Ascension3DGToolEvent event will be sent. The
    * tracker must have already been initialized with RequestOpen(). It is
    * intended that this method be called on application startup to learn the
    * initial state of the tracker system. After that, an observer can be set
    * up to monitor Ascension3DGToolEvents for changes to the system. */
  void RequestSensorInMotionBox(int sensorID);

protected:
  /** protected constructor*/
  Ascension3DGTracker(void);
  virtual ~Ascension3DGTracker(void);

  /** typedef for internal boolean return type */
  typedef Tracker::ResultType   ResultType;

  /** Open communication with the tracking device. */
  virtual ResultType InternalOpen( void );

  /** Close communication with the tracking device. */
  virtual ResultType InternalClose( void );

  /** Put the tracking device into tracking mode. */
  virtual ResultType InternalStartTracking( void );

  /** Take the tracking device out of tracking mode. */
  virtual ResultType InternalStopTracking( void );

  /** Update the status and the transforms for all TrackerTools. */
  virtual ResultType InternalUpdateStatus( void );

  /** Update the status and the transforms. 
    * This function is called by a separate thread. */
  virtual ResultType InternalThreadedUpdateStatus( void );

  /** Verify tracker tool information */
  virtual ResultType VerifyTrackerToolInformation( 
    const TrackerToolType * trackerTool );

  /** Verify the specified tracker measurement frequency. */
  virtual ResultType ValidateSpecifiedFrequency( double frequencyInHz );

  /** Add tracker tool entry from internal containers */
  virtual ResultType AddTrackerToolToInternalDataContainers( const
    TrackerToolType * trackerTool );

  /** Remove tracker tool from internal containers */
  virtual ResultType RemoveTrackerToolFromInternalDataContainers( 
    const TrackerToolType * trackerTool );

  /** Reset the tracking device to put it back to its original state. */
  virtual ResultType InternalReset( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

private:

  Ascension3DGTracker(const Self&);   //purposely not implemented
  void operator=(const Self&);        //purposely not implemented

  /** typedefs for the tracker tool */
  typedef igstk::Ascension3DGTrackerTool       Ascension3DGTrackerToolType;
  typedef Ascension3DGTrackerToolType::Pointer Ascension3DGTrackerToolPointer;
  typedef Ascension3DGTrackerToolType::ConstPointer
                                          Ascension3DGTrackerToolConstPointer;  

  /** Enable all tool ports that have tools plugged into them.
   * {The reference tool port is enabled as a static tool.} */
  void EnableToolPorts( void );

  /** Disable all enabled tool ports. */
  void DisableToolPorts( void );

  /** Check the status code returned from the API call. 
   *  Invoke an event if necessary. Returns the status it was sent. */
  int CheckAPIReturnStatus(int status);

  /** Total number of tools detected. */
  unsigned int                                     m_NumberOfTools;

  enum {TRANSMITTER_OFF = -1};

  /** A mutex for multithreaded access to the transform buffer */
  itk::MutexLock::Pointer                          m_BufferLock;
  
  typedef std::map< std::string, std::vector < double > >
      TrackerToolTransformContainerType;
  /** A buffer to hold tool transforms */
  TrackerToolTransformContainerType                m_ToolTransformBuffer;
  
  typedef std::map< unsigned int, std::string>  ErrorCodeContainerType;
  /** Error map container */
  static ErrorCodeContainerType                    m_ErrorCodeContainer;

  enum ToolAvailabilityStatus {TOOL_UNAVAILABLE, TOOL_AVAILABLE};
  /** Container holding status of the tools. Key is the tool's name. */
  std::map< std::string, ToolAvailabilityStatus >  m_ToolStatusContainer;

  /** Retrieve the 0 based sensor id value given the tool's name. */
  static unsigned short ConvertToolNameToSensorID(const std::string &name);
  /** Retrieve the tool's name given the 0 based sensor id. */
  static std::string ConvertSensorIDToToolName(unsigned short id);

  //The tool status from the last update. It would be better if this were in the
  //Ascension3DGTrackerTool class, but that does not seem workable. The Tracker
  //baseclass keeps its container of tracker tools private. It only has a const
  //getter. From within Ascension3DGTracker, we cannot directly modify 
  //TrackerTools or call non-const TrackerTools member methods. Our only access
  //to the tools is through the public and protected methods the Tracker base
  //provides.
  std::vector<bool>   m_SensorSaturated;
  std::vector<bool>   m_SensorAttached;
  std::vector<bool>   m_SensorInMotionBox;
  bool                m_TransmitterAttached;

  /** Method to invoke the notification event*/
  void InvokeSensorToolEvent(std::string sensorName, int sensorID,
    Ascension3DGToolEventStruct::ToolEvents eventType);

  /** No processing function for the state machine */
  void NoProcessing(){};

  void ReportSensorSaturationProcessing();
  void ReportSensorAttachedProcessing();
  void ReportTransmitterAttachedProcessing();
  void ReportSensorInMotionBoxProcessing();

  //used to keep the sensor id as we go through the state machine
  int                 m_sensorID;

  //states
  igstkDeclareStateMacro(Idle);
  igstkDeclareStateMacro(Initialized);
  igstkDeclareStateMacro(AttemptingToGetSaturation);
  igstkDeclareStateMacro(AttemptingToGetSensorAttached);
  igstkDeclareStateMacro(AttemptingToGetTransmitterAttached);
  igstkDeclareStateMacro(AttemptingToGetSensorInMotionBox);

  //inputs
  igstkDeclareInputMacro(Initialize);
  igstkDeclareInputMacro(GetSensorSaturation);
  igstkDeclareInputMacro(GetSensorAttached);
  igstkDeclareInputMacro(GetTransmitterAttached);
  igstkDeclareInputMacro(GetSensorInMotionBox);
};

}

#endif //__igstk_Ascension3DGTracker_h_
