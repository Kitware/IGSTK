/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkQuickTimeGrabber.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

Made by SINTEF Health Research - Medical technology:
http://www.sintef.no/medtech

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// First version of this class only on Mac OSX for now
#if defined(__APPLE_CC__)

#ifndef __igstkQuickTimeGrabber_h
#define __igstkQuickTimeGrabber_h

#include "igstkVideoGrabber.h"
#include <QuickTime/QuickTime.h>
#include <AGL/agl.h>

namespace igstk
{
/** \class QuickTimeGrabber
 * \brief Provides support for analog video grabbing using QuickTime.
 * The code uses deprecatd QuickTime functions in an attempt to make code that 
 * also runs on Windows: 
 * UnlockPixels(), GetGworldPixMap(), DisposeGworld(), GetPixBaseAddr(), 
 * CTabChanged()...
 *
 * \ingroup VideoGrabber
 *
 */
class QuickTimeGrabber : public VideoGrabber
{
public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( QuickTimeGrabber, VideoGrabber )

  /** Function to set the memory area for the video frame. 
    * Not following the IGSTK framework. 
    * May/should be changed later. */
  void SetVideoMemoryPtr(void* pixels);
  
  
  /** Give processing power to the video sequence grabber. */
  ResultType QuickTimeGrabber::ProcessVideo( void );
  
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
  
  /** Internal fuction to initiate transition to GrabbingState. */
  ResultType InternalGrabbingTransition( void );
  
private:
  // Class variables
    
    /********************************/
    /* Video data structure         */
    /********************************/
    
    typedef struct {
      WindowRef               pWindow;  // window
      Rect                    bounds;   // bounds rect
      GWorldPtr               pGWorld;  // offscreen GWorld
      SeqGrabComponent        seqGrab;  // QT sequence grabber
      SGChannel               sgchanVideo;  // QT video channel
      // unique identifier for our decompression sequence
      ImageSequence           decomSeq;
      // unique identifier for our draw sequence
      ImageSequence           drawSeq;
      long                    drawSize;
      void                    *videoObject;
      TimeValue               timeTag;
      TimeScale               timeScale;
      float                   timeScaleMod;
      CTabHandle              theClut;
      int                     pixelDepth;
      igstk::QuickTimeGrabber *grabber;
    } VideoDataRecord;

  VideoDataRecord   *videoDataPtr;
  //vtkImageData    *videoGrabberImage;
  Boolean           m_isGrabbing;
  void              *videoMemoryPtr;
  Boolean           m_callbackRunning;
  
  // Video callback function
  static pascal OSErr videoGrabDataProc(SGChannel c, Ptr p, long len,
                                        long *offset, long chRefCon,
                                        TimeValue time, short writeType,
                                        long refCon);
                                        void reverseCTable(CTabHandle& cTable);
  
}; // class def
} // namespace igstk

#endif //__igstkQuickTimeGrabber_h

#endif // __APPLE_CC__
