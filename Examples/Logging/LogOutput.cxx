/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    LogOutput.cxx
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

// BeginLatex
//
// The \code{itk::LogOutput} class represents the destination of the logging and
// serves as a base class for other LogOutput classes.  Classes derived from
// the LogOutput class allow the log data to be directed to specific locations
// (e.g. disk files) for recording. For example, the derived class
// StdStreamLogOutput will send the log data to a specific stream or file, and
// the derived class MultipleLogOutput will allow the log data to be sent to
// multiple locations.  Several LogOutput implementations are available. These
// are described next.
//
// EndLatex

#include <fstream>
#include "igstkMacros.h"
#include "itkObject.h"
#include "itkLogger.h"
#include "igstkLogger.h"
// BeginLatex
//
// First, we begin by including header files for using LogOutput objects.
// The \code{itk::StdStreamLogOutput} needs the \code{itkStdStreamLogOutput.h} 
// header file. \code{itk::MultipleLogOutput}, 
// \code{igstk::FLTKTextBufferLogOutput}, and \code{igstk::FLTKTextLogOutput}
//  need proper header files to be included, as follows:
//
// EndLatex

// BeginCodeSnippet
#include "itkStdStreamLogOutput.h"
#include "itkMultipleLogOutput.h"
#include "igstkFLTKTextBufferLogOutput.h"
#include "igstkFLTKTextLogOutput.h"
// EndCodeSnippet
#include "igstkMultipleOutput.h"
#include "itkThreadLogger.h"
#include "itkLoggerOutput.h"
#include "igstkVTKLoggerOutput.h"
#include "itkLoggerManager.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

#include "igstkObject.h"

#undef _MSC_VER
#include "itkLoggerThreadWrapper.h"
#define _MSC_VER


using namespace std;

