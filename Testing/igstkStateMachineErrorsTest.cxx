/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkStateMachineErrorsTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

/**
 *   The purpose of this test is to trigger error conditions
 *   of the state machine and verify that they behave as expected.
 *   
 */

#if defined(_MSC_VER)
//  Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
// Warning about: constructor of the state machine receiving a pointer to this
// from a constructor. This is not a problem in this case, since the state
// machine constructor is not using the pointer, just storing it internally.
#pragma warning( disable : 4355 )
#endif

#include "itkStdStreamLogOutput.h"

#include "igstkRealTimeClock.h"

#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkObject.h"
#include "igstkLogger.h"

#include <iostream>

namespace igstk
{

class Tester1
{
public:

  typedef StateMachine< Tester1 >                     StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer    ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;

  igstkFriendClassMacro( StateMachine< Tester1 > );

  igstkTypeMacro( Tester1, None );

  /** Declarations needed for the Logging */
  igstkLoggerMacro();

  Tester1( LoggerType * logger ):m_StateMachine(this)
    {
    this->SetLogger( logger );

    // Set the state descriptors
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.AddState( m_OneQuarterCredit, "OneQuarterCredit" );
    m_StateMachine.AddState( m_TwoQuarterCredit, "TwoQuarterCredit" );
    m_StateMachine.AddState( m_ThreeQuarterCredit, "ThreeQuarterCredit" );

    // Set the input descriptors
    m_StateMachine.AddInput( m_QuarterInserted, "QuarterInserted" );

    const ActionType NoAction = 0;

    // Programming the machine
    m_StateMachine.AddTransition( m_IdleState,m_QuarterInserted, 
                                            m_OneQuarterCredit,    NoAction );
    m_StateMachine.AddTransition( m_OneQuarterCredit,m_QuarterInserted, 
                                            m_TwoQuarterCredit,    NoAction );
    m_StateMachine.AddTransition( m_TwoQuarterCredit,m_QuarterInserted, 
                                            m_ThreeQuarterCredit,  NoAction );

    // Purposely postponing the rest of the StateMachine programming to the
    // triggerError1() method.
    }

  virtual ~Tester1() {}

  void triggerError1()
    {
    // NOTE that on purpose the m_ThisStateDoesntExist was NOT added 
    // as a state with AddState()
    m_StateMachine.SelectInitialState( m_ThisStateDoesntExist );

    // Then do the right thing: call SelectInitialState() with a state
    // that has been registered
    m_StateMachine.SelectInitialState( m_IdleState );

    // Finish the programming and get ready to run
    m_StateMachine.SetReadyToRun();
    }

  void triggerError2()
    {
    // Purposely calling SelectInitialState() after SetReadyToRun() has been
    // called. This should trigger an error message.
    m_StateMachine.SelectInitialState( m_OneQuarterCredit );
    }

private:

  StateMachineType   m_StateMachine;

  /** List of States */
  StateType m_IdleState;
  StateType m_OneQuarterCredit;
  StateType m_TwoQuarterCredit;
  StateType m_ThreeQuarterCredit;
  StateType m_ThisStateDoesntExist;
  
  /** List of Inputs */
  InputType m_QuarterInserted;
};

class Tester2
{
public:

  typedef StateMachine< Tester2 >                     StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer    ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;


  igstkFriendClassMacro( StateMachine< Tester2 > );

  igstkTypeMacro( Tester2, None );

  /** Declarations needed for the Logging */
  igstkLoggerMacro();

  Tester2( LoggerType * logger ):m_StateMachine(this)
    {
    this->SetLogger( logger );
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.SelectInitialState( m_IdleState );
    m_StateMachine.SetReadyToRun();
    }

  virtual ~Tester2() {}

  void InsertChange() 
    {
    std::cout << "Insert Change" << std::endl;
    // On purpose we didn't set m_QuarterInserted with AddInput();
    m_StateMachine.PushInput( m_QuarterInserted );
    m_StateMachine.ProcessInputs();
    }
 

private:
  StateMachineType   m_StateMachine;

  /** List of States */
  StateType m_IdleState;

  /** List of Inputs */
  InputType m_QuarterInserted;

};

class Tester3
{
public:

  typedef StateMachine< Tester3 >                     StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer    ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;

  
  igstkFriendClassMacro( StateMachine< Tester3 > );

  igstkTypeMacro( Tester3, None );

  /** Declarations needed for the Logging */
  igstkLoggerMacro();

  Tester3( LoggerType * logger ):m_StateMachine(this)
    {
    this->SetLogger( logger );
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.SelectInitialState( m_IdleState );
    m_StateMachine.AddInput( m_QuarterInserted, "QuarterInserted" );
    const ActionType NoAction = 0;
    m_StateMachine.AddTransition( m_IdleState, m_QuarterInserted, 
                                            m_IdleState, NoAction );
    // On purpose NOT calling : m_StateMachine.SetReadyToRun();
    }

