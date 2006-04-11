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
 
//BeginCodeSnippet
class DICOMImageModalityInformationCallback: public itk::Command
{
public:
  typedef DICOMImageModalityInformationCallback    Self;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::Command                      Superclass;
  itkNewMacro(Self);

  typedef igstk::ImageSpatialObject< 
                                short, 
                                3                                
                                       > ImageSpatialObjectType;
  
 
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
//BeginCodeSnippet
class DICOMImagePatientNameInformationCallback: public itk::Command
{
public:
  typedef DICOMImagePatientNameInformationCallback    Self;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::Command                      Superclass;
  itkNewMacro(Self);

  typedef igstk::ImageSpatialObject< 
                                short, 
                                3                                
                                       > ImageSpatialObjectType;
  
 
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
//EndCodeSnippet
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

  //BeginCodeSnippet
  typedef igstk::CTImageReader        ReaderType;
//EndCodeSnippet
  
  ReaderType::Pointer   reader = ReaderType::New();
  reader->SetLogger( logger );

  /* Read in a DICOM series */
  
  std::cout << "Reading the DICOM series : " << argv[1] <<std::endl;
  ReaderType::DirectoryNameType directoryName = argv[1];
  //BeginCodeSnippet
  reader->RequestSetDirectory( directoryName );
  //EndCodeSnippet
  //BeginCodeSnippet
  reader->RequestReadImage();
  //EndCodeSnippet
 /* Add observer to listen to modality info */
  //BeginCodeSnippet
  typedef DICOMImageModalityInformationCallback  
                                     ModalityCallbackType;

  ModalityCallbackType::Pointer dimcb = ModalityCallbackType::New();
  reader->AddObserver( igstk::DICOMModalityEvent(), dimcb );
  reader->RequestModalityInformation(); 
//EndCodeSnippet 
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

