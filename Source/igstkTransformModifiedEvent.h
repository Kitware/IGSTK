/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTransformModifiedEvent.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __igstkTransformModifiedEvent_H
#define __igstkTransformModifiedEvent_H


#include "igstkTransform.h"
#include "igstkEvents.h"


namespace igstk 
{

class  TransformModifiedEvent : public IGSTKEvent 
{ 
public: 
  typedef TransformModifiedEvent Self; 
  typedef IGSTKEvent Superclass; 
  TransformModifiedEvent() {} 
  virtual ~TransformModifiedEvent() {} 
  virtual const char * GetEventName() const { return "TransformModifiedEvent"; } 
  virtual bool CheckEvent(const ::itk::EventObject* e) const 
    { return dynamic_cast<const Self*>(e); } 
  virtual ::itk::EventObject* MakeObject() const 
    { return new Self; } 
  TransformModifiedEvent(const Self&s) :IGSTKEvent(s){}; 
  const Transform & GetTransform() const 
    { return m_Transform; }
  void SetTransform( const Transform & transform ) 
    { m_Transform = transform; }
private: 
  void operator=(const Self&); 

  // Payload of this event.
  Transform  m_Transform;
};


}

#endif