  virtual ~Tester3() {}

  void InsertChange() 
    {
    std::cout << "Insert Change" << std::endl;
    m_StateMachine.PushInput( m_QuarterInserted );
    m_StateMachine.ProcessInputs();
    }

private:
  StateMachineType   m_StateMachine;

  /** List of States */
  StateType m_IdleState;

  /** List of Inputs */
  InputType m_QuarterInserted;

};


class Tester4
{
public:

  typedef StateMachine< Tester4 >                     StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer    ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;

  
  igstkFriendClassMacro( StateMachine< Tester4 > );

  igstkTypeMacro( Tester4, None );

  /** Declarations needed for the Logging */
  igstkLoggerMacro();

  Tester4( LoggerType * logger ):m_StateMachine(this)
    {
    this->SetLogger( logger );
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.SelectInitialState( m_IdleState );
    m_StateMachine.AddInput( m_QuarterInserted, "QuarterInserted" );
    const ActionType NoAction = 0;
    m_StateMachine.AddTransition( m_IdleState, m_QuarterInserted, 
                                                     m_IdleState, NoAction );

    // On purpose invode SetReadyToRun() twice.
    m_StateMachine.SetReadyToRun();
    m_StateMachine.SetReadyToRun();
    }

  virtual ~Tester4() {}

  void InsertChange() 
    {
    std::cout << "Insert Change" << std::endl;
    m_StateMachine.PushInput( m_QuarterInserted );
    m_StateMachine.ProcessInputs();
    }

private:
  StateMachineType   m_StateMachine;

  /** List of States */
  StateType m_IdleState;

  /** List of Inputs */
  InputType m_QuarterInserted;

};

class Tester5
{
public:

  typedef StateMachine< Tester5 >                     StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer    ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;

  
  igstkFriendClassMacro( StateMachine< Tester5 > );

  igstkTypeMacro( Tester5, None );
  
  /** Declarations needed for the Logging */
  igstkLoggerMacro();

  Tester5( LoggerType * logger ):m_StateMachine(this)
    {
    this->SetLogger( logger );
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.SelectInitialState( m_IdleState );
    m_StateMachine.AddInput( m_QuarterInserted, "QuarterInserted" );

    // On purpose NOT adding any transition: for testing error condition.
    // const ActionType NoAction = 0;
    // m_StateMachine.AddTransition( m_IdleState, m_QuarterInserted, 
    //                                               m_IdleState, NoAction );
    m_StateMachine.SetReadyToRun();
    }

  virtual ~Tester5() {}

  void InvokeUndefinedTransition() 
    {
    std::cout << "Insert Change" << std::endl;
    m_StateMachine.PushInput( m_QuarterInserted );
    m_StateMachine.ProcessInputs();
    }
 
private:
  StateMachineType   m_StateMachine;

  /** List of States */
  StateType m_IdleState;

  /** List of Inputs */
  InputType m_QuarterInserted;

};

class Tester6
{
public:

  typedef StateMachine< Tester6 >                     StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer    ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;

  
  igstkFriendClassMacro( StateMachine< Tester6 > );

  igstkTypeMacro( Tester6, None );

  /** Declarations needed for the Logging */
  igstkLoggerMacro();

  Tester6( LoggerType * logger ):m_StateMachine(this)
    {
    this->SetLogger( logger );
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.AddState( m_ChangeMindState, "ChangeMindState" );
    m_StateMachine.SelectInitialState( m_IdleState );

    m_StateMachine.AddInput( m_QuarterInserted, "QuarterInserted" );
    m_StateMachine.AddInput( m_DimeInserted, "DimeInserted" );
    m_StateMachine.AddInput( m_Cancel, "Cancel");
    const ActionType NoAction = 0;
   
    std::cout << "TEST: On purpose NOT adding any transition \
                 for input m_Cancel" << std::endl;
    m_StateMachine.AddTransition( m_IdleState, m_QuarterInserted, 
                                                       m_IdleState, NoAction );

    std::cout << "TEST: On purpose Adding a Transition \
                 for a non existing state " << std::endl;
    m_StateMachine.AddTransition( m_NonRegisteredState, m_QuarterInserted, 
                                                      m_IdleState, NoAction );
    
    std::cout << "TEST: On purpose Adding a Transition \
                 for a non existing input " << std::endl;
    m_StateMachine.AddTransition( m_IdleState, m_NonRegisteredInput, 
                                                      m_IdleState, NoAction );

    std::cout << "TEST: On purpose Adding a Transition \
                 for a non existing new state " << std::endl;
    m_StateMachine.AddTransition( m_IdleState, m_DimeInserted, 
                                             m_NonRegisteredState, NoAction );

    std::cout << "TEST: On purpose Adding a Transition \
                 for a {State,Input} pair " << std::endl;
    std::cout << "      for which a transition ALREADY exists " << std::endl;
    m_StateMachine.AddTransition( m_IdleState, m_QuarterInserted, 
                                                m_ChangeMindState, NoAction );

    m_StateMachine.SetReadyToRun();
    }

