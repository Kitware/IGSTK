/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkDICOMImageReaderErrorsTest.cxx
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

#include "igstkDICOMImageReader.h"
#include "igstkImageSpatialObject.h"

#include "igstkLogger.h"
#include "itkCommand.h"
#include "itkStdStreamLogOutput.h"
#include <itksys/SystemTools.hxx>
#include <fstream>
#include <iostream>

class DICOMImageDirectoryNameDoesNotHaveEnoughFilesCallback :\
                                                     public itk::Command
{
public:
  typedef DICOMImageDirectoryNameDoesNotHaveEnoughFilesCallback Self;
  typedef itk::SmartPointer<Self>                               Pointer;
  typedef itk::Command                                          Superclass;
  itkNewMacro(Self);
  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    }

  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    std::cerr<<"Error event received b/c of a directory that does not have" 
              << " enough number of files "<<std::endl;
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
  typedef itk::SmartPointer<Self>                           Pointer;
  typedef itk::Command                                      Superclass;
  itkNewMacro(Self);
  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    }

  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
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
  typedef itk::SmartPointer<Self>                     Pointer;
  typedef itk::Command                                Superclass;
  itkNewMacro(Self);
  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    }
  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    std::cerr << "Error Event received for empty string directory name "
              << std::endl;
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
  typedef itk::SmartPointer<Self>                           Pointer;
  typedef itk::Command                                      Superclass;
  itkNewMacro(Self);
  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    }
  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    std::cerr<<"Error Event received since the directory name specified " << 
               "is not a directory"<<std::endl;
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

class DICOMImageDirectoryDoesNotContainValidDICOMSeriesCallback: \
                                               public itk::Command
{
public:
  typedef DICOMImageDirectoryDoesNotContainValidDICOMSeriesCallback Self;
  typedef itk::SmartPointer<Self>                                   Pointer;
  typedef itk::Command                                              Superclass;
  itkNewMacro(Self);

  typedef igstk::DICOMImageSeriesFileNamesGeneratingErrorEvent \
                 DICOMImageSeriesFileNamesGeneratingErrorEventType;
    
  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    }
  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & event)
    {
    if(DICOMImageSeriesFileNamesGeneratingErrorEventType().CheckEvent(&event))
      {
      const DICOMImageSeriesFileNamesGeneratingErrorEventType * errorEvent = 
      dynamic_cast< 
        const DICOMImageSeriesFileNamesGeneratingErrorEventType *>( &event );
    
      std::cerr<<"Dicom image series file name generating error"
               <<errorEvent->Get()<<std::endl;
      m_EventReceived = true;
      }
    }
  
 bool GetEventReceived()
   { 
   return m_EventReceived; 
   }

protected:
  DICOMImageDirectoryDoesNotContainValidDICOMSeriesCallback()
    {
    m_EventReceived = false;
    }
private:
  bool m_EventReceived;
};

class DICOMImageInvalidErrorCallback: public itk::Command
{
public:
  typedef DICOMImageInvalidErrorCallback Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::Command                   Superclass;
  itkNewMacro(Self);

  typedef igstk::DICOMImageReadingErrorEvent   DICOMImageReadingErrorEventType;
    
  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    }
  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & event)
    {
    if( DICOMImageReadingErrorEventType().CheckEvent( &event ) )
      {
      const DICOMImageReadingErrorEventType * errorEvent = 
             dynamic_cast< const DICOMImageReadingErrorEventType *>( &event );
    
      std::cerr << "Dicom image reading error event:" 
                << errorEvent->Get()<<std::endl;
      m_EventReceived = true;
      }
    }
  
 bool GetEventReceived()
   { 
   return m_EventReceived; 
   }

protected:
  DICOMImageInvalidErrorCallback()
    {
    m_EventReceived = false;
    }
private:
  bool m_EventReceived;
};

