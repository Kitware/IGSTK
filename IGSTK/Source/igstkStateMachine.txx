/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkStateMachine.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkStateMachine_txx
#define __igstkStateMachine_txx

#include "igstkStateMachine.h"
#include "igstkEvents.h"


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
   * unique. Here we create a new entry by invoking the bracket [] operator.
   * NOTE: This is the *ONLY* place where the bracket operator can be used 
   * safely;
   * in any other case we risk to create a state entry by accident. */
  m_States[ state.GetIdentifier() ] = descriptor;
}


template<class TClass>
std::string 
StateMachine< TClass >
::GetStateDescriptor( const StateIdentifierType & stateId )
{
  StatesConstIterator stateItr = m_States.find( stateId );
    
  if( stateItr != m_States.end() )
    {
    return stateItr->second;
    }
  else
    {
    std::string message = "This state has not been registered";
    return message;
    }

}


template<class TClass>
std::string 
StateMachine< TClass >
::GetInputDescriptor( const InputIdentifierType & inputId )
{
  InputConstIterator inputItr = m_Inputs.find( inputId );
    
  if( inputItr != m_Inputs.end() )
    {
    return inputItr->second;
    }
  else
    {
    std::string message = "This input has not been registered";
    return message;
    }

}


template<class TClass>
void
StateMachine< TClass >
::SelectInitialState( const StateType & initialState )
{

  if( m_ReadyToRun )
    {
    igstkLogMacroStatic( m_This, CRITICAL, "In class " 
      << m_This->GetNameOfClass() 
      << " Error: attempt to invoke SelectInitialState() " 
      << " but the machine is ready to go " 
      << " There is no reason for invoking SelectInitialState() after "
      << " SetReadyToRun(). Something should be wrong in the logic" 
      << " of your state machine programming.\n" );
    return;
    }

  StatesConstIterator  state = 
              m_States.find( initialState.GetIdentifier() );
    
  if( state == m_States.end() )
    {
    igstkLogMacroStatic( m_This, CRITICAL, "In class " 
      << m_This->GetNameOfClass() 
      << " State selected as initial state has not been defined yet. " 
      << " Attempted state = " << initialState.GetIdentifier() << "\n" );
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
    igstkLogMacroStatic( m_This, CRITICAL, "In class " 
      << m_This->GetNameOfClass() 
      << " Error: attempt to invoke SetReadyToRun() " 
      << " but the machine is ready to go " 
      << " There is no reason for invoking SetReadyToRun() twice."
      << " Something should be wrong in the logic or your " 
      << " state machine programming.\n" );
    return;
    }

  if( !m_InitialStateSelected )
    {
    igstkLogMacroStatic( m_This, CRITICAL, "In class " 
      << m_This->GetNameOfClass() 
      << " Error: No initial state has been selected. "
      << " You must select an initial State out of the " 
      << " set of states defined for the State Machine.\n" );
    return;
    }


  // Verify the completeness of the Transition table
  {

    // First, verify that the Transition Table has an entry for every State
    StatesConstIterator stateItr = m_States.begin();
    StatesConstIterator stateEnd = m_States.end();

    while( stateItr != stateEnd )
      {
      TransitionConstIterator transitionsFromThisState =
                                       m_Transitions.find( stateItr->first );

      if( transitionsFromThisState == m_Transitions.end() )
        {
        igstkLogMacroStatic( m_This, CRITICAL, "In class " 
          << m_This->GetNameOfClass() 
          << " Error: The Transition Table is Not Complete.\n"
          << " There are no transitions for \n"
          << " State:  " << stateItr->second << "\n" );
        ++stateItr;
        continue;
        }

      // Second, verify that the State row has entries for every Input.
      InputConstIterator inputItr = m_Inputs.begin();
      InputConstIterator inputEnd = m_Inputs.end();

      TransitionsPerInputConstIterator  transitionEnd = 
                        transitionsFromThisState->second->end();

      while( inputItr != inputEnd )
        {
        TransitionsPerInputConstIterator  transitionItr = 
             transitionsFromThisState->second->find( inputItr->first );

        if( transitionItr == transitionEnd )
          { 
          igstkLogMacroStatic( m_This, CRITICAL, "In class " 
            << m_This->GetNameOfClass() 
            << " Error: The Transition Table is Not Complete.\n"
            << " There is no transition for \n"
            << " State:  " << stateItr->second 
            << " Input:  " << inputItr->second << "\n" );
          }
        ++inputItr;
        }

      ++stateItr;
      }

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
::PushInputBoolean( bool condition, const InputType & inputIfTrue,
                    const InputType & inputIfFalse )
{
  const InputType *input = & inputIfFalse;

  if ( condition )
    {
    input = & inputIfTrue;
    }

  m_QueuedInputs.push( input->GetIdentifier() );
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
    // WARNING: It is very important to do the pop() before invoking 
    // ProcessInput() otherwise the inputs will accumulate in the queue.
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
    igstkLogMacroStatic( m_This, CRITICAL, "In class " 
      << m_This->GetNameOfClass() 
      << " Error: attempt to invoke a StateTransition " 
      << " but the machine is not ready to go" 
      << " You must invoke the method SetReadyToRun() first.\n" );
    return;
    }

  TransitionConstIterator transitionsFromThisState = 
                                 m_Transitions.find( m_State );

  
  if( transitionsFromThisState == m_Transitions.end() )
    {
    igstkLogMacroStatic( m_This, CRITICAL, "In class " 
      << m_This->GetNameOfClass() 
      << " No transitions have been defined for current state "
      << " State = " << m_State
      << " [" << this->GetStateDescriptor( m_State ) << "]\n" );
    return;
    } 

  TransitionsPerInputConstIterator  transitionItr = 
            transitionsFromThisState->second->find( inputIdentifier );

  if( transitionItr == transitionsFromThisState->second->end() )
    {
    igstkLogMacroStatic( m_This, CRITICAL, "In class " 
      << m_This->GetNameOfClass() 
      << " No transitions have been defined for current state and input " 
      << " State = " << m_State
      << " [" << this->GetStateDescriptor( m_State ) << "]" 
      << " Input = " << inputIdentifier 
      << " [" << this->GetInputDescriptor( inputIdentifier ) << "]\n" );
    return;
    } 

  StateActionPair transition = transitionItr->second;

  const StateIdentifierType previousState = m_State;

  // set the new state
  m_State = transition.GetStateIdentifier();
  
  const StateIdentifierType nextState = m_State;
  
  igstkLogMacroStatic( m_This, DEBUG, "State transition is being made : " 
    << m_This->GetNameOfClass() << " "
    << " PointerID " << m_This << " "
    << this->GetStateDescriptor( previousState ) << "(" << previousState << ") "
    << " with " << this->GetInputDescriptor( inputIdentifier ) 
    << "(" << inputIdentifier << ") ---> "
    << this->GetStateDescriptor( nextState ) << "(" << nextState << ").\n" );

  // call the transition function
  if( transition.GetAction() )
    {
    ((*m_This).*(transition.GetAction()))();
    }
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
    igstkLogMacroStatic( m_This, CRITICAL, "In class " 
      << m_This->GetNameOfClass() 
      << " Attempt to add a Transition for a State that does not exist" 
      << " Attempted state     = " << state.GetIdentifier()
      << " [" << this->GetStateDescriptor( state.GetIdentifier() ) << "]" 
      << " Attempted input     = " << input.GetIdentifier()
      << " [" << this->GetInputDescriptor( input.GetIdentifier() ) << "]"
      << " Attempted new state = " << newState.GetIdentifier()
      << " [" << this->GetStateDescriptor( newState.GetIdentifier() ) 
      << "] \n" );
    return;
    } 

  // Then check if the Input exists
  InputConstIterator inputItr = m_Inputs.find( input.GetIdentifier() );
    
  if( inputItr == m_Inputs.end() )
    {
    igstkLogMacroStatic( m_This, CRITICAL, "In class " 
      << m_This->GetNameOfClass() 
      << " Attempt to add a Transition for an Input that does not exist" 
      << " Attempted state     = " << state.GetIdentifier()
      << " [" << this->GetStateDescriptor( state.GetIdentifier() ) << "]" 
      << " Attempted input     = " << input.GetIdentifier()
      << " [" << this->GetInputDescriptor( input.GetIdentifier() ) << "]" 
      << " Attempted new state = " << newState.GetIdentifier()
      << " [" << this->GetStateDescriptor( newState.GetIdentifier() ) 
      << "] \n" );
    return;
    } 


  // Check if the new State exists
  StatesConstIterator newstateItr = m_States.find( newState.GetIdentifier() );

  if( newstateItr == m_States.end() )
    {
    igstkLogMacroStatic( m_This, CRITICAL, "In class " 
      << m_This->GetNameOfClass() 
      << " Attempt to add a Transition for a New State that does not exist" 
      << " Attempted state     = " << state.GetIdentifier()
      << " [" << this->GetStateDescriptor( state.GetIdentifier() ) << "]" 
      << " Attempted input     = " << input.GetIdentifier()
      << " [" << this->GetInputDescriptor( input.GetIdentifier() ) << "]" 
      << " Attempted new state = " << newState.GetIdentifier()
      << " [" << this->GetStateDescriptor( newState.GetIdentifier() ) 
      << "] \n" );
    return;
    } 


  // Search for existing Transitions for that State
  TransitionConstIterator transitionsFromThisState = 
                               m_Transitions.find( state.GetIdentifier() );
  
  if( transitionsFromThisState == m_Transitions.end() )
    {
    // No transition has been created for this particular state.
    // Therefore create a new entry for it.
    TransitionsPerInputContainer *transitionsPerInput = 
                                             new TransitionsPerInputContainer;

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
               transitionsFromThisState->second->find(input.GetIdentifier() );
    if( transitionsFromThisStateAndInput != 
                                     transitionsFromThisState->second->end() )
      {
      // There is already an entry for this input. This is suspicious
      // because the user may be overriding a previous transition by
      // accident. 
      StateIdentifierType newStateIdentifier = 
               transitionsFromThisStateAndInput->second.GetStateIdentifier();
      igstkLogMacroStatic( m_This, CRITICAL, "In class " 
        << m_This->GetNameOfClass() 
        << " Attempt to override an existing transition. "
        << " Please verify the programming of your state machine. "
        << " There is already a transition defined for the combination: " 
        << " State     = " << state.GetIdentifier()
        << " [" << this->GetStateDescriptor( state.GetIdentifier() ) << "] " 
        << " Input     = " << input.GetIdentifier()
        << " [" << this->GetInputDescriptor( input.GetIdentifier() ) << "] "
        << " New state = " << newStateIdentifier
        << " [" << this->GetStateDescriptor( newStateIdentifier ) << "] \n" );
        return;
      }
    else
      {
      // Finally, add the Transition: new State to assume when the specific
      // Input is received.  and the Action to be taken.
      StateActionPair newTransition( newState.GetIdentifier(), action ); 
      (*(transitionsFromThisState->second))[input.GetIdentifier()] 
                                                              = newTransition;
      }
    }
}


