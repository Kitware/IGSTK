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
itkEventMacro( TrackerToolNewInitializationEvent,TrackerToolNewEvent);
itkEventMacro( TrackerToolNewInitializationErrorEvent,TrackerToolNewErrorEvent);
itkEventMacro( TrackerToolNewAttachmentToTrackerEvent,TrackerToolNewEvent);
itkEventMacro( TrackerToolNewAttachmentToTrackerErrorEvent,TrackerToolNewErrorEvent);
itkEventMacro( TrackerToolNewDetachmentFromTrackerEvent,TrackerToolNewEvent);
itkEventMacro( TrackerToolNewDetachmentFromTrackerErrorEvent,TrackerToolNewErrorEvent);
itkEventMacro( TrackerToolNewMadeTransitionToTrackedStateEvent,TrackerToolNewEvent);
itkEventMacro( TrackerToolNewNotAvailableToBeTrackedEvent,TrackerToolNewEvent);
itkEventMacro( ToolTrackingStartedEvent,TrackerToolNewEvent);
itkEventMacro( ToolTrackingStoppedEvent,TrackerToolNewEvent);

class TrackerNew;

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
  igstkGetMacro( Transform, TransformType ); // FIXME : DEPRECATED : A REQUEST SHOULD BE MADE OR THIS SHOULD BE PRIVATE ONLY TO BE USED BY THE TRACKER.

  /** Set the transform (composition of the raw and calibration transform ). */
  void RequestSetTransform( const TransformType & transform ); // FIXME: MUST BE PRIVATE : THE TRACKER MUST BE A FRIEND

  /** Get the calibration transform for this tool. */
  igstkGetMacro( CalibrationTransform, CalibrationTransformType );

  /** Request set the calibration transform for this tool. */
  void RequestSetCalibrationTransform( const CalibrationTransformType & );

  /** Request set the raw transform for this tool. */
  void RequestSetRawTransform( const TransformType & );

  /** Get the raw transform for this tool. */
  igstkGetMacro( RawTransform, TransformType );

  /** Get whether the tool was updated during tracker UpdateStatus() */
  igstkGetMacro( Updated, bool );

  /** Get whether the tool was updated during tracker UpdateStatus() */
  igstkSetMacro( Updated, bool );
  
  /** Request attaching the SpatialObject given as argument as an
   *  object to track with this tracker tool. The SpatialObject will
   *  become a child of the coordinate reference system of this TrackerToolNew,
   *  and in this way its transform to world coordinates will be computed
   *  by composing its callibration transform with the transform relating
   *  the TrackerToolNew and the Tracker 
   */
  void RequestAttachSpatialObject( SpatialObject * );

  /** The "RequestInitialize" method attempts to initialize the tracker tool */
  virtual void RequestInitialize( void );

  /** The "RequestAttachToTracker" method attaches the tracker tool to a
 * tracker*/
  virtual void RequestAttachToTracker( TrackerType * );

  /** The "RequestDetach" method detaches the tracker tool from the 
 * tracker*/
  virtual void RequestDetach( );

  /** Access the unique identifier to the tracker tool 
    * FIXME: when the tracker becomes a friend class of the
    * tracker tool: this method 
    * should be moved to private section */
  std::string GetTrackerToolIdentifier( );

  /** Push TrackingStarted input to the tracker tool 
    * FIXME: when the tracker becomes a friend class of the
    * tracker tool: this method 
    * should be moved to private section */
  virtual void ReportTrackingStarted( );

  /** Push TrackingStopped input to the tracker tool 
    * FIXME: when the tracker becomes a friend class of the
    * tracker tool: this method 
    * should be moved to private section */
  virtual void ReportTrackingStopped( );

  /** Push TrackerToolNotAvailable input to the tracker tool 
    * FIXME: when the tracker becomes a friend class of the
    * tracker tool: this method 
    * should be moved to private section */
  virtual void ReportTrackingToolNotAvailable( );

  /** Push TrackerToolVisible input to the tracker tool 
    * FIXME: when the tracker becomes a friend class of the
    * tracker tool: this method 
    * should be moved to private section */
  virtual void ReportTrackingToolIsInVisibleState( );

protected:

  TrackerToolNew(void);

  ~TrackerToolNew(void);

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Set a unique identifier to the tracker tool */
  void SetTrackerToolIdentifier( std::string identifier );

  private:

  /** Composition of the raw and calibration transform*/
  TransformType      m_Transform;   

  /** Calibration transform for the tool */
  CalibrationTransformType      m_CalibrationTransform; 

  /** raw transform for the tool */
  TransformType                 m_RawTransform; 

  /** Updated flag */
  bool               m_Updated;

  /** Unique identifier of the tracker tool */
  std::string        m_TrackerToolIdentifier;

  /** Coordinate Reference System */
  CoordinateReferenceSystemType::Pointer    m_CoordinateReferenceSystem;


private:

  /** Get boolean variable to check if the tracker tool is 
   * initialized or not */
  virtual bool GetTrackerToolInitialized( );

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( InitializeTool );
  igstkDeclareInputMacro( ToolInitializationSuccess );
  igstkDeclareInputMacro( ToolInitializationFailure );
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
  igstkDeclareStateMacro( AttemptingToInitializeTrackerTool );
  igstkDeclareStateMacro( Initialized );
  igstkDeclareStateMacro( AttemptingToAttachTrackerToolToTracker );
  igstkDeclareStateMacro( Attached );
  igstkDeclareStateMacro( AttemptingToDetachTrackerToolFromTracker );
  igstkDeclareStateMacro( NotAvailable );
  igstkDeclareStateMacro( Tracked );

  /** Attempt method to initialize */
  void AttemptToInitializeProcessing( void );

  /** Attempt method to attach tracker tool to the tracker */
  void AttemptToAttachTrackerToolToTrackerProcessing( void );

  /** Post-processing after a successful tracker tool initialization */
  void TrackerToolInitializationSuccessProcessing( void );

  /** Post-processing after a failed tracker tool initialization */
  void TrackerToolInitializationFailureProcessing( void );

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

  TrackerNew        * m_Tracker;

  /** Define the coordinate system interface 
   */
  igstkCoordinateSystemClassInterfaceMacro();

};

std::ostream& operator<<(std::ostream& os, const TrackerToolNew& o);

}

#endif //__igstk_TrackerToolNew_h_
