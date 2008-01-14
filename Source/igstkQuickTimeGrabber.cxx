/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkQuickTimeGrabber.cxx
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

#include "igstkQuickTimeGrabber.h"

#include <iostream>
#include <fstream>

namespace igstk
{

/** Constructor: Initializes all internal variables. */
QuickTimeGrabber::QuickTimeGrabber(void):m_StateMachine(this)
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber constructor called ...\n");
  
  // allocate memory for the video data
  videoDataPtr = (VideoDataRecord*)NewPtrClear(sizeof(VideoDataRecord));
  videoDataPtr->videoObject = this;
  videoDataPtr->timeTag = 0;
  videoDataPtr->timeScale = 0;
  videoDataPtr->timeScaleMod = 0;
  videoDataPtr->grabber = this;
  
  videoMemoryPtr = NULL;
}

/** Destructor */
QuickTimeGrabber::~QuickTimeGrabber(void)
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber destructor called ...\n");
  
  if (videoDataPtr->seqGrab) {
    if (m_isGrabbing) SGStop(videoDataPtr->seqGrab);
    CloseComponent(videoDataPtr->seqGrab);
    if (videoDataPtr->drawSeq) CDSequenceEnd(videoDataPtr->drawSeq);
  }
  //UnlockPixels(GetGWorldPixMap(videoDataPtr->pGWorld));
  DisposeGWorld(videoDataPtr->pGWorld);
  DisposePtr((char*)videoDataPtr);  
}

QuickTimeGrabber::ResultType 
QuickTimeGrabber::InternalSetVideoBufferSizeProcessing( void )
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber"
                        "::InternalSetVideoBufferSizeProcessing called ...\n");
  
  m_VideoBufferSize = m_VideoBufferSizeToBeSet;
  ResultType result = SUCCESS;
  return result;
}


QuickTimeGrabber::ResultType 
QuickTimeGrabber::InternalSetWantedFramerateProcessing( void )
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber"
                        "::InternalSetWantedFramerateProcessing called ...\n");
  
  m_Framerate = m_FramerateToBeSet;
  ResultType result = SUCCESS;
  return result;
}


void QuickTimeGrabber::SetVideoMemoryPtr(void* pixels)
{
  if (pixels != NULL)
  {
    videoMemoryPtr = pixels;
  } else {
    
    igstkLogMacro( CRITICAL, "QuickTimeGrabber::SetVideoMemoryPtr failed!\n");
    VideoGrabberErrorEvent event;
    std::string s = "QuickTimeGrabber::SetVideoMemoryPtr failed!";
    event.Set(s);
    this->InvokeEvent(event);
  }
}

  /** Internal fuction to initialize grabber.
    * Abstract function to make sure it is implemented in a decendant class. */

