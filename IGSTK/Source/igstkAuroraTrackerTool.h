/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTrackerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAuroraTrackerTool_h
#define __igstkAuroraTrackerTool_h

#include "igstkTrackerTool.h"

namespace igstk
{

class AuroraTracker;

/** \class AuroraTrackerTool
  * \brief An Aurora-specific TrackerTool class.
  *
  * This class provides Aurora-specific functionality for TrackerTools, and
  * also allows compile-time type enforcement for other classes and functions
  * that specifically require an Aurora tool.
  *
  * \ingroup Tracker
  *
  */

class AuroraTrackerTool : public TrackerTool
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( AuroraTrackerTool, TrackerTool )

  /** Request setting the port number. The tool can only be attached to a
   * Tracker after its port number has been defined */
  void RequestSetPortNumber( unsigned int portNumber );

  /** Request set the channel number */
  void RequestSetChannelNumber( unsigned int channelNumber );

  /** Request set the SROM file name */
  void RequestSetSROMFileName( const std::string & filename );

  /** Request set the ToolId */
  void RequestSetToolId( const std::string & toolId );

  /** Request select 5DOF (five degrees of freedom) tracker tool */
  void RequestSelect5DOFTrackerTool( );

  /** Request select 6DOF (six degrees of freedom) tracker tool */
  void RequestSelect6DOFTrackerTool( );

  /** The "RequestAttachToTracker" method attaches the tracker tool to a
   * tracker. */
  virtual void RequestAttachToTracker( AuroraTracker * );

  /** Check if SROM file name specified. */
  bool IsSROMFileNameSpecified() const;

  /** Get SROM file name */
  igstkGetStringMacro( SROMFileName );

  /** Get tracker tool port number */
  igstkGetMacro( PortNumber, unsigned int );

  /** Check if the tracker tool selected is 5DOF or 6DOF  
   * (five or six degrees of freedom) */
  bool IsTrackerTool5DOF() const;
 
protected:

  AuroraTrackerTool();
  virtual ~AuroraTrackerTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Set port number */ 
  void SetPortNumberProcessing( );

  /** Report Invalid port number specified */ 
  void ReportInValidPortNumberSpecifiedProcessing( );

  /** Set channel number */ 
  void SetChannelNumberProcessing( );

  /** Report Invalid channel number specified */ 
  void ReportInValidChannelNumberSpecifiedProcessing( );

  /** Set SROM filename */ 
  void SetSROMFileNameProcessing( );

  /** Report Invalid SROM filename specified */ 
  void ReportInValidSROMFileSpecifiedProcessing( );

  /** Set Tool Id. */ 
  void SetToolIdProcessing( );

  /** Report Invalid Tool Id filename specified */ 
  void ReportInValidToolIdSpecifiedProcessing( );

  /** Report 5DOF tracker tool selected */
  void Report5DOFTrackerToolSelectedProcessing();

  /** Report 6DOF tracker tool selected */
  void Report6DOFTrackerToolSelectedProcessing();

  AuroraTrackerTool(const Self&);    //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

  /** Get boolean variable to check if the tracker tool is 
   * configured or not */
  virtual bool CheckIfTrackerToolIsConfigured(); 

  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( 5DOFTrackerToolSelected );
  igstkDeclareStateMacro( 6DOFTrackerToolSelected );
  igstkDeclareStateMacro( 5DOFTrackerToolPortNumberSpecified );
  igstkDeclareStateMacro( 6DOFTrackerToolPortNumberSpecified );
  igstkDeclareStateMacro( 5DOFTrackerToolChannelNumberSpecified );
  igstkDeclareStateMacro( 5DOFTrackerToolSROMFileNameSpecified );
  igstkDeclareStateMacro( 6DOFTrackerToolSROMFileNameSpecified );
  igstkDeclareStateMacro( 5DOFTrackerToolToolIdSpecified );
  igstkDeclareStateMacro( 6DOFTrackerToolToolIdSpecified );


  /** Inputs to the State Machine */
  igstkDeclareInputMacro( Select5DOFTrackerTool );
  igstkDeclareInputMacro( Select6DOFTrackerTool );
  igstkDeclareInputMacro( ValidPortNumber );
  igstkDeclareInputMacro( InValidPortNumber );
  igstkDeclareInputMacro( ValidChannelNumber );
  igstkDeclareInputMacro( InValidChannelNumber );
  igstkDeclareInputMacro( ValidSROMFileName );
  igstkDeclareInputMacro( InValidSROMFileName );
  igstkDeclareInputMacro( ValidToolId);
  igstkDeclareInputMacro( InValidToolId);

  /** Port number that identifies this tool in the Tracker. */
  unsigned int m_PortNumberToBeSet;
  unsigned int m_PortNumber;

  /** Channel number that identifies this tool in the Tracker. */
  unsigned int m_ChannelNumberToBeSet;
  unsigned int m_ChannelNumber;

  /** SROM file */ 
  std::string m_SROMFileNameToBeSet;
  std::string m_SROMFileName;

  /** ToolId */ 
  std::string m_ToolIdToBeSet;
  std::string m_ToolId;

  bool m_TrackerToolConfigured;
  bool m_ToolIdSpecified;
  bool m_SROMFileNameSpecified;
  bool m_ChannelNumberSpecified;

  bool m_FiveDOFTrackerToolSelected;

};  


} // namespace igstk


#endif  // __igstk_AuroraTrackerTool_h_
