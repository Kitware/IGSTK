/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkEvents.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkEvents_h
#define __igstkEvents_h

/** This file contains the declarations of IGSTK Events used to communicate 
 *  among components. */

#include "igstkTransform.h"
#include "vtkImageData.h"

namespace igstk 
{
#define igstkEventMacro( classname, superclassname ) \
            itkEventMacro( classname, superclassname )

#define igstkLoadedEventMacro( name, superclass, payloadtype ) \
class  name : public superclass \
{ \
public:  \
  typedef name        Self; \
  typedef superclass  Superclass; \
  typedef payloadtype PayloadType; \
  name() {} \
  virtual ~name() {} \
  virtual const char * GetEventName() const { return #name; } \
  virtual bool CheckEvent(const ::itk::EventObject* e) const \
    { return dynamic_cast<const Self*>(e); } \
  virtual ::itk::EventObject* MakeObject() const \
    { return new Self; } \
  name(const Self&s) :superclass(s){}; \
  const PayloadType & Get() const \
    { return m_Payload; }  \
  void Set( const payloadtype & _var ) \
    { m_Payload = _var; }  \
private: \
  void operator=(const Self&);  \
  PayloadType  m_Payload; \
};

namespace EventHelperType 
{
typedef itk::Point< double, 3 >    PointType;
typedef std::string                StringType;
typedef vtkImageData *             VTKImagePointerType;
typedef unsigned int               UnsignedIntType;
typedef signed int                 SignedIntType;
typedef float                      FloatType;
typedef double                     DoubleType;
typedef struct {
  unsigned int minimum;
  unsigned int maximum;
}                                  IntegerBoundsType;
}

#define igstkLoadedObjectEventMacro( name, superclass, payloadtype ) \
class  name : public superclass \
{ \
public:  \
  typedef name        Self; \
  typedef superclass  Superclass; \
  typedef payloadtype PayloadType; \
  name() {} \
  virtual ~name() {} \
  virtual const char * GetEventName() const { return #name; } \
  virtual bool CheckEvent(const ::itk::EventObject* e) const \
    { return dynamic_cast<const Self*>(e); } \
  virtual ::itk::EventObject* MakeObject() const \
    { return new Self; } \
  name(const Self&s) :superclass(s){}; \
  PayloadType* Get() const\
    { return m_Payload.GetPointer(); }  \
  void Set( payloadtype * _var ) \
    { m_Payload = _var; }  \
private: \
  void operator=(const Self&);  \
  PayloadType::Pointer  m_Payload; \
};


#define igstkLoadedConstObjectEventMacro( name, superclass, payloadtype ) \
class  name : public superclass \
{ \
public:  \
  typedef name        Self; \
  typedef superclass  Superclass; \
  typedef payloadtype PayloadType; \
  name() {} \
  virtual ~name() {} \
  virtual const char * GetEventName() const { return #name; } \
  virtual bool CheckEvent(const ::itk::EventObject* e) const \
    { return dynamic_cast<const Self*>(e); } \
  virtual ::itk::EventObject* MakeObject() const \
    { return new Self; } \
  name(const Self&s) :superclass(s){}; \
  const PayloadType* Get() const\
    { return m_Payload.GetPointer(); }  \
  void Set( const payloadtype * _var ) \
    { m_Payload = _var; }  \
private: \
  void operator=(const Self&);  \
  PayloadType::ConstPointer  m_Payload; \
};


#define igstkLoadedTemplatedObjectEventMacro( name, superclass, payloadtype ) \
class  name : public superclass \
{ \
public:  \
  typedef name        Self; \
  typedef superclass  Superclass; \
  typedef payloadtype PayloadType; \
  name() {} \
  virtual ~name() {} \
  virtual const char * GetEventName() const { return #name; } \
  virtual bool CheckEvent(const ::itk::EventObject* e) const \
    { return dynamic_cast<const Self*>(e); } \
  virtual ::itk::EventObject* MakeObject() const \
    { return new Self; } \
  name(const Self&s) :superclass(s){}; \
  PayloadType * Get() const\
    { return m_Payload.GetPointer(); }  \
  void Set( payloadtype * _var ) \
    { m_Payload = _var; }  \
private: \
  void operator=(const Self&);  \
  typename PayloadType::Pointer  m_Payload; \
};

#define igstkLoadedTemplatedConstObjectEventMacro( name, superclass,\
                                                   payloadtype ) \
class  name : public superclass \
{ \
public:  \
  typedef name        Self; \
  typedef superclass  Superclass; \
  typedef payloadtype PayloadType; \
  name() {} \
  virtual ~name() {} \
  virtual const char * GetEventName() const { return #name; } \
  virtual bool CheckEvent(const ::itk::EventObject* e) const \
    { return dynamic_cast<const Self*>(e); } \
  virtual ::itk::EventObject* MakeObject() const \
    { return new Self; } \
  name(const Self&s) :superclass(s){}; \
  const PayloadType * Get() const\
    { return m_Payload.GetPointer(); }  \
  void Set( const payloadtype * _var ) \
    { m_Payload = _var; }  \
private: \
  void operator=(const Self&);  \
  typename PayloadType::ConstPointer  m_Payload; \
};

igstkEventMacro( IGSTKEvent,               itk::UserEvent );
igstkEventMacro( PulseEvent,               IGSTKEvent );
igstkEventMacro( RefreshEvent,             IGSTKEvent );
igstkEventMacro( CompletedEvent,           IGSTKEvent );
igstkEventMacro( InputOutputErrorEvent,    IGSTKEvent );
igstkEventMacro( InputOutputTimeoutEvent,  IGSTKEvent );
igstkEventMacro( OpenPortErrorEvent,       IGSTKEvent );
igstkEventMacro( ClosePortErrorEvent,      IGSTKEvent );
igstkEventMacro( InvalidRequestErrorEvent, IGSTKEvent );

igstkLoadedEventMacro( PointEvent, IGSTKEvent, EventHelperType::PointType );
igstkLoadedEventMacro( TransformModifiedEvent, IGSTKEvent, Transform );
igstkLoadedEventMacro( LandmarkRegistrationErrorEvent, IGSTKEvent, 
                       EventHelperType::DoubleType );
igstkLoadedEventMacro( StringEvent, IGSTKEvent, EventHelperType::StringType );
igstkLoadedEventMacro( UnsignedIntEvent, IGSTKEvent, 
                       EventHelperType::UnsignedIntType );
igstkLoadedEventMacro( IntegerBoundsEvent, IGSTKEvent, 
                       EventHelperType::IntegerBoundsType );
igstkLoadedEventMacro( VTKImageModifiedEvent, IGSTKEvent,
                       EventHelperType::VTKImagePointerType );
igstkLoadedEventMacro( DoubleTypeEvent, IGSTKEvent,
                       EventHelperType::DoubleType );


igstkEventMacro( AxialSliceBoundsEvent,      IntegerBoundsEvent );
igstkEventMacro( SagittalSliceBoundsEvent,   IntegerBoundsEvent );
igstkEventMacro( CoronalSliceBoundsEvent,    IntegerBoundsEvent );
}
#endif
