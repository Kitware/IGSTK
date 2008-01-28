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
#include "igstkCoordinateSystemInterfaceMacros.h"


namespace igstk
{

itkEventMacro( TrackerToolEvent,StringEvent);
itkEventMacro( TrackerToolErrorEvent,TrackerToolEvent);
itkEventMacro( TrackerToolConfigurationEvent,TrackerToolEvent);
itkEventMacro( TrackerToolConfigurationErrorEvent,TrackerToolErrorEvent);
itkEventMacro( TrackerToolAttachmentToTrackerEvent,TrackerToolEvent);
itkEventMacro( TrackerToolAttachmentToTrackerErrorEvent,TrackerToolErrorEvent);
itkEventMacro( TrackerToolDetachmentFromTrackerEvent,TrackerToolEvent);
itkEventMacro( TrackerToolDetachmentFromTrackerErrorEvent,TrackerToolErrorEvent);
itkEventMacro( TrackerToolMadeTransitionToTrackedStateEvent,TrackerToolEvent);
itkEventMacro( TrackerToolNotAvailableToBeTrackedEvent,TrackerToolEvent);
itkEventMacro( ToolTrackingStartedEvent,TrackerToolEvent);
itkEventMacro( ToolTrackingStoppedEvent,TrackerToolEvent);

class Tracker;

/**  \class TrackerTool
  *  \brief Abstract superclass for concrete IGSTK TrackerTool classes.
  *
  *  This class provides a generic implementation of a tool of
  *  a tracker. This may contain hardware specific details of 
  *  the tool, along with the fields for position, orientation
  *  and error associated with the measurement used.
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
  igstkStandardAbstractClassTraitsMacro( TrackerTool, Object )

public:

  igstkFriendClassMacro( Tracker );

  typedef Tracker           TrackerType;
  typedef Transform         TransformType;

  /** Get the tool transform. */
  igstkGetMacro( CalibratedTransformWithRespectToReferenceTrackerTool,
                                   TransformType ); 

  /** Get the calibration transform for this tool. */
  igstkGetMacro( CalibrationTransform, TransformType );

  /**  Set the calibration transform for this tool. */
  void SetCalibrationTransform( const TransformType & );

  /** Get the raw transform for this tool. */
  igstkGetMacro( RawTransform, TransformType );

  /** Get calibrated raw transform for this tool. */
  igstkGetMacro( CalibratedTransform, TransformType );

  /** Get whether the tool was updated during tracker UpdateStatus() */
  igstkGetMacro( Updated, bool );
 
  /** The "RequestConfigure" method attempts to configure the tracker tool */
  virtual void RequestConfigure( void );

  /** The "RequestDetach" method detaches the tracker tool from the 
   * tracker. */
  virtual void RequestDetach( );

  /** Access the unique identifier to the tracker tool */
  std::string GetTrackerToolIdentifier( );

protected:

  TrackerTool(void);

  virtual ~TrackerTool(void);

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Set a unique identifier to the tracker tool */
  void SetTrackerToolIdentifier( std::string identifier );

  /** The "RequestAttachToTracker" method attaches the tracker tool to a
   * tracker. */
  virtual void RequestAttachToTracker( TrackerType * );

private:

  TrackerTool(const Self&);       //purposely not implemented
  void operator=(const Self&);    //purposely not implemented

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

  /** Check if the tracker tool is configured or not. This method should
   *  be implemented in the derived classes. */
  virtual bool CheckIfTrackerToolIsConfigured( ) = 0;

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

  /** Report tracker tool is in visible state. */ 
  void ReportTrackerToolVisibleStateProcessing( void );

  /** Report tracker tool not available state. */ 
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
   * tracker tool. */
  TransformType   m_CalibratedTransformWithRespectToReferenceTrackerTool;

  /** Calibration transform for the tool */
  TransformType                 m_CalibrationTransform; 

  /** Calibrated raw transform for the tool */
  TransformType                 m_CalibratedTransform; 

  /** raw transform for the tool */
  TransformType                 m_RawTransform; 

  /** Updated flag */
  bool               m_Updated;

  /** Unique identifier of the tracker tool */
  std::string        m_TrackerToolIdentifier;

  /** Tracker to which the tool will be attached to */
  Tracker        * m_TrackerToAttachTo;

  /** Define the coordinate system interface 
   */
  igstkCoordinateSystemClassInterfaceMacro();

};

std::ostream& operator<<(std::ostream& os, const TrackerTool& o);

}

#endif //__igstk_TrackerTool_h_
