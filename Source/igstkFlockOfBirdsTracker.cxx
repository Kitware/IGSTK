/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFlockOfBirdsTracker.cxx
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
// Warning about: identifier was truncated to '255' characters in the
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkFlockOfBirdsTracker.h"


namespace igstk
{

/** Constructor: Initializes all internal variables. */
FlockOfBirdsTracker::FlockOfBirdsTracker(void):m_StateMachine(this)
{
  m_NumberOfTools = 0;
  for (unsigned int i = 0; i < NumberOfPorts; i++)
    {
    this->m_PortEnabled[i] = 0;
    this->m_PortHandle[i] = 0;
    }

  for (unsigned int j = 0; j < NumberOfPorts; j++)
    { 
    FlockOfBirdsTrackerToolPointer tool = FlockOfBirdsTrackerToolType::New();
    TrackerPortPointer port = TrackerPortType::New();
    port->AddTool(tool);
    this->AddPort(port);
    }

  this->SetThreadingEnabled( true );

  m_BufferLock = itk::MutexLock::New();

  m_Mode = CONTINUOUS;
  m_Type = POSQUATER;
  m_PositionScaling = _36_INCHES;
}

/** Destructor */
FlockOfBirdsTracker::~FlockOfBirdsTracker(void)
{
}

/** Set the communication object, it will be initialized as necessary
  * for use with the FlockOfBirds */
void FlockOfBirdsTracker::SetCommunication( CommunicationType *communication )
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker:: Entered SetCommunication ...\n");
  m_Communication = communication;
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker:: Exiting SetCommunication ...\n"); 
}

/** Open communication with the tracking device. */
FlockOfBirdsTracker::ResultType FlockOfBirdsTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::InternalOpen called ...\n");

  this->SetMode(CONTINUOUS);
  this->SetType(POSQUATER);
  return SUCCESS;
}

/** Close communication with the tracking device. */
FlockOfBirdsTracker::ResultType FlockOfBirdsTracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::InternalClose called ...\n");
  return SUCCESS;
}

/** Activate the tools attached to the tracking device. */
FlockOfBirdsTracker::ResultType FlockOfBirdsTracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::InternalActivateTools called ...\n");
  // load any SROMS that are needed
  for (unsigned int i = 0; i < NumberOfPorts; i++)
    { 
    if (!m_SROMFileNames[i].empty())
      {
      this->LoadVirtualSROM(i, m_SROMFileNames[i]);
      }
    }

  this->EnableToolPorts();

  m_NumberOfTools = 0;

  for(unsigned int i = 0; i < NumberOfPorts; i++)
    { 
    if( this->m_PortEnabled[i] )
      {
      m_NumberOfTools++;
      }
    }

  return SUCCESS;
}

/** Deactivate the tools attached to the tracking device. */
FlockOfBirdsTracker::ResultType FlockOfBirdsTracker::InternalDeactivateTools( void )
{
  for (unsigned int i = 0; i < NumberOfPorts; i++)
    { 
    if (!m_SROMFileNames[i].empty() &&
        this->m_PortHandle[i] != 0)
      {
      this->ClearVirtualSROM(i);
      }
    }

  this->DisableToolPorts();

  return SUCCESS;
}

/** Put the tracking device into tracking mode. */
FlockOfBirdsTracker::ResultType FlockOfBirdsTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::InternalStartTracking called ...\n");  
  return SUCCESS;
}

/** Take the tracking device out of tracking mode. */
FlockOfBirdsTracker::ResultType FlockOfBirdsTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::InternalStopTracking called ...\n");
  return SUCCESS;
}

/** Reset the tracking device to put it back to its original state. */
FlockOfBirdsTracker::ResultType FlockOfBirdsTracker::InternalReset( void )
{
  return SUCCESS;
}

/** Set the type of operation of the Flock of Birds */
FlockOfBirdsTracker::ResultType
FlockOfBirdsTracker::SetType(const FoBType type) 
{
  char cmd;

  switch (type) 
    {
    case POS:
      cmd = 86;
      break;
    case ANGLE:
      cmd = 87;
      break;
    case MATRIX:
      cmd = 88;
      break;
    case POSANGLE:
      cmd = 89;
      break;
    case POSMATRIX:
      cmd = 90;
      break;
    case QUATER:
      cmd = 92;
      break;
    case POSQUATER:
      cmd = 93;
      break;
    default:
     igstkLogMacro( DEBUG, "FlockOfBirdsTracker::SetType() Unknown Type ...\n");
     return FAILURE;
    }

  if(m_Communication->Write(&cmd,sizeof(char)) != igstk::Communication::SUCCESS)
    {
    return FAILURE;
    }
 
  m_Type = type;
  return SUCCESS;
}

