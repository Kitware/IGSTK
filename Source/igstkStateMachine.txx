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
    return;
    }


  if( m_ReadyToRun )
    {
    std::cerr << "Error: attempt to invoke SetReadyToRun() " << std::endl;
    std::cerr << "but the machine is ready to go " << std::endl;
    std::cerr << "There is no reason for invoking SetReadyToRun() twice." << std::endl;
    std::cerr << "Something should be wrong in the logic or your " << std::endl;
    std::cerr << "state machine programming." << std::endl;
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
    return;
    }

  if( !m_ReadyToRun )
    {
    std::cerr << "Error: attempt to invoke a StateTransition " << std::endl;
    std::cerr << "but the machine is not ready to go" << std::endl;
    std::cerr << "You must invoke the method SetReadyToRun() first." << std::endl;
    return;
    }

  TransitionContainer::const_iterator    transitionsFromThisState = m_Transitions.find( m_State );

  
  if( transitionsFromThisState == m_Transitions.end() )
    {
    std::cerr << "No transitions have been defined for curren state = " << m_State << std::endl;
    return;
    } 

  StatesPerInputContainer::const_iterator  newState = transitionsFromThisState->second->find( input );

  if( newState == transitionsFromThisState->second->end() )
    {
    std::cerr << "No transitions have been defined for curren state and input " << std::endl;
    std::cerr << "State = "  << m_State << std::endl;
    std::cerr << "Input = "  << input   << std::endl;
    return;
    } 

  typename ActionContainer::const_iterator    actionsFromThisState = m_Actions.find( m_State );

  
  if( actionsFromThisState == m_Actions.end() )
    {
    std::cerr << "No actions have been defined for curren state = " << m_State << std::endl;
    return;
    } 

  typename ActionsPerInputContainer::const_iterator  action = actionsFromThisState->second->find( input );

  if( action == actionsFromThisState->second->end() )
    {
    std::cerr << "No actions have been defined for curren state and input " << std::endl;
    std::cerr << "State = "  << m_State << std::endl;
    std::cerr << "Input = "  << input   << std::endl;
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
                 const StateDescriptorType & newstateDescriptor, 
                       TMemberFunctionPointer action )
{

    
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
        ostr << "  State" << transitionsFromThisStateAndInput->first << " -> State";
        ostr << transitionsFromThisStateAndInput->first;
        ostr << " [label=\"" << transitionsFromThisStateAndInput->second << "\"";
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
      ostr << "  State" << *stateId << "  [label=\"";
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

