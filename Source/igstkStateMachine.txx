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
::StateMachine()
{

  m_This = 0;


  m_ReadyToRun = false;

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

  // Release memory from Actions container
  ActionIterator actionsItr = m_Actions.begin();
  ActionIterator actionsEnd = m_Actions.end();
  while( actionsItr != actionsEnd )
    {
    delete actionsItr->second;
    ++actionsItr;
    }
  m_Actions.clear();

}





template<class TClass>
void
StateMachine< TClass >
::SetOwnerClass( TClass * theclass )
{
   m_This = theclass;
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
const typename StateMachine< TClass >::StateIdentifierType & 
StateMachine< TClass >
::GetCurrentStateIdentifier() const
{

   return m_State;

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
    std::cerr << "State selected as initial state has not been defined yet. " << std::endl;
    std::cerr << "Attempted state = " << initialState.GetIdentifier() << std::endl;
    std::cerr.flush();
    return;
    } 

  m_State = state->first;

}




template<class TClass>
void
StateMachine< TClass >
::SetReadyToRun()
{

  if( !m_This )
    {
    std::cerr << "Error: attempt to invoke a SetReadyToRun " << std::endl;
    std::cerr << "but the m_This pointer is null. " << std::endl;
    std::cerr << "The method SetClass() must be invoked with a valid pointer " << std::endl;
    std::cerr << "before attempting to run the machine." << std::endl;
    std::cerr.flush();
    return;
    }


  if( m_ReadyToRun )
    {
    std::cerr << "Error: attempt to invoke SetReadyToRun() " << std::endl;
    std::cerr << "but the machine is ready to go " << std::endl;
    std::cerr << "There is no reason for invoking SetReadyToRun() twice." << std::endl;
    std::cerr << "Something should be wrong in the logic or your " << std::endl;
    std::cerr << "state machine programming." << std::endl;
    std::cerr.flush();
    return;
    }

  m_ReadyToRun = true;
}




template<class TClass>
void
StateMachine< TClass >
::ProcessInput( const InputType & input )
{

  if( !m_This )
    {
    std::cerr << "Error: attempt to invoke StateTransition() " << std::endl;
    std::cerr << "but the m_This pointer is null. " << std::endl;
    std::cerr << "The method SetClass() must be invoked with a valid pointer " << std::endl;
    std::cerr << "before attempting to run the machine." << std::endl;
    std::cerr.flush();
    return;
    }

  if( !m_ReadyToRun )
    {
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
    std::cerr << "No transitions have been defined for current state = " << m_State << std::endl;
    std::cerr.flush();
    return;
    } 

  StatesPerInputConstIterator  newState = 
            transitionsFromThisState->second->find( input.GetIdentifier() );

  if( newState == transitionsFromThisState->second->end() )
    {
    std::cerr << "No transitions have been defined for current state and input " << std::endl;
    std::cerr << "State = "  << m_State << std::endl;
    std::cerr << "Input = "  << input.GetIdentifier() << std::endl;
    std::cerr << std::endl;
    std::cerr.flush();
    return;
    } 

  ActionConstIterator  actionsFromThisState = 
                                                 m_Actions.find( m_State );

  
  if( actionsFromThisState == m_Actions.end() )
    {
    std::cerr << "No actions have been defined for current state = " << m_State << std::endl;
    std::cerr.flush();
    return;
    } 

  ActionsPerInputConstIterator  action = actionsFromThisState->second->find( input.GetIdentifier() );

  if( action == actionsFromThisState->second->end() )
    {
    std::cerr << "No actions have been defined for current state and input " << std::endl;
    std::cerr << "State = "  << m_State << std::endl;
    std::cerr << "Input = "  << input.GetIdentifier() << std::endl;
    std::cerr << "this state has " << actionsFromThisState->second->size() << " action transitions " << std::endl;
    std::cerr << std::endl;
    std::cerr.flush();
    return;
    } 

    

  if( action->second )
    {
    ((*m_This).*(action->second))();
    }

  m_State = newState->second;

}




