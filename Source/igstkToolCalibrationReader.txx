/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolCalibrationReader.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkToolCalibrationReader_txx
#define __igstkToolCalibrationReader_txx

#include "igstkToolCalibrationReader.h"
#include "zlib/zlib.h"

namespace igstk
{ 
/** Constructor */
template <class TCalibration>
ToolCalibrationReader<TCalibration>
::ToolCalibrationReader() : m_StateMachine(this)
{
  m_Calibration = CalibrationType::New();
  m_FileName = "";

  //Set the state descriptors
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( ObjectFileNameRead );
  igstkAddStateMacro( ObjectRead );
  igstkAddStateMacro( ObjectAttemptingRead );

  /** List of  Inputs */
  igstkAddInputMacro( ReadObjectRequest );
  igstkAddInputMacro( ObjectReadingError );
  igstkAddInputMacro( ObjectReadingSuccess );
  igstkAddInputMacro( ObjectFileNameValid );
  igstkAddInputMacro( ObjectFileNameIsEmpty );
  igstkAddInputMacro( ObjectFileNameIsDirectory );
  igstkAddInputMacro( ObjectFileNameDoesNotExist );
  igstkAddInputMacro( RequestCalibration );

  igstkAddTransitionMacro( Idle, ObjectFileNameValid, 
                           ObjectFileNameRead, SetFileName );
  igstkAddTransitionMacro( Idle, ObjectFileNameIsEmpty, 
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ObjectFileNameIsDirectory, 
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ObjectFileNameDoesNotExist, 
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ReadObjectRequest, 
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectFileNameRead, ReadObjectRequest, 
                           ObjectAttemptingRead, AttemptReadObject );
  igstkAddTransitionMacro( ObjectRead, ObjectFileNameValid, 
                           ObjectFileNameRead, SetFileName );
  igstkAddTransitionMacro( ObjectRead, ObjectFileNameIsEmpty, 
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectRead, ObjectFileNameIsDirectory, 
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectRead, ObjectFileNameDoesNotExist, 
                           Idle, ReportInvalidRequest );

  igstkAddTransitionMacro( ObjectRead, RequestCalibration,
                           ObjectRead, ReportCalibration );


  //Errors related to Object reading 
  igstkAddTransitionMacro( ObjectAttemptingRead, ObjectReadingError, 
                           Idle, ReportObjectReadingError );
  igstkAddTransitionMacro( ObjectAttemptingRead, ObjectReadingSuccess, 
                           ObjectRead, ReportObjectReadingSuccess );

  // Select the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();

} 

/** Destructor */
template <class TCalibration>
ToolCalibrationReader<TCalibration>
::~ToolCalibrationReader()  
{
}

/* This function reports invalid requests */
template <class TCalibration>
void
ToolCalibrationReader<TCalibration>
::ReportInvalidRequestProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ToolCalibrationReader::\
                        ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent( ObjectInvalidRequestErrorEvent() );
}


template <class TCalibration>
void
ToolCalibrationReader<TCalibration>
::ReportObjectReadingErrorProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ToolCalibrationReader::\
                        ReportObjectReadingErrorProcessing: called...\n");
  this->InvokeEvent( ObjectReadingErrorEvent() );
}


template <class TCalibration>
void
ToolCalibrationReader<TCalibration>
::ReportObjectReadingSuccessProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ToolCalibrationReader::\
                        ReportObjectReadingSuccessProcessing: called...\n");

  Friends::ToolCalibrationReaderToToolCalibration
   ::ConnectToolCalibration( this, this->m_Calibration.GetPointer() );

  this->InvokeEvent( ObjectReadingSuccessEvent() );
}

