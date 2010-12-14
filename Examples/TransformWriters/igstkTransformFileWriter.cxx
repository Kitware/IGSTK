/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTransformFileWriter.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <itksys/SystemTools.hxx>
#include "igstkTransformFileWriter.h"

namespace igstk
{ 

TransformFileWriter::TransformFileWriter() : 
m_StateMachine( this )
{
  //define the state machine's states 
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AttemptingSetData );
  igstkAddStateMacro( AttemptingSetWriter );
  igstkAddStateMacro( DataSet );
  igstkAddStateMacro( WriterSet );
  igstkAddStateMacro( AttemptingSetWriterHaveData );
  igstkAddStateMacro( AttemptingSetDataHaveWriter );
  igstkAddStateMacro( Initialized );  

  //define the state machines inputs
  igstkAddInputMacro( SetWriter );
  igstkAddInputMacro( SetData );
  igstkAddInputMacro( Write );
  igstkAddInputMacro( Success );
  igstkAddInputMacro( Failure );

  //define the state machine's transitions
  igstkAddTransitionMacro( Idle,
                           SetWriter,
                           AttemptingSetWriter,
                           SetWriter );
  igstkAddTransitionMacro( Idle,
                           SetData,
                           AttemptingSetData,
                           SetData );
  igstkAddTransitionMacro( Idle,
                           Write,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           Success,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           Failure,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingSetData,
                           SetWriter,
                           AttemptingSetData,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingSetData,
                           SetData,
                           AttemptingSetData,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingSetData,
                           Write,
                           AttemptingSetData,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingSetData,
                           Success,
                           DataSet,
                           ReportSetDataSuccess );
  igstkAddTransitionMacro( AttemptingSetData,
                           Failure,
                           Idle,
                           ReportSetDataFailure );

  igstkAddTransitionMacro( AttemptingSetWriter,
                           SetWriter,
                           AttemptingSetWriter,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingSetWriter,
                           SetData,
                           AttemptingSetWriter,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingSetWriter,
                           Write,
                           AttemptingSetWriter,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingSetWriter,
                           Success,
                           WriterSet,
                           ReportSetWriterSuccess );
  igstkAddTransitionMacro( AttemptingSetWriter,
                           Failure,
                           Idle,
                           ReportSetWriterFailure );

