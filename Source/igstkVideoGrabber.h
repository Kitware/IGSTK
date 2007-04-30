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
 *  \brief Generic implementation of the VideoGrabbeer class - Not Finished!
 *
 */
class VideoGrabber : public Object
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( VideoGrabber, Object )
  
  /** Public request methods */
  virtual void RequestSetVideoBufferSize();
  virtual void RequestSetWantedFramerate();
  virtual void RequestSetVideoOutputFormat();
  virtual void RequestSetVideoOutputClipRectangle();
  virtual void RequestSetVideoOutputPadding();
  virtual void RequestSetGrabbingMode();
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
  void GetVideoOutputClipRectangle();
  void GetVideoOutputPadding();
  void GetGrabbingMode();
  
private:

  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingToSetVideoBufferSize );
  igstkDeclareStateMacro( AttemptingToSetWantedFramerate );
  igstkDeclareStateMacro( AttemptingToSetVideoOutputFormat );
  igstkDeclareStateMacro( AttemptingToSetVideoOutputClipRectangle );
  igstkDeclareStateMacro( AttemptingToSetVideoOutputPadding );
  igstkDeclareStateMacro( AttemptingToSetGrabbingMode );
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
  igstkDeclareInputMacro( RequestSetVideoBufferSize );
  igstkDeclareInputMacro( RequestSetWantedFramerate );
  igstkDeclareInputMacro( RequestSetVideoOutputFormat );
  igstkDeclareInputMacro( RequestSetVideoOutputClipRectangle );
  igstkDeclareInputMacro( RequestSetVideoOutputPadding );
  igstkDeclareInputMacro( RequestSetGrabbingMode );
  //igstkDeclareInputMacro( RequestSetTexturePointer );
  igstkDeclareInputMacro( RequestOpen );
  igstkDeclareInputMacro( RequestClose );
  igstkDeclareInputMacro( RequestInitialize );
  igstkDeclareInputMacro( RequestStartGrabbing );
  igstkDeclareInputMacro( RequestStopGrabbing );
  igstkDeclareInputMacro( RequestGrabOneFrame );
  
  igstkDeclareInputMacro( Success );
  igstkDeclareInputMacro( Failure );
  
  
  /** Methods used only by the state machine */
  void NoProcessing();
  void SetVideoBufferSizeProcessing();
  void SetWantedFramerateProcessing();
  void SetVideoOutputFormatProcessing();
  void SetVideoOutputClipRectangleProcessing();
  void SetVideoOutputPaddingProcessing();
  void SetGrabbingModeProcessing();
  
  void InitializeProcessing();
  void ActivateGrabberProcessing();
  void DeactivateGrabberProcessing();
  void GrabOneFrameProcessing();
  void StartGrabbingProcessing();
  void StopGrabbingProcessing();
  
protected:
  VideoGrabber();
  virtual ~VideoGrabber();
};

} // end namespace igstk

#endif //__igstkVideoGrabber_h
