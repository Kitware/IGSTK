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

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "igstkMacros.h"

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

*/

template<class TClass>
class StateMachine
{



public:

   /** Type used to represent the codes of the states */
   typedef std::string     StateDescriptorType;



   /** Type used to represent the codes of the inputs */
   typedef std::string     InputDescriptorType;



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
   void ProcessInput( const InputDescriptorType & input );


      
   /** Set the new state to be assume as a reaction to receiving the
       input code while the StateMachine is in state. The action is
       a member method of the TClass that will be invoked just before
       changing the state. The AddTransition() method is the mechanism
       used for programming the state machine. This method should never
       be invoked while the state machine is running. Unless you want
       to debug a self-modifying machine or an evolutionary machine. */
   void AddTransition( const StateDescriptorType & state, 
                       const InputDescriptorType & input, 
                       const StateDescriptorType & newstate, 
                       TMemberFunctionPointer action );



   /** Set the "this" pointer to the class to which this StateMachine belongs.  */
   void SetOwnerClass( TClass * );



   /** This method terminates the programming mode in which AddTransition()
    *  can be invoked and pass to the runnin mode where ProcessInput() 
    *  can be called. */
   void SetReadyToRun();


   /** Set the descriptor of a state */
   void AddState( const StateDescriptorType & descriptor );


   /** Set the descriptor of an input */
   void AddInput( const InputDescriptorType & descriptor );


   /** This extra typedef is necessary for preventing an Internal Compiler Error in 
       Microsoft Visual C++ 6.0. This typedef is not needed for any other compiler. */
   typedef std::ostream     stdOstreamType;

   /** Export the schematic description of the state machine to a stream.
       This method use the format required as input by the "dot" tool from graphviz. */
   
   void ExportTransitions( stdOstreamType & ostr ) const;


   /** Select Initial state */
   void SelectInitialState( const StateDescriptorType & initialStateDescriptor );


   /** Return the current state */
   const StateDescriptorType & GetCurrentState() const;


private:

   /** Variable that holds the code of the current state */
   StateDescriptorType    m_State;


   /** Variable that holds the code of the current input */
   InputDescriptorType    m_Input;


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


   /** Container of state identifiers  */
   typedef std::vector< StateDescriptorType >   StatesContainer;
   StatesContainer                              m_States;


   /** Container of input identifiers  */
   typedef std::vector< InputDescriptorType >   InputsContainer;
   InputsContainer                              m_Inputs;



   /** Matrix of state transitions. It encodes the next state for 
       every pair of state and input */
   typedef std::map< InputDescriptorType, StateDescriptorType >        StatesPerInputContainer;
   typedef std::map< StateDescriptorType, StatesPerInputContainer * >  TransitionContainer;
   TransitionContainer                                                 m_Transitions;



   /** Matrix of member function pointers. This pointer will
       be associated to member functions in the TClass. They
       are methods returning void and without arguments. 
       The actions are invoked just before the state transition. 
       This means that while the member method is executed, the
       state of the machine is still the state from which the
       transition was triggered.  */
   typedef std::map< InputDescriptorType, TMemberFunctionPointer >      ActionsPerInputContainer;
   typedef std::map< StateDescriptorType, ActionsPerInputContainer * >  ActionContainer;
   ActionContainer                                                      m_Actions; 


};








} // end namespace igstk


#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkStateMachine.txx"
#endif


#endif


