
#include "igstkStateMachine.h"

#include <iostream>


class Tester
{
public:

  typedef igstk::StateMachine< Tester, 3, 3 >   StateMachineType;


  Tester()
    {
    // Connect the state machine to 'this' class.
    m_StateMachine.SetClass( this );

    // Programming the machine
    m_StateMachine.SetTransition( 0, 0, 1, 0 );
    m_StateMachine.SetTransition( 1, 0, 2, 0 );
    m_StateMachine.SetTransition( 2, 0, 3, 0 );
    m_StateMachine.SetTransition( 3, 0, 0, CancelAndReturnChange );

    m_StateMachine.SetTransition( 0, 1, 0, NoEnoughChangeMessage );
    m_StateMachine.SetTransition( 1, 1, 1, NoEnoughChangeMessage );
    m_StateMachine.SetTransition( 2, 1, 2, NoEnoughChangeMessage );
    m_StateMachine.SetTransition( 3, 1, 0, DeliverDrink );

    // Finish the programming and get ready to run
    m_StateMachine.SetReadyToRun();
    }

  
  void InsertChange() 
    {
    std::cout << "Insert Change" << std::endl;
    }
 

  void CancelAndReturnChange() 
    {
    std::cout << "Cancelling and returning Change" << std::endl;
    }

 
  void SelectDrink() 
    {
    std::cout << "Select Drink" << std::endl;
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

  Tester tester;

  typedef igstk::StateMachine< Tester, 3, 3 >   StateMachineType;


  tester.InsertChange();
  tester.InsertChange();
  tester.InsertChange();
  tester.SelectDrink();


  return EXIT_SUCCESS;

}


