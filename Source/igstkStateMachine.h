/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkStateMachine.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_StateMachine_h
#define __igstk_StateMachine_h

#include <iostream>
#include <map>
#include <string>

#include "igstkMacros.h"

#include "igstkStateMachineState.h"
#include "igstkStateMachineInput.h"


namespace igstk
{



/** \class StateMachine
    \brief Generic implementation of the State Machine model.

    This class provides a generic implementation of a state
    machine that can be instantiated as member of any other
    class. 

    A state machine is defined by a set of states, a set of
    inputs and a transition matrix that defines for each pair
    of (state,input) what is the next state to assume.

    \sa StateMachineState
    \sa StateMachineInput
    \sa StateMachineAction

*/

template<class TClass>
class StateMachine
{



public:

   /** Type used to represent the codes of the states */
   typedef StateMachineState<TClass>     StateType;


   /** Type used to represent the unique identifier of the states */
   typedef typename StateType::IdentifierType     StateIdentifierType;


   /** Type used to represent the codes of the inputs */
   typedef StateMachineInput<TClass>     InputType;


   /** Type used to represent the unique identifier of the inputs */
   typedef typename InputType::IdentifierType     InputIdentifierType;


   /** Type for the description of the States */
   typedef std::string       StateDescriptorType;


   /** Type for the description of the Inputs */
   typedef std::string       InputDescriptorType;


   /** Type of the action member funtion of TClass to be invoked at the end of
     * a state transition. */
  typedef  void (TClass::*TMemberFunctionPointer)();



   /** Constructor. It initialize all the transitions to the 
       start state.  */
   StateMachine();



   /** Destructor    */
   ~StateMachine();



   /** Perform the state transition, invoke the corresponding action.
       This is the method that is systematically executed when the 
       state machine is running   */
   void ProcessInput( const InputType & input );


      
   /** Set the new state to be assume as a reaction to receiving the
       input code while the StateMachine is in state. The action is
       a member method of the TClass that will be invoked just before
       changing the state. The AddTransition() method is the mechanism
       used for programming the state machine. This method should never
       be invoked while the state machine is running. Unless you want
       to debug a self-modifying machine or an evolutionary machine. */
   void AddTransition( const StateType & state, 
                       const InputType & input, 
                       const StateType & newstate, 
                       TMemberFunctionPointer action );



   /** Set the "this" pointer to the class to which this StateMachine belongs.  */
   void SetOwnerClass( TClass * );



   /** This method terminates the programming mode in which AddTransition()
    *  can be invoked and pass to the runnin mode where ProcessInput() 
    *  can be called. */
   void SetReadyToRun();


   /** Set the descriptor of a state */
   void AddState( const StateType & state, const StateDescriptorType & description );


   /** Set the descriptor of an input */
   void AddInput( const InputType & input, const InputDescriptorType & description );


   /** This extra typedef is necessary for preventing an Internal Compiler Error in 
       Microsoft Visual C++ 6.0. This typedef is not needed for any other compiler. */
   typedef std::ostream     stdOstreamType;

   /** Export the schematic description of the state machine to a stream.
       This method use the format required as input by the "dot" tool from graphviz. */
   
   void ExportTransitions( stdOstreamType & ostr ) const;


   /** Select Initial state */
   void SelectInitialState( const StateType & initialState );


   /** Return the current state identifier */
   const StateIdentifierType & GetCurrentStateIdentifier() const;


private:

   /** Variable that holds the code of the current state */
   StateIdentifierType    m_State;


   /** Pointer to the class to which this state machine is pointing to.
       The pointer to the class is needed in order to invoke the action
       member functions */
   TClass  *  m_This;


   /** This boolean flag is used as a security clip for separating the 
       programming stage from the running stage. During the programming stage
       the method AddTransition() can be invoked while the method ProcessInput() is
       forbiden. Once the machine is set to run, the method AddTransition() is
       forbiden and the method ProcessInput() becomes available */
   bool m_ReadyToRun;


   /** Container type for States */
   typedef std::map< StateIdentifierType, StateDescriptorType >  StatesContainer;
   typedef typename StatesContainer::iterator        StatesIterator;
   typedef typename StatesContainer::const_iterator  StatesConstIterator;

   /** Container for States */
   StatesContainer   m_States;


    /** Container type for Inputs */
   typedef std::map< InputIdentifierType, InputDescriptorType >  InputsContainer;
   typedef typename InputsContainer::iterator        InputIterator;
   typedef typename InputsContainer::const_iterator  InputConstIterator;

   /** Container for Inputs */
   InputsContainer   m_Inputs;

 
   /** Matrix of state transitions. It encodes the next state for 
       every pair of state and input */
   typedef std::map< InputIdentifierType, StateIdentifierType > StatesPerInputContainer;
   typedef std::map< StateIdentifierType, StatesPerInputContainer * >  TransitionContainer;
   typedef typename TransitionContainer::iterator            TransitionIterator;
   typedef typename TransitionContainer::const_iterator      TransitionConstIterator;
   typedef typename StatesPerInputContainer::iterator        StatesPerInputIterator;
   typedef typename StatesPerInputContainer::const_iterator  StatesPerInputConstIterator;

   TransitionContainer                                                 m_Transitions;



   /** Matrix of member function pointers. This pointer will
       be associated to member functions in the TClass. They
       are methods returning void and without arguments. 
       The actions are invoked just before the state transition. 
       This means that while the member method is executed, the
       state of the machine is still the state from which the
       transition was triggered.  */
   typedef std::map< InputIdentifierType, 
                     TMemberFunctionPointer >      ActionsPerInputContainer;

   typedef typename ActionsPerInputContainer::iterator        ActionsPerInputIterator;
   typedef typename ActionsPerInputContainer::const_iterator  ActionsPerInputConstIterator;

   typedef std::map< StateIdentifierType, 
                     ActionsPerInputContainer * >    ActionContainer;

   typedef typename ActionContainer::iterator        ActionIterator;
   typedef typename ActionContainer::const_iterator  ActionConstIterator;

   ActionContainer                                 m_Actions; 


};








} // end namespace igstk


#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkStateMachine.txx"
#endif


#endif


