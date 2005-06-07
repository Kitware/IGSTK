/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkStateMachine.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_StateMachine_txx
#define __igstk_StateMachine_txx

#include "igstkStateMachine.h"


namespace igstk
{




template<class TClass>
StateMachine< TClass >
::StateMachine( TClass * owner )
{
  m_This = owner;

  m_ReadyToRun = false;

  m_InitialStateSelected = false;
}




template<class TClass>
StateMachine< TClass >
::~StateMachine()
{
  // Release memory from Transitions container
  TransitionIterator transitionItr = m_Transitions.begin();
  TransitionIterator transitionEnd = m_Transitions.end();
  while( transitionItr != transitionEnd )
    {
    delete transitionItr->second;
    ++transitionItr;
    }
  m_Transitions.clear();

}




template<class TClass>
void
StateMachine< TClass >
::AddInput( const InputType & input, const InputDescriptorType & descriptor )
{

   /** The structure of the StateMachineToken guarantees that identifiers are
    * unique. */
   m_Inputs[ input.GetIdentifier() ] = descriptor;

}





template<class TClass>
void
StateMachine< TClass >
::AddState( const StateType & state, const StateDescriptorType & descriptor )
{

   /** The structure of the StateMachineToken guarantees that identifiers are
    * unique. */
   m_States[ state.GetIdentifier() ] = descriptor;

}



template<class TClass>
void
StateMachine< TClass >
::SelectInitialState( const StateType & initialState )
{

  StatesConstIterator  state = 
              m_States.find( initialState.GetIdentifier() );
    
  if( state == m_States.end() )
    {
    std::cerr << "In class " << m_This->GetNameOfClass() << std::endl;
    std::cerr << "State selected as initial state has not been defined yet. " << std::endl;
    std::cerr << "Attempted state = " << initialState.GetIdentifier() << std::endl;
    std::cerr.flush();
    return;
    } 

  m_State = state->first;

  m_InitialStateSelected = true;

}




template<class TClass>
void
StateMachine< TClass >
::SetReadyToRun()
{
  if( m_ReadyToRun )
    {
    std::cerr << "In class " << m_This->GetNameOfClass() << std::endl;
    std::cerr << "Error: attempt to invoke SetReadyToRun() " << std::endl;
    std::cerr << "but the machine is ready to go " << std::endl;
    std::cerr << "There is no reason for invoking SetReadyToRun() twice." << std::endl;
    std::cerr << "Something should be wrong in the logic or your " << std::endl;
    std::cerr << "state machine programming." << std::endl;
    std::cerr.flush();
    return;
    }

  if( !m_InitialStateSelected )
    {
    std::cerr << "In class " << m_This->GetNameOfClass() << std::endl;
    std::cerr << "Error: No initial state has been selected. "   << std::endl;
    std::cerr << "You must select an initial State out of the " << std::endl;
    std::cerr << "set of states defined for the State Machine." << std::endl;
    return;
    }

  m_ReadyToRun = true;
}



template<class TClass>
void
StateMachine< TClass >
::PushInput( const InputType & input )
{
  m_QueuedInputs.push( input.GetIdentifier() );
}



template<class TClass>
void
StateMachine< TClass >
::ProcessInputs()
{

  while( ! m_QueuedInputs.empty() )
    {
    InputIdentifierType inputId = m_QueuedInputs.front();
    m_QueuedInputs.pop();
    // WARNING: It is very important to do the pop() before invoking ProcessInput()
    // otherwise the inputs will accumulate in the queue.
    this->ProcessInput( inputId );
    }

}



template<class TClass>
void
StateMachine< TClass >
::ProcessInput( const InputIdentifierType & inputIdentifier )
{
  if( !m_ReadyToRun )
    {
    std::cerr << "In class " << m_This->GetNameOfClass() << std::endl;
    std::cerr << "Error: attempt to invoke a StateTransition " << std::endl;
    std::cerr << "but the machine is not ready to go" << std::endl;
    std::cerr << "You must invoke the method SetReadyToRun() first." << std::endl;
    std::cerr.flush();
    return;
    }

  TransitionConstIterator transitionsFromThisState = 
                                 m_Transitions.find( m_State );

  
  if( transitionsFromThisState == m_Transitions.end() )
    {
    std::cerr << "In class " << m_This->GetNameOfClass() << std::endl;
    std::cerr << "No transitions have been defined for current state = ";
    std::cerr << m_State << std::endl;
    std::cerr.flush();
    return;
    } 

  TransitionsPerInputConstIterator  transitionItr = 
            transitionsFromThisState->second->find( inputIdentifier );

  if( transitionItr == transitionsFromThisState->second->end() )
    {
    std::cerr << "In class " << m_This->GetNameOfClass() << std::endl;
    std::cerr << "No transitions have been defined for current state and input " << std::endl;
    std::cerr << "State = "  << m_State << std::endl;
    std::cerr << "Input = "  << inputIdentifier << std::endl;
    std::cerr << std::endl;
    std::cerr.flush();
    return;
    } 

  StateActionPair transition = transitionItr->second;

  if( transition.GetAction() )
    {
    ((*m_This).*(transition.GetAction()))();
    }

  m_State = transition.GetStateIdentifier();

}




template<class TClass>
void
StateMachine< TClass >
::AddTransition( const StateType  & state,   
                 const InputType  & input, 
                 const StateType  & newState, 
                 const ActionType & action )
{
 
  // First check if the State exists
  StatesConstIterator stateItr = m_States.find( state.GetIdentifier() );
    
  if( stateItr == m_States.end() )
    {
    std::cerr << "In class " << m_This->GetNameOfClass() << std::endl;
    std::cerr << "Attempt to add a Transition for a State that does not exist" << std::endl;
    std::cerr << "Attempted state     = " << state.GetIdentifier() << std::endl;
    std::cerr << "Attempted input     = " << input.GetIdentifier() << std::endl;
    std::cerr << "Attempted new state = " << newState.GetIdentifier() << std::endl;
    std::cerr.flush();
    return;
    } 

  // Then check if the Input exists
  InputConstIterator inputItr = m_Inputs.find( input.GetIdentifier() );
    
  if( inputItr == m_Inputs.end() )
    {
    std::cerr << "In class " << m_This->GetNameOfClass() << std::endl;
    std::cerr << "Attempt to add a Transition for an Input that does not exist" << std::endl;
    std::cerr << "Attempted state     = " << state.GetIdentifier() << std::endl;
    std::cerr << "Attempted input     = " << input.GetIdentifier() << std::endl;
    std::cerr << "Attempted new state = " << newState.GetIdentifier() << std::endl;
    std::cerr.flush();
    return;
    } 


  // Check if the new State exists
  StatesConstIterator newstateItr = m_States.find( newState.GetIdentifier() );
    
  if( newstateItr == m_States.end() )
    {
    std::cerr << "In class " << m_This->GetNameOfClass() << std::endl;
    std::cerr << "Attempt to add a Transition for a New State that does not exist" << std::endl;
    std::cerr << "Attempted state     = " << state.GetIdentifier() << std::endl;
    std::cerr << "Attempted input     = " << input.GetIdentifier() << std::endl;
    std::cerr << "Attempted new state = " << newState.GetIdentifier() << std::endl;
    std::cerr.flush();
    return;
    } 


  // Search for existing Transitions for that State
  TransitionConstIterator transitionsFromThisState = 
                               m_Transitions.find( state.GetIdentifier() );
  
  if( transitionsFromThisState == m_Transitions.end() )
    {
    // No transition has been created for this particular state.
    // Therefore create a new entry for it.
    TransitionsPerInputContainer *transitionsPerInput = new TransitionsPerInputContainer;

    // Insert the new state that should be assumed if the input is received.
    StateActionPair transition( newState.GetIdentifier(), action );
    (*transitionsPerInput)[ input.GetIdentifier() ] = transition;

    // Add the transitionsPerInput container to the Transitions container.
    m_Transitions[ state.GetIdentifier() ] = transitionsPerInput;
    } 
  else
    {
    // Check if the particular Input has already an Entry here
    TransitionsPerInputConstIterator transitionsFromThisStateAndInput =
                     transitionsFromThisState->second->find( input.GetIdentifier() );
    if( transitionsFromThisStateAndInput != transitionsFromThisState->second->end() )
      {
      // There is already an entry for this input. This is suspicious
      // because the user may be overriding a previous transition by
      // accident. 
      StateIdentifierType newStateIdentifier = 
                      transitionsFromThisStateAndInput->second.GetStateIdentifier();
      std::cerr << "In class " << m_This->GetNameOfClass() << std::endl;
      std::cerr << "Attempt to override an existing transition. "
                << "Please verify the programming of your state machine. "
                << "There is already a transition defined for the combination: " << std::endl;
      std::cerr << "State     = " << state.GetIdentifier() << std::endl;
      std::cerr << "Input     = " << input.GetIdentifier() << std::endl;
      std::cerr << "New state = " << newStateIdentifier    << std::endl;
      std::cerr.flush();
      }
    else
      {
      // Finally, add the Transition: new State to assume when the specific
      // Input is received.  and the Action to be taken.
      StateActionPair newTransition( newState.GetIdentifier(), action ); 
      (*(transitionsFromThisState->second))[input.GetIdentifier()] = newTransition;
      }
    }

}









/** The syntax of the string generated by this method is
    taken from "dot" documentation available at:
    http://www.research.att.com/sw/tools/graphviz/dotguide.pdf
    If this string is saved to a file, the file will be 
    a valid input for the "dot" tool.
 */
template<class TClass>
void
StateMachine< TClass >
::ExportDescription( OutputStreamType & ostr, bool skipLoops=false ) const
{
    ostr << "digraph G {" << std::endl;

    // Export all the transitions between states
    {
    TransitionConstIterator transitionsFromThisState =
                                       m_Transitions.begin();
    while( transitionsFromThisState != m_Transitions.end() )
      {
      TransitionsPerInputConstIterator  
             transitionsFromThisStateAndInput =  
                        transitionsFromThisState->second->begin();
      while( transitionsFromThisStateAndInput != transitionsFromThisState->second->end() )
        {
        // find the label that identify the input.
        InputDescriptorType label;
        InputConstIterator inputItr = m_Inputs.find( 
                                         transitionsFromThisStateAndInput->first ); 
        if( inputItr != m_Inputs.end() )
          {
          label = inputItr->second;
          }
        if( !skipLoops ||
             transitionsFromThisState->first !=
             transitionsFromThisStateAndInput->second.GetStateIdentifier() )
          {
          ostr << transitionsFromThisState->first << " -> ";
          ostr << transitionsFromThisStateAndInput->second.GetStateIdentifier();
          ostr << " [label=\"" << label << "\"";
          ostr << " fontname=Helvetica, fontcolor=Blue";
          ostr << "];" << std::endl;
          }
        ++transitionsFromThisStateAndInput;
        }
      ++transitionsFromThisState;
      }
    }

    // Export the descriptions of all states
    {
    StatesConstIterator  stateId = m_States.begin();
    while( stateId != m_States.end() )
      {
      ostr << stateId->first << "  [label=\"";
      ostr << stateId->second << "\"";
      ostr << " fontname=Helvetica, fontcolor=Black";
      ostr << "];" << std::endl;
      ++stateId;
      }
    }
    ostr << "}" << std::endl;

}



  /** Print Self function */
template<class TClass>
void
StateMachine< TClass >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;

  os << indent << this->m_State << std::endl;
  if( this->m_This )
  {
    os << indent << "the object pointed is set:" << this->m_This << std::endl;
  }
  else
  {
    os << indent << "the object pointed is null" << std::endl;
  }

  os << indent << "ReadyToRun: " << this->m_ReadyToRun << std::endl;
  os << indent << "InitialStateSelected: " << this->m_InitialStateSelected << std::endl;
  os << indent << "Number of States: " << this->m_States.size() << std::endl;
  os << indent << "Number of Inputs: " << this->m_Inputs.size() << std::endl;
  os << indent << "Number of Transitions: " << this->m_Transitions.size() << std::endl;
}

template<class TClass>
void
StateMachine< TClass >
::Print(std::ostream& os, itk::Indent indent) const
{
os << indent << "Token" << " (" << this << ")\n";
this->PrintSelf(os, indent.GetNextIndent());
}

/** Print Self function */
template<class TClass>
std::ostream& operator<<(std::ostream& os, const StateMachine<TClass>& o)
{
  o.Print(os, 0);
  return os;
}


}


#endif