QuickTimeGrabber::ResultType QuickTimeGrabber
                             ::InternalInitializeProcessing( void )
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber"
                        "::InternalInitializeProcessing called ...\n");
                               
  ResultType result = SUCCESS;    
   
  
  try
  {
    OSErr err = noErr;

    // Test if all necessary variables have been set 
    if (videoMemoryPtr == NULL)
      throw "Video memory not assigned!";    
      
    // Deprecated code - The only code that works on both Mac and PC
    
    // Initialize platform specific variables
    Rect videoRect;
    videoRect.top = 0;
    videoRect.left = 0;
    videoRect.bottom = m_VideoOutputDimensions.m_Height - 1;
    videoRect.right = m_VideoOutputDimensions.m_Width - 1;
    
    m_isGrabbing      = false;
    m_callbackRunning = false;
    
    //GWorldPtr m_GWorldPtr;
    //SGChannel m_Channel;
    //SeqGrabComponent  m_SeqGrab;
    
    // Moved to igstkVideoObjectRepresentation m_ImageData 
    //videoGrabberImage = vtkImageData::New();
    //videoGrabberImage->SetScalarTypeToUnsignedChar();
    //videoGrabberImage->SetNumberOfScalarComponents(m_VideoOutputFormat / 8);
    //videoGrabberImage->SetExtent(videoRect.left, videoRect.right,
    //                             videoRect.top, videoRect.bottom, 0, 0);
    //videoGrabberImage->AllocateScalars();
        
    videoDataPtr->pixelDepth = m_VideoOutputFormat;
    
    videoDataPtr->theClut = GetCTable(40); // Defining the color lookup table
    reverseCTable(videoDataPtr->theClut);
    err = QTNewGWorldFromPtr(&(videoDataPtr->pGWorld),  // returned GWorld
                             8,//m_VideoOutputFormat    // pixel format
                             &videoRect,                // bounds
                             videoDataPtr->theClut,     // color table
                             NULL,                      // GDHandle
                             0,                         // flags
                             videoMemoryPtr,            //buffer
                             m_VideoOutputDimensions.m_Width * 
                             m_VideoOutputFormat / 8);  //row bytes
    
    
    if(err!=noErr) 
      throw "QTNewGWorld failed!";
    
    if (videoDataPtr->pGWorld == NULL) 
      throw "QTNewGWorld did not initialize GWorld!";
    
    // Always fails ???
    //err = LockPixels(GetGWorldPixMap(videoDataPtr->pGWorld));
    //if(err!=noErr) 
    //    throw "LockPixels failed!";
    
    videoDataPtr->bounds = videoRect;
    videoDataPtr->seqGrab = OpenDefaultComponent(SeqGrabComponentType, 0);
    
    if (videoDataPtr->seqGrab == NULL) 
      throw "Sequence grabber component not initialized!";
    
    // Initialize sequence grabber component
    if (noErr != SGInitialize(videoDataPtr->seqGrab))
      throw "SGInitialize failed!";
        
    if (noErr != SGNewChannel(videoDataPtr->seqGrab, VideoMediaType, 
                              &(videoDataPtr->sgchanVideo)))
      throw "SGNewChannel failed";
    
    if (noErr != SGSetChannelBounds(videoDataPtr->sgchanVideo, &videoRect))
      throw "SGNewChannel failed";
    
    if (noErr != SGSetChannelUsage(videoDataPtr->sgchanVideo, seqGrabRecord))
    {
      // clean up on failure
      SGDisposeChannel(videoDataPtr->seqGrab, videoDataPtr->sgchanVideo);
      throw "SGSetChannelUsage failed";
    }
        
    if (noErr != 
        SGSetGWorld(videoDataPtr->seqGrab, videoDataPtr->pGWorld, NULL))
      throw "SGSetGWorld failed";
    
    
    // Grabber control panel - Not working correcly???
    //if (noErr != SGSettingsDialog (videoDataPtr->seqGrab, 
    //                               videoDataPtr->sgchanVideo, 
    //                               0, NULL, 0, NULL, 0))
    //  throw "SGSettingsDialog failed";
    
    // Configure Hardware
    VideoDigitizerComponent vd = 
      SGGetVideoDigitizerComponent(videoDataPtr->sgchanVideo);
    VDSetInputStandard(vd,palIn); 
    VDSetInput(vd,2);

    // Don't work???
    //CodecNameSpecListPtr *list = NULL;
    //GetCodecNameList(list, 1);
    //if (noErr != GetCodecNameList(list, 1))
    //  throw "GetCodecNameList failed";
    
    
    // Set paramaters without SGSettingsDialog()
    if (noErr != SGSetVideoCompressorType(videoDataPtr->sgchanVideo, 'raw '))
      throw "SGSetVideoCompressorType failed";
    
    if (noErr != SGSetVideoCompressor(videoDataPtr->sgchanVideo, 8, 0, 
                                      codecMaxQuality, 0, 0))
      throw "SGSetVideoCompressor failed";
    
    
    // specify a data function
    if (noErr != SGSetDataProc(videoDataPtr->seqGrab, 
                               NewSGDataUPP((this->videoGrabDataProc)),
                               (long)videoDataPtr))
      throw "SGSetDataProc failed";
    
    // Prepare QT sequence grabber for record operation
    if (noErr != SGPrepare(videoDataPtr->seqGrab, false, true))
      throw "SGPrepare failed";
    
  }
  catch (char const *str) {
    igstkLogMacro( CRITICAL, "QuickTimeGrabber::InternalInitializeProcessing: "
                   << str << "\n");
    result = FAILURE;
    VideoGrabberErrorEvent event;
    std::string s = "QuickTimeGrabber::InternalInitializeProcessing: ";
    s.append(str);
    event.Set(s);
    this->InvokeEvent(event);
  }
  
  return result;
}

  /** Internal fuction to activate grabber. */

QuickTimeGrabber::ResultType QuickTimeGrabber
                             ::InternalActivateGrabberProcessing( void ) 
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber"
                 "::InternalActivateGrabberProcessing called ...\n");

  ResultType result = SUCCESS;
  return result;
}
  
  /** Internal fuction to deactivate grabber. */

