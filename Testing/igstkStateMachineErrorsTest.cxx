
/**
 *   The purpose of this test is to trigger error conditions
 *   of the state machine and verify that they behave as expected.
 *   
 */

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkStateMachine.h"

#include <iostream>




class Tester1
{
public:

  typedef igstk::StateMachine< Tester1 >   StateMachineType;

  typedef StateMachineType::TMemberFunctionPointer        ActionType;
  typedef StateMachineType::StateType                     StateType;
  typedef StateMachineType::InputType                     InputType;
  typedef StateMachineType::StateIdentifierType           StateIdentifierType;

  FriendClassMacro(StateMachineType);

  Tester1()
    {
    // Connect the state machine to 'this' class.
    m_StateMachine.SetOwnerClass( this );

    // Set the state descriptors
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.AddState( m_OneQuarterCredit, "OneQuarterCredit" );
    m_StateMachine.AddState( m_TwoQuarterCredit, "TwoQuarterCredit" );
    m_StateMachine.AddState( m_ThreeQuarterCredit, "ThreeQuarterCredit" );

    // Set the input descriptors
    m_StateMachine.AddInput( m_QuarterInserted, "QuarterInserted" );

    const ActionType NoAction = 0;

    // Programming the machine
    m_StateMachine.AddTransition( m_IdleState,          m_QuarterInserted, m_OneQuarterCredit,    NoAction );
    m_StateMachine.AddTransition( m_OneQuarterCredit,   m_QuarterInserted, m_TwoQuarterCredit,    NoAction );
    m_StateMachine.AddTransition( m_TwoQuarterCredit,   m_QuarterInserted, m_ThreeQuarterCredit,  NoAction );

    m_StateMachine.SelectInitialState( m_IdleState );

    // Finish the programming and get ready to run
    m_StateMachine.SetReadyToRun();
    }

  void triggerError1()
  {
    // NOTE that on purpose the m_ThisStateDoesntExist was not added as a state
    // with AddState() */
    m_StateMachine.SelectInitialState( m_ThisStateDoesntExist );
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

  typedef igstk::StateMachine< Tester2 >              StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer    ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;

  FriendClassMacro(StateMachineType);

  Tester2()
    {
    // On purpose NOT calling : m_StateMachine.SetOwnerClass( this );
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.SelectInitialState( m_IdleState );
    m_StateMachine.SetReadyToRun();

    }

  void InsertChange() 
    {
    std::cout << "Insert Change" << std::endl;
    // On purpose we didn't set m_QuarterInserted with AddInput();
    m_StateMachine.ProcessInput( m_QuarterInserted );
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

  typedef igstk::StateMachine< Tester3 >              StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer    ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;

  FriendClassMacro(StateMachineType);

  Tester3()
    {
    m_StateMachine.SetOwnerClass( this );
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.SelectInitialState( m_IdleState );
    m_StateMachine.AddInput( m_QuarterInserted, "QuarterInserted" );
    const ActionType NoAction = 0;
    m_StateMachine.AddTransition( m_IdleState, m_QuarterInserted, m_IdleState, NoAction );
    // On purpose NOT calling : m_StateMachine.SetReadyToRun();
    }

  void InsertChange() 
    {
    std::cout << "Insert Change" << std::endl;
    m_StateMachine.ProcessInput( m_QuarterInserted );
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

  typedef igstk::StateMachine< Tester4 >              StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer    ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;

  FriendClassMacro(StateMachineType);

  Tester4()
    {
    m_StateMachine.SetOwnerClass( this );
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.SelectInitialState( m_IdleState );
    m_StateMachine.AddInput( m_QuarterInserted, "QuarterInserted" );
    const ActionType NoAction = 0;
    m_StateMachine.AddTransition( m_IdleState, m_QuarterInserted, m_IdleState, NoAction );

    // On purpose invode SetReadyToRun() twice.
    m_StateMachine.SetReadyToRun();
    m_StateMachine.SetReadyToRun();
    }

  void InsertChange() 
    {
    std::cout << "Insert Change" << std::endl;
    m_StateMachine.ProcessInput( m_QuarterInserted );
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

  typedef igstk::StateMachine< Tester5 >   StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer        ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;

  FriendClassMacro(StateMachineType);

  Tester5()
    {
    m_StateMachine.SetOwnerClass( this );
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.SelectInitialState( m_IdleState );
    m_StateMachine.AddInput( m_QuarterInserted, "QuarterInserted" );
    const ActionType NoAction = 0;
    // On purpose NOT adding any transition: for testing error condition.
    // m_StateMachine.AddTransition( m_IdleState, m_QuarterInserted, m_IdleState, NoAction );
    m_StateMachine.SetReadyToRun();
    }

  void InvokeUndefinedTransition() 
    {
    std::cout << "Insert Change" << std::endl;
    m_StateMachine.ProcessInput( m_QuarterInserted );
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

  typedef igstk::StateMachine< Tester6 >              StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer    ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;

  FriendClassMacro(StateMachineType);

  Tester6()
    {
    m_StateMachine.SetOwnerClass( this );
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.SelectInitialState( m_IdleState );
    m_StateMachine.AddInput( m_QuarterInserted, "QuarterInserted" );
    m_StateMachine.AddInput( m_Cancel, "Cancel");
    const ActionType NoAction = 0;
    // On purpose NOT adding any transition: input m_Cancel
    m_StateMachine.AddTransition( m_IdleState, m_QuarterInserted, m_IdleState, NoAction );
    m_StateMachine.SetReadyToRun();
    }

  void InvokeUndefinedStateInputTransition() 
    {
    std::cout << "Invoking Cancel" << std::endl;
    m_StateMachine.ProcessInput( m_Cancel );
    }
 
private:
  StateMachineType   m_StateMachine;

  /** List of States */
  StateType m_IdleState;

  /** List of Inputs */
  InputType m_QuarterInserted;
  InputType m_Cancel;

};




int igstkStateMachineErrorsTest( int, char * [] )
{

  std::cout << "Construct the State Machine Tester" << std::endl;
  Tester1  tester1;

  std::cout << std::endl << std::endl;
  std::cout << "Trigger error conditions on purpose" << std::endl;

  // The following call are designed for testing the error conditions of the
  // state machine. 

  std::cout << "Invoking as Initial state a state that doesn't exist." << std::endl;
  tester1.triggerError1();

  std::cout << "Invoking SetReadyToRun() (in constructor) without parent class connected." << std::endl;
  Tester2 tester2;

  std::cout << "Invoking ProcessInput() without parent class connected." << std::endl;
  tester2.InsertChange();

  std::cout << "Invoking ProcessInput() without having called SetReadyToRun() ." << std::endl;
  Tester3 tester3;
  tester3.InsertChange();

  std::cout << "Invoking  SetReadyToRun() twice." << std::endl;
  Tester4 tester4;
  tester4.InsertChange();

  std::cout << "Invoking  ProcessInput() in a state without transitions defined." << std::endl;
  Tester5 tester5;
  tester5.InvokeUndefinedTransition();

  std::cout << "Invoking  ProcessInput() in a state,input pair without transitions defined." << std::endl;
  Tester6 tester6;
  tester6.InvokeUndefinedStateInputTransition();


  // The following call 
  return EXIT_SUCCESS;

}


