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
  \file igstkMacros.h defines standard system-wide macros, constants, and other 
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

/** Macro for simplifying the use of logging: the first argument is
 *  the priority (see itkMacro.h) and the second argument is the
 *  message. */
#define igstkLogMacro( x, y)  \
{         \
  if (this->GetLogger() ) \
    {  \
    ::itk::OStringStream message; \
    message << y; \
    this->GetLogger()->Write(::itk::Logger::x, message.str()); \
    }  \
}


/** Macro for simplifying the use of logging: the first argument is
 *  an object that has a logger, the second argument is the priority
 *  (see itkMacro.h) and the second argument is the message. */
#define igstkLogMacroStatic( obj, x, y)  \
{         \
  if (obj->GetLogger() ) \
    {  \
    ::itk::OStringStream message; \
    message << y; \
    obj->GetLogger()->Write(::itk::Logger::x, message.str()); \
    }  \
}


/** Set built-in type.  Creates member Set"name"() (e.g., SetTimeStep(time)); */
#define  igstkSetMacro(name,type) \
virtual void Set##name (const type & _arg) \
{ \
    if (this->m_##name != _arg) \
    { \
        this->m_##name = _arg; \
    } \
} 


/** Get built-in type.  Creates member Get"name"() (e.g., GetTimeStep(time));
 */
#define igstkGetMacro(name,type) \
virtual const type & Get##name () const \
{ \
    return this->m_##name; \
}


/** New Macro creates a new object of a class that is using SmartPointers. This
 * macro differs from the one in ITK in that it DOES NOT uses factories.
 * Factories add run-time uncertainty that is undesirable for IGSTK. */
#define igstkNewMacro(x) \
static Pointer New(void) \
{ \
  Pointer smartPtr; \
  x *rawPtr = new x; \
  smartPtr = rawPtr; \
  rawPtr->UnRegister(); \
  return smartPtr; \
} 


/** Type Macro defines the GetNameOfClass() method for every class where it is
 * invoked. */
#define igstkTypeMacro(thisClass,superclass) \
    virtual const char *GetNameOfClass() const \
        {return #thisClass;} 


/** Create a Macro for friend class. This will take care of platform specific
 * ways of declaring a class as a friend */
#if defined(__GNUC__) 
#define  igstkFriendClassMacro(type) friend class type
#else 
#define  igstkFriendClassMacro(type) friend type
#endif 


/** Macro that defines all the standard elements related to the Logger.
 * LoggerType must be defined before this macro is used. */
#define  igstkLoggerMacro() \
protected: \
  LoggerType* GetLogger() const { return m_Logger; } \
private: \
  mutable LoggerType::Pointer   m_Logger; \
public: \
  void SetLogger(LoggerType* logger) { m_Logger = logger; }


/** Macro that defines all the standard elements related to the StateMachine.
 * This macro factorizes code that should always be present when using the StateMachine. */
#define  igstkStateMachineMacro() \
private: \
  typedef StateMachine< Self > StateMachineType; \
  typedef StateMachineType::TMemberFunctionPointer   ActionType; \
  typedef StateMachineType::StateType                StateType;  \
  typedef StateMachineType::InputType                InputType;  \
  typedef StateMachineType::OutputStreamType OutputStreamType; \
  igstkFriendClassMacro( StateMachine< Self > ); \
  StateMachineType     m_StateMachine; \
public:  \
  void ExportStateMachineDescription( OutputStreamType & ostr, bool skipLoops=false ) const \
   { m_StateMachine.ExportDescription( ostr, skipLoops ); }
  


}
#endif // __igstk_Macros_h_