QuickTimeGrabber::ResultType QuickTimeGrabber
                             ::InternalDeactivateGrabberProcessing( void )
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber"
                 "::InternalDeactivateGrabberProcessing called ...\n");
                            
  m_callbackRunning = false;
  
  ResultType result = SUCCESS;
  return result;
}

/** Give processing power to the video sequence grabber. */

QuickTimeGrabber::ResultType QuickTimeGrabber::ProcessVideo( void )
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber"
                 "::ProcessVideo called ...\n");
  
  ResultType result = SUCCESS;
  
  OSErr err = noErr;
  err = SGIdle(videoDataPtr->seqGrab);
  
  //if ( noErr != SGIdle(videoDataPtr->seqGrab))
  if (noErr != err)
  {
    // Always fails???
    //igstkLogMacro( WARNING, "QuickTimeGrabber"
    //               "::ProcessVideo: "
    //               "SGIdle failed! \n");
    result = FAILURE;
  }            
  return result;
}


  /** Internal fuction to grab one frame. Not finished */

QuickTimeGrabber::ResultType QuickTimeGrabber
                             ::InternalGrabOneFrameProcessing( void )
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber"
                        "::InternalGrabOneFrameProcessing called ...\n");
                               
  ResultType result = SUCCESS;
  
  
  // Give processing power to the video sequence grabber
  /*result = */ProcessVideo();
  
 /*********************
  * To be implemented *
  *********************/
                               
  return result;
}

  
  /** Internal fuction to start grabbing. */

QuickTimeGrabber::ResultType QuickTimeGrabber
                             ::InternalStartGrabbingProcessing( void )
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber"
                        "::InternalStartGrabbingProcessing called ...\n");
                               
  ResultType result = WAIT;
            
  if ( noErr != SGStartRecord(videoDataPtr->seqGrab) )
  {
    igstkLogMacro( CRITICAL, "QuickTimeGrabber"
                   "::InternalStartGrabbingProcessing: "
                   "Failed to start grabbing! \n");
    result = FAILURE;
  }
  
  if (result == WAIT)
    m_isGrabbing = true;
  
  // If callback function already up and running, go to GrabbingState
  if (m_callbackRunning)
    result = SUCCESS;
  
  //this->GrabbingTransition();
 
  return result;
}

  
  /** Internal fuction to stop grabbing. */

QuickTimeGrabber::ResultType QuickTimeGrabber
                             ::InternalStopGrabbingProcessing( void )
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber"
                        "::InternalStopGrabbingProcessing called ...\n");

  ResultType result = SUCCESS;
  
  if ( noErr != SGStop(videoDataPtr->seqGrab) )
  {
    igstkLogMacro( CRITICAL, "QuickTimeGrabber"
                   "::InternalStopGrabbingProcessing: "
                   "Failed to stop grabbing! \n");
    result = FAILURE;
  }
  
  if (result == SUCCESS)
    m_isGrabbing = false;
  
  return result;
}

/** Internal fuction to initiate transition to GrabbingState. */

QuickTimeGrabber::ResultType QuickTimeGrabber
                             ::InternalGrabbingTransition( void )
{
  igstkLogMacro( DEBUG, "QuickTimeGrabber"
                 "::InternalGrabbingTransition called ...\n");
  
  m_callbackRunning = true;
  
  ResultType result = SUCCESS;
  
  return result;
}

