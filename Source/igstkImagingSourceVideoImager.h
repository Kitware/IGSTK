/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImagingSourceVideoImager.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkImagingSourceVideoImager_h
#define __igstkImagingSourceVideoImager_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "igstkVideoImager.h"
#include "igstkImagingSourceVideoImagerTool.h"

#include "unicap.h"
#include "unicap_status.h"
#include <sys/types.h>
#include <linux/types.h>
#include <stdio.h>
#include <map>

class vtkImageData;

namespace igstk {

/** \class ImagingSourceVideoImager
 * \brief This derivation of the VideoImager class provides communication
 * to the ImagingSource frame grabber
 *
 * This class controlls the communication with the video device.
 * The communication with the frame grabber is established with the unicap
 * library over firewire
 *
 * \ingroup VideoImager
 */

class ImagingSourceVideoImager : public VideoImager
{
public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( ImagingSourceVideoImager, VideoImager )

public:

  /** Get the number of tools that have been detected. */
  igstkGetMacro( NumberOfTools, unsigned int );

protected:

  ImagingSourceVideoImager(void);

  virtual ~ImagingSourceVideoImager(void);

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
  virtual ResultType VerifyVideoImagerToolInformation( const VideoImagerToolType * );

  /** The "ValidateSpecifiedFrequency" method checks if the specified frequency is
   * valid for the imaging device that is being used. */
  virtual ResultType ValidateSpecifiedFrequency( double frequencyInHz );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Create an associative container that maps error code to error
    * descritpion */
  static void CreateErrorCodeList();

  /** Get Error description given the error code */
  static std::string GetErrorDescription( unsigned int );

  /** Remove imager tool entry from internal containers */
  virtual ResultType RemoveVideoImagerToolFromInternalDataContainers( const
                                     VideoImagerToolType * imagerTool );

  /** Add imager tool entry to internal containers */
  virtual ResultType AddVideoImagerToolToInternalDataContainers( const
                                     VideoImagerToolType * imagerTool );

private:

  ImagingSourceVideoImager(const Self&);   //purposely not implemented
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

  /** Error map container */
  typedef std::map< unsigned int, std::string>  ErrorCodeContainerType;
  static ErrorCodeContainerType   m_ErrorCodeContainer;

  /** boolean to indicate if error code list is created */
  static bool m_ErrorCodeListCreated;

  /** Container holding status of the tools */
  std::map< std::string, int >  m_ToolStatusContainer;

  /** Members and functions for communication with Unicap library */
  unicap_handle_t handle;
  unicap_format_t format;
  unicap_data_buffer_t buffer;
  unicap_data_buffer_t *returned_buffer;

  unicap_handle_t open_device ();
  void set_format (unicap_handle_t handle);
  size_t uyvy2rgb24( __u8 *dest, __u8 *source, size_t dest_size,
             size_t source_size );

};

}  // namespace igstk

#endif //__igstk_ImagingSourceVideoImager_h_