/** The syntax of the string generated by this method is
 *   taken from "dot" documentation available at:
 *   http://www.research.att.com/sw/tools/graphviz/dotguide.pdf
 *   If this string is saved to a file, the file will be 
 *   a valid input for the "dot" tool. */
template<class TClass>
void
StateMachine< TClass >
::ExportDescription( OutputStreamType & ostr, bool skipLoops=false ) const
{
  ostr << "digraph G {" << std::endl;

  // Export all the transitions between states
  TransitionConstIterator transitionsFromThisState =
                                       m_Transitions.begin();
  while( transitionsFromThisState != m_Transitions.end() )
    {
    TransitionsPerInputConstIterator transitionsFromThisStateAndInput =  
                                    transitionsFromThisState->second->begin();
    while( transitionsFromThisStateAndInput != 
                                     transitionsFromThisState->second->end() )
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
 
  // Export the descriptions of all states
  StatesConstIterator  stateId = m_States.begin();
  while( stateId != m_States.end() )
    {
    ostr << stateId->first << "  [label=\"";
    ostr << stateId->second << "\"";
    ostr << " fontname=Helvetica, fontcolor=Black";
    ostr << "];" << std::endl;
    ++stateId;
    }
  ostr << "}" << std::endl;
}


template<class TClass>
void
StateMachine< TClass >
::ExportDescriptionToLTS( OutputStreamType & ostr, bool skipLoops=false ) const
{
  // Export all the transitions between states
  int out_count = 0;
  int in_count = 0;
  int in_flag = 0;
  int out_flag = 0;
   
  StatesConstIterator  stateId;
  
  TransitionConstIterator transitionsFromThisState = m_Transitions.begin();
                                     
  while( transitionsFromThisState != m_Transitions.end() )
    {
    out_count++;
    ++transitionsFromThisState;
    }
    
  transitionsFromThisState = m_Transitions.begin();
                                     
  while( transitionsFromThisState != m_Transitions.end() )
    {
    TransitionsPerInputConstIterator  transitionsFromThisStateAndInput =  
                                    transitionsFromThisState->second->begin();
    in_count = 0;
                      
    while( transitionsFromThisStateAndInput != 
           transitionsFromThisState->second->end() )
      {
      in_count ++;
      ++transitionsFromThisStateAndInput;
      }
      
    transitionsFromThisStateAndInput = 
                                    transitionsFromThisState->second->begin();

    in_flag = 1;

    while( transitionsFromThisStateAndInput 
           != transitionsFromThisState->second->end() )
      {
      // find the label that identify the input.
      InputDescriptorType label;
      InputConstIterator inputItr = 
                     m_Inputs.find( transitionsFromThisStateAndInput->first ); 
      if( inputItr != m_Inputs.end() )
        {
        label = inputItr->second;
        }
      if( !skipLoops ||
           transitionsFromThisState->first !=
           transitionsFromThisStateAndInput->second.GetStateIdentifier() )
        {
        if (in_flag ==1)
          {
          stateId = m_States.begin();
          while( stateId != m_States.end() )
            {
            if(stateId->first == transitionsFromThisState->first)
              {
              ostr << stateId->second << " = ( ";
              }

            ++stateId;
            }
          }

        // LTSA requires the descriptor to start with a lowercase
        label[0] = tolower( label[0] );

        ostr << label << " -> ";
        
        stateId = m_States.begin();
        while( stateId != m_States.end() )
          {
          if(stateId->first == 
             transitionsFromThisStateAndInput->second.GetStateIdentifier())
            {
            ostr << stateId->second;
            }
          ++stateId;
          }

        if(in_flag<in_count)
          {
          ostr << "|" << std::endl;
          in_flag ++;
          }
        else
          {
          ostr << ")";
          }
        }
      ++transitionsFromThisStateAndInput;
      }  // in-while
      
      out_flag++;
      if( out_flag < out_count )
        {
        ostr << "," << std::endl;
        }
      else
        {
        ostr << "." << std::endl;
        }

      ++transitionsFromThisState;
      } //out-while 
}


template<class TClass>
void
StateMachine< TClass >
::ExportDescriptionToSCXML( OutputStreamType & ostr, bool skipLoops ) const
{   
  // Export the descriptions of all states
  StatesConstIterator  stateId = m_States.begin();

  //write scxml header
  ostr << "<?xml version=\"1.0\" encoding=\"us-ascii\"?>" << std::endl;
  ostr << "<scxml version=\"1.0\"";
  // The following XML namespace should work but is giving the parser problems
  //  xmlns="\"http://www.w3.org/2005/07/scxml\"";
  ostr << std::endl;

  if( stateId != m_States.end() )
    {
    ostr << "initialstate=\""<< stateId->second << "\"" << std::endl;
    }
  ostr << ">" << std::endl;

  //end of scxml header

  //write scxml body
  while( stateId != m_States.end() )
    {
    //write a state 

    ostr << "  <state id=";
    ostr << "\"" << stateId->second <<"\">" << std::endl;

    // Search for existing Transitions for that State
    TransitionConstIterator transitionsFromThisState = m_Transitions.begin();

    while( transitionsFromThisState != m_Transitions.end() )
      {
      if( stateId->first == transitionsFromThisState->first )
        {
        TransitionsPerInputConstIterator transitionsFromThisStateAndInput =  
                                    transitionsFromThisState->second->begin();

        while( transitionsFromThisStateAndInput != 
                                     transitionsFromThisState->second->end() )
          {
          // find the label that identifies the input.
          InputDescriptorType label;

          InputConstIterator inputItr = m_Inputs.find( 
                         transitionsFromThisStateAndInput->first ); 
          if( inputItr != m_Inputs.end() )
            {
            label = inputItr->second;
            }

          // LTSA requires the descriptor to start with a lowercase
          label[0] = tolower( label[0] );

          if( !skipLoops || transitionsFromThisState->first !=
               transitionsFromThisStateAndInput->second.GetStateIdentifier() )
            {
            StateIdentifierType newStateIdentifier = 
               transitionsFromThisStateAndInput->second.GetStateIdentifier();

            ostr << "    <transition event=";
            ostr << "\"" << label << "\">" << std::endl;
            ostr << "      <target next=";

            //get next state
            StatesConstIterator stateItr = m_States.find(newStateIdentifier);

            if( stateItr != m_States.end() )
              {
              ostr << "\"" << stateItr->second;
              }

            ostr << "\"/>" << std::endl;
            ostr << "    </transition>" << std::endl;
            }

          ++transitionsFromThisStateAndInput;

          }
        }  

      ++transitionsFromThisState;
      }

    ++stateId;
    ostr << "  </state>" << std::endl;

    }
 
  //write end tag of scxml file
  ostr <<"</scxml>"<< std::endl;
}


/** Print Self function */
template<class TClass>
void
StateMachine< TClass >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;

  os << indent << this->m_State << std::endl;
  os << indent << "the object pointed is set:" << this->m_This << std::endl;
  os << indent << " and has class name = " 
     << m_This->GetNameOfClass() << std::endl;
  os << indent << "ReadyToRun: " << this->m_ReadyToRun << std::endl;
  os << indent << "InitialStateSelected: " 
     << this->m_InitialStateSelected << std::endl;
  os << indent << "Number of States: " << this->m_States.size() << std::endl;
  os << indent << "Number of Inputs: " << this->m_Inputs.size() << std::endl;
  os << indent << "Number of Transitions: " 
     << this->m_Transitions.size() << std::endl;
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
