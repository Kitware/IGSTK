/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMacros.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkMacros_h
#define __igstkMacros_h

#include "igstkLogger.h"
#include "itkCommand.h"
#include <cstdlib>

/** \file igstkMacros.h defines standard system-wide macros, constants, 
 * and other  common parameters in the IGSTK Library. 
 * It includes the Get, Set and Log macros.
 */

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
// warning C4503: 'insert' : decorated name length exceeded, 
// name was truncated
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
    if (this->GetLogger()->ShouldBuildMessage( ::igstk::Logger::x ) ) \
      { \
      std::ostringstream message; \
      message << y; \
      this->GetLogger()->Write(::itk::Logger::x, message.str()); \
      } \
    }  \
}


/** Macro for simplifying the use of logging: the first argument is
 *  an object that has a logger, the second argument is the priority
 *  (see itkMacro.h) and the second argument is the message. */
#define igstkLogMacroStatic( obj, x, y)  \
{         \
  if (obj->GetLogger() ) \
    {  \
    if (obj->GetLogger()->ShouldBuildMessage( ::igstk::Logger::x ) ) \
      { \
      std::ostringstream message; \
      message << y; \
      obj->GetLogger()->Write(::itk::Logger::x, message.str()); \
      } \
    }  \
}


/** Macro for simplifying the use of logging: the first argument is
 *  a pointer to a logger, the second argument is the priority
 *  (see itkMacro.h) and the second argument is the message. */
