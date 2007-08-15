/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTrackerTool_h
#define __igstkTrackerTool_h

#include "igstkObject.h"
#include "igstkTransform.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkAxesObject.h"


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
  *  \ingroup Tracker
  * 
  */

class TrackerTool : public Object
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( TrackerTool, Object )

public:

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

  /** typedefs for the coordinate reference system */
  typedef AxesObject        CoordinateReferenceSystemType;
  
  /** Get the tool transform. */
  igstkGetMacro( Transform, TransformType ); // FIXME : DEPRECATED : A REQUEST SHOULD BE MADE OR THIS SHOULD BE PRIVATE ONLY TO BE USED BY THE TRACKER.

  /** Set the tool transform (called by Tracker). */
  void RequestSetTransform( const TransformType & transform ); // FIXME: MUST BE PRIVATE : THE TRACKER MUST BE A FRIEND

  /** Get the validity period for this tool. */
  igstkGetMacro( ValidityPeriod, TimePeriodType ); // FIXME : DEPRECATED : NOT USED

  /** Set the validity period for this tool. */
  igstkSetMacro( ValidityPeriod, TimePeriodType ); // FIXME : DEPRECATED : NOT USED
  
  /** Get the ToolType (set by subclasses of this class) */
  igstkGetMacro( ToolType, ToolType ); // FIXME : DEPRECATED : NOT USED

  /** Get the calibration transform for this tool. */
  igstkGetMacro( ToolCalibrationTransform, ToolCalibrationTransformType );

  /** Set the calibration transform for this tool. */
  igstkSetMacro( ToolCalibrationTransform, ToolCalibrationTransformType );

  /** Get the raw, uncalibrated transform for this tool. */
  igstkGetMacro( RawTransform, TransformType );

  /** Set the raw, uncalibrated transform for this tool. */
  igstkSetMacro( RawTransform, TransformType );

  /** Get whether the tool was updated during tracker UpdateStatus() */
  igstkGetMacro( Updated, bool );

  /** Get whether the tool was updated during tracker UpdateStatus() */
  igstkSetMacro( Updated, bool );

  
  /** Request attaching the SpatialObject given as argument as an
   *  object to track with this tracker tool. The SpatialObject will
   *  become a child of the coordinate reference system of this TrackerTool,
   *  and in this way its transform to world coordinates will be computed
   *  by composing its callibration transform with the transform relating
   *  the TrackerTool and the Tracker 
   */
  void RequestAttachSpatialObject( SpatialObject * );

protected:

  TrackerTool(void);

  ~TrackerTool(void);

  igstkSetMacro( ToolType, ToolType );

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 


private:

  /** Position and Orientation of the tool */
  TransformType      m_Transform;   // FIXME: This is deprecated due to Bug 5474.
                                    // This transform should now be computed by the Spatial objects
                                    // that serve as the coordinate reference systems.
                                    // This transform is computed as the composition of the 
                                    // callibration transform (SO -> TrackerTool) and the
                                    // raw transform (TrackerTool -> Tracker ).

  /** Time in milliseconds for which this tool will be reporting results */
  TimePeriodType     m_ValidityPeriod;  // FIXME: DEPRECATED: THIS IS NOT BEING USED : The Transform has its own validity time.
  
  /** The type of the tool reflecting the property of the tool */
  ToolType           m_ToolType;    // FIXME : DEPRECATED : NOT IN USE
  
  /** Raw transform for the tool */
  TransformType      m_RawTransform; // FIXME: This is deprecated due to Bug 5474. 
                                     // This transform is now stored in the parent 
                                     // child relationship between the TrackerTool
                                     // and the Tracker. E.g. all uses of m_RawTransform
                                     // should become m_CoordinateReferenceSystem->GetTransformToParent();

  /** Calibration transform for the tool */
  ToolCalibrationTransformType      m_ToolCalibrationTransform; // FIXME: This is deprecated due to Bug 5474.
                                    // This transform is now the transform between the spatial object being
                                    // tracked and the coordinate system of this tracker tool.
                                    // When a spatial object is attached to a tracker tool, it becomes the child
                                    // of the tracker tool.

  /** Updated flag */
  bool               m_Updated;

  /** Coordinate Reference System */
  CoordinateReferenceSystemType::Pointer    m_CoordinateReferenceSystem;

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( Initialize );
  

  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( Invalid );
  igstkDeclareStateMacro( NotAvailable );
  igstkDeclareStateMacro( Available );
  igstkDeclareStateMacro( Initialized );
  igstkDeclareStateMacro( Tracking );
  igstkDeclareStateMacro( Visible );


};

std::ostream& operator<<(std::ostream& os, const TrackerTool& o);

}

#endif //__igstk_TrackerTool_h_
