/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToken.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkToken.h"
#include "itkFastMutexLock.h"

#include <iostream>


namespace igstk
{



  /** The counter for providing pseudo-unique identifiers for tokens */
  Token::IdentifierType Token::IdentifierCounter = 1;

  /** Used for mutex locking */
  static ::itk::SimpleFastMutexLock    TokenMutex;
    


  Token::Token()
  {
    /** Start mutual exclusion section. This prevent race conditions when
     * multiple threads are creating Tokens simultaneously */
    TokenMutex.Lock();
    
    /** When the IdentifierCounter rolls over (reaches it maximum value and
     * restars from zero) the Uniqueness of identifiers can no longer be
     * guaranted. */
    this->m_Identifier  = IdentifierCounter++;

    TokenMutex.Unlock();
  }


  Token::~Token()
  {
  }


} // end namespace igstk



