/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPivotCalibrationReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
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

namespace Friends
{
/** \class PivotCalibrationReaderToPivotCalibration
 * 
 * \brief This class is intended to make the connection between the 
 * ToolCalibrationReader and its output, the ToolCalibration. 
 *
 * With this class it is possible to enforce encapsulation of the Reader and
 * the ToolCalibration, and make the SetTranslationAndRotation()
 * private, so that developers cannot gain access to the ITK or VTK layers of
 * these two classes.
 *
 */
class PivotCalibrationReaderToPivotCalibration
{
public:

  template<class TCalibrationReader>
  static void
  ConnectToolCalibration( const TCalibrationReader * reader, 
                                PivotCalibration * toolCalibration )
    {
    toolCalibration->SetRootMeanSquareError( 
                                            reader->GetRootMeanSquareError() );
    }

}; // end of PivotCalibrationReaderToPivotCalibration class
} // end of Friend namespace

/** \class PivotCalibrationReader
 * \brief This class reads an XML calibration file and returns a calibration
 *        transform.
 * 
 *
 * \ingroup Object
 */
class PivotCalibrationReader : 
            public ToolCalibrationReader< PivotCalibration >
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( PivotCalibrationReader, 
                              ToolCalibrationReader< PivotCalibration > );

  
  /** Declare the PivotCalibrationReaderToPivotCalibration class 
   *  to be a friend  in order to give it access to the private method 
   *  SetRootMeanSquareError(). */
  igstkFriendClassMacro( 
                   igstk::Friends::PivotCalibrationReaderToPivotCalibration );

protected:

  PivotCalibrationReader();
  ~PivotCalibrationReader();

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

  /** Read the parameters from the file */
  virtual bool RetrieveParametersFromFile( TransformType & transform );

  /** Report the calibration to the calibration object */
  virtual void ReportObjectReadingSuccessProcessing();

private:
  
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  PivotCalibrationReader(const Self&);         //purposely not implemented
  void operator=(const Self&);      //purposely not implemented

  /** Return the RootMeanSquareError */
  float GetRootMeanSquareError() const;

  float m_RootMeanSquareError;

};

} // end namespace igstk

#endif // __igstkPivotCalibrationReader_h
