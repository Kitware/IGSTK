/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerToolNew.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTrackerToolNew_h
#define __igstkTrackerToolNew_h

#include "igstkObject.h"
#include "igstkTransform.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkAxesObject.h"


namespace igstk
{

itkEventMacro( TrackerToolNewEvent,StringEvent);
itkEventMacro( TrackerToolNewErrorEvent,TrackerToolNewEvent);
itkEventMacro( TrackerToolNewConfigurationEvent,TrackerToolNewEvent);
itkEventMacro( TrackerToolNewConfigurationErrorEvent,TrackerToolNewErrorEvent);
itkEventMacro( TrackerToolNewAttachmentToTrackerEvent,TrackerToolNewEvent);
itkEventMacro( TrackerToolNewAttachmentToTrackerErrorEvent,TrackerToolNewErrorEvent);
itkEventMacro( TrackerToolNewDetachmentFromTrackerEvent,TrackerToolNewEvent);
itkEventMacro( TrackerToolNewDetachmentFromTrackerErrorEvent,TrackerToolNewErrorEvent);
itkEventMacro( TrackerToolNewMadeTransitionToTrackedStateEvent,TrackerToolNewEvent);
itkEventMacro( TrackerToolNewNotAvailableToBeTrackedEvent,TrackerToolNewEvent);
itkEventMacro( ToolTrackingStartedEvent,TrackerToolNewEvent);
itkEventMacro( ToolTrackingStoppedEvent,TrackerToolNewEvent);

class TrackerNew;
class PolarisTrackerNew;
class MicronTrackerNew;
class AuroraTrackerNew;

/**  \class TrackerToolNew
  *  \brief Generic implementation of the Tracker tool.
  *
  *  This class provides a generic implementation of a tool of
  *  a tracker. This may contain hardware specific details of 
  *  the tool, along with the fields for position, orientation
  *  and error associated with the measurement used.
  *
  *
  *  \image html  igstkTrackerToolNew.png  "TrackerToolNew State Machine Diagram"
  *  \image latex igstkTrackerToolNew.eps  "TrackerToolNew State Machine Diagram" 
  *
  *  \ingroup Tracker
  * 
  */

class TrackerToolNew : public Object
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( TrackerToolNew, Object )

public:

  igstkFriendClassMacro( TrackerNew );

  typedef TrackerNew        TrackerType;
  typedef Transform         TransformType;
  typedef Transform         CalibrationTransformType;
  typedef double            ErrorType;
  typedef double            TimePeriodType;

  /** typedefs for the coordinate reference system */
  typedef AxesObject        CoordinateReferenceSystemType;
  
  /** Get the tool transform. */
  igstkGetMacro( CalibratedTransformWithRespectToReferenceTrackerTool,
                                   TransformType ); 

  /** Get the calibration transform for this tool. */
  igstkGetMacro( CalibrationTransform, CalibrationTransformType );

  /**  Set the calibration transform for this tool. */
  void SetCalibrationTransform( const CalibrationTransformType & );

  /** Get the raw transform for this tool. */
  igstkGetMacro( RawTransform, TransformType );

  /** Get calibrated raw transform for this tool. */
  igstkGetMacro( CalibratedTransform, TransformType );

  /** Get whether the tool was updated during tracker UpdateStatus() */
  igstkGetMacro( Updated, bool );
 
  /** Request attaching the SpatialObject given as argument as an
   *  object to track with this tracker tool. The SpatialObject will
   *  become a child of the coordinate reference system of this TrackerToolNew,
   *  and in this way its transform to world coordinates will be computed
   *  by composing its callibration transform with the transform relating
   *  the TrackerToolNew and the Tracker 
   */
  void RequestAttachSpatialObject( SpatialObject * );

  /** The "RequestConfigure" method attempts to configure the tracker tool */
  virtual void RequestConfigure( void );

  /** The "RequestAttachToTracker" method attaches the tracker tool to a
 * tracker*/
  virtual void RequestAttachToTracker( TrackerType * );

  /** The "RequestDetach" method detaches the tracker tool from the 
 * tracker*/
  virtual void RequestDetach( );

  /** Access the unique identifier to the tracker tool */
  std::string GetTrackerToolIdentifier( );

protected:

  TrackerToolNew(void);

  ~TrackerToolNew(void);

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Set a unique identifier to the tracker tool */
  void SetTrackerToolIdentifier( std::string identifier );

 private:

 /** Push TrackingStarted input to the tracker tool */
  virtual void ReportTrackingStarted( );

  /** Push TrackingStopped input to the tracker tool  */
  virtual void ReportTrackingStopped( );

  /** Push TrackerToolNotAvailable input to the tracker tool */
  virtual void ReportTrackingToolNotAvailable( );

  /** Push TrackerToolVisible input to the tracker tool */
  virtual void ReportTrackingToolVisible( );

