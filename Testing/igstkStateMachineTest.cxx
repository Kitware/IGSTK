
#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkStateMachine.h"

#include <iostream>




class Tester
{
public:

  typedef igstk::StateMachine< Tester >   StateMachineType;

  typedef StateMachineType::TMemberFunctionPointer        ActionType;
  typedef StateMachineType::StateType                     StateType;
  typedef StateMachineType::InputType                     InputType;
  typedef StateMachineType::StateIdentifierType           StateIdentifierType;

  FriendClassMacro(StateMachineType);

  Tester()
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
      m_StateMachine.AddInput( m_SelectDrink, "SelectDrink" );
      m_StateMachine.AddInput( m_CancelPurchase, "CancelPurchase" );
                               
      const ActionType NoAction = 0;

      // Programming the machine
      m_StateMachine.AddTransition( m_IdleState,          m_QuarterInserted, m_OneQuarterCredit,    NoAction );
      m_StateMachine.AddTransition( m_OneQuarterCredit,   m_QuarterInserted, m_TwoQuarterCredit,    NoAction );
      m_StateMachine.AddTransition( m_TwoQuarterCredit,   m_QuarterInserted, m_ThreeQuarterCredit,  NoAction );
      m_StateMachine.AddTransition( m_ThreeQuarterCredit, m_QuarterInserted, m_IdleState,           & Tester::CancelAndReturnChange );

      m_StateMachine.AddTransition( m_IdleState ,         m_SelectDrink ,    m_IdleState ,          & Tester::NoEnoughChangeMessage );
      m_StateMachine.AddTransition( m_OneQuarterCredit ,  m_SelectDrink ,    m_OneQuarterCredit ,   & Tester::NoEnoughChangeMessage );
      m_StateMachine.AddTransition( m_TwoQuarterCredit ,  m_SelectDrink ,    m_TwoQuarterCredit ,   & Tester::NoEnoughChangeMessage );
      m_StateMachine.AddTransition( m_ThreeQuarterCredit ,m_SelectDrink ,    m_IdleState ,          & Tester::DeliverDrink );

      m_StateMachine.AddTransition( m_IdleState ,         m_CancelPurchase , m_IdleState ,          & Tester::CancelAndReturnChange );
      m_StateMachine.AddTransition( m_OneQuarterCredit ,  m_CancelPurchase , m_IdleState ,          & Tester::CancelAndReturnChange );
      m_StateMachine.AddTransition( m_TwoQuarterCredit ,  m_CancelPurchase , m_IdleState ,          & Tester::CancelAndReturnChange );
      m_StateMachine.AddTransition( m_ThreeQuarterCredit ,m_CancelPurchase , m_IdleState ,          & Tester::CancelAndReturnChange );


      m_StateMachine.SelectInitialState( m_IdleState );

      // Finish the programming and get ready to run
      m_StateMachine.SetReadyToRun();
    }



  
  // Methods that generate input signals
  void InsertChange() 
    {
      std::cout << "Insert Change" << std::endl;
      m_StateMachine.ProcessInput( m_QuarterInserted );
    }
 
 
  void SelectDrink() 
    {
      std::cout << "Select Drink" << std::endl;
      m_StateMachine.ProcessInput( m_SelectDrink );
    }

  void CancelPurchase() 
    {
      std::cout << "Cancelling Purchase" << std::endl;
      m_StateMachine.ProcessInput( m_CancelPurchase );
    }


  void ExportTransitions( std::ostream & ostr ) const
    {
    m_StateMachine.ExportTransitions( ostr );
    }


  const StateIdentifierType & GetCurrentState() const
    {
    return m_StateMachine.GetCurrentStateIdentifier();
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

  /** List of States */
  StateType m_IdleState;
  StateType m_OneQuarterCredit;
  StateType m_TwoQuarterCredit;
  StateType m_ThreeQuarterCredit;
  
  /** List of Inputs */
  InputType m_QuarterInserted;
  InputType m_SelectDrink;
  InputType m_CancelPurchase;

};




int igstkStateMachineTest( int , char * [] )
{

  std::cout << "Construct the State Machine Tester" << std::endl;
  Tester  tester;


  std::cout << std::endl << "We use the machine now " << std::endl << std::endl;
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
  std::cout << "Third test run " << std::endl;

  // Insert some change, and then change your mind and cancel purchase.
  Tester tester2;
  tester2.InsertChange();
  tester2.InsertChange();
  tester2.CancelPurchase();


  // Exercise the GetCurrentState() method
  std::cout << tester.GetCurrentState() << std::endl;
  
  std::cout << std::endl << std::endl;
  std::cout << "Printing out the State Machine description in dot format" << std::endl;
  std::cout << std::endl << std::endl;

  tester.ExportTransitions( std::cout );


  return EXIT_SUCCESS;

}


