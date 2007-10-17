/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkStateMachineInput.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkStateMachineInput_h
#define __igstkStateMachineInput_h

#include "igstkToken.h"


namespace igstk
{

/** \class StateMachineInput
 *  \brief Generic implementation of the Input in a State Machine model.
 *
 *  This class provides a generic representation of a Input.  It is  intended
 *  to be derived in order to implement specific states an to enforce the
 *  following characteristics on the behavior of the State Machine:
 *
 *  \li Preclude the definition of two states with the same name.
 *  \li Provide high performance search for the presence of a particular state.
 *  \li Provide descriptive text-like definition of each state.
 *
 *  These characteristics are enforced by taking advantage of natural checks
 *  performed by the compiler. In particular, the uniqueness of the Input name
 *  will be enforced by making every state to be an C++ variable. The compiler
 *  will produce errors as a result of any attempt to repeat the name of a
 *  variable inside the same namespace.
 *
 *   
 *  \sa StateMachine
 *  \sa StateMachineState
 *  \sa Token
 *  \sa StateMachineAction
 */
template< class T >
class StateMachineInput : public Token
{

public:

  typedef StateMachineInput Self;
  typedef Token             Superclass;

  /** Constructor. It initializes all the member variables */
  StateMachineInput() { }

  /** Destructor */
  virtual ~StateMachineInput() {}

protected:
  
  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const
    {
    Superclass::PrintSelf(os, indent);
    }

};

} // end namespace igstk

#endif