/** Set the mode of operation of the Flock of Birds */
FlockOfBirdsTracker::ResultType
FlockOfBirdsTracker::SetMode(const FoBMode mode) 
{
  char cmd;

  switch (mode) 
    {
    case STREAM:
      cmd = '@';
      break;
    default:
      cmd = 'B';
      break;
    }

  if(m_Communication->Write(&cmd,sizeof(char)) != igstk::Communication::SUCCESS)
    {
    return FAILURE;
    }

  m_Mode = mode;
  return SUCCESS;
}

/** Set the position scaling of the Flock of Birds */
FlockOfBirdsTracker::ResultType
FlockOfBirdsTracker::SetPositionScaling(const FoBPositionScaling scaling)
{
  m_PositionScaling = scaling;
  return SUCCESS;
}

/** Update the status and the transforms for all TrackerTools. */
FlockOfBirdsTracker::ResultType FlockOfBirdsTracker::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::InternalUpdateStatus called ...\n");

  unsigned int recordSize = 2*FoBDataSize[m_Type];
  short * localData = new short[recordSize];
  short* data = (short*)localData;
  char command = 'B';
  unsigned int numberErrors = 0;

  if (m_Mode != STREAM) 
    {
    if(m_Communication->Write(&command,sizeof(char)) != igstk::Communication::SUCCESS)
      {
      igstkLogMacro( CRITICAL,
                    "Cannot write command to serial port.\n" );
      return FAILURE;
      }
    }

  unsigned int bytesRead;

  if (m_Communication->Read((char*)data,recordSize,bytesRead) != igstk::Communication::SUCCESS) 
    {
    igstkLogMacro( CRITICAL,
                  "Cannot read command from serial port.\n" );

    if(m_Mode == STREAM) 
      {
      numberErrors++;
      if (numberErrors > 10)
        {
        this->SetMode(POINTM);
        }
      }
    return FAILURE;
    }

  localData = (short*)data;

  unsigned int i;
  for (i = 0; i < FoBDataSize[m_Type]; i++) 
    {
    *localData++ = (short)((((short)(* (unsigned char *) localData) & 0x7F) |
       (short)(* ((unsigned char *) localData + 1)) << 7)) << 2;
    }

  float* converted = new float[recordSize];

  // Convert the data
  float posk;
  switch (m_PositionScaling) 
    {
    case _36_INCHES:
      posk = POSK36;
      break;
    
    case _72_INCHES:
      posk = POSK72;
      break; 
    //case _144_INCHES:  posk = POSK144; break;
    }

  switch (m_Type) 
    {
    case POS:
      for( i = 0; i < 3; i++ ) 
        {
        converted[i] = (float)((float)data[i] * posk);
        }
      break;
    
    case POSANGLE:
      for( i = 0; i < 3; i++ ) 
        {
        converted[i] = (float)((float)data[i] * posk);
        }
      for( i = 3; i < 6; i++ ) 
        {
        converted[i] = (float)((float)data[i] * ANGK);
        }
      break;
    
    case ANGLE:
      for( i = 0; i < 3; i++ ) 
        {
        converted[i] = (float)((float)data[i] * ANGK);
        }
      break;
    
    case MATRIX:
      for( i = 0; i < 9; i++ ) 
        {
        converted[i] = (float)((float)data[i] / FTW);
        }
      break;

    case POSMATRIX:
      for( i = 0; i < 3; i++ )
        {
        converted[i] = (float)((float)data[i] * posk);
        }
      for( i = 3; i < 12; i++ ) 
        {
        converted[i] = (float)((float)data[i] / FTW);
        }
      break;

    case QUATER:
      for( i = 0; i < 4; i++ )
        {
        converted[i] = (float)((float)data[i] / FTW);
        }
      break;

    case POSQUATER:
      for( i = 0; i < 3; i++ )
        {
        converted[i] = (float)((float)data[i] * posk);
        }
      for( i = 3; i < 7; i++ )
        {
        converted[i] = (float)((float)data[i] / FTW);
        }
      break;

    default:
      return FAILURE;
    }

  // create the transform
  TransformType transform;

  typedef TransformType::VectorType TranslationType;
  TranslationType translation;
  
  //convert inch to mm
  for(i=0;i<3;i++)
    {
    converted[i] *= (float)I_TO_MM;
    }

  m_BufferLock->Lock();

  translation[0] = converted[0];
  translation[1] = converted[1];
  translation[2] = converted[2];

  typedef TransformType::VersorType RotationType;
  RotationType rotation;
  rotation.Set(converted[3],-converted[6],converted[5],converted[4]);

  typedef TransformType::TimePeriodType TimePeriodType;
  const TimePeriodType validityTime = 100.0;

  transform.SetToIdentity(validityTime);
  transform.SetTranslationAndRotation(translation, rotation, 0,
                                      validityTime);

  delete [] localData;
  delete [] converted;

  this->SetToolTransform(0, 0, transform);

  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Update the m_StatusBuffer and the transforms. 
    This function is called by a separate thread. */
