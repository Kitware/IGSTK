/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoImagerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkVideoImagerTool_h
#define __igstkVideoImagerTool_h

#include "igstkObject.h"
#include "igstkTransform.h"
#include "igstkFrame.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkCoordinateSystemInterfaceMacros.h"

class vtkImagedata;

namespace igstk
{

igstkEventMacro( VideoImagerToolEvent, StringEvent);
igstkEventMacro( VideoImagerToolErrorEvent, IGSTKErrorWithStringEvent);
igstkEventMacro( VideoImagerToolConfigurationEvent,VideoImagerToolEvent);
igstkEventMacro( VideoImagerToolConfigurationErrorEvent,
                                                     VideoImagerToolErrorEvent);
igstkEventMacro( InvalidRequestToAttachVideoImagerToolErrorEvent,
                                                     VideoImagerToolErrorEvent);
igstkEventMacro( InvalidRequestToDetachVideoImagerToolErrorEvent,
                                                     VideoImagerToolErrorEvent);
igstkEventMacro( VideoImagerToolAttachmentToVideoImagerEvent,
                                                          VideoImagerToolEvent);
igstkEventMacro( VideoImagerToolAttachmentToVideoImagerErrorEvent,
                                                     VideoImagerToolErrorEvent);
igstkEventMacro( VideoImagerToolDetachmentFromVideoImagerEvent,
                                                          VideoImagerToolEvent);
igstkEventMacro( VideoImagerToolDetachmentFromVideoImagerErrorEvent,
                                                     VideoImagerToolErrorEvent);
igstkEventMacro( VideoImagerToolMadeTransitionToStreamingStateEvent,
                                                          VideoImagerToolEvent);
igstkEventMacro( VideoImagerToolNotAvailableEvent,VideoImagerToolEvent);
igstkEventMacro( ToolImagingStartedEvent,VideoImagerToolEvent);
igstkEventMacro( ToolImagingStoppedEvent,VideoImagerToolEvent);
igstkLoadedEventMacro( FrameModifiedEvent, IGSTKEvent, igstk::Frame);

class VideoImager;

/**  \class VideoImagerTool
  *  \brief Abstract superclass for concrete IGSTK VideoImagerTool classes.
  *
  *  This class provides a generic implementation of an VideoImager.
  *  This may contain hardware specific details of
  *  the video-device.
  *
  *
  *  \image html  igstkVideoImagerTool.png "VideoImagerTool StateMachineDiagram"
  *  \image latex igstkVideoImagerTool.eps "VideoImagerTool StateMachineDiagram"
  *
  *  \ingroup VideoImager
  */

class VideoImagerTool : public Object
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardAbstractClassTraitsMacro( VideoImagerTool, Object )

public:

  igstkFriendClassMacro( VideoImager );

  typedef VideoImager       VideoImagerType;
  typedef Transform         TransformType;
  typedef Frame             FrameType;

  /** Get whether the tool was updated during VideoImager UpdateStatus() */
  igstkGetMacro( Updated, bool );

  /** The "RequestConfigure" method attempts to configure the VideoImager tool*/
  virtual void RequestConfigure( void );

  /** The "RequestDetachFromVideoImager" method detaches the VideoImager tool
   *  from the VideoImager. */
  virtual void RequestDetachFromVideoImager( );

  /** Request to get the internal frame as an event */
  virtual void RequestGetFrame();

  /** Access the unique identifier to the VideoImager tool */
  const std::string GetVideoImagerToolIdentifier( ) const;

  /** The "RequestAttachToVideoImager" method attaches the VideoImager tool to a
   * VideoImager. */
  virtual void RequestAttachToVideoImager( VideoImagerType * );

  /** Get the frame with the current index for this tool. */
  FrameType* GetInternalFrame( void );

  /** Set the frame for this tool. */
  void SetInternalFrame( FrameType* );

  void SetFrameDimensions( unsigned int * );
  void GetFrameDimensions( unsigned int * );

  igstkSetMacro( PixelDepth, unsigned int );
  igstkGetMacro( PixelDepth, unsigned int );

  igstkSetMacro( Delay, unsigned int );
  igstkGetMacro( Delay, unsigned int );

  igstk::Frame* GetFrameFromBuffer(const unsigned int index);
  igstk::Frame* GetTemporalCalibratedFrame();

protected:

  VideoImagerTool(void);
  virtual ~VideoImagerTool(void);

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Set a unique identifier to the VideoImager tool */
  void SetVideoImagerToolIdentifier( const std::string identifier );

private:

  VideoImagerTool(const Self&);       //purposely not implemented
  void operator=(const Self&);    //purposely not implemented

  /** Push ImagingStarted input to the VideoImager tool */
  virtual void RequestReportImagingStarted( );

  /** Push ImagingStopped input to the VideoImager tool  */
  virtual void RequestReportImagingStopped( );

  /** Push VideoImagerToolNotAvailable input to the VideoImager tool */
  virtual void RequestReportImagingToolNotAvailable( );

  /** Push VideoImagerToolStreaming input to the VideoImager tool */
  virtual void RequestReportImagingToolStreaming( );

  /** Push AttachmentToVideoImagerSuccess input to the VideoImager tool*/
  void RequestReportSuccessfulVideoImagerToolAttachment();

