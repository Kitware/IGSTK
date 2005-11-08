/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_TrackerTool_h_
#define __igstk_TrackerTool_h_

#include "itkObject.h"
#include "igstkTransform.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"


namespace igstk
{

/**  \class TrackerTool
  *  \brief Generic implementation of the Tracker tool.
  *
  *  This class provides a generic implementation of a tool of
  *  a tracker. This may contain hardware specific details of 
  *  the tool, along with the fields for position, orientation
  *  and error associated with the measurement used.
  *
  *
  *
  *  \image html  igstkTrackerTool.png  "TrackerTool State Machine Diagram"
  *  \image latex igstkTrackerTool.eps  "TrackerTool State Machine Diagram" 
  *
  *
  */

class TrackerTool : public ::itk::Object
{
public:
  /** Some required typedefs for itk::Object. */

  typedef TrackerTool                    Self;
  typedef itk::Object                    Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(TrackerTool, ::itk::Object);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  

  /** Tool types */
  typedef enum
  {
    UnknownTool         = 0x00,             // unidentified tool type
    TrackedReference    = 0x01,
    TrackedPointer      = 0x02,
    FootPedal           = 0x03,
    SoftwareDefinedTool = 0x04,
    TrackedMicroscope   = 0x05,
    TrackedCArm         = 0x0A,
    TrackedCatheter     = 0x0B,
  } ToolType;

public:

  typedef Transform         TransformType;
  typedef Transform         ToolCalibrationTransformType;
  typedef double            ErrorType;
  typedef double            TimePeriodType;

  /** Get the tool transform. */
  igstkGetMacro( Transform, TransformType );

  /** Set the tool transform (called by Tracker). */
  void SetTransform( const TransformType & transform );

  /** Get the validity period for this tool. */
  igstkGetMacro( ValidityPeriod, TimePeriodType );

  /** Set the validity period for this tool. */
  igstkSetMacro( ValidityPeriod, TimePeriodType );
  
  /** Get the ToolType (set by subclasses of this class) */
  igstkGetMacro( ToolType, ToolType );

  /** Get the calibration transform for this tool. */
  igstkGetMacro( ToolCalibrationTransform, ToolCalibrationTransformType );

  /** Set the calibration transform for this tool. */
  igstkSetMacro( ToolCalibrationTransform, ToolCalibrationTransformType );

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

protected:

  TrackerTool(void);

  ~TrackerTool(void);

  igstkSetMacro( ToolType, ToolType );

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 


private:

  /** Position and Orientation of the tool */
  TransformType      m_Transform;

  /** Time in milliseconds for which this tool will be reporting results */
  TimePeriodType     m_ValidityPeriod;
  
  /** The type of the tool reflecting the property of the tool */
  ToolType           m_ToolType;
  
  /** Calibration transform for the tool */
  ToolCalibrationTransformType      m_ToolCalibrationTransform;

private:

  /** Inputs to the State Machine */
  InputType            m_InitializeInput;
  

  /** States for the State Machine */
  StateType            m_Invalid;
  StateType            m_NotAvailable;
  StateType            m_Available;
  StateType            m_Initialized;
  StateType            m_Tracking;
  StateType            m_Visible;


};

std::ostream& operator<<(std::ostream& os, const TrackerTool& o);

}

#endif //__igstk_TrackerTool_h_
