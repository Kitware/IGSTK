/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolCalibration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
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

/** \class ToolCalibration
 * \brief  Represents the tool calibration structure
 *
 * \ingroup Object
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

  // Should be using State machine
  void RequestSetTime(const char* time);
  void RequestSetDate(const char* date);

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

private:
 
  /** Null operation for a State Machine transition */
  void NoProcessing();
  void SetTimeProcessing();
  void SetDateProcessing();

  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  ToolCalibration(const Self&);     // purposely not implemented
  void operator=(const Self&);      // purposely not implemented
  
  /** Inputs to the State Machine */
  igstkDeclareInputMacro( RequestDate );
  igstkDeclareInputMacro( RequestTime ); 
  igstkDeclareInputMacro( Initialize );
  igstkDeclareInputMacro( ValidTime );
  igstkDeclareInputMacro( InvalidTime );
  igstkDeclareInputMacro( ValidDate );
  igstkDeclareInputMacro( InvalidDate );

  /** States for the State Machine */
  igstkDeclareStateMacro( Initialize );
  igstkDeclareStateMacro( ValidCalibration );
  igstkDeclareStateMacro( InvalidCalibration );
  std::string     m_Date;
  const char*     m_DateToSet;

  std::string     m_Time;
  const char*     m_TimeToSet;
};

} // end namespace igstk

#endif // __igstkToolCalibration_h
