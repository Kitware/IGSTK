/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkStateMachine.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkStateMachine_h
#define __igstkStateMachine_h

#include <iostream>
#include <map>
#include <queue>
#include <string>

#include "igstkMacros.h"
#include "igstkStateMachineState.h"
#include "igstkStateMachineInput.h"


namespace igstk
{


/** \class StateMachine
 *  \brief Generic implementation of the State Machine model.
 *
 *  This class provides a generic implementation of a state
 *  machine that can be instantiated as member of any other
 *  class. 
 *
 *  A state machine is defined by a set of states, a set of
 *  inputs and a transition matrix that defines for each pair
 *  of (state,input) what is the next state to assume.
 *
 *  \sa StateMachineState
 *  \sa StateMachineInput
 *  \sa StateMachineAction
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

  /** Type for the Action to be taken. This is a pointer to a method of the
   * class that will own the State Machine */
  typedef TMemberFunctionPointer   ActionType;

  /** Constructor. It initialize all the transitions to the 
   *  start state.  */
  StateMachine( TClass * );

  /** Destructor    */
  ~StateMachine();

  /** Push a new input in the queue of inputs to be processed.  */
  void PushInput( const InputType & input );

  /** Push one of two inputs onto the queue, according two whether a
   *  condition is true or false */
  void PushInputBoolean( bool condition, const InputType & inputIfTrue,
                          const InputType & inputIfFalse);

  /** Perform the state transition and invoke the corresponding action for
   *  every pending input stored in the input queue.  */
  void ProcessInputs();
     
  /** Set the new state to be assume as a reaction to receiving the
   *  input code while the StateMachine is in state. The action is
   *  a member method of the TClass that will be invoked just before
   *  changing the state. The AddTransition() method is the mechanism
   *  used for programming the state machine. This method should never
   *  be invoked while the state machine is running. Unless you want
   *  to debug a self-modifying machine or an evolutionary machine. */
  void AddTransition( const StateType  & state, 
                      const InputType  & input, 
                      const StateType  & newstate, 
                      const ActionType & action );

  /** This method terminates the programming mode in which AddTransition()
   *  can be invoked and pass to the runnin mode where ProcessInput() 
   *  can be called. */
  void SetReadyToRun();

  /** Set the descriptor of a state */
  void AddState( const StateType & state, 
                 const StateDescriptorType & description );

  /** Set the descriptor of an input */
  void AddInput( const InputType & input, 
                 const InputDescriptorType & description );

  /** This extra typedef is necessary for preventing an Internal Compiler 
   *  Error in  Microsoft Visual C++ 6.0. 
   *  This typedef is not needed for any other compiler. */
   typedef std::ostream               OutputStreamType;

  /** Export the schematic description of the state machine to a stream.
   *  This method use the format required as input by the "dot" tool 
   *  from graphviz. */
  void ExportDescription( OutputStreamType & ostr, bool skipLoops ) const;

  /** Export the schematic description of the state machine to a stream.
   *  This method use the format required as input by the "LTSA" analysis 
   *  tools. 
   *  LTSA (Labeled Transition Systems Analyzer) available from  
   *  http://www-dse.doc.ic.ac.uk/concurrency.   */
  void ExportDescriptionToLTS( OutputStreamType & ostr, bool skipLoops) const;

  /** Export the schematic description of the state machine to a stream.
   *  This method use the "SCXML" format. */
  void ExportDescriptionToSCXML( OutputStreamType & ostr, bool skipLoops) const;

  /** Select Initial state */
  void SelectInitialState( const StateType & initialState );

  /** Print out the content of the class */
  void Print(std::ostream& os, itk::Indent indent) const;

protected:
   
  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;
 
  /** Perform the state transition, invoke the corresponding action.
   *  This is the method that is systematically executed when the 
   *  state machine is running   */
  void ProcessInput( const InputIdentifierType & input );

private:

  /** Variable that holds the code of the current state */
  StateIdentifierType    m_State;

