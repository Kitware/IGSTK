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

#include "itkLogger.h"
#include "itkCommand.h"

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


/** Macro for simplifying the use of logging: the first argument is
 *  a pointer to a logger, the second argument is the priority
 *  (see itkMacro.h) and the second argument is the message. */
#define igstkLogMacro2( logger, x, y)  \
{         \
  if ( logger ) \
    {  \
    ::itk::OStringStream message; \
    message << y; \
    logger->Write(::itk::Logger::x, message.str()); \
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

/** Set character string.  Creates member Set"name"()
 * (e.g., SetFilename(char *)). The macro assumes that
 * the class member (name) is declared a type std::string. */
#define igstkSetStringMacro(name) \
  virtual void Set##name (const char* _arg) \
  { \
    if ( _arg && (_arg == this->m_##name) ) { return;} \
    if (_arg) \
      { \
      this->m_##name = _arg;\
      } \
     else \
      { \
      this->m_##name = ""; \
      } \
    this->Modified(); \
  }


/** Macro that defines all the standard elements related to the StateMachine.
 * This macro factorizes code that should always be present when using the StateMachine. */
#define  igstkStateMachineMacroBase( igstktypename ) \
private: \
  typedef StateMachine< Self > StateMachineType; \
  typedef igstktypename StateMachineType::TMemberFunctionPointer   ActionType; \
  typedef igstktypename StateMachineType::StateType                StateType;  \
  typedef igstktypename StateMachineType::InputType                InputType;  \
  typedef igstktypename StateMachineType::OutputStreamType OutputStreamType; \
  igstkFriendClassMacro( StateMachine< Self > ); \
  StateMachineType     m_StateMachine; \
  typedef ::itk::ReceptorMemberCommand< Self >   ReceptorObserverType; \
  typedef igstktypename ReceptorObserverType::Pointer          ReceptorObserverPointer;  \
public:  \
  void ExportStateMachineDescription( OutputStreamType & ostr, bool skipLoops=false ) const \
   { m_StateMachine.ExportDescription( ostr, skipLoops ); } \
  void ExportStateMachineDescriptionToLTS( OutputStreamType & ostr, bool skipLoops=false ) const \
   { m_StateMachine.ExportDescriptionToLTS( ostr, skipLoops ); }

#define EMPTYPARAMETER

/** This is the StateMachine Macro to be used with non-templated classes */
#define igstkStateMachineMacro()  igstkStateMachineMacroBase( EMPTYPARAMETER )

/** This is the StateMachine Macro to be used with templated classes */
#define igstkStateMachineTemplatedMacro() igstkStateMachineMacroBase( typename )


/** Convenience macro for adding Inputs to the State Machine */
#define igstkAddInputMacro( inputname ) \
    m_StateMachine.AddInput( m_##inputname,  #inputname );


/** Convenience macro for adding States to the State Machine */
#define igstkAddStateMacro( inputname ) \
    m_StateMachine.AddState( m_##inputname,  #inputname );


/** Convenience macro for adding Transitions to the State Machine */
#define igstkAddTransitionMacro( state1, input, state2, action ) \
    m_StateMachine.AddTransition( m_##state1, m_##input, m_##state2,  & Self::action );


/** Convenience macro for the initial standard traits of an class */
#define igstkStandardClassTraitsMacro( classname, superclassname ) \
  typedef classname Self;  \
  typedef superclassname Superclass; \
  typedef ::itk::SmartPointer< Self > Pointer; \
  typedef ::itk::SmartPointer< const Self > ConstPointer; \
  igstkTypeMacro( classname, superclassname);  \
  igstkNewMacro( Self );  \
  typedef ::itk::Logger                  LoggerType; \
  igstkStateMachineMacro(); \
  igstkLoggerMacro(); 


/** Convenience macro for creating an Observer for an Event, its callback
 *  and transducing it into a specified input to the state machine */
#define igstkEventTransductionMacro( event, input ) \
private: \
  ReceptorObserverPointer m_Observer##event##input;  \
  void Callback##event##input( const ::itk::EventObject & ) \
  { \
    m_StateMachine.PushInput( m_##input ); \
  } \
public: \
  void Observe##event(::itk::Object * object ) \
    { \
    m_Observer##event##input = ReceptorObserverType::New(); \
    m_Observer##event##input->SetCallbackFunction( this, & Self::Callback##event##input ); \
    object->AddObserver( event(),m_Observer##event##input ); \
    } 


/** Convenience macro for creating an Observer for an Event with payload, its callback
 *  and transducing it into a specified input to the state machine that takes its value */
#define igstkLoadedEventTransductionMacro( event, input, payload ) \
private: \
  ReceptorObserverPointer m_Observer##event##input;  \
  payload                 m_##payload##ToBeSet; \
  void Callback##event##input( const ::itk::EventObject & eventvar ) \
  { \
    const event * realevent = dynamic_cast < const event * > ( &eventvar ); \
    if( realevent ) \
      { \
      m_##payload##ToBeSet = realevent->Get(); \
      m_StateMachine.PushInput( m_##input ); \
      } \
  } \
public: \
  void Observe##event(::itk::Object * object ) \
    { \
    m_Observer##event##input = ReceptorObserverType::New(); \
    m_Observer##event##input->SetCallbackFunction( this, & Self::Callback##event##input ); \
    object->AddObserver( event(),m_Observer##event##input ); \
    } 


}
#endif // __igstk_Macros_h_

