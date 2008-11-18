/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImagerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkImagerTool_h
#define __igstkImagerTool_h

#include "igstkObject.h"
#include "igstkTransform.h"
#include "igstkFrame.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkCoordinateSystemInterfaceMacros.h"

class vtkImagedata;

namespace igstk
{

igstkEventMacro( ImagerToolEvent, StringEvent);
igstkEventMacro( ImagerToolErrorEvent, IGSTKErrorWithStringEvent);
igstkEventMacro( ImagerToolConfigurationEvent,ImagerToolEvent);
igstkEventMacro( ImagerToolConfigurationErrorEvent,ImagerToolErrorEvent);

igstkEventMacro( InvalidRequestToAttachImagerToolErrorEvent,
               ImagerToolErrorEvent);

igstkEventMacro( InvalidRequestToDetachImagerToolErrorEvent,
               ImagerToolErrorEvent);

igstkEventMacro( ImagerToolAttachmentToImagerEvent,ImagerToolEvent);
igstkEventMacro( ImagerToolAttachmentToImagerErrorEvent,
                                                       ImagerToolErrorEvent);
igstkEventMacro( ImagerToolDetachmentFromImagerEvent,ImagerToolEvent);
igstkEventMacro( ImagerToolDetachmentFromImagerErrorEvent,
                                                       ImagerToolErrorEvent);
igstkEventMacro( ImagerToolMadeTransitionToTrackedStateEvent,
                                                            ImagerToolEvent);
igstkEventMacro( ImagerToolNotAvailableToBeTrackedEvent,ImagerToolEvent);
igstkEventMacro( ToolImagingStartedEvent,ImagerToolEvent);
igstkEventMacro( ToolImagingStoppedEvent,ImagerToolEvent);

igstkLoadedEventMacro( FrameModifiedEvent, IGSTKEvent, igstk::Frame);

class Imager;

/**  \class ImagerTool
  *  \brief Abstract superclass for concrete IGSTK ImagerTool classes.
  *
  *  This class provides a generic implementation of a tool of
  *  an imager. This may contain hardware specific details of 
  *  the tool.
  *
  *
  *  \image html  igstkImagerTool.png  "ImagerTool State Machine Diagram"
  *  \image latex igstkImagerTool.eps  "ImagerTool State Machine Diagram" 
  *
  *  \ingroup Imager
  * 
  */

class ImagerTool : public Object
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardAbstractClassTraitsMacro( ImagerTool, Object )

public:

  igstkFriendClassMacro( Imager );

  typedef Imager            ImagerType;
  typedef Transform         TransformType;
  typedef Frame             FrameType;

  /** Get the calibration transform for this tool. */
//  igstkGetMacro( CalibrationTransform, TransformType );

  ///**  Set the calibration transform for this tool. */
  //void SetCalibrationTransform( const TransformType & );

  /** Get whether the tool was updated during imager UpdateStatus() */
  igstkGetMacro( Updated, bool );
 
  /** The "RequestConfigure" method attempts to configure the imager tool */
  virtual void RequestConfigure( void );

  /** The "RequestDetachFromImager" method detaches the imager tool from the 
   * imager. */
  virtual void RequestDetachFromImager( );

  /** Request to get the internal frame as an event */
  virtual void RequestGetFrame();

  /** Access the unique identifier to the imager tool */
  const std::string GetImagerToolIdentifier( ) const;

  /** The "RequestAttachToImager" method attaches the imager tool to a
   * imager. */
  virtual void RequestAttachToImager( ImagerType * );

  /** Get the frame for this tool. */
  FrameType GetInternalFrame( void );

  /** Set the frame for this tool. */
  void SetInternalFrame( const FrameType & );

  void SetFrameDimensions( unsigned int * );
  void GetFrameDimensions( unsigned int * );

  igstkSetMacro( PixelDepth, unsigned int );
  igstkGetMacro( PixelDepth, unsigned int );

protected:

  ImagerTool(void);

  virtual ~ImagerTool(void);

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Set a unique identifier to the imager tool */
  void SetImagerToolIdentifier( const std::string identifier );


private:

  ImagerTool(const Self&);       //purposely not implemented
  void operator=(const Self&);    //purposely not implemented

  /** Push ImagingStarted input to the imager tool */
  virtual void RequestReportImagingStarted( );

  /** Push ImagingStopped input to the imager tool  */
  virtual void RequestReportImagingStopped( );

  /** Push ImagerToolNotAvailable input to the imager tool */
  virtual void RequestReportImagingToolNotAvailable( );

  /** Push ImagerToolVisible input to the imager tool */
  virtual void RequestReportImagingToolVisible( );

  /** Push AttachmentToImagerSuccess input to the imager tool*/ 
  void RequestReportSuccessfulImagerToolAttachment();