/****************************************************************/
/* Video::videoGrabDataProc                                     */
/* Specifies a data function for use by the sequence grabber    */
/* whenever any channel assigned to the sequence grabber        */
/* writes data, this data function is called and may then       */
/* write the data to another destination.                       */
/****************************************************************/
pascal OSErr QuickTimeGrabber
             ::videoGrabDataProc(SGChannel c, Ptr p, long len, long *offset, 
                                 long chRefCon, TimeValue time, 
                                 short writeType, long refCon) {
                 
  CodecFlags      ignore;
  OSErr           err = noErr;
  VideoDataRecord *videoPtr;
  
  videoPtr=(VideoDataRecord*)refCon;
  
  if (videoPtr->timeScale == 0) {
    // first time here so set the time scale
    err = SGGetChannelTimeScale(c, &videoPtr->timeScale);
    videoPtr->timeScaleMod = (float)videoPtr->timeScale/1000;
    
    // Transition to GrabbingState
    videoPtr->grabber->GrabbingTransition();
  }
  
  if(videoPtr->pixelDepth==88) {
    memcpy( (GetPixBaseAddr(GetGWorldPixMap(videoPtr->pGWorld))),
            p, videoPtr->bounds.right*videoPtr->bounds.bottom); 
  } else {
    if (videoPtr->pGWorld) {
      if (videoPtr->decomSeq == 0) {
        // Set up getting grabbed data into the GWorld
        ImageDescriptionHandle imageDesc = (ImageDescriptionHandle)NewHandle(0);
        // retrieve a channel’s current sample description, 
        // the channel returns a sample description that is
        // appropriate to the type of data being captured
        err = SGGetChannelSampleDescription(c, (Handle)imageDesc);
        
        Rect dvsize;
        dvsize.top = 0;
        dvsize.left = 0;
        dvsize.bottom = (*imageDesc)->height - 1;
        dvsize.right = (*imageDesc)->width - 1;
        
        //Correct the video proportions
        videoPtr->bounds.bottom = (int)round((videoPtr->bounds.right+1) * 
                                             (*imageDesc)->height / 
                                             (float)(*imageDesc)->width);
        
        MatrixRecord scaleMatrix;
        RectMatrix(&scaleMatrix, &dvsize, &videoPtr->bounds);
        
        // begin the process of decompressing a sequence of frames
        // this is a set-up call and is only called once for the sequence 
        // - the ICM will interrogate different codecs
        // and construct a suitable decompression chain, 
        // as this is a time consuming process we don't want to do this
        // once per frame (eg. by using DecompressImage)
        // for more information see Ice Floe #8 
        // http://developer.apple.com/quicktime/icefloe/dispatch008.html
        // the destination is specified as the GWorld
        err = DecompressSequenceBeginS(// receive unique ID for sequence
                                       &videoPtr->decomSeq,
                                       // handle to image description structure
                                       imageDesc,
                                       p,
                                       len,
                                       // port for the DESTINATION image
                                       videoPtr->pGWorld,
                                       // graphics device handle, if port 
                                       // is set, set to NULL
                                       NULL,
                                       // source rectangle defining the portion 
                                       // of the image to decompress 
                                       NULL,
                                       // transformation matrix
                                       &scaleMatrix,
                                       // transfer mode specifier
                                       srcCopy,
                                       // clipping region in dest. 
                                       // coordinate system to use as a mask
                                       (RgnHandle)NULL,
                                       // flags
                                       NULL,
                                       // accuracy in decompression
                                       codecMinQuality,
                                       // compressor identifier or special 
                                       // identifiers ie. bestSpeedCodec
                                       bestSpeedCodec);

        DisposeHandle((Handle)imageDesc);
      }
      
      
      // decompress a frame into the GWorld - can queue a frame for async 
      // decompression when passed in a completion proc
      err = DecompressSequenceFrameS(// sequence ID returned by 
                                     //DecompressSequenceBegin
                                     videoPtr->decomSeq,
                                     // pointer to compressed image data
                                     p,
                                     len,       // size of the buffer
                                     0,         // in flags
                                     &ignore,   // out flags
                                     NULL);     // async completion proc

      if (err) {
        printf("Fault in decompress sequence %d\n",err);
        err = noErr;
        
      }
    }
  }

  videoPtr->timeTag = time;
  return err;
}

/****************************************************************/
/* QuickTimeGrabber::reverseCTable                              */
/* Function for inverting the colortable.                       */
/* This is needed  by grayscale grabbing, since QuickTime       */
/* output is white for scalarvalue=0 and black for              */
/* scalarvalue=255.                                             */
/****************************************************************/
void QuickTimeGrabber::reverseCTable(CTabHandle& cTable)
{
  int oldValueIndex = 0;
  int newValueIndex = (*cTable)->ctSize - oldValueIndex;
  
  while (oldValueIndex < newValueIndex) {
    
    ColorSpec csOld = (*cTable)->ctTable[oldValueIndex];
    (*cTable)->ctTable[oldValueIndex].rgb = 
      (*cTable)->ctTable[newValueIndex].rgb;
    (*cTable)->ctTable[newValueIndex].rgb = csOld.rgb;
    oldValueIndex++;
    newValueIndex = (*cTable)->ctSize - oldValueIndex;
  }
  CTabChanged(cTable);
}


} // namespace

#endif // __APPLE_CC__
