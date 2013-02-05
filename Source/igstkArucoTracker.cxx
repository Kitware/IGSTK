/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:  $RCSfile: igstkArucoTracker.h,v $
  Language:  C++
  Date:    $Date: 2011-03-17 20:12:26 $
  Version:   $Revision: 1.00 $

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

   This software is distributed WITHOUT ANY WARRANTY; without even
   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
   PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in the
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

#include <vnl/vnl_quaternion.h>
#include "itkRealTimeClock.h"

#include "igstkArucoTracker.h"
#include "iostream"
#include "fstream"

namespace igstk
{

/** Constructor */
ArucoTracker::ArucoTracker ( void ) :
m_StateMachine( this )
{
  this->SetThreadingEnabled( true );

  m_BufferLock = itk::MutexLock::New();
  m_CameraCalibrationFileSet = false;
  m_MarkerSizeSet = false;
  m_SimulationVideo = "";
}

/** Desctructor */
ArucoTracker::~ArucoTracker ( void )
{
}

bool ArucoTracker::SetCameraParametersFromYAMLFile(std::string file)
{
  igstkLogMacro( DEBUG, "igstk::ArucoTracker::SetCameraParametersFromYAMLFile called ...\n" )

  bool success = this->m_CameraParameters.readFromYAMLFile( file );

  if(success)
    m_CameraCalibrationFileSet=true;

  return success;
}

/**----------------------------------------------------------------------------
*   InternalOpen
*  ----------------------------------------------------------------------------
*  The "InternalOpen" method opens communication with a tracking device.
*  ----------------------------------------------------------------------------
*/
ArucoTracker::ResultType ArucoTracker::InternalOpen ( void )
{
  igstkLogMacro( DEBUG, "igstk::ArucoTracker::InternalOpen called ...\n" )

  if( !m_CameraCalibrationFileSet)
    return FAILURE;

  if( !m_MarkerSizeSet)
    return FAILURE;

  if(!m_SimulationVideo.empty())
  {
     this->m_VideoCapturer.open(m_SimulationVideo);
  }
  else 
    this->m_VideoCapturer.open( 0 );

  if ( !this->m_VideoCapturer.isOpened() )
  {
    return FAILURE;
  }

  //read first image to get the dimensions
  this->m_VideoCapturer>>this->m_InputImage;
  this->m_CameraParameters.resize( this->m_InputImage.size() );

  igstkLogMacro( INFO, "\n" << "Initialization finished\n" );
  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   InternalClose
*  ----------------------------------------------------------------------------
*  The "InternalClose" method closes communication with a tracking device.
*  ----------------------------------------------------------------------------
*/
ArucoTracker::ResultType ArucoTracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "igstk::ArucoTracker::InternalClose called ...\n" )

