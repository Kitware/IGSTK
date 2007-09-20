/** \class QuickTimeGrabber
  * \brief Provides support for analog video grabbing using QuickTime.
  *
  *
  * \ingroup VideoGrabber
  *
  */

#if defined(__APPLE_CC__) // First version of this class only on Mac OSX for now

#ifndef __igstkQuickTimeGrabber_h
#define __igstkQuickTimeGrabber_h

#include "igstkVideoGrabber.h"

namespace igstk
{

class QuickTimeGrabber : public VideoGrabber
{
public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( QuickTimeGrabber, VideoGrabber )

protected:
  QuickTimeGrabber(void);
  
  ~QuickTimeGrabber(void);
  
  /** typedef for internal boolean return type */
  typedef VideoGrabber::ResultType  ResultType;
  
  /** Internal fuction to set and test video buffer size */
  ResultType InternalSetVideoBufferSizeProcessing( void );
  
  /** Internal fuction to set and test video framerate */
  ResultType InternalSetWantedFramerateProcessing( void );
  
  /** Internal fuction to initialize the grabber */
  ResultType InternalInitializeProcessing( void );

  /** Internal fuction to activate grabber */
  ResultType InternalActivateGrabberProcessing( void );

  /** Internal fuction to deactivate grabber */
  ResultType InternalDeactivateGrabberProcessing( void );
  
  /** Internal fuction to grab one framer */
  ResultType InternalGrabOneFrameProcessing( void );

  /** Internal fuction to start grabbing */
  ResultType InternalStartGrabbingProcessing( void );

  /** Internal fuction to stop grabbing */
  ResultType InternalStopGrabbingProcessing( void );



}; // class def
} // namespace igstk

#endif //__igstkQuickTimeGrabber_h

#endif // __APPLE_CC__
