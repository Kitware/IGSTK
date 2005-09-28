/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkDICOMImageReaderErrorsTest.cxx
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

#include "igstkDICOMImageReader.h"
#include "igstkImageSpatialObject.h"

#include "itkLogger.h"
#include "itkCommand.h"
#include "itkStdStreamLogOutput.h"
#include <itksys/SystemTools.hxx>
#include <fstream>
#include <iostream>

class DICOMImageDirectoryNameDoesNotHaveEnoughFilesCallback : public itk::Command
{
public:
  typedef DICOMImageDirectoryNameDoesNotHaveEnoughFilesCallback Self;
  typedef itk::SmartPointer<Self>      Pointer;
  typedef itk::Command                 Superclass;
  itkNewMacro(Self);
  void Execute(const itk::Object *caller, const itk::EventObject & event)
  {

  }
  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    std::cerr<<"Error event received b/c of a directory that does not have enough number of files "<<std::endl;
    m_EventReceived = true;
  } 
 bool GetEventReceived()
    { 
    return m_EventReceived; 
    }
protected:
  DICOMImageDirectoryNameDoesNotHaveEnoughFilesCallback()
  {
  m_EventReceived = false;
  }
private:
  bool m_EventReceived;
};

class DICOMImageReaderNonExistingDirectoryErrorCallback : public itk::Command
{
public:
  typedef DICOMImageReaderNonExistingDirectoryErrorCallback Self;
  typedef itk::SmartPointer<Self>      Pointer;
  typedef itk::Command                 Superclass;
  itkNewMacro(Self);
  void Execute(const itk::Object *caller, const itk::EventObject & event)
  {

  }
  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    std::cerr<<"Error Event received for a non-Existing directory "<<std::endl;
    m_EventReceived = true;
  } 
 bool GetEventReceived()
    { 
    return m_EventReceived; 
    }
protected:
  DICOMImageReaderNonExistingDirectoryErrorCallback()
  {
  m_EventReceived = false;
  }
private:
  bool m_EventReceived;
};

class DICOMImageReaderEmptyDirectoryErrorCallback : public itk::Command
{
public:
  typedef DICOMImageReaderEmptyDirectoryErrorCallback Self;
  typedef itk::SmartPointer<Self>      Pointer;
  typedef itk::Command                 Superclass;
  itkNewMacro(Self);
  void Execute(const itk::Object *caller, const itk::EventObject & event)
  {
  }
  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    std::cerr << "Error Event received for empty string directory name " << std::endl;
    m_EventReceived = true;
  }
  bool GetEventReceived()
    { 
    return m_EventReceived; 
    }
protected:
  DICOMImageReaderEmptyDirectoryErrorCallback()
  {
  m_EventReceived = false;
  }

private:
  bool m_EventReceived;
};


class DICOMImageReaderInvalidDirectoryNameErrorCallback: public itk::Command
{
public:
  typedef DICOMImageReaderInvalidDirectoryNameErrorCallback Self;
  typedef itk::SmartPointer<Self>      Pointer;
  typedef itk::Command                 Superclass;
  itkNewMacro(Self);
  void Execute(const itk::Object *caller, const itk::EventObject & event)
  {
 
  }
  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    std::cerr<<"Error Event received since the directory name specified is not a directory"<<std::endl;
    m_EventReceived = true;
  }
  
 bool GetEventReceived()
    { 
    return m_EventReceived; 
    }

protected:
  DICOMImageReaderInvalidDirectoryNameErrorCallback()
  {
  m_EventReceived = false;
  }
private:
  bool m_EventReceived;
};

int igstkDICOMImageReaderErrorsTest( int argc, char* argv [])
{

  typedef itk::Logger              LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  typedef short      PixelType;
  const unsigned int Dimension = 3;

  typedef igstk::ImageSpatialObject< 
                                PixelType, 
                                Dimension 
                                       > ImageSpatialObjectType;
  

  typedef igstk::DICOMImageReader< ImageSpatialObjectType >    ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();
  reader->SetLogger( logger );

   //Add observer for invalid directory 
  DICOMImageReaderInvalidDirectoryNameErrorCallback::Pointer didcb = 
                    DICOMImageReaderInvalidDirectoryNameErrorCallback::New();
  reader->AddObserver( igstk::DICOMImageDirectoryIsNotDirectoryErrorEvent(), didcb );

   //Add observer for a non-existing directory 
  DICOMImageReaderNonExistingDirectoryErrorCallback::Pointer dndcb = 
                      DICOMImageReaderNonExistingDirectoryErrorCallback::New();
  reader->AddObserver( igstk::DICOMImageDirectoryDoesNotExistErrorEvent(), dndcb );
    
  //Add observer for a an empty directory name (null string) 
  DICOMImageReaderEmptyDirectoryErrorCallback::Pointer decb = 
                      DICOMImageReaderEmptyDirectoryErrorCallback::New();
  reader->AddObserver( igstk::DICOMImageDirectoryEmptyErrorEvent(), decb );

  //Add observer for a directory which does not have enough number of files 
  DICOMImageDirectoryNameDoesNotHaveEnoughFilesCallback::Pointer ddhefcb = 
                      DICOMImageDirectoryNameDoesNotHaveEnoughFilesCallback::New();
  reader->AddObserver( igstk::DICOMImageDirectoryDoesNotHaveEnoughFilesErrorEvent(), ddhefcb );
  
  std::cout  << "Testing the input directory with an empty string  " << std::endl;
  std::string emptyDirectoryName; // THIS IS EMPTY ON PURPOSE !!
  reader->RequestSetDirectory( emptyDirectoryName );
  
  if( !decb->GetEventReceived() )
    {
    std::cerr << "DICOMImageReader failed to complain about empty string directoryName" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout  << "Testing empty directory " << std::endl;
  std::string           emptyDirectory = argv[1];
  emptyDirectory = emptyDirectory + "/foo";
  itksys::SystemTools::MakeDirectory(emptyDirectory.c_str());
  
  reader->RequestSetDirectory( emptyDirectory );

  if( !ddhefcb->GetEventReceived() )
    {
    std::cerr << "DICOMImageReader failed to complain about empty directory " << std::endl;
    return EXIT_FAILURE;
    }
  std::cout  << "Testing invalid directory (regular file) " << std::endl;
  std::string           invalidDirectory = argv[1];
  invalidDirectory = invalidDirectory + "/foo2";
  itksys::SystemTools::MakeDirectory(invalidDirectory.c_str());
  
  std::string filename = invalidDirectory + "/foo.txt";
  
  std::ofstream outputFile;
  outputFile.open( filename.c_str() );
 
  outputFile << "Dummy file created to test if the DICOMImageReader complain if the name specified is a regular file"<< std::endl;  
 
  outputFile.close();
  
  reader->RequestSetDirectory( filename );

  if( !didcb->GetEventReceived() )
    {
    std::cerr << "DICOMImageReader failed to complain about a non-Directory filename " << std::endl;
    return EXIT_FAILURE;
    }
  
  std::cout  << "Testing for non-existing directory  " << std::endl;
  std::string           nonExistingDirectory = argv[1];
  nonExistingDirectory =    nonExistingDirectory + "/foo3";
  
  reader->RequestSetDirectory( nonExistingDirectory );

  if( !didcb->GetEventReceived() )
    {
    std::cerr << "DICOMImageReader failed to complain about a non-existing directory  " << std::endl;
    return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}

