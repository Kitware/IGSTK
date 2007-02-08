/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolCalibration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkToolCalibration_h
#define __igstkToolCalibration_h

#include "igstkObject.h"
#include "igstkStateMachine.h"
#include "igstkTransform.h"

namespace igstk
{

namespace Friends 
{
class ToolCalibrationReaderToToolCalibration;
}


/** \class ToolCalibration
 * \brief  Represents the tool calibration structure.
 *
 * This class is intended to factorize behaviors and data structures that are
 * common between multiple classes that provide callibrations for tracker
 * tools.
 * 
 * \image html  igstkToolCalibration.png 
 *              "Tool Calibration State Machine Diagram"
 * \image latex igstkToolCalibration.eps 
 *              "Tool Calibration State Machine Diagram"
 *
 * \ingroup Calibration
 */
class ToolCalibration : public Object
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( ToolCalibration, Object )

  typedef Transform                       TransformType;

  /** Method to get the final calibration transform with rotation 
   *  and translation */
  igstkGetMacro( CalibrationTransform, TransformType );

  /** Return the date and time */
  void RequestGetTime();
  void RequestGetDate();
  void RequestGetToolType();
  void RequestGetToolManufacturer();
  void RequestGetToolPartNumber();
  void RequestGetToolSerialNumber();

  /** Type used for representing strings */
  typedef std::string  StringType;

  /** Set the fields */
  void RequestSetTime( const StringType & time );
  void RequestSetDate( const StringType & date );
  void RequestSetToolType( const StringType & type );
  void RequestSetToolManufacturer( const StringType & manufacturer );
  void RequestSetToolPartNumber( const StringType & partNumber );
  void RequestSetToolSerialNumber( const StringType & serialNumber );

  /** The ToolCalibrationReaderToToolCalibration class is declared as a
   * friend in order to be able to set the transform */
  igstkFriendClassMacro(
               igstk::Friends::ToolCalibrationReaderToToolCalibration );

protected:

  ToolCalibration( void );
  virtual ~ToolCalibration( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  TransformType       m_CalibrationTransform;

  /** Return the date via event */
  virtual void ReportDateProcessing();
  /** Return the time via event */
  virtual void ReportTimeProcessing();
  /** Return the Tool Type via event */
  virtual void ReportToolTypeProcessing();
  /** Return the Tool Manufacturer via event */
  virtual void ReportToolManufacturerProcessing();
  /** Return the Tool PartNumber via event */
  virtual void ReportToolPartNumberProcessing();
  /** Return the Tool SerialNumber via event */
  virtual void ReportToolSerialNumberProcessing();

private:
 
  /** Null operation for a State Machine transition */
  void NoProcessing();
  void SetTimeProcessing();
  void SetDateProcessing();
  void SetToolTypeProcessing();
  void SetToolManufacturerProcessing();
  void SetToolPartNumberProcessing();
  void SetToolSerialNumberProcessing();

  void SetTransform(const TransformType & transform);

  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  ToolCalibration(const Self&);     // purposely not implemented
  void operator=(const Self&);      // purposely not implemented
  
  /** Inputs to the State Machine */
  igstkDeclareInputMacro( RequestDate );
  igstkDeclareInputMacro( RequestTime ); 
  igstkDeclareInputMacro( RequestToolType );
  igstkDeclareInputMacro( RequestToolManufacturer );
  igstkDeclareInputMacro( RequestToolPartNumber );
  igstkDeclareInputMacro( RequestToolSerialNumber ); 
  igstkDeclareInputMacro( Initialize );
  igstkDeclareInputMacro( ValidTime );
  igstkDeclareInputMacro( InvalidTime );
  igstkDeclareInputMacro( ValidDate );
  igstkDeclareInputMacro( InvalidDate );
  igstkDeclareInputMacro( ValidToolType );
  igstkDeclareInputMacro( InvalidToolType );
  igstkDeclareInputMacro( ValidToolManufacturer );
  igstkDeclareInputMacro( InvalidToolManufacturer );
  igstkDeclareInputMacro( ValidToolPartNumber );
  igstkDeclareInputMacro( InvalidToolPartNumber );
  igstkDeclareInputMacro( ValidToolSerialNumber );
  igstkDeclareInputMacro( InvalidToolSerialNumber );

  /** States for the State Machine */
  igstkDeclareStateMacro( Initialize );
  igstkDeclareStateMacro( ValidDate );
  igstkDeclareStateMacro( ValidTime );
  igstkDeclareStateMacro( ValidToolType );
  igstkDeclareStateMacro( ValidToolManufacturer );
  igstkDeclareStateMacro( ValidToolPartNumber );
  igstkDeclareStateMacro( ValidToolSerialNumber );
  igstkDeclareStateMacro( ValidCalibration );
  igstkDeclareStateMacro( InvalidCalibration );

  StringType     m_Date;
  StringType     m_DateToSet;
  StringType     m_Time;
  StringType     m_TimeToSet;
  StringType     m_ToolType;
  StringType     m_ToolTypeToSet;
  StringType     m_ToolManufacturer;
  StringType     m_ToolManufacturerToSet;
  StringType     m_ToolPartNumber;
  StringType     m_ToolPartNumberToSet;
  StringType     m_ToolSerialNumber;
  StringType     m_ToolSerialNumberToSet;

};

} // end namespace igstk

#endif // __igstkToolCalibration_h
