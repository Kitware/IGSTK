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
#include <algorithm>


namespace igstk
{




template<class TClass>
StateMachine< TClass >
::StateMachine()
{

  m_This = 0;


  m_ReadyToRun = false;

  // Initialize the machine in the idle state.
  m_State = "";

}




template<class TClass>
StateMachine< TClass >
::~StateMachine()
{

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
::AddInput( const InputDescriptorType & descriptor )
{

   m_Inputs.push_back( descriptor );

}



template<class TClass>
const typename StateMachine< TClass >::StateDescriptorType & 
StateMachine< TClass >
::GetCurrentState() const
{

   return m_State;

}




template<class TClass>
void
StateMachine< TClass >
::AddState( const StateDescriptorType & descriptor )
{

   m_States.push_back( descriptor );

}



template<class TClass>
void
StateMachine< TClass >
::SelectInitialState( const StateDescriptorType & descriptor )
{

  StatesContainer::const_iterator  state = std::find( m_States.begin(), m_States.end(), descriptor );
    
  if( state == m_States.end() )
    {
    std::cerr << "State selected as initial state has not been defined yet. " << std::endl;
    std::cerr << "Attempted state = " << descriptor << std::endl;
    std::cerr.flush();
    return;
    } 

  m_State = *state;

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
::ProcessInput( const InputDescriptorType & input )
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

  TransitionContainer::const_iterator    transitionsFromThisState = m_Transitions.find( m_State );

  
  if( transitionsFromThisState == m_Transitions.end() )
    {
    std::cerr << "No transitions have been defined for current state = " << m_State << std::endl;
    std::cerr.flush();
    return;
    } 

  StatesPerInputContainer::const_iterator  newState = transitionsFromThisState->second->find( input );

  if( newState == transitionsFromThisState->second->end() )
    {
    std::cerr << "No transitions have been defined for current state and input " << std::endl;
    std::cerr << "State = "  << m_State << std::endl;
    std::cerr << "Input = "  << input   << std::endl;
    std::cerr << std::endl;
    std::cerr.flush();
    return;
    } 

  typename ActionContainer::const_iterator    actionsFromThisState = m_Actions.find( m_State );

  
  if( actionsFromThisState == m_Actions.end() )
    {
    std::cerr << "No actions have been defined for current state = " << m_State << std::endl;
    std::cerr.flush();
    return;
    } 

  typename ActionsPerInputContainer::const_iterator  action = actionsFromThisState->second->find( input );

  if( action == actionsFromThisState->second->end() )
    {
    std::cerr << "No actions have been defined for current state and input " << std::endl;
    std::cerr << "State = "  << m_State << std::endl;
    std::cerr << "Input = "  << input   << std::endl;
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
::AddTransition( const StateDescriptorType & stateDescriptor,   
                 const InputDescriptorType & inputDescriptor, 
                 const StateDescriptorType & newStateDescriptor, 
                       TMemberFunctionPointer action )
{
 
  // First check if the State exists
  StatesContainer::const_iterator  state = std::find( m_States.begin(), m_States.end(), stateDescriptor );
    
  if( state == m_States.end() )
    {
    std::cerr << "Attempt to add a Transition for a State that does not exist" << std::endl;
    std::cerr << "Attempted state     = " << stateDescriptor << std::endl;
    std::cerr << "Attempted input     = " << inputDescriptor << std::endl;
    std::cerr << "Attempted new state = " << newStateDescriptor << std::endl;
    std::cerr.flush();
    return;
    } 

  // Then check if the Input exists
  InputsContainer::const_iterator  input = std::find( m_Inputs.begin(), m_Inputs.end(), inputDescriptor );
    
  if( input == m_Inputs.end() )
    {
    std::cerr << "Attempt to add a Transition for an Input that does not exist" << std::endl;
    std::cerr << "Attempted state     = " << stateDescriptor << std::endl;
    std::cerr << "Attempted input     = " << inputDescriptor << std::endl;
    std::cerr << "Attempted new state = " << newStateDescriptor << std::endl;
    std::cerr.flush();
    return;
    } 


  // Check if the new State exists
  StatesContainer::const_iterator  newstate = std::find( m_States.begin(), m_States.end(), newStateDescriptor );
    
  if( newstate == m_States.end() )
    {
    std::cerr << "Attempt to add a Transition for a New State that does not exist" << std::endl;
    std::cerr << "Attempted state     = " << stateDescriptor << std::endl;
    std::cerr << "Attempted input     = " << inputDescriptor << std::endl;
    std::cerr << "Attempted new state = " << newStateDescriptor << std::endl;
    std::cerr.flush();
    return;
    } 


  // Search for existing Transitions for that State
  TransitionContainer::const_iterator    transitionsFromThisState = m_Transitions.find( stateDescriptor );
  
  if( transitionsFromThisState == m_Transitions.end() )
    {
    // No transition has been created for this particular state.
    // Therefore create a new entry for it.
    StatesPerInputContainer *statesPerInput = new StatesPerInputContainer;

    // Insert the new state that should be assumed if the inputDescriptor is received.
    (*statesPerInput)[ inputDescriptor ] = newStateDescriptor;

    // Add the statesPerInput container to the Transitions container.
    m_Transitions[ stateDescriptor ] = statesPerInput;
    } 
  else
    {
    // Check if the particular Input has already an Entry here
    StatesPerInputContainer::const_iterator transitionsFromThisStateAndInput =
                            transitionsFromThisState->second->find( inputDescriptor );
    if( transitionsFromThisStateAndInput != transitionsFromThisState->second->end() )
      {
      // There is already an entry for this input. This is suspicious
      // because the user may be overriding a previous transition by
      // accident. 
      std::cerr << "Attempt to override an existing transition. "
                << "Please verify the programming of your state machine. "
                << "There is already a transition defined for the combination: " << std::endl;
      std::cerr << "State     = " << stateDescriptor << std::endl;
      std::cerr << "Input     = " << inputDescriptor << std::endl;
      std::cerr << "New state = " << transitionsFromThisStateAndInput->second << std::endl;
      std::cerr.flush();
      }
    else
      {
      // Finally, add the Transition: new State to assume when the specific Input is received.
      (*(transitionsFromThisState->second))[inputDescriptor] = newStateDescriptor;
      }
    }


  // Now do the equivalent for the Action

  // Search for existing Actions for that State
  typename ActionContainer::const_iterator  actionsFromThisState = m_Actions.find( stateDescriptor );
  
  if( actionsFromThisState == m_Actions.end() )
    {
    // No action has been created for this particular state.
    // Therefore create a new entry for it.
    ActionsPerInputContainer *statesPerInput = new ActionsPerInputContainer;

    // Insert the new state that should be assumed if the inputDescriptor is received.
    (*statesPerInput)[ inputDescriptor ] = action;

    // Add the statesPerInput container to the Actions container.
    m_Actions[ stateDescriptor ] = statesPerInput;
    } 
  else
    {
    // Check if the particular Input has already an Entry here
    typename ActionsPerInputContainer::const_iterator actionsFromThisStateAndInput =
                            actionsFromThisState->second->find( inputDescriptor );
    if( actionsFromThisStateAndInput != actionsFromThisState->second->end() )
      {
      // There is already an entry for this input. This is suspicious
      // because the user may be overriding a previous action by
      // accident. 
      std::cerr << "Attempt to override an existing action. "
                << "Please verify the programming of your state machine. "
                << "There is already a action defined for the combination: " << std::endl;
      std::cerr << "State     = " << stateDescriptor << std::endl;
      std::cerr << "Input     = " << inputDescriptor << std::endl;
      std::cerr << "New state = " << actionsFromThisStateAndInput->second << std::endl;
      std::cerr.flush();
      }
    else
      {
      // Finally, add the Action: new State to assume when the specific Input is received.
      (*(actionsFromThisState->second))[inputDescriptor] = action;
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
    TransitionContainer::const_iterator    transitionsFromThisState = m_Transitions.begin();
    while( transitionsFromThisState != m_Transitions.end() )
      {
      StatesPerInputContainer::const_iterator  
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
    StatesContainer::const_iterator  stateId = m_States.begin();
    while( stateId != m_States.end() )
      {
      ostr << *stateId << "  [label=\"";
      ostr << *stateId << "\"";
      ostr << " fontname=Helvetica, fontcolor=Black";
      ostr << "];" << std::endl;
      ++stateId;
      }
    }
    ostr << "}" << std::endl;
}



}


#endif