  /** Pointer to the class to which this state machine is pointing to.
   *  The pointer to the class is needed in order to invoke the action
   *  member functions */
  TClass  *  m_This;


  /** This boolean flag is used as a security clip for separating the 
   *  programming stage from the running stage. During the programming stage
   *  the method AddTransition() can be invoked while the method 
   *  ProcessInput() is forbiden. Once the machine is set to run, 
   *  the method AddTransition() is forbiden and the method ProcessInput() 
   *  becomes available */
  bool m_ReadyToRun;

  /** This boolean flag indicates whether the user has defined an initial
   * state or not. The machine will be allowed to run until a valid state
   * is selected as initial state. */
  bool m_InitialStateSelected;

  /** Container type for States */
  typedef std::map< StateIdentifierType, StateDescriptorType > StatesContainer;
  typedef typename StatesContainer::iterator                   StatesIterator;
  typedef typename StatesContainer::const_iterator        StatesConstIterator;

  /** Container for States */
  StatesContainer   m_States;

  /** Get the Descriptor of a particular state. This method must be used instead
   *  of the simple invokation to the m_States[] operator because the [] 
   *  operator creates an entry when the key is not found. */
  StateDescriptorType GetStateDescriptor( const StateIdentifierType & stateId );

  /** Get the Descriptor of a particular input. This method must be used instead
   *  of the simple invokation to the m_Inputs[] operator because the [] 
   *  operator creates an entry when the key is not found. */
  InputDescriptorType GetInputDescriptor( const InputIdentifierType & inputId );
  
  /** Container type for Inputs */
  typedef std::map< InputIdentifierType, InputDescriptorType > InputsContainer;
  typedef typename InputsContainer::iterator                   InputIterator;
  typedef typename InputsContainer::const_iterator       InputConstIterator;
  typedef std::queue< InputIdentifierType >              InputsQueueContainer;

  /** Container for Inputs */
  InputsContainer   m_Inputs;

  /** \class StateActionPair
   *  Pair class containing an output State and an Action to be taken */
  class StateActionPair 
    {
  public:
    StateActionPair()
      {
      this->m_StateIdentifier  = 0;
      this->m_Action = 0;
      }
    StateActionPair( StateIdentifierType state, ActionType action )
      {
      this->m_StateIdentifier  = state;
      this->m_Action = action;
      }
    StateActionPair( const StateActionPair & in )
      {
      this->m_StateIdentifier  = in.m_StateIdentifier;
      this->m_Action = in.m_Action;
      }
    const StateActionPair & operator=( const StateActionPair & in )
      {
      this->m_StateIdentifier = in.m_StateIdentifier;
      this->m_Action = in.m_Action;
      return *this;
      }
    StateIdentifierType GetStateIdentifier() const 
      { 
      return m_StateIdentifier;
      }
    ActionType GetAction() const 
      { 
      return m_Action; 
      }
  private:
    
    StateIdentifierType     m_StateIdentifier;
    ActionType              m_Action;
    };
   
  /** Matrix of state transitions. It encodes the next state for 
   *  every pair of state and input */
  typedef std::map< InputIdentifierType, StateActionPair > 
                                                  TransitionsPerInputContainer;
  typedef std::map< StateIdentifierType, TransitionsPerInputContainer * >  
                                                  TransitionContainer;
  typedef typename TransitionContainer::iterator  TransitionIterator;


  typedef typename TransitionContainer::const_iterator TransitionConstIterator;
  typedef typename TransitionsPerInputContainer::iterator
                                                  TransitionsPerInputIterator;
  typedef typename TransitionsPerInputContainer::const_iterator  
                                              TransitionsPerInputConstIterator;

  TransitionContainer                                 m_Transitions;
  InputsQueueContainer                                m_QueuedInputs;
};

/** Print the object information in a stream. */
template<class TClass>
std::ostream& operator<<(std::ostream& os, const StateMachine<TClass>& o);

} // end namespace igstk


#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkStateMachine.txx"
#endif

#endif
