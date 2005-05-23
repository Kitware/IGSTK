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

#include "igstkCommunication.h"
//#include "igstkNDICyclicRedundancy.h"
#include "igstkNDICommandInterpreter.h"
#include "igstkAuroraTool.h"
#include "igstkTracker.h"

namespace igstk
{
/** \class NDITracker
    \brief Implementation of the Aurora Tracker class.

*/

  enum {
  TR_MISSING       = 0x0001,  // tool or tool port is not available
  TR_OUT_OF_VIEW   = 0x0002,  // cannot obtain transform for tool
  TR_OUT_OF_VOLUME = 0x0004  // tool is not within the sweet spot of system
//  TR_SWITCH1_IS_ON = 0x0010,  // various buttons/switches on tool
//  TR_SWITCH2_IS_ON = 0x0020,
//  TR_SWITCH3_IS_ON = 0x0040
};


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
//#define NDI_MAX_TOOLS 4        // the number of tools aurora can handle
#define NDI_COMMAND_MAX_LEN  2048

  /** Some required typedefs for itk::Object. */
  typedef NDITracker                  Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(NDITracker, Object);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  

  typedef igstk::AuroraTool              AuroraToolType;
  typedef AuroraToolType::Pointer        AuroraToolPointer;
  typedef AuroraToolType::ConstPointer   AuroraToolConstPointer;

  /** The SetCommunication method is used to attach a communication object to the
  tracker object for communication with the tracker hardware. */
  virtual void SetCommunication( CommunicationType *communication );

protected:

  NDITracker(void);

  virtual ~NDITracker(void);

//  virtual void ReadConfigurationFile( const char *file );

  virtual void AttemptToSetUpCommunicationProcessing( void );

  virtual void AttemptToSetUpToolsProcessing( void );

  virtual void AttemptToStartTrackingProcessing( void );

  virtual void AttemptToStopTrackingProcessing( void );

  virtual void UpdateStatusProcessing( void );

  virtual void ResetTrackingProcessing( void ) {};

  virtual void DisableCommunicationProcessing( void );

  virtual void DisableToolsProcessing( void );


  void AttachSROMFileNameToPort( const int portNum, std::string fileName );

private:

  bool LoadVirtualSROM( const int i, std::string SROMFileName) ;

  void ClearVirtualSROM(int tool);

  /** Enable all tool ports that have tools plugged into them.
   * {The reference port is enabled with NDI_STATIC.} */

  void EnableToolPorts( void );

  /** Disable all enabled tool ports. */
  void DisableToolPorts( void );

  /** Find the tool for a specific port handle (-1 if not found). */
  int GetToolFromHandle(int handle);

  void InternalUpdate();

  int PortEnabled[NDI_NUMBER_OF_PORTS];

  int PortHandle[NDI_NUMBER_OF_PORTS];

  int IsDeviceTracking;

//  AuroraToolVectorType   m_AuroraTools;

  std::string m_SROMFileNames[NDI_NUMBER_OF_PORTS];

//  NDICyclicRedundancy     m_CyclicRedundancy;

  NDICommandInterpreter   *m_CommandInterpreter;
};

}

#endif //__igstk_NDITracker_h_
