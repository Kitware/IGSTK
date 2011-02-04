/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerDataLoggerConfigurationFileReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTrackerDataLoggerConfigurationFileReader_h
#define __igstkTrackerDataLoggerConfigurationFileReader_h

#include "igstkStateMachine.h"
#include "igstkTrackerConfigurationXMLFileReaderBase.h"
#include "igstkTrackerConfiguration.h"
#include "igstkMacros.h"
#include "igstkObject.h"


namespace igstk
{
/** \class TrackerDataLoggerConfigurationFileReader
 * 
 *  \brief This class is an IGSTK wrapper for all readers of xml files 
 *         containing a tracker configuration with additional 
 *         data (each tool should send data to the specified host on the
 *         specified port).
 *
 *         This class is an IGSTK wrapper for all readers of xml files 
 *         containing a tracker configuration with additional 
 *         data (each tool should send data to the specified host on the
 *         specified port).
 *         The specific type of tracker (Micron, Aurora...) configuration is 
 *         defined by the actual reader given in the RequestSetReader() method.
 *
 */
class TrackerDataLoggerConfigurationFileReader : public Object 
{
public:

  /**Macro with standard traits declarations (Self, SuperClass, state machine 
    * etc.). */
  igstkStandardClassTraitsMacro( TrackerDataLoggerConfigurationFileReader,
                                 Object )

  /** This is the container type which holds all the data associated with the 
   *  tracker configuration (e.g. COM port...).*/
  typedef TrackerConfiguration::Pointer  TrackerConfigurationPointer;

  typedef std::string FileNameDataType;
  typedef std::map<std::string, std::vector<FileNameDataType> > LoggerDataType;
       
  /**
   * A container with tracker and the names of open files. Each tool will send 
   * its tracking data to one or more locations.
   */
  struct ConfigurationDataType 
    {
    TrackerConfigurationPointer   m_TrackerConfiguration;
    LoggerDataType                m_ToolNamesAndOutputFileNames;
    };

  typedef ConfigurationDataType* ConfigurationDataPointerType;

  /**
   * Set a specific reader, which also defines the type of the tracker.
   * This method generates two events: If the reader is NULL 
   * SetReaderFailureEvent, otherwise SetReaderSuccessEvent.
   */
  void RequestSetReader( TrackerConfigurationXMLFileReaderBase::Pointer 
                         trackerConfigurationReader );

  /**
   * Set the file name from which the data is loaded.
   * This method generates two events: If the file name is empty
   * SetFileNameFailureEvent, otherwise SetFileNameSuccessEvent.
   */
  void RequestSetFileName( const std::string &fileName );

  /**
   * Actually perform the read operation. This method generates a 
   * ReadSuccessEvent upon success and a ReadFailureEvent if reading failed. 
   * If reading fails because the xml reader doesn't match the tracker type
   * then an UnexpectedTrackerTypeEvent is also generated. If
   * the reader hasn't been initialized yet (missing file name or xml reader) 
   * an InvalidRequestErrorEvent is generated.
   */
  void RequestRead();

  /**
   * Get the tracker configuration data. This method generates a 
   * ConfigurationDataEvent if the data was loaded, otherwise an 
   * InvalidRequestErrorEvent.
   */
  void RequestGetData();

  /** This event is generated when the configuration data is requested and the
   *  data is available. */
  igstkLoadedEventMacro( ConfigurationDataEvent, IGSTKEvent,
                         ConfigurationDataPointerType );
 
  /** This event is generated if setting the xml reader succeeded. */
  igstkEventMacro( SetReaderSuccessEvent, IGSTKEvent );

  /** This event is generated if setting the xml reader failed. */
  igstkEventMacro( SetReaderFailureEvent, IGSTKErrorEvent );

  /** This event is generated if setting the file name succeeded. */
  igstkEventMacro( SetFileNameSuccessEvent, IGSTKEvent );
 
  /** This event is generated if setting the file name failed. */
  igstkEventMacro( SetFileNameFailureEvent, IGSTKErrorEvent );

  /** This event is generated if the reading succeeded. */
  igstkEventMacro( ReadSuccessEvent, IGSTKEvent );

  /** This event is generated if reading failed. */
  igstkLoadedEventMacro( ReadFailureEvent, IGSTKErrorEvent, 
                         EventHelperType::StringType );

  /** This event is generated if reading failed because of a mismatch between
      the reader and the actual tracker data. */
  igstkEventMacro( UnexpectedTrackerTypeEvent, IGSTKEvent );

