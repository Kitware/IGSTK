/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTrackerToolNew.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkPolarisTrackerToolNew_h
#define __igstkPolarisTrackerToolNew_h

#include "igstkTrackerToolNew.h"

namespace igstk
{
/** \class PolarisTrackerToolNew
  * \brief A Polaris-specific TrackerTool class.
  *
  * This class is a for providing Polaris-specific functionality
  * for TrackerTools, and also to allow compile-time type enforcement
  * for other classes and functions that specifically require
  * an Polaris tool.
  *
  * \ingroup Tracker
  *
  */

class PolarisTrackerToolNew : public TrackerToolNew
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( PolarisTrackerToolNew, TrackerToolNew )

  /** Request setting the port number. The tool can only be attached to a
   * Tracker after its port number has been defined */
  void RequestSetPortNumber( unsigned int portNumber );

  /** Set the TrackerTool to be wired */
  void RequestSelectWiredTrackerTool(); 

  /** Set the TrackerTool to be wireless */
  void RequestSelectWirelessTrackerTool();

  /** Request set the SROM file name */
  void RequestSetSROMFileName( std::string filename );

  /** Request set the ToolId */
  void RequestSetToolId( std::string toolId );

  /** Check if the tracker tool is wireless type */
  bool IsToolWirelessType();

  /** Check if SROM file name specified*/
  bool IsSROMFileNameSpecified();

  /** Get SROM file name */
  std::string GetSROMFileName();

  /** Get tracker tool port number */
  unsigned int GetPortNumber();

protected:

  PolarisTrackerToolNew();
  ~PolarisTrackerToolNew();

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

  /** Set Tool Id*/ 
  void SetToolIdProcessing( );

  /** Report Invalid Tool Id filename specified */ 
  void ReportInValidToolIdSpecifiedProcessing( );

  PolarisTrackerToolNew(const Self&);   //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

  /** Get boolean variable to check if the tracker tool is 
   * initialized or not */
  virtual bool CheckIfTrackerToolIsConfigured(); 

  /** Get boolean variable to check if SROM file is specified */
  igstkGetMacro( SROMFileNameSpecified, bool );

  /** Get boolean variable to check if the tracker toolId is 
   * specified*/
  igstkGetMacro( ToolIdSpecified, bool );

  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( WirelessTrackerToolSelected );
  igstkDeclareStateMacro( WiredTrackerToolSelected );
  igstkDeclareStateMacro( WiredTrackerToolPortNumberSpecified );
  igstkDeclareStateMacro( WirelessTrackerToolSROMFileNameSpecified );
  igstkDeclareStateMacro( WiredTrackerToolSROMFileNameSpecified );
  igstkDeclareStateMacro( WirelessTrackerToolToolIdSpecified );
  igstkDeclareStateMacro( WiredTrackerToolToolIdSpecified );

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( SelectWiredTrackerTool );
  igstkDeclareInputMacro( SelectWirelessTrackerTool );
  igstkDeclareInputMacro( ValidPortNumber );
  igstkDeclareInputMacro( InValidPortNumber );
  igstkDeclareInputMacro( ValidSROMFileName );
  igstkDeclareInputMacro( InValidSROMFileName );
  igstkDeclareInputMacro( ValidToolId);
  igstkDeclareInputMacro( InValidToolId);

  /** Port number that identifies this tool in the Tracker. */
  unsigned int m_PortNumberToBeSet;
  unsigned int m_PortNumber;

  /** SROM file */ 
  std::string m_SROMFileNameToBeSet;
  std::string m_SROMFileName;

  /** ToolId */ 
  std::string m_ToolIdToBeSet;
  std::string m_ToolId;

  /** boolean to indicate if the tracker tool initialized or not*/
  bool m_TrackerToolConfigured;

  /** boolean to indicate if a tool id is specified or not */
  bool m_ToolIdSpecified;

  /** boolean to indicate if SROM filename is specified or not */
  bool m_SROMFileNameSpecified;

  /** boolean to indicate if a wireless or wired tracker tool
   * type is selected  */
  bool m_WirelessTrackerToolSelected;

};  


} // namespace igstk


#endif  // __igstk_PolarisTrackerToolNew_h_
