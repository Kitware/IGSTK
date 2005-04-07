/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAtamaiNDITracker.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkAtamaiNDITracker.h"
#include "vtkNDITracker.h"
#include "vtkTrackerTool.h"



namespace igstk
{

AtamaiNDITracker::AtamaiNDITracker(void) : Tracker()
{
  // create the VTK tracker instance
  m_VTKTracker = vtkNDITracker::New();

}

AtamaiNDITracker::~AtamaiNDITracker(void)
{
  m_VTKTracker->Delete();
}

void AtamaiNDITracker::AttemptToSetUpCommunicationProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "AtamaiNDITracker::AttemptToSetUpCommunicationProcessing called ...\n");
  this->m_pSetUpCommunicationResultInput = &m_CommunicationEstablishmentFailureInput;
#ifdef WIN32
  this->m_VTKTracker->SetSerialDevice("com1");
#else
  this->m_VTKTracker->SetSerialDevice("/dev/ttyS0");
#endif
  //if (How to test if the serial device opened succeeded)
  this->m_VTKTracker->SetBaudRate(9600);
  this->m_pSetUpCommunicationResultInput = &m_CommunicationEstablishmentSuccessInput;
}


void AtamaiNDITracker::AttemptToSetUpToolsProcessing( void )
{
  this->m_pActivateToolsResultInput = &(this->m_ToolsActivationFailureInput);
  igstkLogMacro( Logger::DEBUG, "AtamaiNDITracker::AttemptToSetUpToolsProcessing called ...\n");
//  this->m_VTKTracker->EnableToolPorts();
  int numTools = this->m_VTKTracker->GetNumberOfTools();
  std::cout << "AtamaiNDITracker::Number of tools = " << numTools << std::endl;
  (this->m_Tools).clear();
  for(int i=0; i< numTools; i++)
  {
    m_Tools.push_back(this->m_VTKTracker->GetTool( i ));
  }
//  if (success???)
  this->m_pActivateToolsResultInput = &(this->m_ToolsActivationSuccessInput);
}

void AtamaiNDITracker::AttemptToStartTrackingProcessing( void )
{
  this->m_pStartTrackingResultInput = &(this->m_StartTrackingFailureInput);
  this->m_VTKTracker->StartTracking();
  //if (success)
  this->m_pStartTrackingResultInput = &(this->m_StartTrackingSuccessInput);
}


void AtamaiNDITracker::AttemptToStopTrackingProcessing( void )
{
  this->m_pStopTrackingResultInput = &(this->m_StopTrackingFailureInput);
  this->m_VTKTracker->StopTracking();
  //if (success)
  this->m_pStopTrackingResultInput = &(this->m_StopTrackingSuccessInput);
}


void AtamaiNDITracker::UpdateStatusProcessing( void )
{
  this->m_VTKTracker->Update();
}


void AtamaiNDITracker::ResetTrackingProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "AtamaiNDITracker::ResetTrackingProcessing called ...\n");
}

void AtamaiNDITracker::DisableCommunicationProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "AtamaiNDITracker::DisableCommunicationProcessing called ...\n");
}

void AtamaiNDITracker::DisableToolsProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "AtamaiNDITracker::DisableToolsProcessing called ...\n");
}

}

