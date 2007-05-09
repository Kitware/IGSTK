/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoGrabber.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkVideoGrabber_h
#define __igstkVideoGrabber_h

#include "igstkObject.h"
#include "igstkStateMachine.h"
#include "igstkMacros.h"

namespace igstk
{

/** \class VideoGrabber
 *  \brief Generic implementation of the VideoGrabbeer class
 *
 */
class VideoGrabber : public Object
{
public:

  /** Macro with standard traits declarations. */
  //igstkStandardClassTraitsMacro( VideoGrabber, Object )
  igstkStandardAbstractClassTraitsMacro( VideoGrabber, Object )
  
  /** Public request methods */
  virtual void RequestSetVideoBufferSize( unsigned long videoBufferSize );
  virtual void RequestSetWantedFramerate(unsigned int framerate);
  virtual void RequestSetVideoOutputFormat( unsigned int format );
  virtual void RequestSetVideoOutputDimensions( unsigned int videoHeight, 
                                                unsigned int videoWidth );
  virtual void RequestSetVideoOutputClipRectangle( unsigned int height, 
                                                   unsigned int width,
                                                   signed int xPosition, 
                                                   signed int yPosition );
  virtual void RequestSetVideoOutputPadding( unsigned int padding );
  
  virtual void RequestOpen();
  virtual void RequestClose();
  virtual void RequestInitialize();
  virtual void RequestStartGrabbing();
  virtual void RequestStopGrabbing();
  virtual void RequestGrabOneFrame();
  
  
  /** Public get methods 
  - Framework functions, finished functions will not return void */
  void GetVideoBufferSize();
  void GetVideoBufferPtr();
  void GetVideoFrameNo();
  void GetNumberOfVideoFrames();
  void GetTimeTag();
  void GetFramerate();
  void GetVideoOutputFormat();
  void GetVideoOutputDimensions();
  void GetVideoOutputClipRectangle();
  void GetVideoOutputPadding();

protected:
  VideoGrabber();
  virtual ~VideoGrabber();

  typedef enum 
    { 
    FAILURE=0, 
    SUCCESS
    } ResultType;
  
  /** Internal fuction to set and test video buffer size. 
    * This method must be overridden by a decendant class 
    * with testing of the input value */
  virtual ResultType InternalSetVideoBufferSizeProcessing( void );
  
  /** Internal fuction to set and test video framerate.
    * This method must be overridden by a decendant class 
    * with testing of the input value */
  virtual ResultType InternalSetWantedFramerateProcessing( void );
  
  /** Internal fuction to set and test video output format.
    * This method must be overridden by a decendant class 
    * with testing of the input value */
  virtual ResultType InternalSetVideoOutputFormatProcessing( void );
  
  /** Internal fuction to set and test video output dimensions.
    * This method must be overridden by a decendant class 
    * with testing of the input value */
  virtual ResultType InternalSetVideoOutputDimensionsProcessing( void );
  
  /** Internal fuction to set and test video output clipping rectangle.
    * This method must be overridden by a decendant class 
    * with testing of the input value */
  virtual ResultType InternalSetVideoOutputClipRectangleProcessing( void );
  
  /** Internal fuction to set and test framerate.
    * This method must be overridden by a decendant class 
    * with testing of the input value */
  virtual ResultType InternalSetVideoOutputPaddingProcessing( void );
  
  /** Internal fuction to initialize grabber.
    * Abstract function to make sure it is implemented in a decendant class. */
  virtual ResultType InternalInitializeProcessing( void ) = 0;
  
  /** Internal fuction to activate grabber.
    * Abstract function to make sure it is implemented in a decendant class. */
  virtual ResultType InternalActivateGrabberProcessing( void ) = 0;
  
  /** Internal fuction to deactivate grabber.
    * Abstract function to make sure it is implemented in a decendant class. */
  virtual ResultType InternalDeactivateGrabberProcessing( void ) = 0;
  
  /** Internal fuction to grab one framer.
    * Abstract function to make sure it is implemented in a decendant class. */
  virtual ResultType InternalGrabOneFrameProcessing( void ) = 0;
  
  /** Internal fuction to start grabbing.
    * Abstract function to make sure it is implemented in a decendant class. */
  virtual ResultType InternalStartGrabbingProcessing( void ) = 0;
  
