/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkStringEvent.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __igstkStringEvent_H
#define __igstkStringEvent_H


#include "igstkEvents.h"


namespace igstk
{

/** \class StringEvent
 *  \brief StringEvent is a base class for events with a string payload. 
 *
 */

class StringEvent : public IGSTKEvent 
{

public: 
  typedef StringEvent              Self; 
  typedef IGSTKEvent                Superclass;
  typedef std::string               StringType;

  /** Constructor */
  StringEvent() {
    m_String = ""; };

  /** Constructor with error code */
  StringEvent( StringType str ) {
    m_String = str; };

  /** Copy constructor */
  StringEvent(const Self & s) : IGSTKEvent( s ) {
    m_String = s.m_String; };
 
  /** Destructor */
  virtual ~StringEvent() {};

  /** Get the name of this event */
  virtual const char * GetEventName() const {
    return "StringEvent"; };

  /** check whether an event derives from this event */
  virtual bool CheckEvent(const ::itk::EventObject* e) const {
    return dynamic_cast<const Self*>(e); };

  /** Make another event of the same type */
  virtual ::itk::EventObject* MakeObject() const {
    return new Self; }

  /** Get the string */
  const StringType GetString() const {
    return m_String; };

  /** Get the string */
  void  SetString( const StringType&  str) {
        m_String = str; };

private:
  void operator=(const Self&); 

  /** Payload of this event */
  StringType             m_String;
};


}

#endif

