/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTransformFileWriter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkTransformFileWriter_h
#define __igstkTransformFileWriter_h

#include "igstkStateMachine.h"
#include "igstkTransformXMLFileWriterBase.h"
#include "igstkPrecomputedTransformData.h"
#include "igstkMacros.h"
#include "igstkObject.h"


namespace igstk
{
/** \class TransformFileWriter
 * 
 *  \brief This class is an IGSTK wrapper for all writers of xml files 
 *         for precomputed transformation. 
 *         The specific type of transformation is defined by the actual writer
 *         specified with the RequestSetWriter() method.
 *
 */
class TransformFileWriter : public Object 
{
public:

  /**Macro with standard traits declarations (Self, SuperClass, state machine 
    * etc.). */
  igstkStandardClassTraitsMacro( TransformFileWriter, Object )

  /** This is the container type which holds all the data associated with the 
   *  transformation (transformation, computation time...).*/
  typedef PrecomputedTransformData  TransformDataType;

  /**
   * Set a specific writer, which also defines the type of transformation.
   */
  void RequestSetWriter(
     const TransformXMLFileWriterBase::Pointer transformWriter );

  /**
   * Set the required data.
   */
  void RequestSetData( TransformDataType::Pointer &transformData,  
                       const std::string &fileName );

  /**
   * Actually perform the write operation. This method generates a 
   * WriteSuccessEvent upon success and a WriteFailureEvent if writing failed. If
   * the writer hasn't been initialized yet (missing TransformDataType, file name
   * or xml writer) an InvalidRequestErrorEvent is generated.
   */
  void RequestWrite();

  
  /** This event is generated if setting the xml writer succeeded. */
  igstkEventMacro( SetWriterSuccessEvent, IGSTKEvent );

  /** This event is generated if setting the xml writer failed. */
  igstkEventMacro( SetWriterFailureEvent, IGSTKErrorEvent );

  /** This event is generated if setting the data succeeded. */
  igstkEventMacro( SetDataSuccessEvent, IGSTKEvent );
 
  /** This event is generated if setting the data failed. */
  igstkEventMacro( SetDataFailureEvent, IGSTKErrorEvent );

  /** This event is generated if writing succeeded. */
  igstkEventMacro( WriteSuccessEvent, IGSTKEvent );

  /** This event is generated if writing failed. */
  igstkLoadedEventMacro( WriteFailureEvent, IGSTKErrorEvent, 
                         EventHelperType::StringType );

  
protected:

  TransformFileWriter( void );
  virtual ~TransformFileWriter( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:

  /** List of state machine states */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingSetData );
  igstkDeclareStateMacro( AttemptingSetWriter );
  igstkDeclareStateMacro( DataSet );
  igstkDeclareStateMacro( WriterSet );
  igstkDeclareStateMacro( AttemptingSetWriterHaveData );
  igstkDeclareStateMacro( AttemptingSetDataHaveWriter );
  igstkDeclareStateMacro( Initialized );
  
  

  /** List of state machine inputs */
  igstkDeclareInputMacro( SetWriter );
  igstkDeclareInputMacro( SetData );
  igstkDeclareInputMacro( Write );
  igstkDeclareInputMacro( Success );
  igstkDeclareInputMacro( Failure );

  /**List of state machine actions*/
  void ReportInvalidRequestProcessing();  
  void SetWriterProcessing();
  void ReportSetWriterSuccessProcessing();
  void ReportSetWriterFailureProcessing();
  void SetDataProcessing();
  void ReportSetDataSuccessProcessing();
  void ReportSetDataFailureProcessing();
  void WriteProcessing();
  void ReportWriteSuccessProcessing();
  void ReportWriteFailureProcessing();
      
  TransformFileWriter( 
    const TransformFileWriter & other );

  const TransformFileWriter & operator=( 
    const TransformFileWriter & right );

  TransformDataType::Pointer          m_TmpTransformData;
  TransformDataType::Pointer          m_TransformData;
  std::string                         m_TmpFileName;
  std::string                         m_FileName;
  TransformXMLFileWriterBase::Pointer m_TmpXMLFileWriter;
  TransformXMLFileWriterBase::Pointer m_XMLFileWriter;

  //if writing fails the WriteProcessing() method updates this string
  //with an appropriate error message which the user recieves when
  //the ReportWriteFailureProcessing() generates the WriteFailureEvent
  EventHelperType::StringType m_WriteFailureErrorMessage;
};


}
#endif //__igstkTransformFileWriter_h
