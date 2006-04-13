/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    DICOMImageReader1.cxx
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

#include "igstkCTImageReader.h"
#include "igstkImageSpatialObject.h"

#include "itkLogger.h"
#include "itkCommand.h"
#include "itkStdStreamLogOutput.h"

//BeginLatex
// This example illustrates how to use the DICOM image reader.
//
//EndLatex

//BeginLatex
//To use this class, appropriate callback subclasses need to be first defined. 
//This procedure is important because information is passed from the reader class 
//to the application using information loaded events. The events could be error 
//events or events loaded with dicom information such as modality and patient ID. 
//
//For example, callback class to observe Modality information is defined as follows.
//EndLatex
//BeginCodeSnippet
class DICOMImageModalityInformationCallback: public itk::Command
{
public:
  typedef DICOMImageModalityInformationCallback    Self;
  typedef itk::SmartPointer<Self>                  Pointer;
  typedef itk::Command                             Superclass;
  itkNewMacro(Self);

  typedef igstk::ImageSpatialObject< 
                                short, 
                                3 >                ImageSpatialObjectType;
  
 
  typedef igstk::DICOMModalityEvent DICOMModalityEventType;
  
  void Execute(const itk::Object *caller, const itk::EventObject & event)
    {

    }
  void Execute(itk::Object *caller, const itk::EventObject & event)
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
//EndCodeSnippet
//BeginLatex
//Simialar callback classes need to be defined to observe patient name and 
//image reading error. Full list of events related with the DICOM image reader
//are given in section ?????
//EndLatex
class DICOMImagePatientNameInformationCallback: public itk::Command
{
public:
  typedef DICOMImagePatientNameInformationCallback    Self;
  typedef itk::SmartPointer<Self>                     Pointer;
  typedef itk::Command                                Superclass;
  itkNewMacro(Self);

  typedef igstk::ImageSpatialObject< 
                                short, 
                                3 >                   ImageSpatialObjectType;
  
 
  typedef igstk::DICOMPatientNameEvent DICOMPatientNameEventType;
  
  void Execute(const itk::Object *caller, const itk::EventObject & event)
    {

    }
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    const DICOMPatientNameEventType * patientNameEvent = 
          dynamic_cast < const DICOMPatientNameEventType* > ( &event );    
    std::cerr << "PatientName= " << patientNameEvent->Get() << std::endl;
      
    }
protected:
  DICOMImagePatientNameInformationCallback()   { };

private:
};
int main( int argc, char* argv[] )
{
  
  igstk::RealTimeClock::Initialize();

  if(argc < 2)
    {
    std::cerr<<"Usage: "<<argv[0]<<"  DICOMSeries "<<std::endl;
    return EXIT_FAILURE;
    }
  
  typedef itk::Logger              LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  //BeginLatex
  //In this example, we would like to read a CT image. Therefore, first 
  //a CT image reader object is insantiated as follows. 
  //EndLatex
  //BeginCodeSnippet
  typedef igstk::CTImageReader        ReaderType;
  ReaderType::Pointer   reader = ReaderType::New();
  //EndCodeSnippet
  //BeginLatex
  //A logger can be linked to the reader.
  //EndLatex
  //BeginCodeSnippet
  reader->SetLogger( logger );
  //EndCodeSnippet
  
  /* Read in a DICOM series */
  
  std::cout << "Reading the DICOM series : " << argv[1] <<std::endl;
  ReaderType::DirectoryNameType directoryName = argv[1];
  //BeginLatex
  //First, dicom image directory is set 
  //EndLatex
  //BeginCodeSnippet
  reader->RequestSetDirectory( directoryName );
  //EndCodeSnippet
  //BeginLatex
  //Next, the user makes a request to read the image.
  //EndLatex
  //BeginCodeSnippet
  reader->RequestReadImage();
  //EndCodeSnippet
  //BeginLatex
  //To access DICOM information about this image, callback objects 
  //need to be instantiated and added to the observer list of the reader
  //object.
  //EndLatex
  //BeginCodeSnippet
  typedef DICOMImageModalityInformationCallback  
                                     ModalityCallbackType;

  ModalityCallbackType::Pointer dimcb = ModalityCallbackType::New();
  reader->AddObserver( igstk::DICOMModalityEvent(), dimcb );
  reader->RequestModalityInformation(); 
  //EndCodeSnippet 
  //BeginLatex
  //Similar operation can be also done to access the patient name 
  //EndLatex
  //BeginCodeSnippet
  /* Add observer to listen to patient name  info */
  typedef DICOMImagePatientNameInformationCallback 
                                        PatientCallbackType;

  PatientCallbackType::Pointer dipncb = PatientCallbackType::New();
  reader->AddObserver( igstk::DICOMPatientNameEvent(), dipncb );
  reader->RequestPatientNameInformation(); 
  //EndCodeSnippet  
  return EXIT_SUCCESS;
}