#define igstkLogMacro2( logger, x, y)  \
{         \
  if ( logger ) \
    {  \
    if (logger->ShouldBuildMessage( ::igstk::Logger::x )) \
      { \
      std::ostringstream message; \
      message << y; \
      logger->Write(::itk::Logger::x, message.str()); \
      } \
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
    virtual const char *GetNameOfClass() const {return #thisClass;} 


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
public: \
  typedef ::igstk::Logger                LoggerType; \
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
  } \
  virtual void Set##name (const std::string & _arg) \
  { \
    this->Set##name( _arg.c_str() ); \
  } \


/** Get character string.  Creates member Get"name"() 
 * (e.g., SetFilename(char *)). The macro assumes that
 * the class member (name) is declared as a type std::string. */
#define igstkGetStringMacro(name) \
  virtual const char* Get##name () const \
  { \
    return this->m_##name.c_str(); \
  }


/** Macro that defines all the standard elements related to the StateMachine.
 *  This macro factorizes code that should always be present when using 
 *  the StateMachine. */
#define  igstkStateMachineMacroBase( igstktypename ) \
private: \
  typedef ::igstk::StateMachine< Self > StateMachineType; \
  typedef igstktypename StateMachineType::TMemberFunctionPointer   ActionType; \
  typedef igstktypename StateMachineType::StateType                StateType;  \
  typedef igstktypename StateMachineType::InputType                InputType;  \
  typedef igstktypename StateMachineType::OutputStreamType OutputStreamType; \
  igstkFriendClassMacro( ::igstk::StateMachine< Self > ); \
  StateMachineType     m_StateMachine; \
  typedef ::itk::ReceptorMemberCommand< Self >   ReceptorObserverType; \
  typedef igstktypename ReceptorObserverType::Pointer \
                                                   ReceptorObserverPointer;  \
public:  \
  void ExportStateMachineDescription( OutputStreamType & ostr, \
                                      bool skipLoops=false ) const \
   { m_StateMachine.ExportDescription( ostr, skipLoops ); } \
  void ExportStateMachineDescriptionToLTS( OutputStreamType & ostr,\
                                           bool skipLoops=false ) const \
   { m_StateMachine.ExportDescriptionToLTS( ostr, skipLoops ); } \
  void ExportStateMachineDescriptionToSCXML( OutputStreamType & ostr,\
                                           bool skipLoops=false ) const \
   { m_StateMachine.ExportDescriptionToSCXML( ostr, skipLoops ); }

#define EMPTYPARAMETER

/** This is the StateMachine Macro to be used with non-templated classes */
#define igstkStateMachineMacro()  igstkStateMachineMacroBase( EMPTYPARAMETER )

/** This is the StateMachine Macro to be used with templated classes */
#define igstkStateMachineTemplatedMacro() igstkStateMachineMacroBase( typename )


/** Convenience macro for declaring Inputs to the State Machine */
#define igstkDeclareInputMacro( inputname ) \
    InputType      m_##inputname##Input 


/** Convenience macro for declaring States of the State Machine */
#define igstkDeclareStateMacro( inputname ) \
    StateType      m_##inputname##State 


/** Convenience macro for adding Inputs to the State Machine */
#define igstkAddInputMacro( inputname ) \
    this->m_StateMachine.AddInput( this->m_##inputname##Input,  \
                                   #inputname"Input" );


/** Convenience macro for adding States to the State Machine */
#define igstkAddStateMacro( statename ) \
    this->m_StateMachine.AddState( this->m_##statename##State,\
                                   #statename"State" );

/** Convenience macro for adding Transitions to the State Machine */
#define igstkAddTransitionMacro( state1, input, state2, action )   \
    this->m_StateMachine.AddTransition( this->m_##state1##State,   \
                                        this->m_##input##Input,    \
                                        this->m_##state2##State,   \
                                      & Self::action##Processing );


/** Convenience macro for selecting the initial States of the State Machine */
#define igstkSetInitialStateMacro( inputname ) \
    this->m_StateMachine.SelectInitialState( this->m_##inputname##State );


/** Convenience macro for pushing an input in the queue of the State Machine */
#define igstkPushInputMacro( inputname ) \
    this->m_StateMachine.PushInput( this->m_##inputname##Input );


/** Convenience macro for the initial standard traits of a class */
#define igstkStandardClassBasicTraitsMacro( classname, superclassname ) \
  typedef classname                         Self;  \
  typedef superclassname                    Superclass; \
  typedef ::itk::SmartPointer< Self >       Pointer; \
  typedef ::itk::SmartPointer< const Self > ConstPointer; \
  igstkTypeMacro( classname, superclassname);  

/** Convenience macro for traits of an abstract non-templated class */
#define igstkStandardAbstractClassTraitsMacro( classname, superclassname ) \
  igstkStandardClassBasicTraitsMacro( classname, superclassname ) \
  igstkStateMachineMacro(); 

/** Convenience macro for traits of a non-templated class */
#define igstkStandardClassTraitsMacro( classname, superclassname ) \
  igstkStandardAbstractClassTraitsMacro( classname, superclassname ) \
  igstkNewMacro( Self );  

/** Convenience macro for the traits of an abstract templated class */
#define igstkStandardTemplatedAbstractClassTraitsMacro( classname, \
                                                       superclassname ) \
  igstkStandardClassBasicTraitsMacro( classname, superclassname ) \
  igstkStateMachineTemplatedMacro(); 

/** Convenience macro for the traits of a templated class */
#define igstkStandardTemplatedClassTraitsMacro( classname, superclassname ) \
  igstkStandardTemplatedAbstractClassTraitsMacro( classname, superclassname ) \
  igstkNewMacro( Self );  


/** Convenience macro for creating an Observer for an Event, its callback
 *  and transducing it into a specified input to the state machine */
#define igstkEventTransductionMacro( event, input ) \
private: \
  ReceptorObserverPointer m_Observer##event##input;  \
  void Callback##event##input##Input( const ::itk::EventObject & ) \
  { \
    igstkPushInputMacro( input ); \
    m_StateMachine.ProcessInputs(); \
  } \
public: \
  void Observe##event##Event(const ::igstk::Object * object ) \
    { \
    m_Observer##event##input = ReceptorObserverType::New(); \
    m_Observer##event##input->SetCallbackFunction( this, \
      & Self::Callback##event##input##Input ); \
    unsigned long tag = object->AddObserver( \
      ::igstk::event##Event(),m_Observer##event##input ); \
    this->RegisterObservedObject( object, tag ); \
    } 


/** Convenience macro for creating an Observer for an Event with payload,
 *  its callback and transducing it into a specified input to the state 
 *  machine that takes its value */
#define igstkLoadedEventTransductionMacro( event, input ) \
private: \
  ReceptorObserverPointer m_Observer##event##input;  \
  ::igstk::event##Event::PayloadType m_##input##InputToBeSet; \
  void Callback##event##input##Input( const ::itk::EventObject & eventvar ) \
  { \
  const ::igstk::event##Event * realevent = \
      dynamic_cast < const ::igstk::event##Event * > ( &eventvar ); \
    if( realevent ) \
      { \
      m_##input##InputToBeSet = realevent->Get(); \
      igstkPushInputMacro( input ); \
      m_StateMachine.ProcessInputs(); \
      } \
  } \
public: \
 void Observe##input##Input(const ::igstk::Object * object ) \
    { \
    m_Observer##event##input = ReceptorObserverType::New(); \
    m_Observer##event##input->SetCallbackFunction( this,\
      & Self::Callback##event##input##Input ); \
    unsigned long tag = object->AddObserver( \
      ::igstk::event##Event(),m_Observer##event##input ); \
    this->RegisterObservedObject( object, tag ); \
    }
#define igstkLoadedObjectEventTransductionMacro( event, input ) \
private: \
  ReceptorObserverPointer m_Observer##event##input;  \
  ::igstk::event##Event::PayloadType *  m_##input##InputToBeSet; \
  void Callback##event##input##Input( const ::itk::EventObject & eventvar ) \
  { \
  const ::igstk::event##Event * realevent = \
     dynamic_cast < const ::igstk::event##Event * > ( &eventvar ); \
    if( realevent ) \
      { \
      m_##input##InputToBeSet = realevent->Get(); \
      igstkPushInputMacro( input ); \
      m_StateMachine.ProcessInputs(); \
      } \
  } \
public: \
 void Observe##input##Input(const ::igstk::Object * object ) \
    { \
    m_Observer##event##input = ReceptorObserverType::New(); \
    m_Observer##event##input->SetCallbackFunction( this,\
      & Self::Callback##event##input##Input ); \
    unsigned long tag = object->AddObserver( \
      ::igstk::event##Event(),m_Observer##event##input ); \
    this->RegisterObservedObject( object, tag ); \
    }


#define igstkObserverMacro( name, eventType, payloadType ) \
class name##Observer : public ::itk::Command \
{ \
public: \
  typedef  name##Observer             Self; \
  typedef  ::itk::Command             Superclass;\
  typedef  ::itk::SmartPointer<Self>  Pointer;\
  itkNewMacro( Self );\
protected:\
  name##Observer() \
    {\
    m_GotObject = false;\
    }\
  ~name##Observer() {}\
public:\
  typedef eventType  EventType;\
  void Execute(itk::Object *caller, const itk::EventObject & event)\
    {\
    const itk::Object * constCaller = caller;\
    this->Execute( constCaller, event );\
    }\
  void Execute(const itk::Object *caller, const itk::EventObject & event)\
    {\
    m_GotObject = false;\
    if( EventType().CheckEvent( &event ) )\
      {\
      const EventType * objectEvent = \
                    dynamic_cast< const EventType *>( &event );\
      if( objectEvent )\
        {\
        m_Object = objectEvent->Get();\
        m_GotObject = true;\
        }\
      }\
    (void) caller;\
    }\
  bool Got##name() const\
    {\
    return m_GotObject;\
    }\
  void Reset() \
    {\
    m_GotObject = false; \
    }\
  payloadType Get##name() const\
    {\
    return m_Object;\
    }\
private:\
  payloadType  m_Object;\
  bool m_GotObject;\
};


#define igstkObserverObjectMacro( name, eventType, payloadType ) \
class name##Observer : public ::itk::Command \
{ \
public: \
  typedef  name##Observer             Self; \
  typedef  ::itk::Command             Superclass;\
  typedef  ::itk::SmartPointer<Self>  Pointer;\
  itkNewMacro( Self );\
protected:\
  name##Observer() \
    {\
    m_GotObject = false;\
    }\
  ~name##Observer() {}\
public:\
  typedef eventType  EventType;\
  void Execute(itk::Object *caller, const itk::EventObject & event)\
    {\
    const itk::Object * constCaller = caller;\
    this->Execute( constCaller, event );\
    }\
  void Execute(const itk::Object *caller, const itk::EventObject & event)\
    {\
    m_GotObject = false;\
    if( EventType().CheckEvent( &event ) )\
      {\
      const EventType * objectEvent = \
                    dynamic_cast< const EventType *>( &event );\
      if( objectEvent )\
        {\
        m_Object = objectEvent->Get();\
        m_GotObject = true;\
        }\
      }\
    (void) caller;\
    }\
  bool Got##name() const\
    {\
    return m_GotObject;\
    }\
  void Reset() \
    {\
    m_GotObject = false; \
    }\
  payloadType::Pointer Get##name() const\
    {\
    return m_Object;\
    }\
private:\
  payloadType::Pointer  m_Object;\
  bool m_GotObject;\
};


#define igstkObserverConstObjectMacro( name, eventType, payloadType ) \
class name##Observer : public ::itk::Command \
{ \
public: \
  typedef  name##Observer             Self; \
  typedef  ::itk::Command             Superclass;\
  typedef  ::itk::SmartPointer<Self>  Pointer;\
  itkNewMacro( Self );\
protected:\
  name##Observer() \
    {\
    m_GotObject = false;\
    }\
  ~name##Observer() {}\
public:\
  typedef eventType  EventType;\
  void Execute(itk::Object *caller, const itk::EventObject & event)\
    {\
    const itk::Object * constCaller = caller;\
    this->Execute( constCaller, event );\
    }\
  void Execute(const itk::Object *caller, const itk::EventObject & event)\
    {\
    m_GotObject = false;\
    if( EventType().CheckEvent( &event ) )\
      {\
      const EventType * objectEvent = \
                    dynamic_cast< const EventType *>( &event );\
      if( objectEvent )\
        {\
        m_Object = objectEvent->Get();\
        m_GotObject = true;\
        }\
      }\
    (void) caller;\
    }\
  bool Got##name() const\
    {\
    return m_GotObject;\
    }\
  void Reset() \
    {\
    m_GotObject = false; \
    }\
  payloadType::ConstPointer Get##name() const\
    {\
    return m_Object;\
    }\
private:\
  payloadType::ConstPointer  m_Object;\
  bool m_GotObject;\
};

}
#endif // __igstk_Macros_h_
