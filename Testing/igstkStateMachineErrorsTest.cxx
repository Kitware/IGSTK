
/**
 *   The purpose of this test is to trigger error conditions
 *   of the state machine and verify that they behave as expected.
 *   
 */

#include "igstkStateMachine.h"

#include <iostream>




class Tester1
{
public:

  typedef igstk::StateMachine< Tester1 >   StateMachineType;

  typedef StateMachineType::TMemberFunctionPointer        ActionType;

  FriendClassMacro(StateMachineType);

  Tester1()
    {
    // Connect the state machine to 'this' class.
    m_StateMachine.SetOwnerClass( this );

    // Set the state descriptors
    m_StateMachine.AddState( "IdleState" );
    m_StateMachine.AddState( "OneQuarterCredit" );
    m_StateMachine.AddState( "TwoQuarterCredit" );
    m_StateMachine.AddState( "ThreeQuarterCredit" );

    // Set the input descriptors
    m_StateMachine.AddInput( "QuarterInserted");
    m_StateMachine.AddInput( "SelectDrink");
    m_StateMachine.AddInput( "CancelPurchase");

    const ActionType NoAction = 0;

    // Programming the machine
    m_StateMachine.AddTransition( "IdleState",          "QuarterInserted", "OneQuarterCredit",    NoAction );
    m_StateMachine.AddTransition( "OneQuarterCredit",   "QuarterInserted", "TwoQuarterCredit",    NoAction );
    m_StateMachine.AddTransition( "TwoQuarterCredit",   "QuarterInserted", "ThreeQuarterCredit",  NoAction );
    m_StateMachine.AddTransition( "ThreeQuarterCredit", "QuarterInserted", "IdleState",           & Tester1::CancelAndReturnChange );

    m_StateMachine.AddTransition(  "IdleState",          "SelectDrink",     "IdleState",          & Tester1::NoEnoughChangeMessage );
    m_StateMachine.AddTransition(  "OneQuarterCredit",   "SelectDrink",     "OneQuarterCredit",   & Tester1::NoEnoughChangeMessage );
    m_StateMachine.AddTransition(  "TwoQuarterCredit",   "SelectDrink",     "TwoQuarterCredit",   & Tester1::NoEnoughChangeMessage );
    m_StateMachine.AddTransition(  "ThreeQuarterCredit", "SelectDrink",     "IdleState",          & Tester1::DeliverDrink );

    m_StateMachine.AddTransition(  "IdleState",          "CancelPurchase",  "IdleState",          & Tester1::CancelAndReturnChange );
    m_StateMachine.AddTransition(  "OneQuarterCredit",   "CancelPurchase",  "IdleState",          & Tester1::CancelAndReturnChange );
    m_StateMachine.AddTransition(  "TwoQuarterCredit",   "CancelPurchase",  "IdleState",          & Tester1::CancelAndReturnChange );
    m_StateMachine.AddTransition(  "ThreeQuarterCredit", "CancelPurchase",  "IdleState",          & Tester1::CancelAndReturnChange );


    m_StateMachine.SelectInitialState( "IdleState");

    // Finish the programming and get ready to run
    m_StateMachine.SetReadyToRun();
    }



  
  // Methods that generate input signals
  void InsertChange() 
    {
    std::cout << "Insert Change" << std::endl;
    m_StateMachine.ProcessInput( "QuarterInserted" );
    }
 
 
  void SelectDrink() 
    {
    std::cout << "Select Drink" << std::endl;
    m_StateMachine.ProcessInput("SelectDrink");
    }

  void CancelPurchase() 
    {
    std::cout << "Cancelling Purchase" << std::endl;
    m_StateMachine.ProcessInput("CancelPurchase");
    }


  void ExportTransitions( std::ostream & ostr ) const
    {
    m_StateMachine.ExportTransitions( ostr );
    }


  const StateMachineType::StateDescriptorType & GetCurrentState() const
    {
    return m_StateMachine.GetCurrentState();
    }


  void triggerError1()
  {
    m_StateMachine.SelectInitialState("ThisStateDoesntExists");
  }

protected:

  // Reaction methods that will be invoked during the 
  // state machine transitions. Note that this methods
  // are protected because they should only be called
  // from the state machine.
  void CancelAndReturnChange() 
    {
    std::cout << "Cancelling and returning Change" << std::endl;
    }


  void DeliverDrink()
    {
    std::cout << "Deliver Drink" << std::endl;
    }


  void NoEnoughChangeMessage()
    {
    std::cout << "You have not inserted enough change" << std::endl;
    }


private:

  StateMachineType   m_StateMachine;

};



class Tester2
{
public:

  typedef igstk::StateMachine< Tester2 >   StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer        ActionType;
  FriendClassMacro(StateMachineType);

  Tester2()
    {
    // On purpose NOT calling : m_StateMachine.SetOwnerClass( this );
    m_StateMachine.AddState( "IdleState" );
    m_StateMachine.SelectInitialState( "IdleState");
    m_StateMachine.SetReadyToRun();
    }
private:
  StateMachineType   m_StateMachine;
};




int igstkStateMachineErrorsTest(int argc, char * argv [])
{

  std::cout << "Construct the State Machine Tester" << std::endl;
  Tester1  tester1;

  std::cout << std::endl << std::endl;
  std::cout << "Trigger error conditions on purpose" << std::endl;

  // The following call are designed for testing the error conditions of the
  // state machine. 

  std::cout << "Invoking as Initial state a state that doesn't exist." << std::endl;
  tester1.triggerError1();


  std::cout << "Invoking as Run when there is no parent class connected." << std::endl;
  Tester2 tester2;


  // The following call 
  return EXIT_SUCCESS;

}