int igstkDICOMImageReaderErrorsTest( int argc, char* argv [])
{
  igstk::RealTimeClock::Initialize();

  if ( argc < 3 )
    {
    std::cerr << "Error missing argument " << std::endl;
    std::cerr << "Usage:  " << argv[0]  
              << "Bad DICOM image Output Directory" << std::endl; 
    return EXIT_FAILURE;
    } 
  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;
  
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
                                Dimension > ImageSpatialObjectType;
  
  class myDicomTestReader : 
    public igstk::DICOMImageReader< ImageSpatialObjectType >
    {
  public:

    typedef myDicomTestReader                            Self;
    typedef igstk::ImageReader< ImageSpatialObjectType > Superclass;

    igstkNewMacro( Self );

    void TestMe()
      {
      const ImageType * imageThatShallNotBeNamed = GetITKImage();
      if( imageThatShallNotBeNamed != NULL )
        {
        std::cout << "Test of private abstract method Passed" << std::endl;
        }
      }
  protected:
    myDicomTestReader() {}
    ~myDicomTestReader() {}
  private:
    typedef Superclass::ImageType ImageType;
    virtual const ImageType * GetITKImage() const { return NULL; }
    };

  typedef myDicomTestReader   ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();
  reader->SetLogger( logger );
 
  //Add observer for invalid directory 
  DICOMImageReaderInvalidDirectoryNameErrorCallback::Pointer didcb = 
                    DICOMImageReaderInvalidDirectoryNameErrorCallback::New();
  reader->AddObserver( 
          igstk::DICOMImageDirectoryIsNotDirectoryErrorEvent(), didcb );

  //Add observer for a non-existing directory 
  DICOMImageReaderNonExistingDirectoryErrorCallback::Pointer dndcb = 
                      DICOMImageReaderNonExistingDirectoryErrorCallback::New();
  reader->AddObserver( 
          igstk::DICOMImageDirectoryDoesNotExistErrorEvent(), dndcb );
    
  //Add observer for a an empty directory name (null string) 
  DICOMImageReaderEmptyDirectoryErrorCallback::Pointer decb = 
                      DICOMImageReaderEmptyDirectoryErrorCallback::New();
  reader->AddObserver( igstk::DICOMImageDirectoryEmptyErrorEvent(), decb );

  //Add observer for a directory which does not have enough number of files 
  DICOMImageDirectoryNameDoesNotHaveEnoughFilesCallback::Pointer ddhefcb = 
            DICOMImageDirectoryNameDoesNotHaveEnoughFilesCallback::New();
  reader->AddObserver( 
      igstk::DICOMImageDirectoryDoesNotHaveEnoughFilesErrorEvent(), ddhefcb );

  //Add observer for a directory containing non-DICOM files 
  DICOMImageDirectoryDoesNotContainValidDICOMSeriesCallback::Pointer disgcb = 
            DICOMImageDirectoryDoesNotContainValidDICOMSeriesCallback::New();
  reader->AddObserver( 
      igstk::DICOMImageSeriesFileNamesGeneratingErrorEvent(), disgcb );
  
  //Add observer for reading invalid/corrupted dicom files 
  DICOMImageInvalidErrorCallback::Pointer dircb = 
                      DICOMImageInvalidErrorCallback::New();
  reader->AddObserver( igstk::DICOMImageReadingErrorEvent(), dircb );


  // On purpose invoke read image before the directory has been set.
  // This exercises the error condition of "InvalidRequestEvent"
  std::cout << "On purpose request the image before even giving "  
            << "the directory name" << std::endl;
  reader->RequestReadImage();
  
  std::cout  << "Testing the input directory with an empty string  " 
             << std::endl;
  std::string emptyDirectoryName; // THIS IS EMPTY ON PURPOSE !!
  reader->RequestSetDirectory( emptyDirectoryName );
  
  if( !decb->GetEventReceived() )
    {
    std::cerr << "DICOMImageReader failed to complain about empty string" 
              << "directoryName" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout  << "Testing empty directory " << std::endl;
  std::string           emptyDirectory = argv[2];
  emptyDirectory = emptyDirectory + "/foo";
  itksys::SystemTools::MakeDirectory(emptyDirectory.c_str());
  
  reader->RequestSetDirectory( emptyDirectory );

  if( !ddhefcb->GetEventReceived() )
    {
    std::cerr << "DICOMImageReader failed to complain about "  
              << "empty directory " << std::endl;
    return EXIT_FAILURE;
    }
  std::cout  << "Testing invalid directory (regular file) " << std::endl;
  std::string           invalidDirectory = argv[2];
  invalidDirectory = invalidDirectory + "/foo2";
  itksys::SystemTools::MakeDirectory(invalidDirectory.c_str());
  
  std::string filename = invalidDirectory + "/foo.txt";
  
  std::ofstream outputFile;
  outputFile.open( filename.c_str() );
 
  outputFile << "Dummy file created to test if the DICOMImageReader complain "
             << "if the name specified is a regular file"<< std::endl;  
 
  outputFile.close();
  
  reader->RequestSetDirectory( filename );

  if( !didcb->GetEventReceived() )
    {
    std::cerr << "DICOMImageReader failed to complain about a " 
              << "non-Directory filename " << std::endl;
    return EXIT_FAILURE;
    }
  
  std::cout  << "Testing for non-existing directory  " << std::endl;
  std::string           nonExistingDirectory = argv[2];
  nonExistingDirectory =    nonExistingDirectory + "/foo3";
  
  reader->RequestSetDirectory( nonExistingDirectory );

  if( !didcb->GetEventReceived() )
    {
    std::cerr << "DICOMImageReader failed to complain about " 
              << "a non-existing directory  " << std::endl;
    return EXIT_FAILURE;
    }

  std::cout  << "Testing for directory containing non-dicom files " 
             << std::endl;
  std::string           directoryWithNonDicomFiles = argv[2];
  
  directoryWithNonDicomFiles = directoryWithNonDicomFiles + "/foo4";
  itksys::SystemTools::MakeDirectory(directoryWithNonDicomFiles.c_str());
  
  // Add four dummy files
  std::string filename1 = directoryWithNonDicomFiles + "/foo1.txt";
  std::string filename2 = directoryWithNonDicomFiles + "/foo2.txt";
  std::string filename3 = directoryWithNonDicomFiles + "/foo3.txt";
  std::string filename4 = directoryWithNonDicomFiles + "/foo4.txt";
  
  outputFile.open( filename1.c_str() );
  outputFile << "Dummy file created to test the DICOMImageReader " 
             << std::endl;
  outputFile.close();
  
  outputFile.open( filename2.c_str() );
  outputFile << "Dummy file2 created to test the DICOMImageReader " 
             << std::endl;
  outputFile.close();
    
  outputFile.open( filename3.c_str() );
  outputFile << "Dummy file3 created to test the DICOMImageReader " 
             << std::endl;
  outputFile.close();
 
  outputFile.open( filename4.c_str() );
  outputFile << "Dummy file4 created to test the DICOMImageReader " 
             << std::endl;
  outputFile.close();
  
  reader->RequestSetDirectory( directoryWithNonDicomFiles);

  if( !didcb->GetEventReceived() )
    {
    std::cerr << "DICOMImageReader failed to complain about a " 
              << "non-existing directory  " << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Testing image reading error for a bad/corrupted/invalid " 
            << "dicom image" << std::endl;

  std::string directorywithCorruptedDicomFiles = argv[1];
  reader->RequestSetDirectory ( directorywithCorruptedDicomFiles ); 
  reader->RequestReadImage( );  

  
  return EXIT_SUCCESS;
}