  igstkAddTransitionMacro( DataSet,
                           SetWriter,
                           AttemptingSetWriterHaveData,
                           SetWriter );
  igstkAddTransitionMacro( DataSet,
                           SetData,
                           AttemptingSetData,
                           SetData );
  igstkAddTransitionMacro( DataSet,
                           Write,
                           DataSet,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( DataSet,
                           Success,
                           DataSet,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( DataSet,
                           Failure,
                           DataSet,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( WriterSet,
                           SetWriter,
                           AttemptingSetWriter,
                           SetWriter );
  igstkAddTransitionMacro( WriterSet,
                           SetData,
                           AttemptingSetDataHaveWriter,
                           SetData );
  igstkAddTransitionMacro( WriterSet,
                           Write,
                           WriterSet,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( WriterSet,
                           Success,
                           WriterSet,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( WriterSet,
                           Failure,
                           WriterSet,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingSetWriterHaveData,
                           SetWriter,
                           AttemptingSetWriterHaveData,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingSetWriterHaveData,
                           SetData,
                           AttemptingSetWriterHaveData,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingSetWriterHaveData,
                           Write,
                           AttemptingSetWriterHaveData,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingSetWriterHaveData,
                           Success,
                           Initialized,
                           ReportSetWriterSuccess );
  igstkAddTransitionMacro( AttemptingSetWriterHaveData,
                           Failure,
                           DataSet,
                           ReportSetWriterFailure );

  igstkAddTransitionMacro( AttemptingSetDataHaveWriter,
                           SetWriter,
                           AttemptingSetDataHaveWriter,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingSetDataHaveWriter,
                           SetData,
                           AttemptingSetDataHaveWriter,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingSetDataHaveWriter,
                           Write,
                           AttemptingSetDataHaveWriter,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingSetDataHaveWriter,
                           Success,
                           Initialized,
                           ReportSetDataSuccess );
  igstkAddTransitionMacro( AttemptingSetDataHaveWriter,
                           Failure,
                           WriterSet,
                           ReportSetDataFailure );

  igstkAddTransitionMacro( Initialized,
                           SetWriter,
                           AttemptingSetWriterHaveData,
                           SetWriter );
  igstkAddTransitionMacro( Initialized,
                           SetData,
                           AttemptingSetDataHaveWriter,
                           SetData );
  igstkAddTransitionMacro( Initialized,
                           Write,
                           Initialized,
                           Write );
  igstkAddTransitionMacro( Initialized,
                           Success,
                           Initialized,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Initialized,
                           Failure,
                           Initialized,
                           ReportInvalidRequest );
  
  
  //set the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // done setting the state machine, Writey to run
  this->m_StateMachine.SetReadyToRun();
}


TransformFileWriter::~TransformFileWriter()
{
}

void 
TransformFileWriter::RequestSetWriter(
   const TransformXMLFileWriterBase::Pointer transformWriter )
{
  igstkLogMacro( DEBUG, "igstk::TransformFileWriter::"
                        "RequestSetWriter called...\n" );
  this->m_TmpXMLFileWriter = transformWriter;

  igstkPushInputMacro( SetWriter );
  this->m_StateMachine.ProcessInputs();

}


void 
TransformFileWriter::RequestSetData( TransformDataType::Pointer &transformData,  
                                      const std::string &fileName  )
{
  igstkLogMacro( DEBUG, "igstk::TransformFileWriter::"
                        "RequestSetData called...\n" );
  this->m_TmpFileName = fileName;
  this->m_TmpTransformData = transformData;

  igstkPushInputMacro( SetData );
  this->m_StateMachine.ProcessInputs();
}


void 
TransformFileWriter::RequestWrite()
{
  igstkLogMacro( DEBUG, "igstk::TransformFileWriter::"
                        "RequestWrite called...\n" );
  
  igstkPushInputMacro( Write );
  this->m_StateMachine.ProcessInputs();
}


void 
TransformFileWriter::ReportInvalidRequestProcessing()
{
  igstkLogMacro( DEBUG, "igstk::TransformFileWriter::"
                 "ReportInvalidRequestProcessing called...\n" );
  this->InvokeEvent( InvalidRequestErrorEvent() );

}


void 
TransformFileWriter::SetWriterProcessing()
{
  if( this->m_TmpXMLFileWriter.IsNull() ||
    ( this->m_TransformData.IsNotNull() && 
      !this->m_TmpXMLFileWriter->IsCompatible(this->m_TransformData) ) )
    {
    igstkPushInputMacro( Failure );
    }
  else
    {
    this->m_XMLFileWriter = this->m_TmpXMLFileWriter;
    this->m_TmpXMLFileWriter = NULL;
    igstkPushInputMacro( Success );
    }
  this->m_StateMachine.ProcessInputs();
}


void 
TransformFileWriter::ReportSetWriterSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                 "igstk::TransformFileWriter::"
                 "ReportSetWriteeSuccessProcessing called...\n");
  this->InvokeEvent( SetWriterSuccessEvent() );
}


void 
TransformFileWriter::ReportSetWriterFailureProcessing()
{
  igstkLogMacro( DEBUG,
                 "igstk::TransformFileWriter::"
                 "ReportSetWriterFailureProcessing called...\n");
  this->InvokeEvent( SetWriterFailureEvent() );
}

void 
TransformFileWriter::SetDataProcessing()
{
  if( this->m_TmpFileName.empty()  || this->m_TmpTransformData.IsNull() ||
    ( this->m_XMLFileWriter.IsNotNull() && 
      !this->m_XMLFileWriter->IsCompatible(this->m_TmpTransformData) ) )
    {
    igstkPushInputMacro( Failure );
    }
  else
    {
    this->m_FileName = this->m_TmpFileName;
    this->m_TransformData = this->m_TmpTransformData;
    this->m_TmpFileName.clear();
    this->m_TmpTransformData = NULL;
    igstkPushInputMacro( Success );
    }
  this->m_StateMachine.ProcessInputs();
}


void 
TransformFileWriter::ReportSetDataSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                 "igstk::TransformFileWriter::"
                 "ReportSetDataSuccessProcessing called...\n");
  this->InvokeEvent( SetDataSuccessEvent() );
}


void 
TransformFileWriter::ReportSetDataFailureProcessing()
{
  igstkLogMacro( DEBUG,
                 "igstk::TransformFileWriter::"
                 "ReportSetDataFailureProcessing called...\n");
  this->InvokeEvent( SetDataFailureEvent() );
}


void 
TransformFileWriter::WriteProcessing()
{
  //check that the file name is not a directory
  if( itksys::SystemTools::FileIsDirectory( this->m_FileName.c_str() ) )
    {
    std::ostringstream msg;
    msg<<"File ("<<this->m_FileName<<") is a directory";
    this->m_WriteFailureErrorMessage = msg.str();
    igstkPushInputMacro( Failure );
    }
  else
    {
    this->m_XMLFileWriter->SetFilename( this->m_FileName );
    this->m_XMLFileWriter->SetObject( this->m_TransformData.GetPointer() );
    try 
      {       //write the xml file
      this->m_XMLFileWriter->WriteFile();
      igstkPushInputMacro( Success );
      }
    //an exception is thrown if the xml file cannot be written
    catch( std::exception &e ) 
      { 
      this->m_WriteFailureErrorMessage = e.what();
      igstkPushInputMacro( Failure );
      }
    }
  this->m_StateMachine.ProcessInputs();
}


void 
TransformFileWriter::ReportWriteSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                 "igstk::TransformFileWriter::"
                 "ReportWriteSuccessProcessing called...\n");
  this->InvokeEvent( WriteSuccessEvent() );
}


void 
TransformFileWriter::ReportWriteFailureProcessing()
{
  WriteFailureEvent evt;
  evt.Set( this->m_WriteFailureErrorMessage );

  igstkLogMacro( DEBUG,
                 "igstk::TransformFileWriter::"
                 "ReportWriteFailureProcessing called...\n");
  this->InvokeEvent( evt );
}


void  
TransformFileWriter::PrintSelf( std::ostream& itkNotUsed(os), 
                                itk::Indent itkNotUsed(indent) ) const
{
}


}//namespace