  /** Push AttachmentToImagerFailure input to the imager tool*/ 
  void RequestReportFailedImagerToolAttachment();

  /** Set whether the tool was updated during imager UpdateStatus() */
  igstkSetMacro( Updated, bool );

  /** Check if the imager tool is configured or not. This method should
   *  be implemented in the derived classes. */
  virtual bool CheckIfImagerToolIsConfigured( ) const = 0;

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ConfigureTool );
  igstkDeclareInputMacro( ToolConfigurationSuccess );
  igstkDeclareInputMacro( ToolConfigurationFailure );
  igstkDeclareInputMacro( AttachToolToImager );
  igstkDeclareInputMacro( ImagingStarted );
  igstkDeclareInputMacro( ImagingStopped );
  igstkDeclareInputMacro( ImagerToolNotAvailable );
  igstkDeclareInputMacro( ImagerToolVisible );
  igstkDeclareInputMacro( DetachImagerToolFromImager ); 
  igstkDeclareInputMacro( AttachmentToImagerSuccess );
  igstkDeclareInputMacro( AttachmentToImagerFailure );
  igstkDeclareInputMacro( DetachmentFromImagerSuccess );
  igstkDeclareInputMacro( DetachmentFromImagerFailure );
  igstkDeclareInputMacro( GetFrame );
 
  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingToConfigureImagerTool );
  igstkDeclareStateMacro( Configured );
  igstkDeclareStateMacro( AttemptingToAttachImagerToolToImager );
  igstkDeclareStateMacro( Attached );
  igstkDeclareStateMacro( AttemptingToDetachImagerToolFromImager );
  igstkDeclareStateMacro( NotAvailable );
  igstkDeclareStateMacro( Tracked );

  /** Attempt method to configure */
  void AttemptToConfigureProcessing( void );

  /** Attempt method to attach imager tool to the imager */
  void AttemptToAttachImagerToolToImagerProcessing( void );

  /** Post-processing after a successful imager tool configuration */
  void ImagerToolConfigurationSuccessProcessing( void );

  /** Post-processing after a failed imager tool configuration */
  void ImagerToolConfigurationFailureProcessing( void );

  /** Post-processing after a successful imager tool to imager 
      attachment attempt . */ 
  void ImagerToolAttachmentToImagerSuccessProcessing( void );

  /** Post-processing after a failed attachment attempt . */ 
  void ImagerToolAttachmentToImagerFailureProcessing( void );

  /** Attempt method to detach imager tool from the imager */
  void AttemptToDetachImagerToolFromImagerProcessing( void );

  /** Post-processing after a successful detachment of the imager tool
   *  from the imager. */ 
  void ImagerToolDetachmentFromImagerSuccessProcessing( void );

  /** Post-processing after a failed detachment attempt. */ 
  void ImagerToolDetachmentFromImagerFailureProcessing( void );

  /** Retrieve frame as an event. */
  void GetFrameProcessing( void );

  /** Report imager tool is in visible state. */ 
  void ReportImagerToolVisibleStateProcessing( void );

  /** Report imager tool not available state. */ 
  void ReportImagerToolNotAvailableProcessing( void );

  /** Report imaging started */ 
  void ReportImagingStartedProcessing( void );

  /** Report imaging stopped */ 
  void ReportImagingStoppedProcessing( void );

  /** Report invalid request */ 
  void ReportInvalidRequestProcessing( void );

  /** Report invalid request to attach the imager tool. */ 
  void ReportInvalidRequestToAttachImagerToolProcessing( void );

  /** Report invalid request to detach the imager tool. */ 
  void ReportInvalidRequestToDetachImagerToolProcessing( void );

  /** No operation for state machine transition */ 
  void NoProcessing( void );

  /** Calibration transform for the tool */
 // TransformType                 m_CalibrationTransform; 

  /** Calibrated raw transform for the tool */
  //TransformType                 m_CalibratedTransform; 

  /** raw transform for the tool */
  //TransformType                 m_RawTransform; 

  /** raw frame for the tool */
  FrameType                       m_Frame; 

  unsigned int                  m_FrameDimensions[3];

  unsigned int                  m_PixelDepth;

  /** Updated flag */
  bool               m_Updated;

  /** Unique identifier of the imager tool */
  std::string        m_ImagerToolIdentifier;

  /** Imager to which the tool will be attached to */
  Imager        * m_ImagerToAttachTo;

  /** Define the coordinate system interface 
   */
  igstkCoordinateSystemClassInterfaceMacro();

  /** Helper coordinate system used point of reference 
   * for the calibration Transform. */
  CoordinateSystem::Pointer   m_CalibrationCoordinateSystem;
};

std::ostream& operator<<(std::ostream& os, const ImagerTool& o);

}

#endif //__igstk_ImagerTool_h_
