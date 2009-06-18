/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    VideoFrameGrabberAndViewerWebcamWin.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __VideoFrameGrabberAndViewerWebcamWin_h
#define __VideoFrameGrabberAndViewerWebcamWin_h

#include "VideoFrameGrabberAndViewerWebcamWinGUI.h"

#include "igstkConfigure.h"

#include "igstkMacros.h"
#include "igstkObject.h"
#include "igstkEvents.h"

#include "igstkAxesObject.h"
#include "igstkAxesObjectRepresentation.h"

#include "igstkVideoFrameSpatialObject.h"
#include "igstkVideoFrameRepresentation.h"

#include "igstkWebcamWinVideoImager.h"
#include "igstkWebcamWinVideoImagerTool.h"

#include "itkStdStreamLogOutput.h"

namespace igstk{
/** \class VideoFrameGrabberAndViewerWebcamWin
*
* \brief Implementation class for VideoFrameGrabberAndViewerWebcamWinGUI.
*
* This class implements the main application.
*
*/
class VideoFrameGrabberAndViewerWebcamWin : 
                                   public VideoFrameGrabberAndViewerWebcamWinGUI
{
public:

  /** Typedefs */
  igstkStandardClassBasicTraitsMacro( VideoFrameGrabberAndViewerWebcamWin,
                                      VideoFrameGrabberAndViewerWebcamWinGUI );

  igstkLoggerMacro();

  /** typedef for axes spatial objects */
  typedef igstk::AxesObject                           AxesObjectType;
  typedef igstk::AxesObjectRepresentation             AxesRepresentationType;

  /** video frame spatial object and representation*/
  typedef igstk::VideoFrameSpatialObject<unsigned char, 3 >
                                            VideoFrameSpatialObjectType;
  VideoFrameSpatialObjectType::Pointer      m_VideoFrame;

  typedef igstk::VideoFrameRepresentation<VideoFrameSpatialObjectType>
                            VideoFrameRepresentationType;

  VideoFrameRepresentationType::Pointer m_VideoFrameRepresentationForVideoView;


  /** VideoImager object and VideoImager tool*/
  igstk::VideoImagerTool::Pointer                m_WebcamWinVideoImagerTool;

  /** Public request methods from the GUI */
  virtual void RequestPrepareToQuit();
  void RequestInitialize();
  void RequestShutdown();

  VideoFrameGrabberAndViewerWebcamWin();
  virtual ~VideoFrameGrabberAndViewerWebcamWin();

private:

  VideoFrameGrabberAndViewerWebcamWin(const Self&); 
  void operator=(const Self&); 


  /** Define a initial world coordinate system */
  AxesObjectType::Pointer                   m_WorldReference;
  AxesRepresentationType::Pointer           m_WorldReferenceRepresentation;

  /** Log file */
  std::ofstream                             m_LogFile;

  class ErrorObserver : public itk::Command
  {
  public:
    typedef ErrorObserver                    Self;
    typedef ::itk::Command                   Superclass;
    typedef ::itk::SmartPointer<Self>        Pointer;
    typedef ::itk::SmartPointer<const Self>  ConstPointer;

    igstkNewMacro(Self)
    igstkTypeMacro(ErrorObserver, itk::Command)

    /** When an error occurs in an IGSTK component it will invoke this method
     *  with the appropriate error event object as a parameter.*/
    virtual void Execute(itk::Object *caller,
                         const itk::EventObject & event) throw (std::exception);

    /** When an error occurs in an IGSTK component it will invoke this method
     *  with the appropriate error event object as a parameter.*/
    virtual void Execute(const itk::Object *caller,
                         const itk::EventObject & event) throw (std::exception);

    /**Clear the current error.*/
    void ClearError() { this->m_ErrorOccured = false;
                        this->m_ErrorMessage.clear(); }
    /**If an error occurs in one of the observed IGSTK components this method
     * will return true.*/
    bool ErrorOccured() { return this->m_ErrorOccured; }
    /**Get the error message associated with the last IGSTK error.*/
    void GetErrorMessage(std::string &errorMessage)
    {
      errorMessage = this->m_ErrorMessage;
    }

  protected:

    /**Construct an error observer for all the possible errors that occur in
     * the observed IGSTK components.*/
    ErrorObserver();
    virtual ~ErrorObserver(){}
  private:
    bool m_ErrorOccured;
    std::string m_ErrorMessage;
    std::map<std::string,std::string> m_ErrorEvent2ErrorMessage;

    //purposely not implemented
    ErrorObserver(const Self&);
    void operator=(const Self&);
  };

  ErrorObserver::Pointer m_ErrorObserver;
  std::string m_ErrorMessage;

  VideoImager::Pointer       m_VideoImager;
  std::vector<VideoImagerTool::Pointer> m_Tools;

};

}//end of igstk
#endif
