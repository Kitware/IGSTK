/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCommunicationTest.cxx
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
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>

#include "igstkCommunication.h"
#include "igstkRealTimeClock.h"

int igstkCommunicationTest( int, char * [] )
{
  igstk::RealTimeClock::Initialize();
  
  typedef igstk::Communication  CommunicationType;
    
  CommunicationType::Pointer communicator = CommunicationType::New();

  communicator->SetUseReadTerminationCharacter(true);

  communicator->SetReadTerminationCharacter( '\r' );

  communicator->OpenCommunication();

  std::cout << communicator << std::endl;

  communicator->CloseCommunication();

  return EXIT_SUCCESS;
}
