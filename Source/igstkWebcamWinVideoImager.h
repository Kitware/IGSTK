/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkWebcamWinVideoImager.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkWebcamWinVideoImager_h
#define __igstkWebcamWinVideoImager_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif
#include "igstkConfigure.h"

#include "igstkVideoImager.h"
#include "igstkWebcamWinVideoImagerTool.h"

#include <sys/types.h>
#include <stdio.h>
#include <map>
#include <signal.h>

#include "cv.h"
#include "highgui.h"

namespace igstk {

/** \class WebcamWinVideoImager
 * \brief This derivation of the VideoImager class provides communication
 * to a Webcam in Windows
 *
 * \ingroup VideoImager
 */

class WebcamWinVideoImager : public VideoImager
{
public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( WebcamWinVideoImager, VideoImager )

public:

  /** Get the number of tools that have been detected. */
  igstkGetMacro( NumberOfTools, unsigned int );

protected:

  WebcamWinVideoImager(void);

  virtual ~WebcamWinVideoImager(void);

  /** Typedef for internal boolean return type. */
  typedef VideoImager::ResultType   ResultType;

  /** Open communication with the imaging device. */
  virtual ResultType InternalOpen( void );

  /** Close communication with the imaging device. */
  virtual ResultType InternalClose( void );

  /** Put the imaging device into imaging mode. */
  virtual ResultType InternalStartImaging( void );

  /** Take the imaging device out of imaging mode. */
  virtual ResultType InternalStopImaging( void );

  /** Update the status and the transforms for all VideoImagerTools. */
  virtual ResultType InternalUpdateStatus( void );

  /** Update the status and the frames.
      This function is called by a separate thread. */
  virtual ResultType InternalThreadedUpdateStatus( void );

  /** Reset the imaging device to put it back to its original state. */
  virtual ResultType InternalReset( void );

  /** Verify imager tool information */
  virtual ResultType VerifyVideoImagerToolInformation( 
                                                  const VideoImagerToolType * );

  /** The "ValidateSpecifiedFrequency" method checks if the specified
   * frequency is valid for the imaging device that is being used. */
  virtual ResultType ValidateSpecifiedFrequency( double frequencyInHz );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Remove imager tool entry from internal containers */
  virtual ResultType RemoveVideoImagerToolFromInternalDataContainers( const
                                     VideoImagerToolType * imagerTool );

  /** Add imager tool entry to internal containers */
  virtual ResultType AddVideoImagerToolToInternalDataContainers( const
                                     VideoImagerToolType * imagerTool );

private:

  WebcamWinVideoImager(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

  /** Initialize camera */
  bool Initialize();

  /** A mutex for multithreaded access to the buffer arrays */
  itk::MutexLock::Pointer  m_BufferLock;

  /** Total number of tools detected. */
  unsigned int   m_NumberOfTools;

  /** A buffer to hold frames */
  typedef std::map< std::string, igstk::Frame* >
                                VideoImagerToolFrameContainerType;

  typedef igstk::Frame   FrameType;
  VideoImagerToolFrameContainerType           m_ToolFrameBuffer;

  /** Container holding status of the tools */
  std::map< std::string, int >  m_ToolStatusContainer;

  /** Members and functions for communication with the webcam */

public:
  /** A mutex for multithreaded access to frameBuffer */
  static itk::MutexLock::Pointer  m_FrameBufferLock;

#ifdef IGSTK_USE_OpenCV
  CvCapture *m_Capture;
  IplImage  *m_Cvframe;
#endif
};

}  // namespace igstk

#endif //__igstk_WebcamWinVideoImager_h_
