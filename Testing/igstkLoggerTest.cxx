/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLoggerTest.cxx
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
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <fstream>

#include "igstkLogger.h"

class LoggerTesterClass
{
  typedef igstk::Logger  LoggerType;
public:
  void SetLogger( LoggerType * logger )
  {
    m_Logger = logger;
  }
  void ProduceLogOutput()
  {
    if(m_Logger.IsNotNull() )
      {
      m_Logger->GetMultipleOutput() << 12.4;
      m_Logger->GetMultipleOutput() << " text ";
      m_Logger->GetMultipleOutput() << 23;
      }
  }
  void TestPriorityLevels()
  {
    igstkLogMacro( LoggerType::DEBUG, "This is a DEBUG message" );
    igstkLogMacro( LoggerType::FATAL, "This is a FATAL message" );
    igstkLogMacro( LoggerType::WARNING, "This is a WARNING message" );
  }
  LoggerType * GetLogger() 
  {
    return m_Logger;
  }
private:
  LoggerType::Pointer m_Logger;
};


int igstkLoggerTest( int, char * [] )
{
    typedef igstk::Logger  LoggerType;
    
    LoggerType::Pointer logger1 = LoggerType::New();

    LoggerType::Pointer logger2 = LoggerType::New();

    std::ofstream file1("fileLoggerOutput1.txt");
    std::ofstream file2("fileLoggerOutput2.txt");
      
    if( file1.fail() || file2.fail() )
      {
      std::cerr << "Problem opening file" << std::endl;
      return EXIT_FAILURE;
      }

    logger1->AddOutputStream( std::cout );
    logger1->AddOutputStream( file1 );
    logger1->AddOutputStream( file2 );

    std::ofstream file3("fileLoggerOutput3.txt");
    std::ofstream file4("fileLoggerOutput4.txt");
      
    if( file3.fail() || file4.fail() )
      {
      std::cerr << "Problem opening file" << std::endl;
      return EXIT_FAILURE;
      }

    logger1->AddOutputStream( std::cout );
    logger1->AddOutputStream( file1 );
    logger1->AddOutputStream( file2 );

    logger2->AddOutputStream( std::cout );
    logger2->AddOutputStream( file3 );
    logger2->AddOutputStream( file4 );

    logger1->SetPriorityLevel( LoggerType::WARNING );
    logger2->SetPriorityLevel( LoggerType::DEBUG );

    LoggerTesterClass testerA;

    testerA.SetLogger( logger1 );
    testerA.ProduceLogOutput();
    testerA.TestPriorityLevels();

    LoggerTesterClass testerB;

    testerB.SetLogger( logger2 );
    testerB.ProduceLogOutput();
    testerB.TestPriorityLevels();

    return EXIT_SUCCESS;
}