template <class TCalibration>
void
ToolCalibrationReader<TCalibration>
::RequestSetFileName( const FileNameType & filename )
{
  igstkLogMacro( DEBUG, "igstk::ToolCalibrationReader::\
                        RequestSetFileName called...\n");
  m_FileNameToBeSet = filename;

  if( filename.empty() )
    {
    this->m_StateMachine.PushInput( this->m_ObjectFileNameIsEmptyInput );
    this->m_StateMachine.ProcessInputs();
    return;
    }

  if( !itksys::SystemTools::FileExists( filename.c_str() ) )
    {
    this->m_StateMachine.PushInput( this->m_ObjectFileNameDoesNotExistInput );
    this->m_StateMachine.ProcessInputs();
    return;
    }

  if( itksys::SystemTools::FileIsDirectory( filename.c_str() ))
    {
    this->m_StateMachine.PushInput( this->m_ObjectFileNameIsDirectoryInput );
    this->m_StateMachine.ProcessInputs();
    return;
    }
  
  this->m_StateMachine.PushInput( this->m_ObjectFileNameValidInput );
  this->m_StateMachine.ProcessInputs();
}


template <class TCalibration>
void
ToolCalibrationReader<TCalibration>
::SetFileNameProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ToolCalibrationReader::\
                        SetFileNameProcessing called...\n");
  m_FileName = m_FileNameToBeSet;
}

template <class TCalibration>
void
ToolCalibrationReader<TCalibration>
::RequestGetCalibration() 
{
  m_StateMachine.PushInput( m_RequestCalibrationInput );
  m_StateMachine.ProcessInputs();
}

template <class TCalibration>
void
ToolCalibrationReader<TCalibration>
::ReportCalibrationProcessing() 
{
  CalibrationModifiedEvent event;
  event.Set( this->m_Calibration );
  this->InvokeEvent( event );
}

/** Request to read the object file */
template <class TCalibration>
void
ToolCalibrationReader<TCalibration>
::RequestReadObject()
{
  igstkLogMacro( DEBUG, "igstk::ToolCalibrationReader::\
                        RequestReadObject called...\n");
  this->m_StateMachine.PushInput( this->m_ReadObjectRequestInput);
  this->m_StateMachine.ProcessInputs();
}



/** Add a parameter */
template <class TCalibration>
bool 
ToolCalibrationReader<TCalibration>
::AddParameter( const ParameterNameType & name, float value )
{
  std::vector<ParameterType>::const_iterator it = m_Parameters.begin();
  while(it != m_Parameters.end())
    {
    if((*it).first == name)
      {
      // we have a duplicate name
      return false;
      }
    it++;
    }

  ParameterType param;
  param.first = name;
  param.second = value;
  m_Parameters.push_back(param);
  return true;
}
 
/** Get a parameter given its name */
template <class TCalibration>
bool ToolCalibrationReader<TCalibration>
::GetParameter( const ParameterNameType & name, float & value )
{
  std::vector<ParameterType>::const_iterator it = m_Parameters.begin();
  while(it != m_Parameters.end())
    {
    if((*it).first == name)
      {
      value = (*it).second;
      return true;
      }
    it++;
    }
  return false;
}


/** Get an error given its name */
template <class TCalibration>
bool ToolCalibrationReader<TCalibration>
::GetError( const ParameterNameType & name,float & value )
{
  std::vector<ErrorType>::const_iterator it = m_Errors.begin();
  while(it != m_Errors.end())
    {
    if((*it).first == name)
      {
      value = (*it).second;
      return true;
      }
    it++;
    }
  return false;
}

/** Return the transform */
template <class TCalibration>
const typename ToolCalibrationReader<TCalibration>::TransformType &
ToolCalibrationReader<TCalibration>
::GetTransform() const
{
  return m_Transform;
}

/** Return the calibration */
template <class TCalibration>
const typename ToolCalibrationReader<TCalibration>::CalibrationType *
ToolCalibrationReader<TCalibration>
::GetCalibration() const
{
  return m_Calibration;
}

