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
//#include "vtkPlaneSource.h"
#include "vtkCamera.h"

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
//typedef vtkPlaneSource *           VTKPlaneSourcePointerType;
typedef vtkCamera *                VTKCameraPointerType;
typedef unsigned int               UnsignedIntType;
typedef signed int                 SignedIntType;
typedef float                      FloatType;
typedef double                     DoubleType;
typedef struct {
  unsigned int minimum;
  unsigned int maximum;
}                                  IntegerBoundsType;
typedef struct {
  double xmin;
  double xmax;
  double ymin;
  double ymax;
  double zmin;
  double zmax;
}                                  ImageBoundsType;
typedef struct {
  unsigned int xmin;
  unsigned int xmax;
  unsigned int ymin;
  unsigned int ymax;
  unsigned int zmin;
  unsigned int zmax;
}                                  ImageExtentType;
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
igstkEventMacro( IGSTKErrorEvent,          IGSTKEvent );
igstkEventMacro( PulseEvent,               IGSTKEvent );
igstkEventMacro( RefreshEvent,             IGSTKEvent );
igstkEventMacro( CompletedEvent,           IGSTKEvent );
igstkEventMacro( InputOutputErrorEvent,    IGSTKErrorEvent );
igstkEventMacro( InputOutputTimeoutEvent,  IGSTKErrorEvent );
igstkEventMacro( OpenPortErrorEvent,       IGSTKErrorEvent );
igstkEventMacro( ClosePortErrorEvent,      IGSTKErrorEvent );
igstkEventMacro( InvalidRequestErrorEvent, IGSTKErrorEvent );
igstkEventMacro( TransformNotAvailableEvent, InvalidRequestErrorEvent );
igstkEventMacro( TransformExpiredErrorEvent, TransformNotAvailableEvent );

igstkLoadedEventMacro( PointEvent, IGSTKEvent, EventHelperType::PointType );
igstkLoadedEventMacro( LandmarkRegistrationErrorEvent, IGSTKErrorEvent, 
                       EventHelperType::DoubleType );
igstkLoadedEventMacro( StringEvent, IGSTKEvent, EventHelperType::StringType );
igstkLoadedEventMacro( UnsignedIntEvent, IGSTKEvent, 
                       EventHelperType::UnsignedIntType );

igstkLoadedEventMacro( IntegerBoundsEvent, IGSTKEvent, 
                       EventHelperType::IntegerBoundsType );

igstkLoadedEventMacro( ImageBoundsEvent, IGSTKEvent, 
                       EventHelperType::ImageBoundsType );

igstkLoadedEventMacro( ImageExtentEvent, IGSTKEvent, 
                       EventHelperType::ImageExtentType );

igstkLoadedEventMacro( VTKImageModifiedEvent, IGSTKEvent,
                       EventHelperType::VTKImagePointerType );

//igstkLoadedEventMacro( VTKPlaneModifiedEvent, IGSTKEvent,
//                       EventHelperType::VTKPlaneSourcePointerType );

igstkLoadedEventMacro( VTKCameraModifiedEvent, IGSTKEvent,
                       EventHelperType::VTKCameraPointerType );

igstkLoadedEventMacro( DoubleTypeEvent, IGSTKEvent,
                       EventHelperType::DoubleType );

igstkLoadedEventMacro( IGSTKErrorWithStringEvent, IGSTKErrorEvent, 
                       EventHelperType::StringType );

igstkEventMacro( AxialSliceBoundsEvent,      IntegerBoundsEvent );
igstkEventMacro( SagittalSliceBoundsEvent,   IntegerBoundsEvent );
igstkEventMacro( CoronalSliceBoundsEvent,    IntegerBoundsEvent );
}
#endif
