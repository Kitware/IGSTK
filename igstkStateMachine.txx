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




template<class TClass, unsigned int VStates, unsigned int VInputs>
StateMachine< TClass, VStates, VInputs >
::StateMachine()
{

  m_This = 0;


  // Set all the transitions to the origin state
  for( InputType input=0; input < VInputs; input++)
    {
    for(StateType state=0; state < VStates; state++)
      {
      m_Transition[ state ][ input ] = 0;
      }
    }

  m_ReadyToRun = false;

}




template<class TClass, unsigned int VStates, unsigned int VInputs>
StateMachine< TClass, VStates, VInputs >
::~StateMachine()
{

}





template<class TClass, unsigned int VStates, unsigned int VInputs>
void
StateMachine< TClass, VStates, VInputs >
::SetClass( TClass * theclass )
{
   m_This = theclass;
}




template<class TClass, unsigned int VStates, unsigned int VInputs>
void
StateMachine< TClass, VStates, VInputs >
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




template<class TClass, unsigned int VStates, unsigned int VInputs>
void
StateMachine< TClass, VStates, VInputs >
::StateTransition()
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

  if( m_State >= VStates )
    {
    std::cerr << "m_State code is out of range. " << std::endl;
    std::cerr << "It should be between 0 and " << VStates  << std::endl;
    std::cerr << "but its value is " << m_State << std::endl;
    return;
    }


  if( m_Input >= VInputs )
    {
    std::cerr << "Input code is out of range. " << std::endl;
    std::cerr << "It should be between 0 and " << VInputs  << std::endl;
    std::cerr << "but its value is " << m_Input << std::endl;
    return;
    }

  StateType newstate = m_Transition[ m_State ][ m_Input ];


  TMemberFunctionPointer action = m_Action[ m_State ][ m_Input ];

  if( action )
    {
    ((*m_This).*(action))();
    }

  m_State = newstate;

}









template<class TClass, unsigned int VStates, unsigned int VInputs>
void
StateMachine< TClass, VStates, VInputs >
::SetTransition( StateType state,    unsigned int input, 
                 StateType newstate, TMemberFunctionPointer action )
{

  if( m_ReadyToRun )
    {
    std::cerr << "Error: attempt to invoke a SetTransition " << std::endl;
    std::cerr << "but the machine is ready to go " << std::endl;
    std::cerr << "All invokations to SetTransition() must be done " << std::endl;
    std::cerr << "before calling SetReadyToRun()." << std::endl;
    return;
    }


  if( state >= VStates )
    {
    std::cerr << "State code is out of range. " << std::endl;
    std::cerr << "It should be between 0 and " << VStates  << std::endl;
    std::cerr << "but its value is " << state << std::endl;
    return;
    }


  if( input >= VInputs )
    {
    std::cerr << "Input code is out of range. " << std::endl;
    std::cerr << "It should be between 0 and " << VInputs  << std::endl;
    std::cerr << "but its value is " << input << std::endl;
    return;
    }


  if( newstate >= VStates )
    {
    std::cerr << "New State code is out of range. " << std::endl;
    std::cerr << "It should be between 0 and " << VStates  << std::endl;
    std::cerr << "but its value is " << newstate << std::endl;
    return;
    }


  m_Transition[ state ][ input ] = newstate;

  m_Action[ state ][ input ] = action;

}

 


}


#endif