  virtual ~Tester6() {}

  void InvokeUndefinedStateInputTransition() 
    {
    std::cout << "Invoking Cancel" << std::endl;
    m_StateMachine.PushInput( m_Cancel );
    m_StateMachine.ProcessInputs();
    }
 
private:
  StateMachineType   m_StateMachine;

  /** List of States */
  StateType m_IdleState;
  StateType m_NonRegisteredState;
  StateType m_ChangeMindState;

  /** List of Inputs */
  InputType m_QuarterInserted;
  InputType m_DimeInserted;
  InputType m_Cancel;
  InputType m_NonRegisteredInput;
};

class Tester7
{
public:

  typedef StateMachine< Tester7 >                     StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer    ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;

  
  igstkFriendClassMacro( StateMachine< Tester7 > );

  igstkTypeMacro( Tester7, None );

  /** Declarations needed for the Logging */
  igstkLoggerMacro();

  Tester7( LoggerType * logger ):m_StateMachine(this)
    {
    this->SetLogger( logger );
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.AddInput( m_QuarterInserted, "QuarterInserted" );
    const ActionType NoAction = 0;
    m_StateMachine.AddTransition( m_IdleState, m_QuarterInserted, 
                                                      m_IdleState, NoAction );
    // On purpose NOT invoking SelectInitialState() before calling 
    // SetReadyToRun()
    // COMMENTED OUT ON PURPOSE:  
    // m_StateMachine.SelectInitialState( m_IdleState );
    m_StateMachine.SetReadyToRun();
    }

  virtual ~Tester7() {}

  void InsertChange() 
    {
    std::cout << "Insert Change" << std::endl;
    m_StateMachine.PushInput( m_QuarterInserted );
    m_StateMachine.ProcessInputs();
    }

private:
  StateMachineType   m_StateMachine;

  /** List of States */
  StateType m_IdleState;

  /** List of Inputs */
  InputType m_QuarterInserted;

};

} // namespace igstk


int igstkStateMachineErrorsTest( int, char * [] )
{
  igstk::RealTimeClock::Initialize();

  itk::StdStreamLogOutput::Pointer coutput = itk::StdStreamLogOutput::New();
  coutput->SetStream( std::cout );
    
  typedef igstk::Object::LoggerType             LoggerType;
  LoggerType::Pointer logger = LoggerType::New();
  
  // Setting the logger
  logger->SetName("org.igstk.rootLogger");
  logger->SetPriorityLevel( itk::Logger::CRITICAL);
  logger->SetLevelForFlushing( itk::Logger::CRITICAL);

  logger->AddLogOutput( coutput );

  std::cout << "Construct the State Machine Tester" << std::endl;
  igstk::Tester1  tester1( logger );
    
  std::cout << std::endl << std::endl;
  std::cout << "Trigger error conditions on purpose" << std::endl;

  // The following calls are designed for testing the error conditions of the
  // state machine. 

  std::cout << "Invoking as Initial state a state that doesn't exist." 
            << std::endl;
  tester1.triggerError1();

  std::cout << "Invoking as SetInitialState() after SetReadyToRun() has been called." 
            << std::endl;
  tester1.triggerError2();

  std::cout << "Invoking SetReadyToRun() (in constructor) without \
               parent class connected." << std::endl;
  igstk::Tester2 tester2( logger );

  std::cout << "Invoking ProcessInputs() without parent class connected." 
            << std::endl;
  tester2.InsertChange();

  std::cout << "Invoking ProcessInputs() without having called \
               SetReadyToRun() ." << std::endl;
  igstk::Tester3 tester3( logger );

  tester3.InsertChange();

  std::cout << "Invoking  SetReadyToRun() twice." << std::endl;
  igstk::Tester4 tester4( logger );

  tester4.InsertChange();
  std::cout << "Invoking  ProcessInputs() in a state without transitions \
               defined." << std::endl;
  igstk::Tester5 tester5( logger );
  
  tester5.InvokeUndefinedTransition();

  std::cout << "Invoking  ProcessInputs() in a state,input pair without \
               transitions defined." << std::endl;
  igstk::Tester6 tester6( logger );

  tester6.InvokeUndefinedStateInputTransition();

  std::cout << "Invoking  SetReadyToRun() without having called \
               SetInitialState()" << std::endl;
  igstk::Tester7 tester7( logger );

  tester7.InsertChange();

  // The following call 
  return EXIT_SUCCESS;

}
