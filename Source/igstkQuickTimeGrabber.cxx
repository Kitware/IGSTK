#if defined(__APPLE_CC__)   // First version of this class only on Mac OSX for now

#include "igstkQuickTimeGrabber.h"

#include <iostream>
#include <fstream>

namespace igstk
{

/** Constructor: Initializes all internal variables. */
QuickTimeGrabber::QuickTimeGrabber(void):m_StateMachine(this)
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber constructor called ...\n");
}

/** Destructor */
QuickTimeGrabber::~QuickTimeGrabber(void)
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber destructor called ...\n");
}

QuickTimeGrabber::ResultType 
QuickTimeGrabber::InternalSetVideoBufferSizeProcessing( void )
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber::InternalSetVideoBufferSizeProcessing called ...\n");

  ResultType result = SUCCESS;
  return result;
}


QuickTimeGrabber::ResultType 
QuickTimeGrabber::InternalSetWantedFramerateProcessing( void )
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber::InternalSetWantedFramerateProcessing called ...\n");

  ResultType result = SUCCESS;
  return result;
}

  /** Internal fuction to initialize grabber.
    * Abstract function to make sure it is implemented in a decendant class. */

QuickTimeGrabber::ResultType QuickTimeGrabber::InternalInitializeProcessing( void )
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber::InternalInitializeProcessing called ...\n");

  ResultType result = SUCCESS;
  return result;
}

  /** Internal fuction to activate grabber.
    * Abstract function to make sure it is implemented in a decendant class. */

QuickTimeGrabber::ResultType QuickTimeGrabber::InternalActivateGrabberProcessing( void ) 
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber::InternalActivateGrabberProcessing called ...\n");

  ResultType result = SUCCESS;
  return result;
}
  
  /** Internal fuction to deactivate grabber.
    * Abstract function to make sure it is implemented in a decendant class. */

QuickTimeGrabber::ResultType QuickTimeGrabber::InternalDeactivateGrabberProcessing( void )
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber::InternalDeactivateGrabberProcessing called ...\n");

  ResultType result = SUCCESS;
  return result;
}
  
  /** Internal fuction to grab one framer.
    * Abstract function to make sure it is implemented in a decendant class. */

QuickTimeGrabber::ResultType QuickTimeGrabber::InternalGrabOneFrameProcessing( void )
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber::InternalGrabOneFrameProcessing called ...\n");

  ResultType result = SUCCESS;
  return result;
}

  
  /** Internal fuction to start grabbing.
    * Abstract function to make sure it is implemented in a decendant class. */

QuickTimeGrabber::ResultType QuickTimeGrabber::InternalStartGrabbingProcessing( void )
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber::InternalStartGrabbingProcessing called ...\n");

  ResultType result = SUCCESS;
  return result;
}

  
  /** Internal fuction to stop grabbing.
    * Abstract function to make sure it is implemented in a decendant class. */

QuickTimeGrabber::ResultType QuickTimeGrabber::InternalStopGrabbingProcessing( void )
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber::InternalStopGrabbingProcessing called ...\n");

  ResultType result = SUCCESS;
  return result;
}


} // namespace

#endif // __APPLE_CC__