  this->m_VideoCapturer.release();
  if ( this->m_VideoCapturer.isOpened() )
  {
    return FAILURE;
  }

  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   InternalReset
*  ----------------------------------------------------------------------------
*  The "InternalReset" method resets tracker to a known configuration.
*  ----------------------------------------------------------------------------
*/
ArucoTracker::ResultType ArucoTracker::InternalReset( void )
{
  igstkLogMacro( DEBUG, "igstk::ArucoTracker::InternalReset called ...\n" )
  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   InternalStartTracking
*  ----------------------------------------------------------------------------
*  The "InternalStartTracking" method starts tracking.
*  ----------------------------------------------------------------------------
*/
ArucoTracker::ResultType ArucoTracker::InternalStartTracking( void )
{
  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   InternalStopTracking
*  ----------------------------------------------------------------------------
*  The "InternalStopTracking" method stops tracking.
*  ----------------------------------------------------------------------------
*/
ArucoTracker::ResultType ArucoTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ArucoTracker::InternalStopTracking called ...\n" )
  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   InternalUpdateStatus
*  ----------------------------------------------------------------------------
*  The "InternalUpdateStatus" method updates tracker status.
*  ----------------------------------------------------------------------------
*/
ArucoTracker::ResultType ArucoTracker::InternalUpdateStatus( void )
{
  igstkLogMacro( DEBUG,
         "igstk::ArucoTracker::InternalUpdateStatus called ...\n" )
  TrackerToolsContainerType trackerToolContainer = GetTrackerToolContainer();

  TrackerToolsContainerType::iterator inputItr = trackerToolContainer.begin();
  TrackerToolsContainerType::iterator inputEnd = trackerToolContainer.end();

  // set all tracker to not yet updated
  while( inputItr != inputEnd )
  {
    this->SetTrackerToolTransformUpdate( trackerToolContainer[inputItr->first], false );
    ++inputItr;
  }

  this->m_BufferLock->Lock();

  // iterate over initialized tracker tools and update
  inputItr = trackerToolContainer.begin();
  while( inputItr != inputEnd )
  {
    for( unsigned int i=0; i < this->m_Markers.size(); i++ )
    {
      std::string toolStringID = inputItr->first;
      if( m_Markers[i].id == atoi(toolStringID.c_str()) )
      {
        try
        {
          // create the transform
          TransformType transform;

          typedef TransformType::VectorType TranslationType;
          TranslationType translation;

          typedef TransformType::VersorType RotationType;
          RotationType rotation;

          // get rotation vector (rotation axis, rotation angle) from the marker
          RotationType::VectorType RVect;

          cv::Mat cvRMat = this->m_Markers[i].Rvec;
          RVect[0]= cvRMat.at<float>(0,0);
          RVect[1]= cvRMat.at<float>(1,0);
          RVect[2]= cvRMat.at<float>(2,0);

          double angle = RVect.GetNorm();
          RVect.Normalize();

          rotation.Set(RVect,angle);

          translation[0] = this->m_Markers[i].Tvec.at<float>( 0, 0 );
          translation[1] = this->m_Markers[i].Tvec.at<float>( 1, 0 );
          translation[2] = this->m_Markers[i].Tvec.at<float>( 2, 0 );

          // report error value
          // Get error value from the tracker.
          typedef TransformType::ErrorType  ErrorType;
          ErrorType errorValue = 0;

          long lTime = this->GetValidityTime();

          transform.SetTranslationAndRotation( translation,
                             rotation,
                             errorValue,
                             lTime );

          // set the raw transform
          this->SetTrackerToolRawTransform( trackerToolContainer [toolStringID], transform );
          this->SetTrackerToolTransformUpdate( trackerToolContainer [toolStringID], true );
          // report to the tracker tool that the tracker is Visible
          this->ReportTrackingToolVisible(trackerToolContainer[toolStringID]);
        }
        catch( std::exception &ex )
        {
          igstkLogMacro( CRITICAL,
            "igstk::ArucoTracker::InternalUpdateStatus Exception:" << ex.what() )
          return FAILURE;
        }
      }
    }
    ++inputItr;
  }

  this->m_BufferLock->Unlock ();

  inputItr = trackerToolContainer.begin();
  inputEnd = trackerToolContainer.end();

  // set all tracker not updated to invisible
  while( inputItr != inputEnd )
  {
  if(!trackerToolContainer[inputItr->first]->GetUpdated())
    this->ReportTrackingToolNotAvailable(
      trackerToolContainer[inputItr->first]);
  ++inputItr;
  }
  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   InternalThreadedUpdateStatus
*  ----------------------------------------------------------------------------
*  The "InternalThreadedUpdateStatus" method updates tracker status.
*  ----------------------------------------------------------------------------
*/
ArucoTracker::ResultType
ArucoTracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG,
  "igstk::ArucoTracker::InternalThreadedUpdateStatus called ...\n" )

  try
  {
    this->m_VideoCapturer.grab();
    this->m_VideoCapturer.retrieve( this->m_InputImage );

    this->m_BufferLock->Lock();
    this->m_MDetector.detect( this->m_InputImage,
                              this->m_Markers,
                              this->m_CameraParameters,
                              m_MarkerSize );
    this->m_BufferLock->Unlock();
  }
  catch( std::exception &ex )
  {
    igstkLogMacro( CRITICAL,
      "igstk::ArucoTracker::InternalThreadedUpdateStatus Exception:" << ex.what() )
    return FAILURE;
  }

  return SUCCESS;
}

/**
 * Set marker size in mm.
 */
void ArucoTracker::SetMarkerSize(unsigned int size)
{
  if(size > 0)
  {
    m_MarkerSize = size;
    m_MarkerSizeSet=true;
  }
  else
  {
    m_MarkerSizeSet=false;
  }
}

/**
 * Grab current image from video stream. The current video
 * frame is independent of the image stream used for tracking.
 */
cv::Mat ArucoTracker::GetCurrentVideoFrame()
{
  cv::Mat tmpImage;
  this->m_VideoCapturer.grab();
  this->m_VideoCapturer.retrieve( tmpImage );
  return tmpImage;
}

/**----------------------------------------------------------------------------
*   PrintSelf
*  ----------------------------------------------------------------------------
*  Print the object information in a stream.
*  ----------------------------------------------------------------------------
*/
void ArucoTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  igstkLogMacro( DEBUG,
         "igstk::ArucoTracker::PrintSelf called ...\n" )

  Superclass::PrintSelf( os, indent );
}

/**----------------------------------------------------------------------------
*   VerifyTrackerToolInformation
*  ----------------------------------------------------------------------------
*  Verify if a tracker tool information is correct before attaching
*  ----------------------------------------------------------------------------
*/
ArucoTracker::ResultType
ArucoTracker::VerifyTrackerToolInformation (
   const TrackerToolType* trackerTool )
{
  igstkLogMacro( DEBUG,
  "igstk::ArucoTracker::VerifyTrackerToolInformation called ...\n" )

  if ( trackerTool == NULL )
  {
    igstkLogMacro( CRITICAL, "TrackerTool is not defined" )
    return FAILURE;
  }

  TrackerToolsContainerType trackerToolContainer = GetTrackerToolContainer();

  TrackerToolsContainerType::iterator inputItr = trackerToolContainer.begin();
  TrackerToolsContainerType::iterator inputEnd = trackerToolContainer.end();

  // check if marker name already set
  while( inputItr != inputEnd )
  {
    // if new tool marker name is already in the tool container
    // return failure
    if(std::strcmp(trackerTool->GetTrackerToolIdentifier().c_str(),
                   (inputItr->first).c_str()) == 0)
    return FAILURE;

    ++inputItr;
  }

  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   ValidateSpecifiedFrequency
*  ----------------------------------------------------------------------------
*  ----------------------------------------------------------------------------
*/
ArucoTracker::ResultType
ArucoTracker::ValidateSpecifiedFrequency( double frequencyInHz )
{
  igstkLogMacro( DEBUG,
   "igstk::ArucoTracker::ValidateSpecifiedFrequency called ...\n" )

  int maxFrequency = m_VideoCapturer.get(CV_CAP_PROP_FPS);
  // this pproperty is not supported for some integrated webcams
  // accept set frequency
  if(maxFrequency == 0)
  {
    if(frequencyInHz > 0)
      return SUCCESS;
  }
  else
  {
    if ( frequencyInHz <= 0.0 || frequencyInHz > maxFrequency )
    {
      return FAILURE;
    }
  }
  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   RemoveTrackerToolFromInternalDataContainers
*  ----------------------------------------------------------------------------
*  This method will remove entries of the traceker tool
*   from internal data containers
*  ----------------------------------------------------------------------------
*/
ArucoTracker::ResultType
ArucoTracker::RemoveTrackerToolFromInternalDataContainers(
 const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG,
   "igstk::ArucoTracker::RemoveTrackerToolFromInternalDataContainers \
   called ...\n" )

   if ( trackerTool == NULL )
    {
      return FAILURE;
    }

  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   AddTrackerToolToInternalDataContainers
*  ----------------------------------------------------------------------------
*  Add tracker tool entry to internal containers
*  ----------------------------------------------------------------------------
*/
ArucoTracker::ResultType
ArucoTracker::AddTrackerToolToInternalDataContainers (
   const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG,
  "igstk::ArucoTracker::AddTrackerToolToInternalDataContainers \
   called ...\n" )

   if ( trackerTool == NULL )
    {
      return FAILURE;
    }

  return SUCCESS;
}

} // end of namespace igstk
