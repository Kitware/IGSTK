/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTracker.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in the
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkAuroraTracker.h"
#include "igstkNDICommandInterpreter.h"

#include <iostream>
#include <fstream>

namespace igstk
{

/** Constructor: Initializes all internal variables. */
AuroraTracker::AuroraTracker(void):m_StateMachine(this)
{
  for ( unsigned int i=0; i< MAX_WIRED_PORT_NUMBER; i++)
  {
    m_HasSpliter[i] = 0;
  }
  m_SimulatedTestMaintainCoverage = false;
}

/** Destructor */
AuroraTracker::~AuroraTracker(void)
{
}

/** Verify tracker tool information. */
AuroraTracker::ResultType AuroraTracker
::VerifyTrackerToolInformation( const TrackerToolType * trackerTool ) 
{
  //   Verify that 
  //   
  //   1) the tracker tool information provided by the user matches with
  //   the information available in the SROM file.
  //   
  //   
  //   To get the tool information
  //       == SROM file has to be loaded and port handle created.
  //   
  igstkLogMacro( DEBUG,
    "AuroraTracker::VerifyTrackerToolInformation called ...\n");

  TrackerToolType * trackerToolNonConst = 
                    const_cast<TrackerToolType*>(trackerTool);

  AuroraTrackerToolType * auroraTrackerTool = 
             dynamic_cast< AuroraTrackerToolType * > ( trackerToolNonConst );   

  if ( auroraTrackerTool == NULL )
    {
    igstkLogMacro ( DEBUG, "Tracker tool probably not aurora type " );
    return FAILURE;
    } 

  bool SROMFileSpecified  = auroraTrackerTool->IsSROMFileNameSpecified();

  CommandInterpreterType::Pointer commandInterpreter = 
    this->GetCommandInterpreter();


  
  // free ports that are waiting to be freed
  commandInterpreter->PHSR(CommandInterpreterType::NDI_STALE_HANDLES);
  unsigned int numberOfHandles = commandInterpreter->GetPHSRNumberOfHandles();
  unsigned int tool;
  for (tool = 0; tool < numberOfHandles; tool++)
    {
    const int ph = commandInterpreter->GetPHSRHandle(tool);
    commandInterpreter->PHF(ph);

    // if failed to release handle, print error but continue on
    this->CheckError(commandInterpreter);
    }

  // port handle
  int ph;

  // search ports with uninitialized handles
  bool foundTool = false;

  for( unsigned int safetyCount = 0; safetyCount < NUMBER_OF_ATTEMPTS; 
                                                               safetyCount++)
    {
    commandInterpreter->PHSR(
    CommandInterpreterType::NDI_UNINITIALIZED_HANDLES);

    if (this->CheckError(commandInterpreter) == FAILURE)
      {
      igstkLogMacro( WARNING, 
         "igstk::AuroraTracker::Error searching for uninitialized ports \n");
      return FAILURE;
      }

    unsigned int ntools = commandInterpreter->GetPHSRNumberOfHandles();
    igstkLogMacro( INFO, "Uninitialized number of handles found: " 
                         << ntools << "\n" );

    //if no tools are found, attempt again
    if ( ntools == 0 )
      {
      continue;
      }

    for( unsigned int toolNumber = 0; toolNumber < ntools; toolNumber++ )
      {
      ph = commandInterpreter->GetPHSRHandle( toolNumber );

      // Get port handle information
      commandInterpreter->PHINF(ph, CommandInterpreterType::NDI_PORT_LOCATION);

      // get the physical port identifier
      char location[512];
      commandInterpreter->GetPHINFPortLocation(location);

      // physical port number
      unsigned int port = 0;

      if (location[9] == '0') // wired tool
        {
        unsigned int ndiport = (location[10]-'0')*10 + (location[11]-'0');

        if (ndiport > 0 && ndiport <= MAX_WIRED_PORT_NUMBER)
          {
          port = ndiport - 1;
          // check if the port number specified 
          if ( port != auroraTrackerTool->GetPortNumber() )
            {
            //this port doesn't match with what is specified by the user
            //check the other uninitialized ports found 
            igstkLogMacro( DEBUG, "Detected Port number: " 
                           << port 
                           << " doesn't match with what is provided " 
                           << auroraTrackerTool->GetPortNumber() << "\n");
            }
          else
            {
            foundTool = true;
            break;
            }
          }
        }
      }

    if( foundTool )
      {
      break;
      }
    }

  // No new port handle found, we will exit with failure here
  // But if we have found a spliter in previous operation, meaning there will
  // be two tools attached to the same port, we should go ahead and use
  // those port handle
  if( !foundTool && !m_HasSpliter[auroraTrackerTool->GetPortNumber()] &&
      !m_SimulatedTestMaintainCoverage)
  {
  igstkLogMacro(CRITICAL, 
    "Uninitialized port that corresponds to what is specified: "
    << auroraTrackerTool->GetPortNumber() << " not found");
  return FAILURE;
  }

  // if SROM file is specified then, override the
  // SROM image file on the hardware using PVWR.
  if( SROMFileSpecified ) 
    {
    std::ifstream sromFile; 
    std::string SROMFileName = auroraTrackerTool->GetSROMFileName();
    sromFile.open(SROMFileName.c_str(), std::ios::binary );

    if (!sromFile.is_open())
      {
      igstkLogMacro( WARNING, "AuroraTracker::Failing to open"
                   << SROMFileName << " ...\n");
      return FAILURE;
      }

    // most SROM files don't contain the whole 1024 bytes, they only
    // contain whatever is necessary, so the rest should be filled with zero
    const unsigned int SROM_FILE_DATA_SIZE = 1024;
    char data[SROM_FILE_DATA_SIZE]; 
    memset( data, 0, SROM_FILE_DATA_SIZE );
    sromFile.read( data, SROM_FILE_DATA_SIZE );
    sromFile.close();

    for ( unsigned int i = 0; i < SROM_FILE_DATA_SIZE; i += 64)
      {
      // holds hexidecimal data to be sent to device
      char hexbuffer[129]; 

      // convert data to hexidecimal and write to virtual SROM in
      // 64-byte chunks
      commandInterpreter->HexEncode(hexbuffer, &data[i], 64);
      commandInterpreter->PVWR(ph, i, hexbuffer);
      }
    }

  if ( !m_HasSpliter[auroraTrackerTool->GetPortNumber()] )
  {  
    // initialize the port 
    commandInterpreter->PINIT(ph);

    if (this->CheckError(commandInterpreter) == SUCCESS)
      {
      igstkLogMacro(INFO, "Port handle initialized successfully \n");
      }
    else
      {
      igstkLogMacro(CRITICAL, "Failure initializing the port \n ");
      }
  }
  
  // Search for port handle for the second channel, see if there is a spliter
  int ph2;

  // If it is a 5DOF tool and the port has not been initialized ()
  if ( auroraTrackerTool->IsTrackerTool5DOF()  && 
                          !m_HasSpliter[auroraTrackerTool->GetPortNumber()] )
    {
    // search for splits
    commandInterpreter->PHSR( CommandInterpreterType::NDI_UNENABLED_HANDLES);
    if (this->CheckError(commandInterpreter) == FAILURE)
      {
      igstkLogMacro( WARNING, 
        "igstk::AuroraTracker::Error searching for channel splits \n");
      return FAILURE;
      }

    unsigned int numberOfTools = commandInterpreter->GetPHSRNumberOfHandles();
    for( unsigned int toolNumber = 0; toolNumber < numberOfTools; toolNumber++ )
    {
      ph2 = commandInterpreter->GetPHSRHandle( toolNumber );
      
      if ( ph2 == ph )// skip ph (channel 0);
      {
        continue;
      }
      // Get port handle information
      commandInterpreter->PHINF(ph2, CommandInterpreterType::NDI_PORT_LOCATION);

      // get the physical port identifier
      char location[512];
      commandInterpreter->GetPHINFPortLocation(location);

      // physical port number
      unsigned int port = 0;

      if (location[9] == '0') // wired tool
      {
        unsigned int ndiport = (location[10]-'0')*10 + (location[11]-'0');

        if (ndiport > 0 && ndiport <= MAX_WIRED_PORT_NUMBER)
        {
          port = ndiport - 1;
          // check if the port number is the same with ph
          if ( port == auroraTrackerTool->GetPortNumber() )
          {
            // found two port handles on the same port
            // There is a channel spliter 
            igstkLogMacro( DEBUG, "Found channel split on port:" 
                                                           << port << "\n" );
            m_HasSpliter[port]       = 1;
            m_SpliterHandle[port][0] = ph;
            m_SpliterHandle[port][1] = ph2;
            // Use the channel as requested
            if (auroraTrackerTool->GetChannelNumber() == 1)
            {
              ph = ph2;
            }
            break;
          }
        }
      }
    }

    }
    else if ( auroraTrackerTool->IsTrackerTool5DOF() )
    {
      unsigned int port = auroraTrackerTool->GetPortNumber();
      unsigned int channel = auroraTrackerTool->GetChannelNumber();
      
      // There are only two channels 0 and 1, use 1 if channel is not set
      if (channel > MAX_CHANNEL_NUMBER)
      {
      channel = MAX_CHANNEL_NUMBER;
      }

    ph = m_SpliterHandle[port][channel];
    }
  
  commandInterpreter->PHINF(ph, CommandInterpreterType::NDI_BASIC);

  // tool identity and type information
  char identity[512];
  commandInterpreter->GetPHINFToolInfo(identity);
  igstkLogMacro(INFO, "Tool Information: " << identity << "\n"); 

  // use tool type information to figure out mode for enabling
  int mode = CommandInterpreterType::NDI_DYNAMIC;

  if (identity[1] == CommandInterpreterType::NDI_TYPE_BUTTON)
    { // button-box or foot pedal
    mode = CommandInterpreterType::NDI_BUTTON_BOX;
    }
  else if (identity[1] == CommandInterpreterType::NDI_TYPE_REFERENCE)
    { // reference
    mode = CommandInterpreterType::NDI_STATIC;
    }

  // enable the tool
  commandInterpreter->PENA(ph, mode);

  // print any warnings
  this->CheckError(commandInterpreter);

  //tool information
  commandInterpreter->PHINF(ph,
                                CommandInterpreterType::NDI_PORT_LOCATION |
                                CommandInterpreterType::NDI_PART_NUMBER |
                                CommandInterpreterType::NDI_BASIC );

  if (this->CheckError(commandInterpreter) == FAILURE)
    {
    igstkLogMacro(CRITICAL, "Failure accessing tool information \n" );
    return FAILURE;
    }

  // get the physical port identifier
  char location[512];
  commandInterpreter->GetPHINFPortLocation(location);

  // physical port number
  unsigned int port = 0;

  if (location[9] == '0') // wired tool
    {
    unsigned int ndiport = (location[10]-'0')*10 + (location[11]-'0');

    if (ndiport > 0 && ndiport <= MAX_PORT_NUMBER)
      {
      port = ndiport - 1;
      // Verify port number specified 
      if ( port != auroraTrackerTool->GetPortNumber() )
        {
        igstkLogMacro(CRITICAL, 
          "The tracker tool is probably inserted into the wrong port: " 
          << "The port number specified for the tool doesn't match with " 
          "what is detected from the hardware \n");
        return FAILURE;
        }
      }
    }

  // if a tool part number is specified by the user, check if that matches
  // with the port handle information
  if( auroraTrackerTool->IsPartNumberSpecified() )   
    {
    char toolPartNumber[21];
    commandInterpreter->GetPHINFPartNumber( toolPartNumber );

    igstkLogMacro(INFO, "Part number: " << toolPartNumber << "\n" );

    if( toolPartNumber != auroraTrackerTool->GetPartNumber())
      {
      igstkLogMacro(CRITICAL, 
          "The part number specified doesn't match with the information from "
          "the port handle \n");
      return FAILURE;
      }
    }

  const int status = commandInterpreter->GetPHINFPortStatus();

  // port status
  igstkLogMacro(INFO, "Port status information: " << status << "\n" ); 

  // tool status
  igstkLogMacro(INFO, 
    "Tool status: " <<  commandInterpreter->GetPHINFPortStatus() << "\n" );

  // tool type
  igstkLogMacro(INFO, 
    "Tool type: " << commandInterpreter->GetPHINFToolType() << "\n" );   

  // tool accessories
  igstkLogMacro(INFO, 
    "Tool accessories: " << commandInterpreter->GetPHINFAccessories() << "\n" );

  // tool marker type
  igstkLogMacro(INFO, 
    "Marker type: " << commandInterpreter->GetPHINFMarkerType() << "\n" );
 
  // Set the port handle to be added
  this->SetPortHandleToBeAdded ( ph );

  return SUCCESS;
}

/**The "ValidateSpecifiedFrequency" method checks if the specified
  * frequency is valid for the tracking device that is being used. */
AuroraTracker::ResultType 
AuroraTracker::ValidateSpecifiedFrequency( double frequencyInHz )
{
  const double MAXIMUM_FREQUENCY = 50;
  if ( frequencyInHz < 0.0 || frequencyInHz > MAXIMUM_FREQUENCY )
    {
    return FAILURE;
    } 
  return SUCCESS;
}

AuroraTracker::ResultType 
AuroraTracker::
RemoveTrackerToolFromInternalDataContainers
( const TrackerToolType * trackerTool ) 
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTracker::RemoveTrackerToolFromInternalDataContainers "
    "called ...\n");

  if ( trackerTool == NULL )
    {
    return FAILURE;
    }

  TrackerToolType * trackerToolNonConst = 
                                  const_cast<TrackerToolType*>(trackerTool);

  AuroraTrackerToolType * auroraTrackerTool = 
            dynamic_cast< AuroraTrackerToolType * > ( trackerToolNonConst );   

  
  m_HasSpliter[auroraTrackerTool->GetPortNumber()] = 0;

  return Superclass::RemoveTrackerToolFromInternalDataContainers( trackerTool);
}
/** Print Self function */
void AuroraTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

}


} // end of namespace igstk
