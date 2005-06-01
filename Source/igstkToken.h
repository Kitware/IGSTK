/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToken.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_Token_h
#define __igstk_Token_h

#include <string>
#include <itkLightObject.h>
#include "igstkMacros.h"


namespace igstk
{


/** \class Token
    \brief Class that provides unique identifiers.

    This class is intended to be used as the base class for the
    StatMachineState and StateMachineInput classes. It factorizes common
    behavior regarding the use of the unique identifier and the use of a text
    description.
    
    \sa StateMachine
    \sa StateMachineInput
    \sa StateMachineState
    \sa StateMachineAction

*/

class Token
{



public:


   /** Type used to represent the codes of the inputs */
   typedef unsigned long    IdentifierType;


   /** Constructor. It initializes all the member variables */
   Token();



   /** Destructor    */
   ~Token();



   /** Returns the unique identifier of the current instance */
   igstkGetMacro( Identifier, IdentifierType );

   

   /** \warning A SetIndentifier() method should never be created since it will
    * open a door for violating the uniqueness of the Identifier.  
    *
    *    SetIndentifier();      Purposely not created.
    *
    */

   void Print(std::ostream& os, itk::Indent indent) const;

protected:

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:


   /** Variable that holds the unique identifier of this State */
   IdentifierType    m_Identifier;

   
   /** Static variable that will provide an unique Id to every new class
    * instance */
   static IdentifierType   IdentifierCounter;


};


std::ostream& operator<<(std::ostream& os, const Token& o);

} // end namespace igstk


#endif


