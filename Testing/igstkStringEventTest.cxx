/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkStringEventTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "igstkMacros.h"
#include "igstkStringEvent.h"


int igstkStringEventTest( int , char* [] )
{

  igstk::StringEvent event;

  std::string message = "if it is not tested, it is broken !";

  event.SetString( message );

  std::string message2 = event.GetString();

  if( message2 != message )
    {
    std::cerr << "ERROR: inconsistency between SetString() and GetString()" << std::endl;
    std::cerr << "Set = " << message << std::endl;
    std::cerr << "Get = " << message2 << std::endl;
    return EXIT_FAILURE;
    }

  std::string eventName = event.GetEventName();
  if( eventName != "StringEvent" );
    {
    std::cerr << "ERROR: in GetEventName() it returned " << eventName << std::endl;
    return EXIT_FAILURE;
    }

  if( !event.CheckEvent( &event ) )
    {
    std::cerr << "ERROR: in CheckEvent() it didn't recognized itself" << std::endl;
    return EXIT_FAILURE;
    }

  igstk::StringEvent * newEvent = 
      dynamic_cast< igstk::StringEvent *>( event.MakeObject() );

  if( newEvent->CheckEvent( &event )   || 
      event.CheckEvent( newEvent )     || 
      newEvent->GetEventName() != event.GetEventName() )
    {
    std::cerr << "ERROR: MakeObject() did not returned an event of the expected type" << std::endl;
    return EXIT_FAILURE;
    }

  delete newEvent;

  return EXIT_SUCCESS;
}

