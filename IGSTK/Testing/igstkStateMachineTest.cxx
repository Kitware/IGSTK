/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkStateMachineTest.cxx
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
// Warning about: identifier was truncated to '255' characters in 
// the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
// Warning about: constructor of the state machine receiving a pointer to this
// from a constructor. This is not a problem in this case, since the state
// machine constructor is not using the pointer, just storing it internally.
#pragma warning( disable : 4355 )
#endif

#include "igstkLogger.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkRealTimeClock.h"

#include <iostream>


namespace igstk
{

class Tester
{
public:

  typedef StateMachine< Tester >   StateMachineType;

  typedef StateMachineType::TMemberFunctionPointer        ActionType;
  typedef StateMachineType::StateType                     StateType;
  typedef StateMachineType::InputType                     InputType;
  typedef StateMachineType::StateIdentifierType           StateIdentifierType;

  
  igstkFriendClassMacro(StateMachine< Tester >);

  igstkTypeMacro( Tester, None );

  Tester():m_StateMachine(this)
    {
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
    m_StateMachine.AddTransition( m_IdleState,         m_QuarterInserted, 
                                  m_OneQuarterCredit,  NoAction );
    m_StateMachine.AddTransition( m_OneQuarterCredit,  m_QuarterInserted, 
                                  m_TwoQuarterCredit,  NoAction );
    m_StateMachine.AddTransition( m_TwoQuarterCredit,  m_QuarterInserted, 
                                  m_ThreeQuarterCredit, NoAction );
    m_StateMachine.AddTransition( m_ThreeQuarterCredit, m_QuarterInserted, 
                               m_IdleState,& Tester::CancelAndReturnChange );
    m_StateMachine.AddTransition( m_IdleState , m_SelectDrink ,
                             m_IdleState , & Tester::NoEnoughChangeMessage );
    m_StateMachine.AddTransition( m_OneQuarterCredit ,  m_SelectDrink ,
                      m_OneQuarterCredit , & Tester::NoEnoughChangeMessage );
    m_StateMachine.AddTransition( m_TwoQuarterCredit ,  m_SelectDrink ,
                        m_TwoQuarterCredit , & Tester::NoEnoughChangeMessage );
    m_StateMachine.AddTransition( m_ThreeQuarterCredit ,m_SelectDrink ,
                                        m_IdleState , & Tester::DeliverDrink );
    m_StateMachine.AddTransition( m_IdleState , m_CancelPurchase , 
                              m_IdleState , & Tester::CancelAndReturnChange );
    m_StateMachine.AddTransition( m_OneQuarterCredit ,  m_CancelPurchase , 
                               m_IdleState , & Tester::CancelAndReturnChange );
    m_StateMachine.AddTransition( m_TwoQuarterCredit ,  m_CancelPurchase ,
                               m_IdleState , & Tester::CancelAndReturnChange );
    m_StateMachine.AddTransition( m_ThreeQuarterCredit ,m_CancelPurchase , 
                                m_IdleState ,& Tester::CancelAndReturnChange );

    m_StateMachine.SelectInitialState( m_IdleState );

    // Finish the programming and get ready to run
    m_StateMachine.SetReadyToRun();

    std::cout << m_StateMachine << std::endl;
    std::cout << m_IdleState << std::endl;
    std::cout << m_SelectDrink << std::endl;
    }

  virtual ~Tester() {};

  // Methods that generate input signals
  void InsertChange() 
    {
    std::cout << "Insert Change" << std::endl;
    m_StateMachine.PushInput( m_QuarterInserted );
    m_StateMachine.ProcessInputs();
    }
  
  void SelectDrink() 
    {
    std::cout << "Select Drink" << std::endl;
    m_StateMachine.PushInput( m_SelectDrink );
    m_StateMachine.ProcessInputs();
    }

  void CancelPurchase() 
    {
    std::cout << "Cancelling Purchase" << std::endl;
    m_StateMachine.PushInput( m_CancelPurchase );
    m_StateMachine.ProcessInputs();
    }

  void SelectDrinkOrCancelPurchase(bool condition)
    {
    std::cout << "Select Drink if true, Cancel Purchase if false: " 
              << condition << std::endl;
    m_StateMachine.PushInputBoolean(condition, m_SelectDrink,m_CancelPurchase );
    m_StateMachine.ProcessInputs();
    }

  void ExportDescription( std::ostream & ostr, bool skipLoops ) const
    {
    m_StateMachine.ExportDescription( ostr, skipLoops );
    }

  void ExportDescriptionToLTS( std::ostream & ostr, bool skipLoops ) const
    {
    m_StateMachine.ExportDescriptionToLTS( ostr, skipLoops );
    }

  /** Declarations needed for the Logging */
  igstkLoggerMacro();

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

} // namespace igstk

int igstkStateMachineTest( int , char * [] )
{

  igstk::RealTimeClock::Initialize();


  std::cout << "Construct the State Machine Tester" << std::endl;
  igstk::Tester  tester;


  std::cout << std::endl << "We use the machine now " << std::endl << std::endl;
  // This is the cannonical path for using the class. 
  tester.InsertChange();
  tester.InsertChange();
  tester.InsertChange();
  tester.SelectDrink();

  std::cout << std::endl << std::endl;
  std::cout << "Second test run " << std::endl;

  // Try to get drink early on purpose, to test error states.
  tester.InsertChange();
  tester.SelectDrink();
  tester.InsertChange();
  tester.SelectDrink();
  tester.InsertChange();
  tester.SelectDrink();

  std::cout << std::endl << std::endl;
  std::cout << "Third test run " << std::endl;

  // Insert some change, and then change your mind and cancel purchase.
  igstk::Tester tester2;
  tester2.InsertChange();
  tester2.InsertChange();
  tester2.CancelPurchase();

  std::cout << std::endl << std::endl;
  std::cout << "Fourth test run " << std::endl;

  // use a condition set whether to purchase or cancel
  bool makePurchase;

  makePurchase = 0;
  igstk::Tester tester3;
  tester3.InsertChange();
  tester3.InsertChange();
  tester3.InsertChange();
  tester3.SelectDrinkOrCancelPurchase(makePurchase);
  makePurchase = 1;
  tester3.InsertChange();
  tester3.InsertChange();
  tester3.InsertChange();
  tester3.SelectDrinkOrCancelPurchase(makePurchase);

  std::cout << std::endl;

  bool skipLoops = false;

  std::cout << std::endl << std::endl;
  std::cout << "Printing out the State Machine description in dot format" 
            << std::endl;
  std::cout << std::endl << std::endl;

  tester.ExportDescription( std::cout, skipLoops );
 
  std::cout << std::endl << std::endl;
  std::cout << "Printing out the State Machine description in lts format" 
            << std::endl;
  std::cout << std::endl << std::endl;

  tester.ExportDescriptionToLTS( std::cout, skipLoops );

  
  std::cout << std::endl << std::endl;
  std::cout << "Testing now with loops = true " << std::endl;
  std::cout << std::endl << std::endl;

  skipLoops = true;

  std::cout << std::endl << std::endl;
  std::cout << "Printing out the State Machine description in dot format" 
            << std::endl;
  std::cout << std::endl << std::endl;

  tester.ExportDescription( std::cout, skipLoops );
 
  std::cout << std::endl << std::endl;
  std::cout << "Printing out the State Machine description in lts format" 
            << std::endl;
  std::cout << std::endl << std::endl;

  tester.ExportDescriptionToLTS( std::cout, skipLoops );

  return EXIT_SUCCESS;

}