template<class TClass>
void
StateMachine< TClass >
::AddTransition( const StateType & state,   
                 const InputType & input, 
                 const StateType & newState, 
                       TMemberFunctionPointer action )
{
 
  // First check if the State exists
  StatesConstIterator stateItr = m_States.find( state.GetIdentifier() );
    
  if( stateItr == m_States.end() )
    {
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
    StatesPerInputContainer *statesPerInput = new StatesPerInputContainer;

    // Insert the new state that should be assumed if the input is received.
    (*statesPerInput)[ input.GetIdentifier() ] = newState.GetIdentifier();

    // Add the statesPerInput container to the Transitions container.
    m_Transitions[ state.GetIdentifier() ] = statesPerInput;
    } 
  else
    {
    // Check if the particular Input has already an Entry here
    StatesPerInputConstIterator transitionsFromThisStateAndInput =
                     transitionsFromThisState->second->find( input.GetIdentifier() );
    if( transitionsFromThisStateAndInput != transitionsFromThisState->second->end() )
      {
      // There is already an entry for this input. This is suspicious
      // because the user may be overriding a previous transition by
      // accident. 
      std::cerr << "Attempt to override an existing transition. "
                << "Please verify the programming of your state machine. "
                << "There is already a transition defined for the combination: " << std::endl;
      std::cerr << "State     = " << state.GetIdentifier() << std::endl;
      std::cerr << "Input     = " << input.GetIdentifier() << std::endl;
      std::cerr << "New state = " << transitionsFromThisStateAndInput->second << std::endl;
      std::cerr.flush();
      }
    else
      {
      // Finally, add the Transition: new State to assume when the specific Input is received.
      (*(transitionsFromThisState->second))[input.GetIdentifier()] = newState.GetIdentifier();
      }
    }


  // Now do the equivalent for the Action

  // Search for existing Actions for that State
  ActionConstIterator  actionsFromThisState = 
                            m_Actions.find( state.GetIdentifier() );
  
  if( actionsFromThisState == m_Actions.end() )
    {
    // No action has been created for this particular state.
    // Therefore create a new entry for it.
    ActionsPerInputContainer *statesPerInput = new ActionsPerInputContainer;

    // Insert the new state that should be assumed if the input is received.
    (*statesPerInput)[ input.GetIdentifier() ] = action;

    // Add the statesPerInput container to the Actions container.
    m_Actions[ state.GetIdentifier() ] = statesPerInput;
    } 
  else
    {
    // Check if the particular Input has already an Entry here
      ActionsPerInputConstIterator actionsFromThisStateAndInput =
                            actionsFromThisState->second->find( input.GetIdentifier() );
    if( actionsFromThisStateAndInput != actionsFromThisState->second->end() )
      {
      // There is already an entry for this input. This is suspicious
      // because the user may be overriding a previous action by
      // accident. 
      std::cerr << "Attempt to override an existing action. "
                << "Please verify the programming of your state machine. "
                << "There is already a action defined for the combination: " << std::endl;
      std::cerr << "State     = " << state.GetIdentifier() << std::endl;
      std::cerr << "Input     = " << input.GetIdentifier() << std::endl;
      // Commenting out the following line: Not compiling on bcc32 
      //std::cerr << "New state = " << actionsFromThisStateAndInput->second << std::endl;
      std::cerr.flush();
      }
    else
      {
      // Finally, add the Action: new State to assume when the specific Input is received.
      (*(actionsFromThisState->second))[ input.GetIdentifier() ] = action;
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
::ExportTransitions( stdOstreamType & ostr ) const
{
    ostr << "digraph G {" << std::endl;

    // Export all the transitions between states
    {
    TransitionConstIterator transitionsFromThisState =
                                       m_Transitions.begin();
    while( transitionsFromThisState != m_Transitions.end() )
      {
      StatesPerInputConstIterator  
             transitionsFromThisStateAndInput =  
                        transitionsFromThisState->second->begin();
      while( transitionsFromThisStateAndInput != transitionsFromThisState->second->end() )
        {
        ostr << transitionsFromThisState->first << " -> ";
        ostr << transitionsFromThisStateAndInput->second;
        ostr << " [label=\"" << transitionsFromThisStateAndInput->first << "\"";
        ostr << " fontname=Helvetica, fontcolor=Blue";
        ostr << "];" << std::endl;
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
      ostr << stateId->second << "  [label=\"";
      ostr << stateId->second << "\"";
      ostr << " fontname=Helvetica, fontcolor=Black";
      ostr << "];" << std::endl;
      ++stateId;
      }
    }
    ostr << "}" << std::endl;
}



}


#endif