int main( int , char* [] )
{
  
  // BeginLatex
  // 
  // \subsubsection{StdStreamLogOutput}
  // \label{Chapter:Logging:StdStreamLogOutput}
  // \index{logging!StdStreamLogOutput}
  // The \code{itk::StdStreamLogOutput} encapsulates and allows the log data to
  // be sent to the standard output stream, which can be a console output
  // stream, an error output stream, or a file output stream. 
  //
  // Each LogOutput object must be created by the \code{New()} method and then
  // set to have a reference to a stream object.  The example code below shows
  // how to create \code{itk::StdStreamLogOutput} objects and set streams for
  // the console and a file output stream.  The prepared LogOutput objects are
  // added to a logger, see Chapter
  // \ref{Chapter:Logging:StructureOfLoggingComponent}. The commands are as
  // follows:
  // 
  // EndLatex

  // BeginCodeSnippet
  itk::StdStreamLogOutput::Pointer 
                  consoleLogOutput = itk::StdStreamLogOutput::New();
  consoleLogOutput->SetStream( std::cout );

  itk::StdStreamLogOutput::Pointer fileLogOutput = 
                                        itk::StdStreamLogOutput::New();
  ofstream fout("log.txt");
  fileLogOutput->SetStream( fout );

  // Creating a logger object
  typedef igstk::Object::LoggerType             IGSTKLoggerType;

  IGSTKLoggerType::Pointer   logger = IGSTKLoggerType::New();
  logger->SetName( "MainLogger" );

  // Attaching LogOutput objects to a logger
  logger->AddLogOutput( consoleLogOutput );
  logger->AddLogOutput( fileLogOutput );

  // Setting a priority level for a logger
  logger->SetPriorityLevel( IGSTKLoggerType::DEBUG );

  // Testing to print a debug message
  logger->Debug("Debug message\n");
  logger->Critical("Critical message\n");
  logger->Fatal("Fatal message\n");

  // Creating a LoggerManager object
  itk::LoggerManager::Pointer manager = itk::LoggerManager::New();

  // Creating a logger object through the LoggerManager object
  typedef itk::Logger ITKLoggerType;

  ITKLoggerType::Pointer logger2 = 
    manager->CreateLogger( "org.itk.logTester.logger", 
    itk::LoggerBase::DEBUG, itk::LoggerBase::CRITICAL );

  // Adding a LogOutput object to the LoggerManager object
  manager->AddLogOutput(consoleLogOutput);

  // Testing to print a debug message
  manager->Write(itk::LoggerBase::DEBUG, "This is a DEBUG message.\n");


  // EndCodeSnippet

  // BeginLatex
  // 
  // \subsubsection{FLTKTextBufferLogOutput}
  // \label{Chapter:Logging:FLTKTextBufferLogOutput}
  // \index{logging!FLTKTextBufferLogOutput}
  // The \code{igstk::FLTKTextBufferLogOutput} forwards messages to a FLTK text 
  // buffer, which is in Fl\_Text\_Buffer type. 
  // This buffer can be used for other FLTK widgets.
  // 
  // Let's create an \code{igstk::FLTKTextBufferLogOutput} object,
  //
  // EndLatex

  // BeginCodeSnippet
  igstk::FLTKTextBufferLogOutput::Pointer 
                       fltkBufferOutput = igstk::FLTKTextBufferLogOutput::New();
  // EndCodeSnippet

  // BeginLatex
  //
  // Then, the following codes create an FLTK window and 
  // an Fl\_Text\_Display widget, which is
  // contained in the window:
  //
  // EndLatex

  // BeginCodeSnippet
  Fl_Window *win = new Fl_Window(0,0,400,300,
                                 "igstkFLTKTextBufferLogOutputExample");
  Fl_Text_Display *textDisplay = new Fl_Text_Display(0,0,400,300,NULL);
  // EndCodeSnippet

  // BeginLatex
  //
  // An Fl\_Text\_Buffer object is created,  
  // then, the Fl\_Text\_Display widget is set 
  // to have a pointer to the text buffer using the buffer() method:
  //
  // EndLatex

  // BeginCodeSnippet
  Fl_Text_Buffer *textBuffer = new Fl_Text_Buffer();
  textDisplay->buffer(textBuffer);
  // EndCodeSnippet

  // BeginLatex
  //
  // The end() method denotes the end of the GUI code.
  // The show() method makes the window visible:
  //
  // EndLatex

  // BeginCodeSnippet
  win->end();
  win->show();
  // EndCodeSnippet

  // BeginLatex
  //
  // Finally, we assign the FLTK text buffer object as a stream for the
  // FLTKTextBufferLogOutput object and write some messages.  The
  // \code{Write()} method only writes a message on a text buffer object.It
  // does not update the display.  The \code{Flush()} method actually updates
  // the display to show the contents of the buffer.  Although the following
  // example uses \code{Write()} and \code{Flush()} methods from the LogOutput
  // object, it is recommended that a logger object be used instead.
  //
  // EndLatex
  
  // BeginCodeSnippet
  fltkBufferOutput->SetStream(*textBuffer);
  fltkBufferOutput->Write("This is a test message.\n");
  fltkBufferOutput->Flush();
  // EndCodeSnippet

  // BeginLatex
  //
  // \subsubsection{FLTKTextLogOutput}
  // \label{Chapter:Logging:FLTKTextLogOutput}
  // \index{logging!FLTKTextLogOutput}
  // The \code{igstk::FLTKTextLogOutput} displays messages in a FLTK text
  // window, which is in Fl\_Text\_Display type.  This is meant to display log
  // messages on a GUI window.  The \mbox{FLTKTextLogOutput} does not need to
  // use an explicit \code{Flush()} method because updating the display is
  // automatically done by the FLTK text display object.
  //
  // An \code{igstk::FLTKTextLogOutput} object is created; then, an
  // Fl\_Text\_Display object is set as a stream of the FLTKTextLogOutput
  // object.  The \code{Write()} method prints some messages on the text
  // display widget.
  //
  // EndLatex

  // BeginCodeSnippet
  igstk::FLTKTextLogOutput::Pointer fltkTextOutput = 
                                    igstk::FLTKTextLogOutput::New();
  fltkTextOutput->SetStream(*textDisplay);
  fltkTextOutput->Write("This is a test message.\n");
  // EndCodeSnippet

  // BeginLatex
  //
  // \subsubsection{MultipleLogOutput}
  // \label{Chapter:Logging:MultipleLogOutput}
  // \index{logging!MultipleLogOutput} 
  // The \code{itk::MultipleLogOutput} method
  // aggregates multiple LogOutput objects.  It is used in the LoggerBase
  // class so that logger classes can contain multiple LogOutput objects.
  //
  // The following example shows how to create the
  // \code{itk::MultipleLogOutput} object and add multiple LogOutput objects to
  // the MultipleLogOutput object.  Whenever an application sends messages to
  // the MultipleLogOutput object, all the messages are redirected to every
  // LogOutput object in the MultipleLogOutput object. The commands are as
  // follows:
  //
  // EndLatex

  // BeginCodeSnippet
  itk::MultipleLogOutput::Pointer multipleLogOutput = 
                                  itk::MultipleLogOutput::New();
  multipleLogOutput->AddLogOutput(consoleLogOutput);
  multipleLogOutput->AddLogOutput(fileLogOutput);
  multipleLogOutput->Write("This is a test message.\n");
  // EndCodeSnippet

  //Patrick took out the duplicated documents

  //
  typedef igstk::MultipleOutput  MultipleOutputType;  
  MultipleOutputType multipleOutput;

  std::ofstream file1("fileMultipleOutput1.txt");

  multipleOutput.AddOutputStream( std::cout );
  multipleOutput.AddOutputStream( file1 );
  multipleOutput << " text \n";

  // BeginLatex
  //
  // \subsubsection{Extending LogOutput}
  // \label{Chapter:Logging:ExtendingLogOutput}
  // \index{logging!Extending LogOutput} 
  // Custom LogOutputs derived from the
  // LogOutput class can encapsulate other output streams, such as a TCP/IP
  // connection, OS-dependent system log records, a database
  // table, and so on. Developers only need to provide methods for writing,
  // flushing a buffer, and setting up the output stream(s).
  //
  // EndLatex

  // BeginLatex
  //
  // \subsection{Redirecting ITK and VTK Log Messages to Logger}
  // \label{Chapter:Logging:Redirecting}
  // \index{logging!Redirecting ITK, VTK logs to Logger}
  // 
  // \subsubsection{Overriding itk::OutputWindow}
  // \label{Chapter:Logging:OutputWindow}
  // \index{logging!itk::OutputWindow}
  // The \code{itk::LoggerOutput} class can override \code{itk::OutputWindow} 
  // and redirect log messages from ITK to a logger object. ITK applications can
  // still use conventional ITK log codes and those messages can be sent to any 
  // type of logger by using the \code{itk::LoggerOutput} class. 
  // \code{itkLoggerOutput.h} is the header file for using the 
  // \code{itk::LoggerOutput} class.
  //
  // The following code fragment shows how to create an \code{itk::LoggerOutput}
  // object and override \code{itk::OutputWindow} to redirect messages from ITK 
  // to a logger object. Once the setup using \code{OverrideITKWindow()} and 
  // \code{SetLogger()} methods is done, every message sent toward ITK is 
  // redirected to a logger object. The commands are as follows:
  //
  // EndLatex

  // BeginCodeSnippet
  // Create an ITK LoggerOutput and then test it.
  itk::LoggerOutput::Pointer itkOutput = itk::LoggerOutput::New();
  itkOutput->OverrideITKWindow();
  itkOutput->SetLogger(logger);

  // Test messages for ITK OutputWindow
  itk::OutputWindow::Pointer outputWindow = itk::OutputWindow::GetInstance();

  outputWindow->DisplayText("This is from ITK\n");
  outputWindow->DisplayDebugText("This is from ITK\n");
  outputWindow->DisplayWarningText("This is from ITK\n");
  outputWindow->DisplayErrorText("This is from ITK\n");
  outputWindow->DisplayGenericOutputText("This is from ITK\n");
  // EndCodeSnippet

  // BeginLatex
  //
  // \subsubsection{Overriding vtkOutputWindow}
  // \label{Chapter:Logging:vtkOutputWindow}
  // \index{logging!Overriding vtkOutputWindow}
  // The \code{igstk::VTKLoggerOutput} redirects log messages from VTK to a 
  // logger object. This class plays a similar role as \code{itk::LoggerOutput} 
  // but is included in IGSTK. \code{igstkVTKLoggerOutput.h} should be included 
  // for using the \code{igstk::VTKLoggerOutput} class.
  //
  // The structure of the example code is the same as the example for the
  // \code{itk::LoggerOutput} class.  An application creates an
  // \code{igstk::VTKLoggerOutput} object and calls the
  // \code{OverrideVTKWindow()} and \code{SetLogger()} methods.  After that,
  // all the messages toward vtkOutputWindow are redirected to a logger object.
  // The commands are as follows:
  // EndLatex

  // BeginCodeSnippet
  igstk::VTKLoggerOutput::Pointer vtkOutput = igstk::VTKLoggerOutput::New();
  vtkOutput->OverrideVTKWindow();
  vtkOutput->SetLogger(logger);

  // Test messages for VTK OutputWindow
  vtkOutputWindow::GetInstance()->DisplayText(
                                    "This is from vtkOutputWindow\n");
  vtkOutputWindow::GetInstance()->DisplayDebugText(
                                    "This is from vtkOutputWindow\n");
  vtkOutputWindow::GetInstance()->DisplayWarningText(
                                    "This is from vtkOutputWindow\n");
  vtkOutputWindow::GetInstance()->DisplayErrorText(
                                    "This is from vtkOutputWindow\n");
  vtkOutputWindow::GetInstance()->DisplayGenericWarningText(
                                    "This is from vtkOutputWindow\n");
  // EndCodeSnippet

  // BeginLatex
  //
  // \subsection{Multi-Threaded Logging}
  // \label{Chapter:Logging:MultithreadedLogging}
  // \index{logging!Multi-threaded Logging}

  // \subsubsection{LoggerThreadWrapper}
  // \label{Chapter:Logging:LoggerThreadWrapper}
  // \index{logging!LoggerThreadWrapper}
  // \code{itk::LoggerThreadWrapper} is a template class that wraps a logger
  // class to enable logging in a separate thread. LoggerThreadWrapper inherits
  // the logger class given as a template argument.  Logging method calls made
  // through a LoggerThreadWrapper object are queued and performed whenever the
  // thread takes the available computational resource.  LoggerThreadWrapper
  // provides more flexibility than ThreadLogger by allowing various types of
  // Loggers to be used; however, LoggerThreadWrapper may be problematic when
  // used with compilers with weak C++ template support.

  // To use this class, an application should include the header file
  // \code{itkLoggerThreadWrapper.h}. The example code
  // fragment shows how to wrap a logger within the
  // \code{itk::LoggerThreadWrapper} class:
  //
  // EndLatex

  // BeginCodeSnippet
  typedef itk::LoggerThreadWrapper<itk::Logger> LoggerType;
  LoggerType::Pointer wrappedLogger = LoggerType::New();
  // EndCodeSnippet

  // BeginLatex
  // 
  // The wrapped logger is the same as other loggers except that it runs in a
  // separate thread from the thread that uses the logger object.
  //
  // \subsubsection{ThreadLogger}
  // \label{Chapter:Logging:ThreadLogger}
  // \index{logging!ThreadLogger}
  // The \code{itk::ThreadLogger} provides the same functionality as
  // LoggerThreadWrapper except that ThreadLogger is derived from the logger
  // class. If different types of loggers are necessary instead of the simple
  // logger class, these classes derive from the ThreadLogger class by creating
  // a new class or by using LoggerThreadWrapper.

  // An application should include the header file called
  // \code{itkThreadLogger.h} to use this class. The example code fragment
  // shows how to create an \code{itk::ThreadLogger} object.
  //
  // EndLatex

  // BeginCodeSnippet
  itk::ThreadLogger::Pointer threadLogger = itk::ThreadLogger::New();
  // EndCodeSnippet

  Fl::run();

  return EXIT_SUCCESS;
}
