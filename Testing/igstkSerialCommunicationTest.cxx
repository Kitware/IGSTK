/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkLogger.h"
#include "igstkSerialCommunication.h"

int igstkSerialCommunicationTest( int, char * [] )
{
  typedef igstk::SerialCommunication      SerialCommunicationType;
  typedef igstk::Logger                   LoggerType; 
    
  // logger object created for logging mouse activities
  LoggerType            logger;
  logger.AddOutputStream( std::cout );
  logger.SetPriorityLevel( igstk::Logger::DEBUG );

  SerialCommunicationType   serialComm;

  serialComm.SetLogger( &logger );

  serialComm.OpenCommunicationPort( 1 );
  serialComm.SetDataBufferSizeParameters();
  serialComm.SetupCommunicationParameters();
  serialComm.SetCommunicationTimeoutParameters();

  serialComm.CloseCommunication();

  return EXIT_SUCCESS;
}