FlockOfBirdsTracker::ResultType FlockOfBirdsTracker::InternalThreadedUpdateStatus( void )
{
  //igstkLogMacro( DEBUG, "FlockOfBirdsTracker::InternalThreadedUpdateStatus "
  //               "called ...\n");

  return SUCCESS;
}

/** Specify an SROM file to be used with a passive or custom tool. */
void FlockOfBirdsTracker::AttachSROMFileNameToPort( const unsigned int portNum,
                                               std::string fileName )
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::AttachSROMFileNameToPort called..\n");

  // the first 3 ports are active, don't allow SROMS for them
  if ( (portNum >= 3) && (portNum <= NumberOfPorts) )
    {
    m_SROMFileNames[portNum] = fileName;
    }
}

/** Load a virtual SROM, given the file name of the ROM file */
bool FlockOfBirdsTracker::LoadVirtualSROM( const unsigned int tool,
                                      const std::string SROMFileName) 
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::LoadVirtualSROM called...\n");

  if (this->m_PortHandle[tool] != 0)
    {
    igstkLogMacro( WARNING, "An SROM has already been loaded for tool "
                   << tool << "\n");
    return false;
    }

  std::ifstream sromFile;
  sromFile.open(SROMFileName.c_str(), std::ios::binary );

  if (!sromFile.is_open())
    {
    igstkLogMacro( WARNING, "FlockOfBirdsTracker::LoadVirtualSROM: couldn't "
                   "find SROM file " << SROMFileName << " ...\n");
    return false;
    }

  return true;
}

/** Clear a previously loaded SROM. */
void FlockOfBirdsTracker::ClearVirtualSROM(const unsigned int tool)
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::ClearVirtualSROM called...\n");
  this->m_PortEnabled[tool] = 0;
}

/** Enable all tool ports that are occupied. */
void FlockOfBirdsTracker::EnableToolPorts()
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::EnableToolPorts called...\n");

  for (unsigned int port = 0; port < NumberOfPorts; port++)
    {
    this->m_PortEnabled[port] = 1;
    }

}

/** Disable all tool ports. */
void FlockOfBirdsTracker::DisableToolPorts( void )
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::DisableToolPorts called...\n");
  
  for (unsigned int port = 0; port < NumberOfPorts; port++)
    {
    this->m_PortEnabled[port] = 0;
    }
}


/** Print Self function */
void FlockOfBirdsTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  unsigned int i;

  for( i = 0; i < NumberOfPorts; ++i )
    {
    os << indent << "Port " << i << " Enabled: " << m_PortEnabled[i] 
       << std::endl;
    }
  for( i = 0; i < NumberOfPorts; ++i )
    {
    os << indent << "Port " << i << " Handle: " << m_PortHandle[i] 
       << std::endl;
    }
  os << indent << "Number of tools: " << m_NumberOfTools << std::endl;
  for( i = 0; i < NumberOfPorts; ++i )
    {
    os << indent << "SROM filename " << i << " : " << m_SROMFileNames[i]
       << std::endl;
    }
}


} // end of namespace igstk

