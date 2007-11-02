/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTrackerToolNew.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAuroraTrackerToolNew_h
#define __igstkAuroraTrackerToolNew_h

#include "igstkTrackerToolNew.h"

namespace igstk
{
/** \class AuroraTrackerTool
  * \brief An Aurora-specific TrackerTool class.
  *
  * This class is a for providing Aurora-specific functionality
  * for TrackerTools, and also to allow compile-time type enforcement
  * for other classes and functions that specifically require
  * an Aurora tool.
  *
  * \ingroup Tracker
  *
*/

class AuroraTrackerToolNew : public TrackerToolNew
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( AuroraTrackerToolNew, TrackerToolNew )

  /** Request setting the port number. The tool can only be attached to a
   * Tracker after its port number has been defined */
  void RequestSetPort( unsigned int portNumber );

  /** Request set the channel number */
  void RequestSetChannelNumber( unsigned int channelNumber );

  /** Request set the SROM file name */
  void RequestSetSROMFileName( std::string filename );

  /** Request set the ToolId */
  void RequestSetToolId( std::string toolId );

  /** Request select 5DOF tracker tool */
  void RequestSelect5DOFTrackerTool( );

  /** Request select 6DOF tracker tool */
  void RequestSelect6DOFTrackerTool( );

protected:

  AuroraTrackerToolNew();
  ~AuroraTrackerToolNew();

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

  /** Set Tool Id*/ 
  void SetToolIdProcessing( );

  /** Report Invalid Tool Id filename specified */ 
  void ReportInValidToolIdSpecifiedProcessing( );

  /** Report 5DOF tracker tool selected */
  void Report5DOFTrackerToolSelectedProcessing();

  /** Report 6DOF tracker tool selected */
  void Report6DOFTrackerToolSelectedProcessing();

  AuroraTrackerToolNew(const Self&);    //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

  /** Get boolean variable to check if the tracker tool is 
   * initialized or not */
  igstkGetMacro( TrackerToolInitialized, bool );

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
  igstkDeclareInputMacro( 5DOFTrackerToolSelected );
  igstkDeclareInputMacro( 6DOFTrackerToolSelected );
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

  bool m_TrackerToolInitialized;
  bool m_ToolIdSpecified;
  bool m_SROMFileNameSpecified;
  bool m_ChannelNumberSpecified;

  bool m_5DOFTrackerToolSelected;

};  


} // namespace igstk


#endif  // __igstk_AuroraTrackerToolNew_h_