  /**
   * \class Class that observers both read success, 
   * igstk::TrackerDataLoggerConfigurationFileReader::ReadSuccessEvent
   * and read failure, 
   * igstk::TrackerDataLoggerConfigurationFileReader::ReadFailureEvent,
   * igstk::TrackerDataLoggerConfigurationFileReader::
   * UnexpectedTrackerTypeEvent,
   * events generated by the TrackerDataLoggerConfigurationFileReader class.
   */
  class ReadFailSuccessObserver : public ::itk::Command
    {
  public:
    typedef  ReadFailSuccessObserver    Self;
    typedef  ::itk::Command             Superclass;
    typedef  ::itk::SmartPointer<Self>  Pointer;
    itkNewMacro( Self );
  protected:
    ReadFailSuccessObserver() :
      m_GotSuccess( false ),
      m_GotFailure( false ),       
      m_GotUnexpectedTrackerType( false ) 
      { }
    ~ReadFailSuccessObserver() {}
  public:
    void Execute(itk::Object *caller, const itk::EventObject & event)
      {
      const itk::Object * constCaller = caller;
      this->Execute( constCaller, event );
      }
    void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & event)
      {
      if( dynamic_cast<const igstk::TrackerDataLoggerConfigurationFileReader::
        ReadSuccessEvent *>( &event) )
        {
        m_GotSuccess = true;
        }
      else if( const igstk::TrackerDataLoggerConfigurationFileReader::
        ReadFailureEvent *rfe = 
        dynamic_cast<const igstk::TrackerDataLoggerConfigurationFileReader::
        ReadFailureEvent *>( &event) )
        {
        m_GotFailure = true;
        m_FailureMessage = rfe->Get();
        }
      else if( dynamic_cast<const
        igstk::TrackerDataLoggerConfigurationFileReader::
        UnexpectedTrackerTypeEvent *>( &event) )
        {
        m_GotUnexpectedTrackerType = true;
        }
      }
    bool GotSuccess() {return this->m_GotSuccess;}
    bool GotFailure() {return this->m_GotFailure;}
    bool GotUnexpectedTrackerType() {return this->m_GotUnexpectedTrackerType;}
    void Reset() 
      { 
      this->m_GotSuccess = false; 
      this->m_GotFailure = false;
      this->m_GotUnexpectedTrackerType = false;
      this->m_FailureMessage.clear();
      }
    std::string GetFailureMessage() const
      {
      return m_FailureMessage;
      }
  private:
    std::string m_FailureMessage;
    bool m_GotSuccess;
    bool m_GotFailure;
    bool m_GotUnexpectedTrackerType;
    };


protected:

  TrackerDataLoggerConfigurationFileReader( void );
  virtual ~TrackerDataLoggerConfigurationFileReader( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:

  /** List of state machine states */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingSetFileName );
  igstkDeclareStateMacro( AttemptingSetReader );
  igstkDeclareStateMacro( FileNameSet );
  igstkDeclareStateMacro( ReaderSet );
  igstkDeclareStateMacro( AttemptingSetReaderHaveFileName );
  igstkDeclareStateMacro( AttemptingSetFileNameHaveReader );
  igstkDeclareStateMacro( Initialized );  
  igstkDeclareStateMacro( AttemptingRead );
  igstkDeclareStateMacro( HaveData );

  /** List of state machine inputs */
  igstkDeclareInputMacro( SetReader );
  igstkDeclareInputMacro( SetFileName );
  igstkDeclareInputMacro( Read );
  igstkDeclareInputMacro( GetData  );
  igstkDeclareInputMacro( Success );
  igstkDeclareInputMacro( Failure );

  /**List of state machine actions*/
  void ReportInvalidRequestProcessing();  
  void SetReaderProcessing();
  void ReportSetReaderSuccessProcessing();
  void ReportSetReaderFailureProcessing();
  void SetFileNameProcessing();
  void ReportSetFileNameSuccessProcessing();
  void ReportSetFileNameFailureProcessing();
  void ReadProcessing();
  void ReportReadSuccessProcessing();
  void ReportReadFailureProcessing();
  void GetDataProcessing();

  ConfigurationDataType                          m_LoggerTrackerConfiguration;
  TrackerConfigurationXMLFileReaderBase::Pointer m_TmpXMLFileReader;
  TrackerConfigurationXMLFileReaderBase::Pointer m_XMLFileReader;
  std::string                                    m_TmpFileName;
  std::string                                    m_FileName;

  //if reading fails the ReadProcessing() method updates this string
  //with an appropriate error message which the user recieves when
  //the ReportReadFailureProcessing() generates the ReadFailureEvent
  EventHelperType::StringType m_ReadFailureErrorMessage;

/**
 * This class observes the TrackerToolTransformUpdateEvent for a specific tool.
 * It checks that the event is for the relevant tool and then gets the tool's
 * transform w.r.t. the "world" coordinate system. The tool
 * information is written to output file(s). 
 */
// BeginLatex
// \code{ToolUpdatedObserver} class observes the
// TrackerToolTransformUpdateEvent for a specific tool. It checks that
// the event is for the relevant tool and then gets the tool's
// transform to its parent and outputs it to a file.
// EndLatex

   
};

}

#endif //__igstkTrackerDataLoggerConfigurationFileReader_h
