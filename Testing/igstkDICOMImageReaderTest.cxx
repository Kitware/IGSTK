/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkDICOMImageReaderTest.cxx
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
#include "igstkCTImageReader.h"

#include "igstkLogger.h"
#include "itkCommand.h"
#include "itkStdStreamLogOutput.h"

namespace igstkDICOMImageReaderTestNamespace
{
  
class DICOMImageModalityInformationCallback: public itk::Command
{
public:
  typedef DICOMImageModalityInformationCallback   Self;
  typedef itk::SmartPointer<Self>                 Pointer;
  typedef itk::Command                            Superclass;
  itkNewMacro(Self);

  typedef igstk::ImageSpatialObject<short,3> ImageSpatialObjectType;
  
 
  typedef igstk::DICOMModalityEvent DICOMModalityEventType;
  
  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    }

  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & event)
    {
    if( DICOMModalityEventType().CheckEvent( &event ) )
      {
      const DICOMModalityEventType * modalityEvent = 
                   dynamic_cast< const DICOMModalityEventType *>( &event );
      std::cout << "Modality= " << modalityEvent->Get() << std::endl;
      }   
    }

protected:
  DICOMImageModalityInformationCallback()   { };

private:
};

class DICOMImagePatientNameInformationCallback: public itk::Command
{
public:
  typedef DICOMImagePatientNameInformationCallback  Self;
  typedef itk::SmartPointer<Self>                   Pointer;
  typedef itk::Command                              Superclass;
  itkNewMacro(Self);

  typedef igstk::ImageSpatialObject<short,3> ImageSpatialObjectType;
 
  typedef igstk::DICOMPatientNameEvent DICOMPatientNameEventType;
  
  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    }
  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & event)
    {
    const DICOMPatientNameEventType * patientNameEvent = 
          dynamic_cast<const DICOMPatientNameEventType*> (&event);
    std::cerr << "PatientName= " << patientNameEvent->Get() << std::endl;  
    }

protected:
  DICOMImagePatientNameInformationCallback()   { };

private:
};


class myDicomTestReader : 
  public igstk::DICOMImageReader< igstk::ImageSpatialObject< short, 3 > >
{
public:

  typedef igstk::DICOMImageReader< 
                  igstk::ImageSpatialObject< short, 3 > > DICOMReaderSuperclass;

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( myDicomTestReader, DICOMReaderSuperclass )

  void TestMe()
    {
    m_CheckTheNullImage = true;
    const ImageType * imageThatShallNotBeNamed = GetITKImage();
    if( imageThatShallNotBeNamed != NULL )
      {
      std::cout << "Test of private abstract method Passed" << std::endl;
      }
    m_CheckTheNullImage = false;
    }

protected:
   myDicomTestReader():m_StateMachine(this),m_CheckTheNullImage(false) {}
   ~myDicomTestReader() {}
private:
  typedef Superclass::ImageType ImageType;
  virtual const ImageType * GetITKImage() const 
    { 
    if(m_CheckTheNullImage)
      {
      return NULL;
      }
    else
      { 
      return this->Superclass::GetITKImage();
      }
    }

  myDicomTestReader(const Self&);         //purposely not implemented
  void operator=(const Self&);        //purposely not implemented

  bool m_CheckTheNullImage;
};


} // end of igstkDICOMImageReaderTestNamespace namespace


int igstkDICOMImageReaderTest( int argc, char* argv[] )
{

  igstk::RealTimeClock::Initialize();

  if(argc < 2)
    {
    std::cerr<<"Usage: "<<argv[0]<<"  DICOMSeries "<<std::endl;
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


  //typedef igstk::CTImageReader         ReaderType;
  typedef igstkDICOMImageReaderTestNamespace::myDicomTestReader   ReaderType;


  ReaderType::Pointer   reader = ReaderType::New();
  reader->SetLogger( logger );

  if( reader->FileSuccessfullyRead() )
    {
    std::cerr << "FileSuccessfullyRead() is malfunctioning. It returned true ";
    std::cerr << "when the image has not even been read" << std::endl;
    return EXIT_FAILURE;
    }
      
  /* Read in a DICOM series */
  std::cout << "Reading the DICOM series : " << argv[1] <<std::endl;
  ReaderType::DirectoryNameType directoryName = argv[1];
  reader->RequestSetDirectory( directoryName );
  reader->RequestReadImage();
  
  /* Add observer to listen to modality info */
  typedef 
    igstkDICOMImageReaderTestNamespace::DICOMImageModalityInformationCallback  
                                                          ModalityCallbackType;

  ModalityCallbackType::Pointer dimcb = ModalityCallbackType::New();
  reader->AddObserver( igstk::DICOMModalityEvent(), dimcb );
  reader->RequestGetModalityInformation(); 
 
  /* Add observer to listen to patient name  info */
  typedef 
  igstkDICOMImageReaderTestNamespace::DICOMImagePatientNameInformationCallback
                                                           PatientCallbackType;

  PatientCallbackType::Pointer dipncb = PatientCallbackType::New();
  reader->AddObserver( igstk::DICOMPatientNameEvent(), dipncb );
  reader->RequestGetPatientNameInformation(); 

  reader->Print( std::cout );

  /* Exercising the Unsafe GetPatientName() method */
  if( reader->FileSuccessfullyRead() )
    {
    ReaderType::DICOMInformationType  patientName = reader->GetPatientName();
    ReaderType::DICOMInformationType  patientID   = reader->GetPatientID();
    ReaderType::DICOMInformationType  modality    = reader->GetModality();
    
    std::cout << "Patient Name = " << patientName << std::endl;
    std::cout << "Patient ID = "   << patientID << std::endl;
    std::cout << "Modality = "     << modality << std::endl;
    }
  else
    {
    std::cout << "The file was not successfully read" << std::endl;
    return EXIT_FAILURE;
    }

  /* Testing the return of null pointer in the GetITKImage method */
  reader->TestMe();

  
  return EXIT_SUCCESS;
}
