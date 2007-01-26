/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    LogOutput.cxx
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

// BeginLatex
//
// The itk::LogOutput class represents the destination of the logging and
// serves as a base class for other LogOutput classes.  Classes derived from
// the LogOutput class allow the log data to be directed to specific locations
// (e.g. disk files) for recording. For example, the derived class
// StdStreamLogOutput will send the log data to a specific stream or file, and
// the derived class MultipleLogOutput will allow the log data to be sent to
// multiple locations.  Several LogOutput implementations are available, these
// are described next.
//
// EndLatex

#include <fstream>
#include "igstkMacros.h"
#include "itkObject.h"
#include "itkLogger.h"
// BeginLatex
//
// First, we begin by including header files for using LogOutput objects.
// The itk::StdStreamLogOutput needs the itkStdStreamLogOutput.h header file.
// itk::MultipleLogOutput, igstk::FLTKTextBufferLogOutput, and
// igstk::FLTKTextLogOutput need proper header files to be included as follows:
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
#include <FL/Fl.h>
#include <FL/Fl_Window.h>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

#undef _MSC_VER
#include "itkLoggerThreadWrapper.h"
#define _MSC_VER


using namespace std;

int main( int argc, char* argv[] )
{
  
  // BeginLatex
  // 
  // \subsubsection{StdStreamLogOutput}
  // \label{Chapter:Logging:StdStreamLogOutput}
  // \index{logging!StdStreamLogOutput}
  // The itk::StdStreamLogOutput encapsulates and allows the log data to be
  // sent to the standard output stream, which can be a console output stream,
  // an error output stream, or a file output stream. 
  //
  // Each LogOutput object must be created by the New() method and then set to
  // have a reference to a stream object.  The example code below shows how to
  // create itk::StdStreamLogOutput objects and set streams for the console and
  // a file output stream.  The prepared LogOutput objects are added to a
  // logger. \ref{Chapter:Logging:StructureOfLoggingComponent}
  // 
  // EndLatex

  // BeginCodeSnippet
  itk::StdStreamLogOutput::Pointer 
                  consoleLogOutput = itk::StdStreamLogOutput::New();
  consoleLogOutput->SetStream( std::cout );

  itk::StdStreamLogOutput::Pointer fileLogOutput = itk::StdStreamLogOutput::New();
  ofstream fout("log.txt");
  fileLogOutput->SetStream( fout );

  // Creating a Logger object
  itk::Logger::Pointer   logger = itk::Logger::New();
  logger->SetName( "MainLogger" );
  // Attaching LogOutput objects to a logger
  logger->AddLogOutput( consoleLogOutput );
  logger->AddLogOutput( fileLogOutput );
  // Setting a priority level for a logger
  logger->SetPriorityLevel( itk::Logger::DEBUG );
  // Testing to print a debug message
  logger->Debug("Debug message\n");
  logger->Critical("Critical message\n");
  logger->Fatal("Fatal message\n");

  // Creating a LoggerManager object
  itk::LoggerManager::Pointer manager = itk::LoggerManager::New();
  // Creating a Logger object through the LoggerManager object
  itk::Logger::Pointer logger2 = manager->CreateLogger( "org.itk.logTester.logger", 
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
  // The igstk::FLTKTextBufferLogOutput forwards messages to a FLTK text buffer 
  // which is in Fl\underline{ }Text\underline{ }Buffer type. 
  // This buffer can be used for other FLTK widgets.
  // 
  // Let's create an igstk::FLTKTextBufferLogOutput object.
  //
  // EndLatex

  // BeginCodeSnippet
  igstk::FLTKTextBufferLogOutput::Pointer 
                       fltkBufferOutput = igstk::FLTKTextBufferLogOutput::New();
  // EndCodeSnippet

  // BeginLatex
  //
  // Then, the following codes create an FLTK window and 
  // an Fl\underline{ }Text\underline{ }Display widget, which is
  // contained in the window.
  //
  // EndLatex

  // BeginCodeSnippet
  Fl_Window *win = new Fl_Window(0,0,400,300,
                                 "igstkFLTKTextBufferLogOutputExample");
  Fl_Text_Display *textDisplay = new Fl_Text_Display(0,0,400,300,NULL);
  // EndCodeSnippet

  // BeginLatex
  //
  // An Fl\underline{ }Text\underline{ }Buffer object is created and 
  // then the Fl\underline{ }Text\underline{ }Display widget is set 
  // to have a pointer to the text buffer using the buffer() method.
  //
  // EndLatex

  // BeginCodeSnippet
  Fl_Text_Buffer *textBuffer = new Fl_Text_Buffer();
  textDisplay->buffer(textBuffer);
  // EndCodeSnippet

  // BeginLatex
  //
  // The end() method is called to finish containing widgets in the window.
  // The show() method makes the window visible.
  //
  // EndLatex

  // BeginCodeSnippet
  win->end();
  win->show();
  // EndCodeSnippet

  // BeginLatex
  //
  // Finally, Let's assign the FLTK text buffer object as a stream for the
  // FLTKTextBufferLogOutput object and write some message.  The Write() method
  // only writes a message on a text buffer object but it doesn't update the
  // display.  The Flush() method actually updates the display to show the
  // contents of the buffer.  Although the following example uses Write() and
  // Flush() methods from the LogOutput object, it is recommended that a Logger
  // object be used instead.
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
  // The igstk::FLTKTextLogOutput displays messages in FLTK text window which
  // is in Fl\underline{ }Text\underline{ }Display type.  This is meant to
  // diplay log messages on a GUI window.  The FLTKTextLogOutput doesn't need
  // to use an explicit Flush() method because updating the display is
  // automatically done by the FLTK text display object.
  //
  // An igstk::FLTKTextLogOutput object is created 
  // and then an Fl\underline{ }Text\underline{ }Display object is set 
  // as a stream of the FLTKTextLogOutput object.
  // Write() method prints some messages on the text display widget.
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
  // The itk::MultipleLogOutput aggregates multiple LogOutput objects in it. 
  // It is used in the LoggerBase class so that Logger classes 
  // can contain multiple LogOutput objects.
  //
  // The following example shows how to create the itk::MultipleLogOutput object
  // and add multiple LogOutput objects to the MultipleLogOutput object.
  // Whenever an application sends messages to the MultipleLogOutput object,
  // all the messages are redirected to every LogOutput object
  // in the MultipleLogOutput object.
  //
  // EndLatex

  // BeginCodeSnippet
  itk::MultipleLogOutput::Pointer multipleLogOutput = 
                                  itk::MultipleLogOutput::New();
  multipleLogOutput->AddLogOutput(consoleLogOutput);
  multipleLogOutput->AddLogOutput(fileLogOutput);
  multipleLogOutput->Write("This is a test message.\n");
  // EndCodeSnippet

  //
  // \subsubsection{MultipleOutput}
  // \label{Chapter:Logging:MultipleOutput}
  // \index{logging!MultipleOutput}
  // The igstk::MultipleOutput class is a special 
  // C++ output stream that will forward data to any other output streams 
  // that have been connected to it.  In other words, it is used to branch a 
  // single output into several outputs.  
  // This class is not derived from LogOutput itself.
  //
  // As the following example shows, the igstk::MultipleOutput object is
  // created without the New() method and can contain multiple output streams.
  // This object plays a role as an aggregate output stream object redirecting
  // messages toward output streams it has.
  //

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
  // Custom LogOutputs derived from LogOutput class can encapsulate other 
  // output streams such as TCP/IP connection, OS-dependent system log records,
  // the ring of files, database table, and so on. Developers only need to 
  // provide methods for writing, flushing a buffer, and setting up 
  // the output stream(s).
  //
  // EndLatex

  // BeginLatex
  //
  // \subsection{Redirecting ITK, VTK log messages to Logger}
  // \label{Chapter:Logging:Redirecting}
  // \index{logging!Redirecting ITK, VTK logs to Logger}
  // 
  // \subsubsection{Overriding itk::OutputWindow}
  // \label{Chapter:Logging:OutputWindow}
  // \index{logging!itk::OutputWindow}
  // The itk::LoggerOutput class can override itk::OutputWindow and redirect
  // log messages from ITK to a Logger object. ITK applications can still use
  // conventional ITK log codes and those messages can be sent to any type of
  // Logger by using itk::LoggerOutput class.  itkLoggerOutput.h is the header
  // file for using the itk::LoggerOutput class.
  //
  // The following code fragment shows how to create an itk::LoggerOutput
  // object and override itk::OutputWindow to redirect messages from ITK to a
  // Logger object.  Once the setup using OverrideITKWindow() and SetLogger()
  // methods are done, every message sent toward ITK is redirected to a Logger
  // object.
  //
  // EndLatex

  // BeginCodeSnippet
  // Create an ITK LoggerOutput and then test it.
  itk::LoggerOutput::Pointer itkOutput = itk::LoggerOutput::New();
  itkOutput->OverrideITKWindow();
  itkOutput->SetLogger(logger);

  // Test messages for ITK OutputWindow
  itk::OutputWindow::GetInstance()->DisplayText("This is from ITK\n");
  itk::OutputWindow::GetInstance()->DisplayDebugText("This is from ITK\n");
  itk::OutputWindow::GetInstance()->DisplayWarningText("This is from ITK\n");
  itk::OutputWindow::GetInstance()->DisplayErrorText("This is from ITK\n");
  itk::OutputWindow::GetInstance()->DisplayGenericOutputText("This is from ITK\n");
  // EndCodeSnippet

  // BeginLatex
  //
  // \subsubsection{Overriding vtkOutputWindow}
  // \label{Chapter:Logging:vtkOutputWindow}
  // \index{logging!Overriding vtkOutputWindow}
  // The igstk::VTKLoggerOutput redirects log messages from VTK to a Logger
  // object. This class plays a similar role as itk::LoggerOutput but is
  // included in IGSTK.  igstkVTKLoggerOutput.h should be included for using
  // the igstk::VTKLoggerOutput class.
  //
  // The structure of the example code is the same as the example for the
  // itk::LoggerOutput class.  An application creates an igstk::VTKLoggerOutput
  // object and call OverrideVTKWindow() and SetLogger() methods.  After that,
  // all the messages toward vtkOutputWindow are redirected to a Logger object.
  //
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
  // \subsection{Multi-threaded Logging}
  // \label{Chapter:Logging:MultithreadedLogging}
  // \index{logging!Multi-threaded Logging}

  // \subsubsection{LoggerThreadWrapper}
  // \label{Chapter:Logging:LoggerThreadWrapper}
  // \index{logging!LoggerThreadWrapper}
  // The itk::LoggerThreadWrapper is a template class that wraps a Logger class
  // to enable logging in a separate thread. LoggerThreadWrapper inherits the
  // Logger class given as a template argument.  Logging method calls made
  // through a LoggerThreadWrapper object are queued and performed whenever the
  // thread takes the available computational resource.  LoggerThreadWrapper
  // provides more flexibility than ThreadLogger by allowing various types of
  // Loggers to be used, however LoggerThreadWrapper may be problematic when
  // used with compilers with weak C++ template support.

  // An application should include the header file called
  // itkLoggerThreadWrapper.h to use this class.  The example code fragment
  // shows how to wrap a logger within the itk::LoggerThreadWrapper class.
  //
  // EndLatex

  // BeginCodeSnippet
  itk::LoggerThreadWrapper<itk::Logger>::Pointer 
                     wrappedLogger = itk::LoggerThreadWrapper<itk::Logger>::New();
  // EndCodeSnippet

  // BeginLatex
  // 
  // The wrapped logger is the same as other loggers except that it runs on a
  // separate thread from the thread that uses the logger object.
  //
  // \subsubsection{ThreadLogger}
  // \label{Chapter:Logging:ThreadLogger}
  // \index{logging!ThreadLogger}
  // The itk::ThreadLogger provides the same functionality as
  // LoggerThreadWrapper except that ThreadLogger is derived from the Logger
  // class. If different types of loggers are necessary instead of the simple
  // Logger class, these classes derive from ThreadLogger class by creating a
  // new class or using LoggerThreadWrapper.

  // An application should include the header file called itkThreadLogger.h to
  // use this class.  The example code fragment shows how to create an
  // itk::ThreadLogger object.
  //
  // EndLatex

  // BeginCodeSnippet
  itk::ThreadLogger::Pointer threadLogger = itk::ThreadLogger::New();
  // EndCodeSnippet

  Fl::run();

  return EXIT_SUCCESS;
}

