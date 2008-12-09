#ifndef __igstkTrackerConfigurationFileReader_h
#define __igstkTrackerConfigurationFileReader_h

#include "igstkStateMachine.h"
#include "igstkTrackerConfigurationXMLFileReaderBase.h"
#include "igstkTrackerConfiguration.h"
#include "igstkMacros.h"
#include "igstkObject.h"


namespace igstk
{
/** \class TrackerConfigurationFileReader
 * 
 *  \brief This class is an IGSTK wrapper for all readers of xml files 
 *         containing a tracker configuration. 
 *         The specific type of tracker configuration is defined by the 
 *         actual reader specified with the RequestSetReader() method.
 *
 */
class TrackerConfigurationFileReader : public Object 
{
public:

  /**Macro with standard traits declarations (Self, SuperClass, state machine 
    * etc.). */
  igstkStandardClassTraitsMacro( TrackerConfigurationFileReader, Object )

  /** This is the container type which holds all the data associated with the 
   *  tracker configuration (e.g. COM port...).*/
   //typedef const TrackerConfiguration::Pointer  TrackerConfigurationDataType;

   typedef TrackerConfigurationXMLFileReaderBase::Pointer   FileReaderBasePointerType;

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
   * ReadSuccessEvent upon success and a ReadFailureEvent if reading failed. If
   * the reader hasn't been initialized yet (missing file name or xml reader) 
   * an InvalidRequestErrorEvent is generated.
   */
  void RequestRead();

  /**
   * Get the tracker configuration data. This method generates a 
   * TrackerConfigurationDataEvent if the data was loaded, otherwise an 
   * InvalidRequestErrorEvent.
   */
  void RequestGetData();

  /** This event is generated when the configuration data is requested and the
   *  data is available. */
  class  TrackerConfigurationDataEvent : public IGSTKEvent 
  { 
  public:  
    typedef TrackerConfigurationDataEvent        Self; 
    typedef IGSTKEvent                           Superclass; 
    typedef igstk::TrackerConfiguration::Pointer PayloadType; 
    TrackerConfigurationDataEvent() {} 
    virtual ~TrackerConfigurationDataEvent() {} 
    virtual const char * GetEventName() const { 
      return "TrackerConfigurationDataEvent"; } 
    virtual bool CheckEvent(const ::itk::EventObject* e) const { 
      return dynamic_cast<const Self*>(e); } 
    virtual ::itk::EventObject* MakeObject() const { 
      return new Self; } 
    TrackerConfigurationDataEvent(const Self&s) :Superclass(s){}; 
    const PayloadType Get() const { 
      return m_Payload; }  
    void Set( PayloadType _var ) { 
      m_Payload = _var; }  
  private: 
    void operator=(const Self&);  
    PayloadType  m_Payload; 
  };
 
   /**
   * Class that observers both read success, 
   * igstk::TrackerConfigurationFileReader::ReadSuccessEventand 
   * and read failure,
   * igstk::TrackerConfigurationFileReader::ReadFailureEvent
   * events generated by the TrackerConfigurationFileReader class.
   */
  class ReadTrackerConfigurationFailSuccessObserver : public ::itk::Command
  {
  public:
    typedef  ReadTrackerConfigurationFailSuccessObserver    Self;
    typedef  ::itk::Command             Superclass;
    typedef  ::itk::SmartPointer<Self>  Pointer;
    itkNewMacro( Self );
  protected:
    ReadTrackerConfigurationFailSuccessObserver() : m_GotFailure( false ), m_GotSuccess( false ) {}
    ~ReadTrackerConfigurationFailSuccessObserver() {}
  public:
    void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      const itk::Object * constCaller = caller;
      this->Execute( constCaller, event );
    }
    void Execute(const itk::Object *caller, const itk::EventObject & event)
    {
    const igstk::TrackerConfigurationFileReader::ReadSuccessEvent *rse = 
      dynamic_cast<const igstk::TrackerConfigurationFileReader::ReadSuccessEvent *>( &event);

    if( rse )
      {
      m_GotSuccess = true;
      }
    else
      {
      const igstk::TrackerConfigurationFileReader::ReadFailureEvent *rfe = 
        dynamic_cast<const igstk::TrackerConfigurationFileReader::ReadFailureEvent *>( &event);

      if( rfe )
        {
        m_GotFailure = true;
        m_FailureMessage = rfe->Get();
        }
      }
    }

    bool GotSuccess() {return this->m_GotSuccess;}
    bool GotFailure() {return this->m_GotFailure;}
    void Reset() 
    { 
      this->m_GotSuccess = false; 
      this->m_GotFailure = false;
      this->m_FailureMessage.clear();
    }
    std::string GetFailureMessage() const
    {
      return m_FailureMessage;
    }
  private:
    std::string m_FailureMessage;
    bool m_GotFailure;
    bool m_GotSuccess;
  };

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

protected:

  TrackerConfigurationFileReader( void );
  ~TrackerConfigurationFileReader( void );

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

  igstk::TrackerConfiguration::Pointer m_TrackerConfiguration;
  
  FileReaderBasePointerType m_TmpXMLFileReader;
  FileReaderBasePointerType m_XMLFileReader;

  std::string m_TmpFileName;
  std::string m_FileName;

              //if reading fails the ReadProcessing() method updates this string
              //with an appropriate error message which the user recieves when
              //the ReportReadFailureProcessing() generates the ReadFailureEvent
  EventHelperType::StringType m_ReadFailureErrorMessage;
};


}
#endif //__igstkTrackerConfigurationFileReader_h
