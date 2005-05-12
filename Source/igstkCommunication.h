
/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCommunication.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_Communication_h_
#define __igstk_Communication_h_


#include "itkObject.h"
#include "igstkMacros.h"


namespace igstk
{
/** \class Communication
    \brief Class Communication is the base class for communication between
    the tracker class and the hardware tracking device. This communication
    may be established via serial port, parallel port, ethernet, or an 
    offline communication (through a file or class emulating tracker response).
    Classes specific to each such communication would be derived from this 
    base Communication class.
*/

class Communication  : public itk::Object
{
 
public: 

  typedef Communication Self; 
  typedef itk::Object Superclass; 
  typedef itk::SmartPointer< Self > Pointer; 
  typedef itk::SmartPointer< const Self > ConstPointer; 

  /** Method for defining the name of the class */ 
  igstkTypeMacro(Communication, Object); 

  /** Method for creation through the object factory */ 
  igstkNewMacro(Self); 

  /** The method OpenCommunication sets up communication as per the data
  provided. */
  virtual bool OpenCommunication( const char *XMLFileName = NULL );

  /** The method CloseCommunication closes the communication. */
  virtual bool CloseCommunication( void );

  /** SendString method sends the string via communication link. */
  virtual bool SendString( const char *data ) { return false; };

  /** ReceiveString method receives string via communication link. */
  virtual bool ReceiveString( char *data ) { return false; }; 

  /** Flush "flushes" out any commands in the buffer through the communication link. */
  virtual bool Flush( void ) (return false; };

protected:

    /** Constructor is protected in order to enforce 
     *  the use of the New() operator */
    Communication(void);

    virtual ~Communication(void);

};

} // end of namespace igstk

#endif //__igstk_Communication_h_

