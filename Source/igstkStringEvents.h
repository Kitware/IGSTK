/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkStringEvents.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __igstkStringEvents_H
#define __igstkStringEvents_H


#include "igstkEvents.h"


namespace igstk
{

/** \class StringEvents
 *  \brief StringEvents is a base class for events with a string payload 
 *
 */

class StringEvents : public IGSTKEvent 
{

public: 
  typedef StringEvents              Self; 
  typedef IGSTKEvent                Superclass;
  typedef std::string               StringType;

  /** Constructor */
  StringEvents() {
    m_String = ""; };

  /** Constructor with error code */
  StringEvents( StringType str ) {
    m_String = str; };

  /** Copy constructor */
  StringEvents(const Self & s) : IGSTKEvent( s ) {
    m_String = s.m_String; };
 
  /** Destructor */
  virtual ~StringEvents() {};

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
  void   SetString(StringType str) {
         m_String = str; };

private:
  void operator=(const Self&); 

  // Payload of this event.
  StringType             m_String;
};


}

#endif