/** Return the calibration */
template <class TCalibration>
typename ToolCalibrationReader<TCalibration>::CalibrationType *
ToolCalibrationReader<TCalibration>
::GetCalibration()
{
  return m_Calibration;
}


/** Add an error */
template <class TCalibration>
bool ToolCalibrationReader<TCalibration>
::AddError( const ParameterNameType & name, float value )
{
  std::vector<ErrorType>::const_iterator it = m_Errors.begin();
  while(it != m_Errors.end())
    {
    if((*it).first == name)
      {
      // we have a duplicate name
      return false;
      }
    it++;
    }

  ErrorType error;
  error.first = name;
  error.second = value;
  m_Errors.push_back(error);
  return true;
}

/** Convert a current Buffer to Windows file type */
template <class TCalibration>
void
ToolCalibrationReader<TCalibration>
::ConvertBufferToWindowsFileType(std::string & buffer)
{
  size_t cc;
  const char* inch = buffer.c_str();
  std::vector<char> outBuffer;
  size_t inStrSize = buffer.size();
  // Reserve enough space for most files
  outBuffer.reserve(inStrSize+1000);

  for ( cc = 0; cc < inStrSize; ++ cc )
    {
    if ( *inch == '\n' )
      {
      if ( cc == 0 || *(inch-1) != '\r' )
        {
        outBuffer.push_back('\r');
        }
      }
    outBuffer.push_back(*inch);
    inch ++;
    }
  outBuffer.push_back(0);
  buffer = &*outBuffer.begin();
}

