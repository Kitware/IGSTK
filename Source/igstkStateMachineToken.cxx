/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkStateMachineToken.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkStateMachineToken.h"

#include <iostream>


namespace igstk
{



  StateMachineToken::IdentifierType StateMachineToken::IdentifierCounter = 1;



  StateMachineToken::StateMachineToken()
  {
    this->m_Identifier  = IdentifierCounter++;

    if( IdentifierCounter == 0 )
      {
      std::cerr << "Maximum number of token identifiers has been reached." << std::endl;
      std::cerr << "Uniqueness of identifiers can no longer be guaranted." << std::endl;
      }
  }


  StateMachineToken::~StateMachineToken()
  {
  }


} // end namespace igstk



