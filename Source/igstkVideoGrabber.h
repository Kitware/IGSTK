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
#include "igstkEvents.h"

namespace igstk
{

/** \class VideoGrabber
 *  \brief Generic implementation of the VideoGrabbeer class
 *
 */
class VideoGrabber : public Object
{
public:

  /** Container for video dimensions */
  struct VideoDimensions
    {
    unsigned int m_Height;
    unsigned int m_Width;
    };
  
  /** Container for video clipping rectangle */
  struct VideoClipRectangle
    {
    /** Height and width of rectangle */
    unsigned int m_Height;
    unsigned int m_Width;
    
    /** Starting X and Y position of rectangle */
    signed int   m_XPosition;
    signed int   m_YPosition;
    };
  
  /** Macro with standard traits declarations. */
  //igstkStandardClassTraitsMacro( VideoGrabber, Object )
  igstkStandardAbstractClassTraitsMacro( VideoGrabber, Object )
  
  /** Public request methods */
  
  /** Set methods */
    
  /** Set seize of the VideoGrabber videobuffer
    * \param bufferSize Size of video buffer */
  virtual void RequestSetVideoBufferSize( unsigned long bufferSize );
  
  /** Set framerate.
   * \param framerate Desired framerate */
  virtual void RequestSetWantedFramerate(unsigned int framerate);
  
  /** Set the video output format. The possible formats are dependant on video 
   * grabber hardware.
   * \param format The format to use */
  virtual void RequestSetVideoOutputFormat( unsigned int format );
  
  /** Set desired video output dimensions.
    * \param height Desired height of video output
    * \param width Desired width of video output */
  virtual void RequestSetVideoOutputDimensions(  unsigned int videoHeight, 
                                                 unsigned int videoWidth  );
  
  /** Set rectangle for clipping the video output to desired size.
   * \param height Height of rectangle
   * \param width Width of rectalgle
   * \param xPosition X position of the upper rightmost rectangle corner
   * \param yPosition Y Position of the upper righmost rectangle corner */
  virtual void RequestSetVideoOutputClipRectangle( unsigned int height, 
                                                   unsigned int width,
                                                   signed int xPosition, 
                                                   signed int yPosition );
  
  /** Apply padding around the video output to get video on a desired format.
    * \param padding The padding value (in pixels) to use*/
  virtual void RequestSetVideoOutputPadding( unsigned int padding );
  
  /** Activate VideoGrabber */
  virtual void RequestOpen();
  
  /** Deactivate VideoGrabber. Stops grabbing if VideoGrabber is currently 
    * grabbing */
  virtual void RequestClose();
  
  /** Initialize VideoGrabber. Verifies that the grabber hardware is 
    * correctly initialized. */
  virtual void RequestInitialize();
  
  /** Start grabbing to buffer */
  virtual void RequestStartGrabbing();
  
  /** Stop grabbing */
  virtual void RequestStopGrabbing();
  
  /** Grab one video frame. */
  virtual void RequestGrabOneFrame();
  
  /** Get methods */
  void RequestGetVideoBufferSize();
  void RequestGetVideoFrameNo();
  void RequestGetNumberOfVideoFrames();
  void RequestGetTimeTag();
  void RequestGetFramerate();
  void RequestGetVideoOutputFormat();
  void RequestGetVideoOutputDimensions();
  void RequestGetVideoOutputClipRectangle();
  void RequestGetVideoOutputPadding();
  
  /** Event types */
  igstkLoadedEventMacro( SignedIntEvent, IGSTKEvent, 
                         EventHelperType::SignedIntType );
  igstkLoadedEventMacro( UnsignedLongEvent, IGSTKEvent, 
                         unsigned long );
  igstkLoadedEventMacro( VideoDimensionsEvent, IGSTKEvent, 
                         VideoDimensions );
  igstkLoadedEventMacro( VideoClipRectangleEvent, IGSTKEvent, 
                         VideoClipRectangle );

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
  
  /** Internal functions to report from RequestGet methods. 
    * These methods should be overridden by a decentand class */
  /*virtual void InternalRequestVideoBufferSizeProcessing();
  virtual void InternalRequestVideoFrameNoProcessing();
  virtual void InternalRequestNumberOfVideoFramesProcessing();
  virtual void InternalRequestTimeTagProcessing();
  virtual void InternalRequestFramerateProcessing();
  virtual void InternalRequestVideoOutputFormatProcessing();
  virtual void InternalRequestVideoOutputDimensionsProcessing();
  virtual void InternalRequestVideoOutputClipRectangleProcessing();
  virtual void InternalRequestVideoOutputPaddingProcessing();*/
  
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
  
  igstkDeclareInputMacro( RequestVideoBufferSize );
  igstkDeclareInputMacro( RequestVideoFrameNo );
  igstkDeclareInputMacro( RequestNumberOfVideoFrames );
  igstkDeclareInputMacro( RequestTimeTag );
  igstkDeclareInputMacro( RequestFramerate );
  igstkDeclareInputMacro( RequestVideoOutputFormat );
  igstkDeclareInputMacro( RequestVideoOutputDimensions );
  igstkDeclareInputMacro( RequestVideoOutputClipRectangle );
  igstkDeclareInputMacro( RequestVideoOutputPadding );
  
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
  
  void ReportVideoBufferSizeProcessing();
  void ReportVideoFrameNoProcessing();
  void ReportNumberOfVideoFramesProcessing();
  void ReportTimeTagProcessing();
  void ReportFramerateProcessing();
  void ReportVideoOutputFormatProcessing();
  void ReportVideoOutputDimensionsProcessing();
  void ReportVideoOutputClipRectangleProcessing();
  void ReportVideoOutputPaddingProcessing();
  
  void InitializeProcessing();
  void ActivateGrabberProcessing();
  void DeactivateGrabberProcessing();
  void GrabOneFrameProcessing();
  void StartGrabbingProcessing();
  void StopGrabbingProcessing();
  
  /** Temporary input variables for state machine */
  unsigned long       m_VideoBufferSizeToBeSet;
  unsigned int        m_FramerateToBeSet;
  unsigned int        m_VideoOutputFormatToBeSet;
  VideoDimensions     m_VideoOutputDimensionsToBeSet;
  VideoClipRectangle  m_VideoOutputClipRectangleToBeSet;
  unsigned int        m_VideoOutputPaddingToBeSet;
  
  /** Variable to save the framrate */
  unsigned long       m_VideoBufferSize;
  unsigned int        m_Framerate;
  unsigned int        m_VideoOutputFormat;
  VideoDimensions     m_VideoOutputDimensions;
  VideoClipRectangle  m_VideoOutputClipRectangle;
  unsigned int        m_VideoOutputPadding;
};

} // end namespace igstk

#endif //__igstkVideoGrabber_h