  /** Push AttachmentToVideoImagerFailure input to the VideoImager tool*/
  void RequestReportFailedVideoImagerToolAttachment();

  /** Set whether the tool was updated during VideoImager UpdateStatus() */
  igstkSetMacro( Updated, bool );

  /** Check if the VideoImager tool is configured or not. This method should
   *  be implemented in the derived classes. */
  virtual bool CheckIfVideoImagerToolIsConfigured( ) const = 0;

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ConfigureTool );
  igstkDeclareInputMacro( ToolConfigurationSuccess );
  igstkDeclareInputMacro( ToolConfigurationFailure );
  igstkDeclareInputMacro( AttachToolToVideoImager );
  igstkDeclareInputMacro( ImagingStarted );
  igstkDeclareInputMacro( ImagingStopped );
  igstkDeclareInputMacro( VideoImagerToolNotAvailable );
  igstkDeclareInputMacro( VideoImagerToolStreaming );
  igstkDeclareInputMacro( DetachVideoImagerToolFromVideoImager );
  igstkDeclareInputMacro( AttachmentToVideoImagerSuccess );
  igstkDeclareInputMacro( AttachmentToVideoImagerFailure );
  igstkDeclareInputMacro( DetachmentFromVideoImagerSuccess );
  igstkDeclareInputMacro( DetachmentFromVideoImagerFailure );
  igstkDeclareInputMacro( GetFrame );

  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingToConfigureVideoImagerTool );
  igstkDeclareStateMacro( Configured );
  igstkDeclareStateMacro( AttemptingToAttachVideoImagerToolToVideoImager );
  igstkDeclareStateMacro( Attached );
  igstkDeclareStateMacro( AttemptingToDetachVideoImagerToolFromVideoImager );
  igstkDeclareStateMacro( NotAvailable );
  igstkDeclareStateMacro( Streaming );

  /** Attempt method to configure */
  void AttemptToConfigureProcessing( void );

  /** Attempt method to attach VideoImager tool to the VideoImager */
  void AttemptToAttachVideoImagerToolToVideoImagerProcessing( void );

  /** Post-processing after a successful VideoImager tool configuration */
  void VideoImagerToolConfigurationSuccessProcessing( void );

  /** Post-processing after a failed VideoImager tool configuration */
  void VideoImagerToolConfigurationFailureProcessing( void );

  /** Post-processing after a successful VideoImager tool to VideoImager
      attachment attempt . */
  void VideoImagerToolAttachmentToVideoImagerSuccessProcessing( void );

  /** Post-processing after a failed attachment attempt . */
  void VideoImagerToolAttachmentToVideoImagerFailureProcessing( void );

  /** Attempt method to detach VideoImager tool from the VideoImager */
  void AttemptToDetachVideoImagerToolFromVideoImagerProcessing( void );

  /** Post-processing after a successful detachment of the VideoImager tool
   *  from the VideoImager. */
  void VideoImagerToolDetachmentFromVideoImagerSuccessProcessing( void );

  /** Post-processing after a failed detachment attempt. */
  void VideoImagerToolDetachmentFromVideoImagerFailureProcessing( void );

  /** Retrieve frame as an event. */
  void GetFrameProcessing( void );

  /** Report VideoImager tool is in streaming state. */
  void ReportVideoImagerToolStreamingStateProcessing( void );

  /** Report VideoImager tool not available state. */
  void ReportVideoImagerToolNotAvailableProcessing( void );

  /** Report imaging started */
  void ReportImagingStartedProcessing( void );

  /** Report imaging stopped */
  void ReportImagingStoppedProcessing( void );

  /** Report invalid request */
  void ReportInvalidRequestProcessing( void );

  /** Report invalid request to attach the VideoImager tool. */
  void ReportInvalidRequestToAttachVideoImagerToolProcessing( void );

  /** Report invalid request to detach the VideoImager tool. */
  void ReportInvalidRequestToDetachVideoImagerToolProcessing( void );

  /** No operation for state machine transition */
  void NoProcessing( void );

  /** Ring buffer for the tool */
  void AddFrameToBuffer( igstk::Frame* frame);

  /** Ring buffer with frames */
  std::vector< igstk::Frame* > *m_FrameRingBuffer;

  /** next frame will be stored at frameRingBuffer[m_Index] */
  int                           m_Index;
  unsigned int                  m_NumberOfFramesInBuffer;
  unsigned int                  m_MaxBufferSize;
  unsigned int                  m_Delay;
  unsigned int                  m_FrameDimensions[3];
  unsigned int                  m_PixelDepth;

  /** Updated flag */
  bool                          m_Updated;

  /** Unique identifier of the VideoImager tool */
  std::string        m_VideoImagerToolIdentifier;

  /** VideoImager to which the tool will be attached to */
  VideoImager        * m_VideoImagerToAttachTo;

  /** Define the coordinate system interface  */
  igstkCoordinateSystemClassInterfaceMacro();

  /** Helper coordinate system used point of reference
   * for the calibration Transform. */
  CoordinateSystem::Pointer   m_CalibrationCoordinateSystem;
};

std::ostream& operator<<(std::ostream& os, const VideoImagerTool& o);

}

#endif //__igstk_VideoImagerTool_h_
