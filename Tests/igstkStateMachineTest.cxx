
#include "igstkStateMachine.h"

#include <iostream>




class Tester
{
public:

  typedef igstk::StateMachine< Tester >   StateMachineType;

  typedef StateMachineType::TMemberFunctionPointer        ActionType;

#if defined(__GNUC__)
  friend class StateMachineType;
#else
  friend StateMachineType;
#endif
  

  Tester()
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
      m_StateMachine.AddTransition( "ThreeQuarterCredit", "QuarterInserted", "IdleState",           & Tester::CancelAndReturnChange );

      m_StateMachine.AddTransition(  "IdleState",          "SelectDrink",     "IdleState",          & Tester::NoEnoughChangeMessage );
      m_StateMachine.AddTransition(  "OneQuarterCredit",   "SelectDrink",     "OneQuarterCredit",   & Tester::NoEnoughChangeMessage );
      m_StateMachine.AddTransition(  "TwoQuarterCredit",   "SelectDrink",     "TwoQuarterCredit",   & Tester::NoEnoughChangeMessage );
      m_StateMachine.AddTransition(  "ThreeQuarterCredit", "SelectDrink",     "IdleState",          & Tester::DeliverDrink );

      m_StateMachine.AddTransition(  "IdleState",          "CancelPurchase",  "IdleState",          & Tester::CancelAndReturnChange );
      m_StateMachine.AddTransition(  "OneQuarterCredit",   "CancelPurchase",  "IdleState",          & Tester::CancelAndReturnChange );
      m_StateMachine.AddTransition(  "TwoQuarterCredit",   "CancelPurchase",  "IdleState",          & Tester::CancelAndReturnChange );
      m_StateMachine.AddTransition(  "ThreeQuarterCredit", "CancelPurchase",  "IdleState",          & Tester::CancelAndReturnChange );


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





int igstkStateMachineTest(int argc, char * argv [])
{

  Tester  tester;


  // This is the cannonical path for using the class. 
  tester.InsertChange();
  tester.InsertChange();
  tester.InsertChange();
  tester.SelectDrink();




  std::cout << std::endl << std::endl;
  std::cout << "Second test run " << std::endl;

  // Try to get drink ealy on purpose, to test error states.
  tester.InsertChange();
  tester.SelectDrink();
  tester.InsertChange();
  tester.SelectDrink();
  tester.InsertChange();
  tester.SelectDrink();

  
  std::cout << std::endl << std::endl;
  std::cout << "Printing out the State Machine description in dot format" << std::endl;
  std::cout << std::endl << std::endl;

  tester.ExportTransitions( std::cout );


  return EXIT_SUCCESS;

}


