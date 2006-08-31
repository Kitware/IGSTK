/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    ReadVascularNetworkObject.cxx
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

// Software Guide : BeginLatex
//
// \index{igstk::ReadVascularNetworkObject}
// This example describes how to use the \doxygen{VascularNetwork} to read a
// SpatialObject vascular tree from a file.
// Let's start by including the appropriate header files.
// Software Guide : EndLatex 
// Software Guide : BeginCodeSnippet
#include "igstkVascularNetworkReader.h"
#include "itkStdStreamLogOutput.h"
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// The SpatialObject readers return the output object via events, 
// therefore, we declare an observer using the igstkObserverObjectMacro.
// This macro expect three arguments: a) the name of the observer -to
// be determined by the user-, b) the type of event to observe, c) the type
// of the object to be returned.
//
// Software Guide : EndLatex 
// Software Guide : BeginCodeSnippet
igstkObserverObjectMacro(VascularNetwork,
    ::igstk::VascularNetworkReader::VascularNetworkModifiedEvent,
    ::igstk::VascularNetworkObject)
// Software Guide : EndCodeSnippet

int main( int argc, char *argv[] )
{
 if(argc<2)
   {
   std::cout << "Usage: " << argv[0] << " filename" << std::endl;
   return EXIT_FAILURE;
   }

// Software Guide : BeginLatex
//
// First we declare the VascularNetwork using standard type
// definition and smart pointers.
//
// Software Guide : EndLatex 
// Software Guide : BeginCodeSnippet
  typedef igstk::VascularNetworkReader    ReaderType;
  ReaderType::Pointer  reader = ReaderType::New();
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// We then plug a logger to the reader to get information
// about the reading process (see the logging chapter for more
// information).
//
// Software Guide : EndLatex 
// Software Guide : BeginCodeSnippet
  typedef itk::Logger              LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;

  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  reader->SetLogger( logger );
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// Then we set the filename using the \code{RequestSetFileName()}
// function.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  std::string filename = argv[1];
  reader->RequestSetFileName( filename );
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// Finally we ask the reader to read the object.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet  
  reader->RequestReadObject();
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// In order to get the output object we first plug the observer into 
// the reader.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  VascularNetworkObserver::Pointer vascularNetworkObserver 
                                            = VascularNetworkObserver::New();
  reader->AddObserver(ReaderType::VascularNetworkModifiedEvent(),
                      vascularNetworkObserver);
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// Then we request the output vascular network.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  reader->RequestGetVascularNetwork();
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// If everything went well, the observer should 
// receive the vascular network. We can check that this is the case
// using the \code{GotVascularNetwork()} function of the observer.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  if(!vascularNetworkObserver->GotVascularNetwork())
    {
    std::cout << "No VascularNetwork!" << std::endl;
    return EXIT_FAILURE;
    }
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// Finally we get the output using the \code{GetVascularNetwork()} function.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef ReaderType::VascularNetworkType       VascularNetworkType;
  typedef VascularNetworkType::VesselObjectType VesselObjectType;

  VascularNetworkType::Pointer network = 
                                vascularNetworkObserver->GetVascularNetwork();
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// and we display the information to make sure everything is right.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  network->Print(std::cout);
// Software Guide : EndCodeSnippet

  return EXIT_SUCCESS;
}
