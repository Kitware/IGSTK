#ifndef __igstkDICOMImageReader_txx
#define   __igstkDICOMImageReader_txx

#include "igstkDICOMImageReader.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
template <class TPixelType>
DICOMImageReader<TPixelType>::DICOMImageReader() : m_StateMachine(this), m_Logger(NULL)
{ 

  //Set the state descriptors
  m_StateMachine.AddState(m_IdleState, "IdleState");
  m_StateMachine.AddState(m_ImageDirectoryNameReadState, "ImageDirectoryNameReadState");
  m_StateMachine.AddState(m_ImageReadState, "ImageReadState");

 /** List of State Inputs */

  m_StateMachine.AddInput(m_ImageDirectoryNameInput,"ImageDirectoryNameInput");
  m_StateMachine.AddInput(m_ReadImageRequestInput,"ImageReadRequestInput");

  m_StateMachine.AddTransition(m_IdleState,m_ImageDirectoryNameInput,m_ImageDirectoryNameReadState,&DICOMImageReader::ReadDirectoryName);
  m_StateMachine.AddTransition(m_ImageDirectoryNameReadState,m_ReadImageRequestInput,m_ImageReadState,&DICOMImageReader::ReadImage);


   // Select the initial state of the state machine
  m_StateMachine.SelectInitialState( m_IdleState );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();

  // Create the DICOM GDCM file reader
  m_names = itk::GDCMSeriesFileNames::New();
  m_io = itk::GDCMImageIO::New();
  m_ImageSeriesReader = ImageSeriesReaderType::New();
} 

/** Destructor */
template <class TPixelType>
DICOMImageReader<TPixelType>::~DICOMImageReader()  
{

}

template <class TPixelType>
void DICOMImageReader<TPixelType>::SetDirectory( const char *directory )
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::SetDirectoryName called...\n");
  this->SetImageDirectoryName(directory);
  this->m_StateMachine.PushInput( this->m_ImageDirectoryNameInput);
  this->m_StateMachine.ProcessInputs();
}


template <class TPixelType>
void DICOMImageReader<TPixelType>:: RequestImageRead()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::RequestImageRead called...\n");
  this->m_StateMachine.PushInput( this->m_ReadImageRequestInput);
  this->m_StateMachine.ProcessInputs();
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::RequestImageRead called...\n");
}

/** Read in the DICOM series image */
template <class TPixelType>
void DICOMImageReader<TPixelType>::ReadDirectoryName()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReadDirectoryName called...\n");
  m_names->SetInputDirectory( m_ImageDirectoryName);
  m_ImageSeriesReader->SetFileNames( m_names->GetInputFileNames() );
}

/** Read in the DICOM series image */
template <class TPixelType>
void DICOMImageReader<TPixelType>::ReadImage( )
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReadImage called...\n");
  m_ImageSeriesReader->SetImageIO( m_io );
  m_ImageSeriesReader->Update();
  this->m_itkExporter->SetInput( m_ImageSeriesReader->GetOutput() );
}

/* The ReportInvalidRequest function reports invalid requests */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportInvalidRequest()
{
    igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReportInvalidRequest called...\n");
    this->InvokeEvent(InvalidRequestErrorEvent());
}

/** Get the vtk image data  */
template <class TPixelType>
vtkImageData * 
DICOMImageReader<TPixelType>::GetVTKImageData() const 
 {
   return  this->m_vtkImporter->GetOutput();
 }


/** Get the itk image data */
template <class TPixelType>
typename DICOMImageReader<TPixelType>::ImageConstPointer 
DICOMImageReader<TPixelType>::GetITKImageData() const
{
  return( m_ImageSeriesReader->GetOutput() );
}


/** Print Self function */
template <class TPixelType>
void DICOMImageReader<TPixelType>::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk

#endif