  /** Internal fuction to stop grabbing.
    * Abstract function to make sure it is implemented in a decendant class. */
  virtual ResultType InternalStopGrabbingProcessing( void ) = 0;
  
private:
    
  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingToSetVideoBufferSize );
  igstkDeclareStateMacro( AttemptingToSetWantedFramerate );
  igstkDeclareStateMacro( AttemptingToSetVideoOutputFormat );
  igstkDeclareStateMacro( AttemptingToSetVideoOutputDimensions );
  igstkDeclareStateMacro( AttemptingToSetVideoOutputClipRectangle );
  igstkDeclareStateMacro( AttemptingToSetVideoOutputPadding );
  igstkDeclareStateMacro( AttemptingToSetTexturePointer );
  
  igstkDeclareStateMacro( AttemptingToInitialize );
  igstkDeclareStateMacro( AttemptingToStartGrabbing );
  igstkDeclareStateMacro( AttemptingToStopGrabbing );
  
  igstkDeclareStateMacro( GrabberReady );
  igstkDeclareStateMacro( AttemptingToActivateGrabber );
  igstkDeclareStateMacro( AttemptingToDeactivateGrabber );
  igstkDeclareStateMacro( GrabberActive );
  igstkDeclareStateMacro( AttemptingToGrabOneFrame );
  igstkDeclareStateMacro( Grabbing );
  
  /** Inputs to the state machine and it's designed transitions */
  igstkDeclareInputMacro( SetVideoBufferSize );
  igstkDeclareInputMacro( SetWantedFramerate );
  igstkDeclareInputMacro( SetVideoOutputFormat );
  igstkDeclareInputMacro( SetVideoOutputDimensions );
  igstkDeclareInputMacro( SetVideoOutputClipRectangle );
  igstkDeclareInputMacro( SetVideoOutputPadding );
  igstkDeclareInputMacro( Open );
  igstkDeclareInputMacro( Close );
  igstkDeclareInputMacro( Initialize );
  igstkDeclareInputMacro( StartGrabbing );
  igstkDeclareInputMacro( StopGrabbing );
  igstkDeclareInputMacro( GrabOneFrame );
  
  igstkDeclareInputMacro( Success );
  igstkDeclareInputMacro( Failure );
  
  
  /** Methods used only by the state machine */
  void NoProcessing();
  void SetVideoBufferSizeProcessing();
  void SetWantedFramerateProcessing();
  void SetVideoOutputFormatProcessing();
  void SetVideoOutputDimensionsProcessing();
  void SetVideoOutputClipRectangleProcessing();
  void SetVideoOutputPaddingProcessing();
  
  void InitializeProcessing();
  void ActivateGrabberProcessing();
  void DeactivateGrabberProcessing();
  void GrabOneFrameProcessing();
  void StartGrabbingProcessing();
  void StopGrabbingProcessing();
  
  /** Temporary input variables for state machine */
  unsigned long m_VideoBufferSizeToBeSet;
  unsigned int  m_FramerateToBeSet;
  unsigned int  m_VideoOutputFormatToBeSet;
  unsigned int  m_VideoOutputHeightToBeSet;
  unsigned int  m_VideoOutputWidthToBeSet;
  unsigned int  m_VideoOutputClipRectangleHeightToBeSet;
  unsigned int  m_VideoOutputClipRectangleWidthToBeSet;
  signed int    m_VideoOutputClipRectangleXPositionToBeSet;
  signed int    m_VideoOutputClipRectangleYPositionToBeSet;
  unsigned int  m_VideoOutputPaddingToBeSet;
  
  /** Variable to save the framrate */
  unsigned long m_VideoBufferSize;
  unsigned int  m_Framerate;
  unsigned int  m_VideoOutputFormat;
  unsigned int  m_VideoOutputHeight;
  unsigned int  m_VideoOutputWidth;
  unsigned int  m_VideoOutputClipRectangleHeight;
  unsigned int  m_VideoOutputClipRectangleWidth;
  signed int    m_VideoOutputClipRectangleXPosition;
  signed int    m_VideoOutputClipRectangleYPosition;
  unsigned int  m_VideoOutputPadding;
};

} // end namespace igstk

#endif //__igstkVideoGrabber_h