  /** Report successful tracker tool attachment */ 
  void ReportSuccessfulTrackerToolAttachment();

  /** Report failure in tracker tool attachment attempt */ 
  void ReportFailedTrackerToolAttachment();

  /** Set calibrated raw transform with respect to a reference
    * tracker tool */
  void SetCalibratedTransformWithRespectToReferenceTrackerTool
                                ( const TransformType & transform ); 

  /** Set the raw transform for this tool. */
  void SetRawTransform( const TransformType & );

  /** Set the calibrated raw transform for this tool. */
  void SetCalibratedTransform( const TransformType & );

  /** Set whether the tool was updated during tracker UpdateStatus() */
  igstkSetMacro( Updated, bool );

  /** Get boolean variable to check if the tracker tool is 
   * configured or not */
  virtual bool CheckIfTrackerToolIsConfigured( );

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ConfigureTool );
  igstkDeclareInputMacro( ToolConfigurationSuccess );
  igstkDeclareInputMacro( ToolConfigurationFailure );
  igstkDeclareInputMacro( AttachToolToTracker );
  igstkDeclareInputMacro( TrackingStarted );
  igstkDeclareInputMacro( TrackingStopped );
  igstkDeclareInputMacro( TrackerToolNotAvailable );
  igstkDeclareInputMacro( TrackerToolVisible );
  igstkDeclareInputMacro( DetachTrackerToolFromTracker ); 
  igstkDeclareInputMacro( AttachmentToTrackerSuccess );
  igstkDeclareInputMacro( AttachmentToTrackerFailure );
  igstkDeclareInputMacro( DetachmentFromTrackerSuccess );
  igstkDeclareInputMacro( DetachmentFromTrackerFailure );
 
  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingToConfigureTrackerTool );
  igstkDeclareStateMacro( Configured );
  igstkDeclareStateMacro( AttemptingToAttachTrackerToolToTracker );
  igstkDeclareStateMacro( Attached );
  igstkDeclareStateMacro( AttemptingToDetachTrackerToolFromTracker );
  igstkDeclareStateMacro( NotAvailable );
  igstkDeclareStateMacro( Tracked );

  /** Attempt method to configure */
  void AttemptToConfigureProcessing( void );

  /** Attempt method to attach tracker tool to the tracker */
  void AttemptToAttachTrackerToolToTrackerProcessing( void );

  /** Post-processing after a successful tracker tool configuration */
  void TrackerToolConfigurationSuccessProcessing( void );

  /** Post-processing after a failed tracker tool configuration */
  void TrackerToolConfigurationFailureProcessing( void );

  /** Post-processing after a successful tracker tool to tracker 
      attachment attempt . */ 
  void TrackerToolAttachmentToTrackerSuccessProcessing( void );

  /** Post-processing after a failed attachment attempt . */ 
  void TrackerToolAttachmentToTrackerFailureProcessing( void );

  /** Attempt method to detach tracker tool from the tracker */
  void AttemptToDetachTrackerToolFromTrackerProcessing( void );

  /** Post-processing after a successful detachment of the tracker tool
   *  from the tracker. */ 
  void TrackerToolDetachmentFromTrackerSuccessProcessing( void );

  /** Post-processing after a failed detachment attempt . */ 
  void TrackerToolDetachmentFromTrackerFailureProcessing( void );

  /** Report tracker tool is in visible state*/ 
  void ReportTrackerToolVisibleStateProcessing( void );

  /** Report tracker tool not available state*/ 
  void ReportTrackerToolNotAvailableProcessing( void );

  /** Report tracking started */ 
  void ReportTrackingStartedProcessing( void );

  /** Report tracking stopped */ 
  void ReportTrackingStoppedProcessing( void );

  /** Report invalid request */ 
  void ReportInvalidRequestProcessing( void );

  /** No operation for state machine transition */ 
  void NoProcessing( void );

  /** Calibrated raw transform with respect to reference
    * tracker tool */
  TransformType      
       m_CalibratedTransformWithRespectToReferenceTrackerTool;   

  /** Calibration transform for the tool */
  CalibrationTransformType      m_CalibrationTransform; 

  /** Calibrated raw transform for the tool */
  CalibrationTransformType      m_CalibratedTransform; 

  /** raw transform for the tool */
  TransformType                 m_RawTransform; 

  /** Updated flag */
  bool               m_Updated;

  /** Unique identifier of the tracker tool */
  std::string        m_TrackerToolIdentifier;

  /** Coordinate Reference System */
  CoordinateReferenceSystemType::Pointer    m_CoordinateReferenceSystem;

  /** Tracker to which the tool will be attached to */
  TrackerNew        * m_TrackerToAttachTo;

  /** Define the coordinate system interface 
   */
  igstkCoordinateSystemClassInterfaceMacro();

};

std::ostream& operator<<(std::ostream& os, const TrackerToolNew& o);

}

#endif //__igstk_TrackerToolNew_h_
