/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMacros.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstk_Macros_h_
#define __igstk_Macros_h_

/**
  igstkMacro.h defines standard system-wide macros, constants, and other 
  common parameters in the IGstk Library. It includes the Get, Set and
  Log macros.
*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
   #pragma warning( disable : 4786 )
  // warning C4503: 'insert' : decorated name length exceeded, name was truncated
  #pragma warning ( disable : 4503 )
#endif

namespace igstk
{

#define igstkLogMacro( x, y)  \
{         \
    if (this->GetLogger() && (this->GetLogger()->GetPriorityLevel()>= x) ) \
    {  \
        *(this->GetLogger()) << y; \
    } \
}


/** Set built-in type.  Creates member Set"name"() (e.g., SetTimeStep(time)); */
#define  SetMacro(name,type) \
virtual void Set##name (const type _arg) \
{ \
    if (this->m_##name != _arg) \
    { \
        this->m_##name = _arg; \
    } \
} 


/** Get built-in type.  Creates member Get"name"() (e.g., GetTimeStep(time)); */
#define GetMacro(name,type) \
virtual const type & Get##name () const \
{ \
    return this->m_##name; \
}

/** Create a Macro for friend class. This will take care of platform specific
 * ways of declaring a class as a friend */
#if defined(__GNUC__) 
#define  FriendClassMacro(type) friend class type
#else 
#define  FriendClassMacro(type) friend type
#endif 





}
#endif // __igstk_Macros_h_
