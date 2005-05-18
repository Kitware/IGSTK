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
#include "igstkCommunication.h"
#include "igstkNDICyclicRedundancy.h"
#include "igstkNDICommandInterpreter.h"

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

#define NDI_NUMBER_OF_PORTS  4
#define NDI_COMMAND_MAX_LEN  2048

  /** Some required typedefs for itk::Object. */
  typedef NDITracker                  Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(NDITracker, Object);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  

  /** The SetCommunication method is used to attach a communication object to the
  tracker object for communication with the tracker hardware. */
  virtual void SetCommunication( CommunicationType *communication );

protected:

  NDITracker(void);

  virtual ~NDITracker(void);

//  virtual void ReadConfigurationFile( const char *file );

  virtual void AttemptToSetUpCommunicationProcessing( void );

  virtual void AttemptToSetUpToolsProcessing( void );

  virtual void AttemptToStartTrackingProcessing( void ) {};

  virtual void AttemptToStopTrackingProcessing( void );

  virtual void UpdateStatusProcessing( void ) {};

  virtual void ResetTrackingProcessing( void ) {};

  virtual void DisableCommunicationProcessing( void ) {};

  virtual void DisableToolsProcessing( void ) {};


  void AttachSROMFileNameToPort( const int portNum, std::string fileName );

private:

  bool LoadVirtualSROM( const int i, std::string SROMFileName) { return true; };

  /*
  void SendCommand(const char *command, bool addCRC = true);

// Sends a raw command to the tracking unit.
  char *NDITracker::SendCommand(const char *command);

  char m_CommandBuffer[NDI_COMMAND_MAX_LEN];
*/

  std::string m_SROMFileNames[NDI_NUMBER_OF_PORTS];

  NDICyclicRedundancy     m_CyclicRedundancy;

  NDICommandInterpreter   *m_CommandInterpreter;
};

}

#endif //__igstk_NDITracker_h_
