/*=========================================================================

  Program:   PivotCalibration Guided Surgery Software Toolkit
  Module:    igstkPivotCalibrationReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkPivotCalibrationReader_h
#define __igstkPivotCalibrationReader_h

#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkObject.h"
#include "igstkEvents.h"

#include "igstkPivotCalibration.h"
#include "igstkToolCalibrationReader.h"

namespace igstk
{

/** \class PivotCalibrationReader
 * \brief This class read an XML calibration file and returns a calibration
 *        transform.
 * 
 *
 * \ingroup Object
 */
class PivotCalibrationReader : public ToolCalibrationReader<igstk::PivotCalibration>
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( PivotCalibrationReader, 
                                 ToolCalibrationReader<igstk::PivotCalibration> )

  /** Transform typedef */
  typedef Superclass::CalibrationType       CalibrationType;
  
  /** Type for representing the string of the filename. */
  typedef std::string    FileNameType;

  /** Method to pass the directory name containing the spatial object data */
  void RequestSetFileName( const FileNameType & filename );

  /** This method request Object read **/
  void RequestReadObject();

 /** This method request the calibration **/
  void RequestGetCalibration();

protected:

  PivotCalibrationReader();
  ~PivotCalibrationReader();

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

  itkEventMacro( ObjectReaderEvent,              IGSTKEvent        );
  itkEventMacro( ObjectReadingErrorEvent,        ObjectReaderEvent );
  itkEventMacro( ObjectReadingSuccessEvent,      ObjectReaderEvent );
  itkEventMacro( ObjectInvalidRequestErrorEvent, ObjectReaderEvent );

  virtual void AttemptReadObjectProcessing();

  /** Return the calibration via event */
  virtual void ReportCalibrationProcessing();

private:
  
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  PivotCalibrationReader(const Self&);         //purposely not implemented
  void operator=(const Self&);      //purposely not implemented

   /** List of States */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( ObjectFileNameRead );
  igstkDeclareStateMacro( ObjectRead );
  igstkDeclareStateMacro( ObjectAttemptingRead );

  /** List of State Inputs */
  igstkDeclareInputMacro( RequestCalibration ); 
  igstkDeclareInputMacro( ReadObjectRequest );
  igstkDeclareInputMacro( ObjectFileNameValid ); 
  igstkDeclareInputMacro( ObjectFileNameIsEmpty ); 
  igstkDeclareInputMacro( ObjectFileNameIsDirectory ); 
  igstkDeclareInputMacro( ObjectFileNameDoesNotExist ); 

  /** Error related state inputs */
  igstkDeclareInputMacro( ObjectReadingError );
  igstkDeclareInputMacro( ObjectReadingSuccess );

  /** Method to be invoked only by the StateMachine. Accepts the filename */
  void SetFileNameProcessing();

  /** Method to be invoked only by the StateMachine. Reports that an input was
   * received during a State where that input is not a valid request. For
   * example, asking to read the file before providing the filename */
  void ReportInvalidRequestProcessing();

  /** Method to be invoked only by the StateMachine. This function reports an
   * error while reading */
  void ReportObjectReadingErrorProcessing();

  /** Method to be invoked only by the StateMachine. This function reports the
   * success of the reading process. */
  void ReportObjectReadingSuccessProcessing();

  FileNameType m_FileNameToBeSet;

};

} // end namespace igstk

#endif // __igstkPivotCalibrationReader_h
