/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkOpenIGTLinkVideoImager.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkOpenIGTLinkVideoImager_h
#define __igstkOpenIGTLinkVideoImager_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "igstkVideoImager.h"
#include "igstkOpenIGTLinkVideoImagerTool.h"
#include "igtlServerSocket.h"
#include "igtlImageMessage.h"

#include <map>

class vtkImageData;

namespace igstk {

/** \class OpenIGTLinkVideoImager
 * \brief This imager provides support for socket communication (using the
 * Open IGTLink protocol) to the OpenIGTLink system
 *
 * \ingroup VideoImager
 */

class OpenIGTLinkVideoImager : public VideoImager
{
public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( OpenIGTLinkVideoImager, VideoImager )

  /** Communication type */
  typedef igtl::ServerSocket     CommunicationType;

  /** The SetCommunication method is used to attach a communication
    * object to the tracker object. */
  void SetCommunication( CommunicationType *communication );

protected:

  OpenIGTLinkVideoImager(void);

  virtual ~OpenIGTLinkVideoImager(void);

  /** Typedef for internal boolean return type. */
  typedef VideoImager::ResultType   ResultType;

  /** Open communication with the tracking device. */
  virtual ResultType InternalOpen( void );

  /** Close communication with the tracking device. */
  virtual ResultType InternalClose( void );

  /** Put the tracking device into tracking mode. */
  virtual ResultType InternalStartImaging( void );

  /** Take the tracking device out of tracking mode. */
  virtual ResultType InternalStopImaging( void );

  /** Update the status and the transforms for all VideoImagerTools. */
  virtual ResultType InternalUpdateStatus( void );

  /** Update the status and the transforms.
      This function is called by a separate thread. */
  virtual ResultType InternalThreadedUpdateStatus( void );

  /** Reset the tracking device to put it back to its original state. */
  virtual ResultType InternalReset( void );

  /** Verify imager tool information */
  virtual ResultType VerifyVideoImagerToolInformation(
                                                  const VideoImagerToolType * );

  /** The "ValidateSpecifiedFrequency" method checks if the specified frequency
   * is valid for the tracking device that is being used. */
  virtual ResultType ValidateSpecifiedFrequency( double frequencyInHz );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Remove imager tool entry from internal containers */
  virtual ResultType RemoveVideoImagerToolFromInternalDataContainers( const
                                     VideoImagerToolType * imagerTool );

  /** Add imager tool entry from internal containers */
  virtual ResultType AddVideoImagerToolToInternalDataContainers( const
                                     VideoImagerToolType * imagerTool );

private:

  OpenIGTLinkVideoImager(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

  /** Initialize camera and algorithm attributes such as Frame interleave
      template matching tolerance, extrapolate frame etc */
  bool Initialize();

  /** A mutex for multithreaded access to the buffer arrays */
  itk::MutexLock::Pointer  m_BufferLock;

  /** A buffer to hold frames */
  typedef std::map< std::string, igstk::Frame* >
                                             VideoImagerToolFrameContainerType;

  VideoImagerToolFrameContainerType          m_ToolFrameBuffer;

  /** Container holding status of the tools */
  std::map< std::string, int >  m_ToolStatusContainer;

  /** The "Communication" instance */
  CommunicationType::Pointer     m_Communication;
  igtl::MessageHeader::Pointer   m_HeaderMsg;
  igtl::ImageMessage::Pointer    m_ImgMsg;
  igtl::Socket::Pointer          m_Socket;
};

}

#endif //__igstk_OpenIGTLinkVideoImager_h_
