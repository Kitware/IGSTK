/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkEvents.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstk_Events_h_
#define __igstk_Events_h_

/** 
 * This file contains the declarations of IGSTK Events used to communicate among components.
 */

#include "igstkTransform.h"

namespace igstk 
{
#define igstkEventMacro( classname, superclassname ) \
            itkEventMacro( classname, superclassname )

#define igstkLoadedEventMacro( name, superclass, payloadtype ) \
class  name : public superclass \
{ \
public:  \
  typedef name Self; \
  typedef superclass Superclass; \
  name() {} \
  virtual ~name() {} \
  virtual const char * GetEventName() const { return #name; } \
  virtual bool CheckEvent(const ::itk::EventObject* e) const \
    { return dynamic_cast<const Self*>(e); } \
  virtual ::itk::EventObject* MakeObject() const \
    { return new Self; } \
  name(const Self&s) :superclass(s){}; \
  const payloadtype & Get() const \
    { return m_Payload; }  \
  void Set( const payloadtype & _var ) \
    { m_Payload = _var; }  \
private: \
  void operator=(const Self&);  \
  payloadtype  m_Payload; \
};

igstkEventMacro( IGSTKEvent,               itk::UserEvent );
igstkEventMacro( PulseEvent,               IGSTKEvent );
igstkEventMacro( RefreshEvent,             IGSTKEvent );
igstkEventMacro( CompletedEvent,           IGSTKEvent );
igstkEventMacro( InputOutputErrorEvent,    IGSTKEvent );
igstkEventMacro( InputOutputTimeoutEvent,  IGSTKEvent );
igstkEventMacro( OpenPortErrorEvent,       IGSTKEvent );
igstkEventMacro( ClosePortErrorEvent,      IGSTKEvent );

namespace EventHelperType 
{
typedef itk::Point< double, 3 >    PointType;
typedef std::string                StringType;
}

igstkLoadedEventMacro( PointEvent, IGSTKEvent, EventHelperType::PointType );
igstkLoadedEventMacro( TransformModifiedEvent, IGSTKEvent, Transform );
igstkLoadedEventMacro( StringEvent, IGSTKEvent, EventHelperType::StringType );
}


#endif
