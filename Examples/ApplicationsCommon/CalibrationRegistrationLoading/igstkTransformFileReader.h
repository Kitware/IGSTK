#ifndef __igstkTransformFileReader_h
#define __igstkTransformFileReader_h

#include "igstkStateMachine.h"
#include "igstkTransformXMLFileReaderBase.h"
#include "igstkPrecomputedTransformData.h"
#include "igstkMacros.h"
#include "igstkObject.h"


namespace igstk
{
/** \class TransformFileReader
 * 
 *  \brief This class is an IGSTK wrapper for all readers of xml files 
 *         containing a precomputed transformation. 
 *         The specific type of transformation is defined by the actual reader
 *         specified with the RequestSetReader() method.
 *
 */
class TransformFileReader : public Object 
{
public:

  /**Macro with standard traits declarations (Self, SuperClass, state machine 
    * etc.). */
  igstkStandardClassTraitsMacro( TransformFileReader, Object )

  /** This is the container type which holds all the data associated with the 
   *  transformation (transformation, computation time...).*/
  typedef PrecomputedTransformData  TransformDataType;

  /**
   * Set a specific reader, which also defines the type of transformation.
   */
  void RequestSetReader( TransformXMLFileReaderBase::Pointer transformReader );

  /**
   * Set the file name from which the data is loaded.
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
   * Get the transformation data. This method generates a TransformDataEvent if
   * the data was loaded, otherwise an InvalidRequestErrorEvent.
   */
  void RequestGetData();

  /** This event is generated when the transformation data is requested and the
   *  data is available. */
  igstkLoadedEventMacro( TransformDataEvent, IGSTKEvent,
                         TransformDataType::Pointer );
 
     /** This event is generated if setting the xml reader succeeded. */
  igstkEventMacro( SetReaderSuccessEvent, IGSTKEvent );

    /** This event is generated if setting the xml reader failed. */
  igstkEventMacro( SetReaderFailureEvent, IGSTKErrorEvent );

    /** This event is generated if setting the file name succeeded. */
  igstkEventMacro( SetFileNameSuccessEvent, IGSTKEvent );
 
    /** This event is generated if setting the file name failed. */
  igstkEventMacro( SetFileNameFailureEvent, IGSTKErrorEvent );

    /** This event is generated if setting the file name succeeded. */
  igstkEventMacro( ReadSuccessEvent, IGSTKEvent );

    /** This event is generated if setting the file name failed. */
  igstkLoadedEventMacro( ReadFailureEvent, IGSTKErrorEvent, 
                         EventHelperType::StringType );

protected:

  TransformFileReader( void );
  ~TransformFileReader( void );

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

  TransformDataType::Pointer m_TransformData;
  TransformXMLFileReaderBase::Pointer m_TmpXMLFileReader;
  TransformXMLFileReaderBase::Pointer m_XMLFileReader;
  std::string m_TmpFileName;
  std::string m_FileName;

              //if reading fails the ReadProcessing() method updates this string
              //with an appropriate error message which the user recieves when
              //the ReportReadFailureProcessing() generates the ReadFailureEvent
  EventHelperType::StringType m_ReadFailureErrorMessage;
};


}
#endif //__igstkTransformFileReader_h
