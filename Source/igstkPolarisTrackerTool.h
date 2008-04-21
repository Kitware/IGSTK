/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTrackerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkPolarisTrackerTool_h
#define __igstkPolarisTrackerTool_h

#include "igstkTrackerTool.h"

namespace igstk
{

igstkEventMacro( PolarisTrackerToolEvent,StringEvent);
igstkEventMacro( PolarisTrackerToolErrorEvent, TrackerToolErrorEvent);
igstkEventMacro( InvalidPolarisPortNumberErrorEvent,
                                                PolarisTrackerToolErrorEvent);
igstkEventMacro( InvalidPolarisSROMFilenameErrorEvent,
                                                PolarisTrackerToolErrorEvent);
igstkEventMacro( InvalidPolarisPartNumberErrorEvent,
                                                PolarisTrackerToolErrorEvent);

class PolarisTracker;

/** \class PolarisTrackerTool
  * \brief A Polaris-specific TrackerTool class.
  *
  * This class is a for providing Polaris-specific functionality
  * for TrackerTools, and also to allow compile-time type enforcement
  * for other classes and functions that specifically require
  * an Polaris tool.
  *
  *  \image html  igstkPolarisTrackerTool.png  "PolarisTrackerTool State
  *  Machine Diagram"
  *
  *  \image latex igstkPolarisTrackerTool.eps  "PolarisTrackerTool State
  *  Machine Diagram" 
  *
  * \ingroup Tracker
  *
  */

class PolarisTrackerTool : public TrackerTool
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( PolarisTrackerTool, TrackerTool )

  /** Request setting the port number. The tool can only be attached to a
   * tracker after its port number has been defined. Valid port numbers are
   * in [0,11]. */
  void RequestSetPortNumber( unsigned int portNumber );

  /** Set the TrackerTool to be wired */
  void RequestSelectWiredTrackerTool(); 

  /** Set the TrackerTool to be wireless */
  void RequestSelectWirelessTrackerTool();

  /** Request set the SROM file name */
  void RequestSetSROMFileName( const std::string & filename );

  /** Request set the PartNumber */
  void RequestSetPartNumber( const std::string & toolId );

  /** Check if the tracker tool is wireless type */
  bool IsToolWirelessType() const;

  /** Check if SROM file name specified. */
  bool IsSROMFileNameSpecified() const;

  /** Check if Tool part number is specified. */
  bool IsPartNumberSpecified() const;

  /** Get SROM file name */
  igstkGetStringMacro( SROMFileName );

  /** Get tracker tool port number */
  igstkGetMacro( PortNumber, unsigned int );

  /** Get tracker tool part number */
  igstkGetStringMacro( PartNumber );


protected:

  PolarisTrackerTool();
  virtual ~PolarisTrackerTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

private:

  /** Report that a wired tracker tool selected */
  void ReportWiredTrackerToolSelectedProcessing();

  /** Report that a wireless tracker tool selected */
  void ReportWirelessTrackerToolSelectedProcessing();

  /** Set port number */ 
  void SetPortNumberProcessing( );

  /** Report Invalid port number specified */ 
  void ReportInValidPortNumberSpecifiedProcessing( );

  /** Set SROM filename */ 
  void SetSROMFileNameProcessing( );

  /** Report Invalid SROM filename specified */ 
  void ReportInValidSROMFileSpecifiedProcessing( );

  /** Set part number  */ 
  void SetPartNumberProcessing( );

  /** Report Invalid part number specified */ 
  void ReportInValidPartNumberSpecifiedProcessing( );

  PolarisTrackerTool(const Self&);   //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

  /** Get boolean variable to check if the tracker tool is 
   * initialized or not */
  virtual bool CheckIfTrackerToolIsConfigured() const; 

  /** Get boolean variable to check if SROM file is specified */
  igstkGetMacro( SROMFileNameSpecified, bool );

  /** Get boolean variable to check if the tracker toolId is 
   * specified. */
  igstkGetMacro( PartNumberSpecified, bool );

  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( WirelessTrackerToolSelected );
  igstkDeclareStateMacro( WiredTrackerToolSelected );
  igstkDeclareStateMacro( WiredTrackerToolPortNumberSpecified );
  igstkDeclareStateMacro( WirelessTrackerToolSROMFileNameSpecified );
  igstkDeclareStateMacro( WiredTrackerToolSROMFileNameSpecified );
  igstkDeclareStateMacro( WirelessTrackerToolPartNumberSpecified );
  igstkDeclareStateMacro( WiredTrackerToolPartNumberSpecified );

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( SelectWiredTrackerTool );
  igstkDeclareInputMacro( SelectWirelessTrackerTool );
  igstkDeclareInputMacro( ValidPortNumber );
  igstkDeclareInputMacro( InValidPortNumber );
  igstkDeclareInputMacro( ValidSROMFileName );
  igstkDeclareInputMacro( InValidSROMFileName );
  igstkDeclareInputMacro( ValidPartNumber);
  igstkDeclareInputMacro( InValidPartNumber);

  /** Port number that identifies this tool in the Tracker. */
  unsigned int m_PortNumberToBeSet;
  unsigned int m_PortNumber;

  /** SROM file */ 
  std::string m_SROMFileNameToBeSet;
  std::string m_SROMFileName;

  /** PartNumber */ 
  std::string m_PartNumberToBeSet;
  std::string m_PartNumber;

  /** boolean to indicate if the tracker tool initialized or not. */
  bool m_TrackerToolConfigured;

  /** boolean to indicate if a tool part number is specified or not. */
  bool m_PartNumberSpecified;

  /** boolean to indicate if SROM filename is specified or not. */
  bool m_SROMFileNameSpecified;

  /** boolean to indicate if a wireless or wired tracker tool
   * type is selected.  */
  bool m_WirelessTrackerToolSelected;

};  


} // namespace igstk


#endif  // __igstk_PolarisTrackerTool_h_
