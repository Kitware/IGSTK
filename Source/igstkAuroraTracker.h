/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_NDITracker_h_
#define __igstk_NDITracker_h_

#include "itkXMLFile.h"
#include "igstkTracker.h"

namespace igstk
{
/** \class NDITracker
    \brief Implementation of the Aurora Tracker class.

*/

class NDIConfigurationReader : public itk::XMLReaderBase
{
  virtual int CanReadFile(const char* name);
  virtual void StartElement(const char * name,const char **atts);
  virtual void EndElement(const char *name);
  virtual void CharacterDataHandler(const char *inData, int inLength);
};

class NDITracker : public igstk::Tracker
{
public:

  /** Some required typedefs for itk::Object. */
  typedef NDITracker                  Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(NDITracker, Object);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  

protected:

  NDITracker(void);

  virtual ~NDITracker(void);

  virtual void ReadConfigurationFile( const char *file );

  virtual void AttemptToSetUpCommunicationProcessing( void );


private:


};

}

#endif //__igstk_NDITracker_h_
