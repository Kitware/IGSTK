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
#include <vtkNDITracker.h>
#include <vtkTrackerTool.h>
#include <vtkCallbackCommand.h>
#include <vtkMath.h>
#include <vtkTransform.h>

namespace igstk
{

// a VTK error callback function
void AtamaiNDITrackerErrorCallback(vtkObject *vtkobj, unsigned long,
                                   void *itkobj, void *)
{
  AtamaiNDITracker *tracker = static_cast<AtamaiNDITracker *>(itkobj);
  tracker->m_VTKError = 1;
} 


AtamaiNDITracker::AtamaiNDITracker(void) : Tracker()
{
  // create the VTK tracker instance and an error callback
  m_VTKTracker = vtkNDITracker::New();
  m_VTKErrorCommand = vtkCallbackCommand::New();
  m_VTKErrorCommand->SetCallback(AtamaiNDITrackerErrorCallback);
  m_VTKErrorCommand->SetClientData(this);
  m_VTKTracker->AddObserver(vtkCommand::ErrorEvent, m_VTKErrorCommand);
  m_VTKError = 0;

}


AtamaiNDITracker::~AtamaiNDITracker(void)
{
  m_VTKTracker->Delete();
  m_VTKErrorCommand->Delete();
}


AtamaiNDITracker::ResultType
AtamaiNDITracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "AtamaiNDITracker::AttemptToSetUpCommunicationProcessing called ...\n");

  // clear the error indicator
  m_VTKError = 0;

  // set the com port and baud rate
#ifdef WIN32
  m_VTKTracker->SetSerialDevice("COM1:");
#else
  m_VTKTracker->SetSerialDevice("/dev/ttyS0");
#endif
  m_VTKTracker->SetBaudRate(115200);

  // probe the com port to see if we can talk to the device
  if (m_VTKTracker->Probe())
    {
    return SUCCESS;
    }

  // if probe failed, try sending a serial break to reset the device
  char *reply = m_VTKTracker->Command(0); // null command causes reset
  if (strncmp(reply,"RESET",5) == 0 && m_VTKError == 0)
    {
    return SUCCESS;
    }
  
  return FAILURE;
}


AtamaiNDITracker::ResultType
AtamaiNDITracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, "AtamaiNDITracker::AttemptToSetUpToolsProcessing called ...\n");

  m_VTKError = 0;

  // there should be some code right here to load any SROMS that are needed
  // for (int j = 0; j < numSroms; j++)
  //   { 
  //   m_VTKTracker->LoadVirtualSROM(j, sromFilename[j]);
  //   }

  // the vtkNDITracker doesn't enable the tools until you call StarTracking
  m_VTKTracker->StartTracking();

  // get the maximum number of tools that this device can support
  int maxTools = m_VTKTracker->GetNumberOfTools();
  // count the number of ports that aren't empty
  int numTools = 0;
  m_VTKTrackerTools.clear();
  this->ClearPorts();
  int portEnabled[VTK_NDI_NTOOLS];
  m_VTKTracker->GetPortEnabled(portEnabled);
  for (int i = 0; i < maxTools; i++)
    { 
      if( portEnabled[i] )
      {
      m_VTKTrackerTools.push_back(m_VTKTracker->GetTool(i));
      numTools++;
      TrackerToolPointer tool = TrackerToolType::New();
      TrackerPortPointer port = TrackerPortType::New();
      port->AddTool(tool);
      this->AddPort(port);
      }
    }

  // stop tracking, since we only wanted to activate the tools
  m_VTKTracker->StopTracking();

  if (m_VTKError != 0)
    {
    return FAILURE;
    }

  return SUCCESS;
}

AtamaiNDITracker::ResultType
AtamaiNDITracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, "AtamaiNDITracker::AttemptToStartTrackingProcessing called ...\n");

  m_VTKError = 0;
  m_VTKTracker->StartTracking();
  if (m_VTKError != 0)
    {
    return FAILURE;
    }

  return SUCCESS;
}


AtamaiNDITracker::ResultType
AtamaiNDITracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, "AtamaiNDITracker::StopTrackingProcessing called ...\n")
  m_VTKError = 0;
  m_VTKTracker->StopTracking();
  if (m_VTKError != 0)
    {
    return FAILURE;
    }

  return SUCCESS;
}


AtamaiNDITracker::ResultType
AtamaiNDITracker::InternalUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "AtamaiNDITracker::UpdateStatusProcessing called ...\n");

  m_VTKError = 0;
  m_VTKTracker->Update();
  if (m_VTKError != 0)
    {
    return FAILURE;
    }

  // loop through m_VTKTrackerTools, and for each tool get the vtkTransform,
  // convert to TransformType, and then call SetToolTransform()
  int numTools = m_VTKTrackerTools.size();

  // a vector full of zeros
  double origin[3];
  origin[0] = 0.0;
  origin[1] = 0.0;
  origin[2] = 0.0;

  for (int i = 0; i < numTools; i++)
    {
    vtkTrackerTool *pTool = m_VTKTracker->GetTool(i);

    if ( pTool->IsMissing() )
      {
      igstkLogMacro( DEBUG, "Tool Missing...\n");
      continue;
      }

    if ( pTool->IsOutOfView() )
      {
      igstkLogMacro( DEBUG, "Tool out of View...\n");
      continue;
      }

    vtkTransform *vtktrans = m_VTKTrackerTools[i]->GetTransform();
    // here is the most direct way of getting the position and orientation
    double position[3];
    double orientation[3][3];
    vtktrans->Update();
    // for a linear transform, this just pulls the values from the 4x4 matrix
    vtktrans->InternalTransformDerivative(origin, position, orientation);
    double quaternion[4];
    vtkMath::Matrix3x3ToQuaternion(orientation, quaternion);
    
    // create the transform
    TransformType transform;

    typedef TransformType::VectorType TranslationType;
    TranslationType translation;
    translation[0] = position[0];
    translation[1] = position[1];
    translation[2] = position[2] + 1900; // correction for origin of Polaris in mm.

    typedef TransformType::VersorType RotationType;
    RotationType rotation;
    rotation.Set(quaternion[0],quaternion[1],quaternion[2],quaternion[3]);

    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = 0.5; // actually it varies

    typedef TransformType::TimePeriodType TimePeriodType;
    TimePeriodType validityTime = 100.0;

    transform.SetToIdentity(validityTime);
    transform.SetTranslationAndRotation(translation, rotation, errorValue,
                                        validityTime);

    this->SetToolTransform(i,0,transform);
    }

  return SUCCESS;
}


AtamaiNDITracker::ResultType
AtamaiNDITracker::InternalReset( void )
{
  igstkLogMacro( DEBUG, "AtamaiNDITracker::ResetTrackingProcessing called ...\n");

  if (m_VTKTracker->IsTracking())
    {
    m_VTKTracker->StopTracking();
    }
  // send a serial break to force the device to reset
  m_VTKTracker->Command(0);

  return SUCCESS;
}


AtamaiNDITracker::ResultType
AtamaiNDITracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "AtamaiNDITracker::DisableCommunicationProcessing called ...\n");
  // nothing to do: communication is disabled when vtkTracker isn't tracking
  return SUCCESS;
}


AtamaiNDITracker::ResultType
AtamaiNDITracker::InternalDeactivateTools( void )
{
  igstkLogMacro( DEBUG, "AtamaiNDITracker::DisableToolsProcessing called ...\n");
  // nothing to do: tools are disabled when vtkTracker isn't tracking
  return SUCCESS;
}

/** Print Self function */
void AtamaiNDITracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

}

