/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_Tracker_h_
#define __igstk_Tracker_h_

#include <vector>

#include "itkObject.h"

#include "igstkCommunication.h"
#include "igstkLogger.h"
#include "igstkStateMachine.h"
#include "igstkTrackerPort.h"

namespace igstk
{
/** \class Tracker
    \brief Generic implementation of the Tracker class.

    This class provides a generic implementation of a tracker
    class. It has two member variables, an instant of a state
    machine, and reference to an instance of "communication"
    class. 

    The state machine implements the basic state transitions
    of a tracker.

    The communications object manages communication, either
    through serial/parallel ports, or through data files (for
    offline execution of tracker.)
*/

class Tracker : public itk::Object
{
    typedef igstk::Communication           CommunicationType;
    typedef igstk::Logger                  LoggerType;
    typedef igstk::StateMachine< Tracker > StateMachineType;
    typedef StateMachineType::TMemberFunctionPointer ActionType;
    typedef StateMachineType::StateType              StateType;
    typedef StateMachineType::InputType              InputType;
    typedef StateMachineType::StateIdentifierType    StateIdentifierType;

    FriendClassMacro( StateMachineType );
    FriendClassMacro( TrackerPort );

public:

    /** Some required typedefs for itk::Object. */

    typedef Tracker                        Self;
    typedef itk::SmartPointer<Self>        Pointer;
    typedef itk::SmartPointer<const Self>  ConstPointer;


    /**  Run-time type information (and related methods). */
    itkTypeMacro(Tracker, Object);

    /** Method for creation of a reference counted object. */
    NewMacro(Self);  

    /** The "Initialize" method initializes the tracker. The input 
    is a file in XML format describing the set up configuration. The
    configuraion file specifies type of communication between the 
    tracker object and the actual hardware (a file name for offline
    operation), number of ports, number of tools on each port, port
    and tool description, etc.
    */
    virtual void Initialize( const char *fileName = NULL );

    /** The "Reset" tracker method should be used to bring the tracker
    to some defined default state. */
    virtual void Reset( void );

    /** The "StartTracking" method readies the tracker for tracking the
    tools connected to the tracker. */
    virtual void StartTracking( void );

    /** The "EndTracking" stops tracker from tracking the tools. */
    virtual void StopTracking( void );

    /** The "UpdateToolStatus" method is used for updating the tools 
    status when the tracker is in tracking state. */
    virtual void UpdateToolStatus( void );

    /** The SetLogger method is used to attach a logger object to the
    tracker object for logging purposes. */
    void SetLogger( LoggerType* logger );

    /** The GetLogger method return pointer to the logger object. */
    LoggerType* GetLogger(  void );

    /** Type used to represent the codes of the states */
    const StateIdentifierType & GetCurrentState() const;

protected:

    Tracker(void);

    virtual ~Tracker(void);

    /** The "SetupCommunication" method initializes the communication. */
    virtual void SetUpCommunication( void ); 

    /** The "SetupTools" method initializes all the tracking tools. */
    virtual void SetUpTools( void );    


protected:

    /** Vector of all tool ports on the tracker */
    std::vector< TrackerPort >        m_Ports;

private:

    /** The "Communication" instance */
    CommunicationType        *m_pCommunication;

    /** The "StateMachine" instance */
    StateMachineType         m_StateMachine;

    /** List of States */
    StateType                m_IdleState;
    StateType                m_CommunicatingState;
    StateType                m_ToolsActiveState;
    StateType                m_TrackingState;

    /** List of Inputs */
    InputType                m_SetUpCommunication;
    InputType                m_SetUpTools;
    InputType                m_StartTracking;
    InputType                m_UpdateToolStatus;
    InputType                m_StopTracking;
    
    /** The Logger instance */
    LoggerType               *m_pLogger;
};

}

#endif //__igstk_Tracker_h_
