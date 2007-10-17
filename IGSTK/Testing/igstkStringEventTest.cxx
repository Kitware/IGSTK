/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkStringEventTest.cxx
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
#pragma warning ( disable : 4786 )
#endif

#include "igstkMacros.h"
#include "igstkEvents.h"


int igstkStringEventTest( int , char* [] )
{

  igstk::RealTimeClock::Initialize();


  igstk::StringEvent event;

  std::string message = "if it is not tested, it is broken !";

  event.Set( message );

  std::string message2 = event.Get();

  if( message2 != message )
    {
    std::cerr << "ERROR: inconsistency between SetString() and GetString()" 
              << std::endl;
    std::cerr << "Set = " << message << std::endl;
    std::cerr << "Get = " << message2 << std::endl;
    return EXIT_FAILURE;
    }

  std::string eventName = event.GetEventName();
  std::string expectedName = "StringEvent";
  if( eventName != expectedName )
    {
    std::cerr << "ERROR: in GetEventName() it returned " 
              << eventName << std::endl;
    std::cerr << "The expected name is " << expectedName << std::endl;
    return EXIT_FAILURE;
    }

  if( !event.CheckEvent( &event ) )
    {
    std::cerr << "ERROR: in CheckEvent() it didn't recognized itself" 
              << std::endl;
    return EXIT_FAILURE;
    }

  igstk::StringEvent * newEvent = 
      dynamic_cast< igstk::StringEvent *>( event.MakeObject() );

  if( !newEvent->CheckEvent( &event )   || 
      !event.CheckEvent( newEvent )     || 
      newEvent->GetEventName() != event.GetEventName() )
    {
    std::cerr << "ERROR: MakeObject() did not returned an event \
                 of the expected type" << std::endl;
    return EXIT_FAILURE;
    }

  delete newEvent;

  return EXIT_SUCCESS;
}
