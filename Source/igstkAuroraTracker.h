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

#ifndef __igstk_AuroraTracker_h_
#define __igstk_AuroraTracker_h_

#include "itkXMLFile.h"

#include "igstkCommunication.h"
#include "igstkNDICommandInterpreter.h"
#include "igstkAuroraTool.h"
#include "igstkTracker.h"

namespace igstk
{
/** \class AuroraTracker
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

// the number of ports to allow
#define NDI_NUMBER_OF_PORTS  4

class NDIConfigurationReader : public itk::XMLReaderBase
{
  virtual int CanReadFile(const char* name);
  virtual void StartElement(const char * name,const char **atts);
  virtual void EndElement(const char *name);
  virtual void CharacterDataHandler(const char *inData, int inLength);
};


class AuroraTracker : public igstk::Tracker
{
public:

  /** typedefs for the tool */
  typedef igstk::AuroraTool              AuroraToolType;
  typedef AuroraToolType::Pointer        AuroraToolPointer;
  typedef AuroraToolType::ConstPointer   AuroraToolConstPointer;

  /** typedef for command interpreter */
  typedef igstk::NDICommandInterpreter   CommandInterpreterType;

  /** typedef for internal boolean return type */
  typedef Tracker::ResultType   ResultType;

  /** Some required typedefs for itk::Object. */
  typedef AuroraTracker                  Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /** typedef for CommunicationType */
  typedef Communication                 CommunicationType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(AuroraTracker, Object);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  

  /** The SetCommunication method is used to attach a communication
      object to the tracker object. */
  void SetCommunication( CommunicationType *communication );

  igstkGetMacro( NumberOfTools, unsigned int );

  void AttachSROMFileNameToPort( const int portNum, std::string fileName );

protected:

  AuroraTracker(void);

  virtual ~AuroraTracker(void);

  virtual ResultType InternalOpen( void );

  virtual ResultType InternalClose( void );

  virtual ResultType InternalActivateTools( void );

  virtual ResultType InternalDeactivateTools( void );

  virtual ResultType InternalStartTracking( void );

  virtual ResultType InternalStopTracking( void );

  virtual ResultType InternalUpdateStatus( void );

  virtual ResultType InternalReset( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Load a virtual SROM, given the file name of the ROM file */
  bool LoadVirtualSROM( const int i, std::string SROMFileName) ;

  /** Clear the virtual SROM for a tool */
  void ClearVirtualSROM( int tool );

  /** Enable all tool ports that have tools plugged into them.
   * {The reference tool port is enabled as a static tool.} */
  void EnableToolPorts( void );

  /** Disable all enabled tool ports. */
  void DisableToolPorts( void );

  /** Find the tool for a specific port handle (-1 if not found). */
  int GetToolFromHandle( int handle );

  int m_PortEnabled[NDI_NUMBER_OF_PORTS];

  int m_PortHandle[NDI_NUMBER_OF_PORTS];

  unsigned int   m_NumberOfTools;

  std::string    m_SROMFileNames[NDI_NUMBER_OF_PORTS];

  /** The "Communication" instance */
  CommunicationType::Pointer       m_Communication;

  /** The command interpreter */
  CommandInterpreterType::Pointer  m_CommandInterpreter;
};

}

#endif //__igstk_AuroraTracker_h_