/** Read the spatialobject file */
template <class TCalibration>
bool 
ToolCalibrationReader<TCalibration>
::ParseXML()
{
  m_Parameters.clear();
  m_Errors.clear();

  // Check the crc32
  // Read the file
  std::ifstream file;
  file.open(m_FileName.c_str(), std::ios::binary | std::ios::in);
  if(!file.is_open())
    {
    std::cout << "Cannot open file: " << m_FileName.c_str() << std::endl;
    return false;
    }
  file.seekg(0,std::ios::end);
  unsigned long fileSize = file.tellg();
  file.seekg(0,std::ios::beg);

  char* buf = new char[fileSize+1];
  file.read(buf,fileSize);
  buf[fileSize] = 0; 
  std::string buffer(buf);
  buffer.resize(fileSize);
  delete [] buf;
  file.close();

  // Convert the buffer to Windows file type
  this->ConvertBufferToWindowsFileType(buffer);

  // Find the <CRC32> tag
  long int crcpos = buffer.find("<CRC32>");
  if(crcpos == -1)
    {
    std::cout << "No CRC32 tag found in the file!" << std::endl;
    return false;
    }
 
  buffer.resize(crcpos);

  unsigned long crc = crc32(0L,(Bytef*)buffer.c_str(),crcpos);

  igstkLogMacro( DEBUG, "igstk::ToolCalibrationReader::ParseXML called...\n");
  XMLToolCalibrationReader::Pointer reader = XMLToolCalibrationReader::New();
  reader->SetFilename(m_FileName.c_str());
  if(!reader->CanReadFile(m_FileName.c_str()))
    {
    return false;
    }

  std::vector<XMLToolCalibrationReader::tag> tags = reader->GetTags();
  std::vector<XMLToolCalibrationReader::tag>::const_iterator it = tags.begin();
  while(it != tags.end())
    {
    // Creation block
    if(!itksys::SystemTools::Strucmp((*it).name.c_str(),"Creation"))
      {
      std::vector<XMLToolCalibrationReader::PairType> values = (*it).values;
      std::vector<XMLToolCalibrationReader::PairType>::const_iterator it2 
                                                            = values.begin();
      while(it2 != values.end())
        {
        if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"date"))
          {
          m_Calibration->RequestSetDate((*it2).second.c_str());
          }
        else if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"time"))
          {
          m_Calibration->RequestSetTime((*it2).second.c_str());
          }
        else if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"method"))
          {
          if(itksys::SystemTools::Strucmp((*it2).second.c_str(),
                                  m_Calibration->GetNameOfClass()))
            {
            return false;
            }
          }  
        it2++;
        }
      }

    // Tool block
    if(!itksys::SystemTools::Strucmp((*it).name.c_str(),"Tool"))
      {
      std::vector<XMLToolCalibrationReader::PairType> values = (*it).values;
      std::vector<XMLToolCalibrationReader::PairType>::const_iterator it2 
                                                            = values.begin();
      while(it2 != values.end())
        {
        if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"type"))
          {
          m_Calibration->RequestSetToolType((*it2).second.c_str());
          }
        else if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),
                                                            "manufacturer"))
          {
          m_Calibration->RequestSetToolManufacturer((*it2).second.c_str());
          }
        else if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),
                                                            "partNumber"))
          {
          m_Calibration->RequestSetToolPartNumber((*it2).second.c_str());
          }
        else if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),
                                                            "serialNumber"))
          {
          m_Calibration->RequestSetToolSerialNumber((*it2).second.c_str());
          }
        it2++;
        }
      }

    // Parameter block
    if(!itksys::SystemTools::Strucmp((*it).name.c_str(),"Parameter"))
      {
      std::vector<XMLToolCalibrationReader::PairType> values = (*it).values;
      std::vector<XMLToolCalibrationReader::PairType>::const_iterator it2 
                                                            = values.begin();
      std::string name = "";
      while(it2 != values.end())
        {
        if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"name"))
          {
          name = (*it2).second;
          }
        else if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"value"))
          {
          if(name.size()>0)
            {
            this->AddParameter(name.c_str(),atof((*it2).second.c_str()));
            }
          }
        it2++;
        }
      }

    // Error block
    if(!itksys::SystemTools::Strucmp((*it).name.c_str(),"Error"))
      {
      std::vector<XMLToolCalibrationReader::PairType> values = (*it).values;
      std::vector<XMLToolCalibrationReader::PairType>::const_iterator it2 
                                                            = values.begin();
      std::string name = "";
      while(it2 != values.end())
        {
        if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"name"))
          {
          name = (*it2).second;
          }
        else if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"value"))
          {
          if(name.size()>0)
            {
            this->AddError(name.c_str(),atof((*it2).second.c_str()));
            }
          }
        it2++;
        }
      }

    // CRC32 Block
    if(!itksys::SystemTools::Strucmp((*it).name.c_str(),"CRC32"))
      {
      std::vector<XMLToolCalibrationReader::PairType> values = (*it).values;
      if(atof((*(values.begin())).first.c_str()) != crc)
        {
        std::cout << "CRC is wrong: " 
                  << (*(values.begin())).first.c_str() << " : " 
                  << atof((*(values.begin())).first.c_str())
                  << " (expected: " << crc << ")" << std::endl;
        return false;
        }
      }
    it++;
    }
  return true;
}


/** Read the XML file */
template <class TCalibration>
void
ToolCalibrationReader<TCalibration>
::AttemptReadObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::CalibrationReader::\
                      AttemptReadObjectProcessing called...\n");
  
  if( this->ParseXML() )
    {
    this->m_StateMachine.PushInput( this->m_ObjectReadingErrorInput );
    return;
    }

  TransformType retrievedTransform;
  
  bool errorStatus = this->RetrieveParametersFromFile( retrievedTransform );
  
  if( errorStatus )
    {
    this->m_StateMachine.PushInput( this->m_ObjectReadingErrorInput );
    }
  else 
    {
    this->m_Transform = retrievedTransform;

    this->m_StateMachine.PushInput( this->m_ObjectReadingSuccessInput );
    }
}


/** Print Self function */
template <class TCalibration>
void
ToolCalibrationReader<TCalibration>
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Calibration " << m_Calibration << std::endl;
  os << indent << "Transform   " << m_Transform << std::endl;
}


} // end namespace igstk

#endif
