/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTerasonImager.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTerasonImager_h
#define __igstkTerasonImager_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "igstkImager.h"
#include "igstkTerasonImagerTool.h"
#include "igtlServerSocket.h"
#include "igtlImageMessage.h"

class vtkImageData;

// TerasonImager utilitiy classes declarations.

#include <map>

namespace igstk {

/** \class TerasonImager
 * \brief This imager provides support for socket communication (using the 
 * Open IGTLink protocol) to the Terason system 
 *
 * 
 * \ingroup Imager
 */

class TerasonImager : public Imager
{
public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( TerasonImager, Imager )

  /** Communication type */
  typedef igtl::ServerSocket     CommunicationType;
  //typedef igtl::ClientSocket     CommunicationType;


  /** The SetCommunication method is used to attach a communication
    * object to the tracker object. */
  void SetCommunication( CommunicationType *communication );

public:

  /** Get the number of tools that have been detected. */
  igstkGetMacro( NumberOfTools, unsigned int );

protected:

  TerasonImager(void);

  virtual ~TerasonImager(void);

  /** Typedef for internal boolean return type. */
  typedef Imager::ResultType   ResultType;

  /** Open communication with the tracking device. */
  virtual ResultType InternalOpen( void );

  /** Close communication with the tracking device. */
  virtual ResultType InternalClose( void );

  /** Put the tracking device into tracking mode. */
  virtual ResultType InternalStartImaging( void );

  /** Take the tracking device out of tracking mode. */
  virtual ResultType InternalStopImaging( void );

  /** Update the status and the transforms for all ImagerTools. */
  virtual ResultType InternalUpdateStatus( void );

  /** Update the status and the transforms.
      This function is called by a separate thread. */
  virtual ResultType InternalThreadedUpdateStatus( void );

  /** Reset the tracking device to put it back to its original state. */
  virtual ResultType InternalReset( void );

  /** Verify imager tool information */
  virtual ResultType VerifyImagerToolInformation( const ImagerToolType * );

  /** The "ValidateSpecifiedFrequency" method checks if the specified frequency is 
   * valid for the tracking device that is being used. */
  virtual ResultType ValidateSpecifiedFrequency( double frequencyInHz );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Create an associative container that maps error code to error
    * descritpion */
  static void CreateErrorCodeList();

  /** Get Error description given the error code */
  static std::string GetErrorDescription( unsigned int );

  /** Remove imager tool entry from internal containers */
  virtual ResultType RemoveImagerToolFromInternalDataContainers( const
                                     ImagerToolType * imagerTool );

  /** Add imager tool entry from internal containers */
  virtual ResultType AddImagerToolToInternalDataContainers( const
                                     ImagerToolType * imagerTool );


private:

  TerasonImager(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

  /** Initialize camera and algorithm attributes such as Frame interleave
      template matching tolerance, extrapolate frame etc */
  bool Initialize();

  /** A mutex for multithreaded access to the buffer arrays */
  itk::MutexLock::Pointer  m_BufferLock;

  /** Total number of tools detected. */
  unsigned int   m_NumberOfTools;

  bool m_FirstFrame;

  /** A buffer to hold frames */
//  typedef std::map< std::string, igtl::ImageMessage::Pointer >
//                                ImagerToolFrameContainerType;

  typedef std::map< std::string, igstk::Frame >
                                ImagerToolFrameContainerType;

  ImagerToolFrameContainerType           m_ToolFrameBuffer;

  /** Error map container */
  typedef std::map< unsigned int, std::string>  ErrorCodeContainerType;
  static ErrorCodeContainerType   m_ErrorCodeContainer;

  /** boolean to indicate if error code list is created */
  static bool m_ErrorCodeListCreated;

  /** Container holding status of the tools */
  std::map< std::string, int >  m_ToolStatusContainer;

   /** The "Communication" instance */
  CommunicationType::Pointer       m_Communication;

  igtl::MessageHeader::Pointer    m_HeaderMsg;

  igtl::ImageMessage::Pointer     imgMsg;

  igtl::Socket::Pointer            m_Socket;

};

}

#endif //__igstk_TerasonImager_h_
