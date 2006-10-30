/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    Logging1.cxx
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

#include "itkObject.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkMacros.h"
#include <fstream>

using namespace std;

// BeginLatex
//
// This example shows how to extend Logger for printing 
// log messages in a custom format.
//
// EndLatex

// BeginLatex
// 
// XMLLogger class is defined to construct log messages in XML format.
// Indentation is automatically done here.
// Most of XML viewers show the hierarchical structure of log messages
// and provide UIs to collapse and expand subelements.
// XMLLogger opens a new element when the first character of the log message 
// is \begin{verbatim}'<'\end{verbatim}
// and closes a element when the first character is 
// \begin{verbatim}'>'\end{verbatim}.
// Otherwise, a self-closing element is created when no angular bracket is 
// used for the first character.
// BuildFormattedEntry() method is redefined in XMLLogger class for overriding 
// default formatting.
// It creates a string containing a timestamp, a logger name, 
// the priority level of a message, and a message content.
// Some of these components can be omitted if unnecessary. 
// The logger name was omitted here to shorten the length of messages.
//
// EndLatex

// BeginCodeSnippet
namespace igstk
{

class XMLLogger: public itk::Logger
{
public:

  typedef XMLLogger                                   Self;
  typedef itk::SmartPointer<Self>                     Pointer;
  typedef itk::Logger                                 Superclass;
  
  igstkNewMacro(Self);

  /** Provides a XML-formatted log entry */
  virtual std::string BuildFormattedEntry(PriorityLevelType level, 
    std::string const & content)
    {
    static std::string m_LevelString[] = { "MUSTFLUSH", "FATAL", 
      "ERROR", "WARNING", "INFO", "DEBUG", "NOTSET" };
    itk::OStringStream s;
    s.precision(30);
    if( content.at(0) == '<' )
      {
      for( int i = 0; i < m_Depth; ++i )
        {
        s << "  ";
        }
    
      s << "<Log timestamp='" << m_Clock->GetTimeStamp() 
        <<  "' level='" <<  m_LevelString[level] 
        << "' message='" << content.substr(1, content.size()-1) << "'>"
        << std::endl;
      ++m_Depth;
      }
    else if( content.at(0) == '>' )
      {
      --m_Depth;
      for( int i = 0; i < m_Depth; ++i )
        {
        s << "  ";
        }
    
      s << "</Log>" << std::endl;
      }
    else
      {
      for( int i = 0; i < m_Depth; ++i )
        {
        s << "  ";
        }
      s << "<Log timestamp='" << m_Clock->GetTimeStamp() 
        <<  "' level='" <<  m_LevelString[level] 
        << "' message='" << content << "'/>"
        << std::endl;
      }
    return s.str();
    }

protected:
  /** Constructor */
  XMLLogger() {m_Depth = 0;}

  /** Destructor */
  virtual ~XMLLogger() {};

private:
  
  int m_Depth;
  
};

} // namespace igstk

// EndCodeSnippet

int main( int argc, char* argv[] )
{
  
  // BeginLatex
  // 
  // The following code fragment creates an XMLLogger instance, 
  // StdStreamLogOutput instances connected to a log file and the console,
  // and then sets parameters for the logger.
  //
  // EndLatex

  // BeginCodeSnippet
  typedef igstk::XMLLogger              LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  LogOutputType::Pointer logOutput2 = LogOutputType::New();
  ofstream fout("log.xml");
  logOutput->SetStream( fout );
  logOutput2->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->AddLogOutput( logOutput2 );
  logger->SetPriorityLevel( itk::Logger::DEBUG );
  // EndCodeSnippet

  // BeginLatex
  // 
  // The XMLLogger prints log messages in XML format so that the log messages
  // are structured hierarchically.
  // After running this example, open the generated log.xml file 
  // using an XML viewer.
  //
  // EndLatex

  // BeginCodeSnippet
  logger->Debug("<main()");

  logger->Debug("Hello world1");

  logger->Critical("<nested 1");
  logger->Debug("Hello world2");

  logger->Info("<nested 2");
  logger->Debug("Hello world3");
  logger->Error("Hello world4");
  logger->Info(">nested 2");

  logger->Debug("Hello world5");
  logger->Critical(">nested 1");

  logger->Debug(">main()");
  // EndCodeSnippet

  return EXIT_SUCCESS;
}

// BeginLatex
// 
// log.xml file is generated as the following:
//
// \begin{verbatim}
// <Log timestamp='24444134232.34433' level='DEBUG' message='main()'>
//  <Log timestamp='24444134232.3451' level='DEBUG' message='Hello world1'/>
//  <Log timestamp='24444134232.345699' level='ERROR' message='nested 1'>
//   <Log timestamp='24444134232.346279' level='DEBUG' message='Hello world2'/>
//   <Log timestamp='24444134232.346897' level='INFO' message='nested 2'>
//    <Log timestamp='24444134232.347507' level='DEBUG' message='Hello world3'/>
//    <Log timestamp='24444134232.348164' level='ERROR' message='Hello world4'/>
//   </Log>
//   <Log timestamp='24444134232.348892' level='DEBUG' message='Hello world5'/>
//  </Log>
// </Log>
// \end{verbatim}
// 
// EndLatex
