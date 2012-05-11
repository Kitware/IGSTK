/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkAtracsysEasyTrackTracker.h,v $
  Language:  C++
  Date:      $Date: 2011-05-09 13:29:45 $
  Version:   $Revision: 1.0 $

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "math.h"

#include "igstkAtracsysEasyTrackTracker.h"
#include "igstkAtracsysEasyTrackTrackerTool.h"
#include <itksys/SystemTools.hxx>

#include <sstream>
#include <iostream>
#include <fstream>

#include "vtkMatrix4x4.h"


namespace igstk
{

// init static objects
bool AtracsysEasyTrackTracker::m_BTrackingEnabled = false;
bool AtracsysEasyTrackTracker::m_BDeviceOpened = false;
glbDevice *AtracsysEasyTrackTracker::m_SpDevice = NULL;
glbHandle AtracsysEasyTrackTracker::m_ShHandle;
AtracsysEasyTrackTracker::PnpMarker *AtracsysEasyTrackTracker::m_SpMarkers[4] =
                                                  { NULL, NULL, NULL, NULL };
unsigned AtracsysEasyTrackTracker::m_SuCounter = 0;
AtracsysEasyTrackTracker* AtracsysEasyTrackTracker::m_TrackerObject = NULL;
bool AtracsysEasyTrackTracker::m_HasError = false;
unsigned int AtracsysEasyTrackTracker::m_ErrorCode = 0;
char *AtracsysEasyTrackTracker::m_ErrorString = NULL;

/** Constructor: Initializes all internal variables. */
AtracsysEasyTrackTracker::AtracsysEasyTrackTracker(void) :
m_StateMachine(this)
{
  // save this object as tracker object
  m_TrackerObject = this;

  // Set the thread
  this->SetThreadingEnabled( true );

  // Lock for the data buffer that is used to transfer the
  // transformations from thread that is communicating
  // with the tracker to the main thread.
  m_BufferLock = itk::MutexLock::New();
}

/** Destructor */
AtracsysEasyTrackTracker::~AtracsysEasyTrackTracker(void)
{
  if( m_BDeviceOpened )
  apiClose(m_ShHandle);
}

AtracsysEasyTrackTracker::ResultType AtracsysEasyTrackTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::AtracsysEasyTrackTracker::InternalOpen called ...\n");

  if( m_BDeviceOpened )
  return SUCCESS;

  // set error code to OKAY
  m_HasError = false;

  // set error callback for ETK api
  apiSetErrorCallback( igstk::AtracsysEasyTrackTracker::ErrorCallback );

  // open and init ETK library. TODO: config file handling
  m_ShHandle = apiInitialize( NULL );

  // set pnp marker callback
  apiSetPnPMarkerCallback( m_ShHandle,
                              igstk::AtracsysEasyTrackTracker::PnpMarkerCallback );

  // get ETK master device
  m_SpDevice = apiGetDevice( m_ShHandle, ETK_MASTER, 0 );

  // get error code from callback & check ptrs
  if( !m_ShHandle || !m_SpDevice ){
    return FAILURE;
    }

  if(m_HasError){
    ReportError();
    return FAILURE;
    }

  m_BDeviceOpened = true;
  return SUCCESS;
}

/** Verify tracker tool information. */
AtracsysEasyTrackTracker::ResultType
AtracsysEasyTrackTracker
::VerifyTrackerToolInformation( const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG,
    "igstk::AtracsysEasyTrackTracker::VerifyTrackerToolInformation called ...\n");

  // purposely not impelmented because the EasyTrack 500 is an active optical
  // system and tracker tools are set through port numbers
  return SUCCESS;
}

/** Detach System. */
AtracsysEasyTrackTracker::ResultType AtracsysEasyTrackTracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "igstk::AtracsysEasyTrackTracker::InternalClose called ...\n");

  if( !m_BDeviceOpened )
  return SUCCESS;

  igstkLogMacro( DEBUG, "trying apiClose.\n" );
  apiClose( m_ShHandle );
  igstkLogMacro( DEBUG, "after apiClose.\n" );

  m_BDeviceOpened = false;
  return SUCCESS;
}

/** Put the tracking device into tracking mode. */
AtracsysEasyTrackTracker::ResultType AtracsysEasyTrackTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG,
    "igstk::AtracsysEasyTrackTracker::InternalStartTracking called ...\n");

  if( m_BTrackingEnabled )
  return SUCCESS;

  // set error to okay
  m_HasError = false;

  // set marker callback

  apiSetMarkerCallback( m_ShHandle, igstk::AtracsysEasyTrackTracker::MarkerCallback );
  apiSetOption( m_SpDevice, "LEDMASK", (void*)
                    (ETK_MARKER1 | ETK_MARKER2 | ETK_MARKER3 | ETK_MARKER4) );
  apiStart (m_SpDevice);

  // check error
  if(m_HasError){
    ReportError();
    return FAILURE;
    }

  m_BTrackingEnabled = true;
  return SUCCESS;
}

/** Take the tracking device out of tracking mode. */
AtracsysEasyTrackTracker::ResultType AtracsysEasyTrackTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG,
    "igstk::AtracsysEasyTrackTracker::InternalStopTracking called ...\n");

  if( !m_BTrackingEnabled )
  return SUCCESS;

  // set error to OKAY
  m_HasError = false;

  apiSetMarkerCallback( m_ShHandle, NULL );
  apiStop(m_SpDevice);

  // check error
  if(m_HasError){
    ReportError();
    return FAILURE;
    }

  m_BTrackingEnabled = false;

  return SUCCESS;
}

/** Reset the tracking device to put it back to its original state. */
AtracsysEasyTrackTracker::ResultType AtracsysEasyTrackTracker::InternalReset( void )
{
  igstkLogMacro( DEBUG, "igstk::AtracsysEasyTrackTracker::InternalReset called ...\n");

  InternalStopTracking();

  return SUCCESS;
}

/** Update the status and the transforms for all TrackerTools. */
AtracsysEasyTrackTracker::ResultType AtracsysEasyTrackTracker::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG,
    "igstk::AtracsysEasyTrackTracker::InternalUpdateStatus called ...\n");

  // This method and the InternalThreadedUpdateStatus are both called
  // continuously in the Tracking state.  This method is called from
  // the main thread, while InternalThreadedUpdateStatus is called
  // from the thread that actually communicates with the device.
  // A shared memory buffer is used to transfer information between
  // the threads, and it must be locked when either thread is
  // accessing it.
  m_BufferLock->Lock();

  typedef TrackerToolTransformContainerType::const_iterator  InputConstIterator;

  InputConstIterator inputItr = this->m_ToolTransformBuffer.begin();
  InputConstIterator inputEnd = this->m_ToolTransformBuffer.end();


  TrackerToolsContainerType trackerToolContainer =
    this->GetTrackerToolContainer();


  while( inputItr != inputEnd )
    {
    // only report tools that are in view
    if (! this->m_ToolStatusContainer[inputItr->first])
      {
      igstkLogMacro( DEBUG, "igstk::CertusTracker::InternalUpdateStatus: " <<
                     "tool " << inputItr->first << " is not in view\n");

      // report to the tracker tool that the tracker is not available
      this->ReportTrackingToolNotAvailable(
        trackerToolContainer[inputItr->first]);

      ++inputItr;
      continue;
      }
    // report to the tracker tool that the tracker is Visible
    this->ReportTrackingToolVisible(trackerToolContainer[inputItr->first]);

    // create the transform
    TransformType transform;

    typedef TransformType::VectorType TranslationType;
    TranslationType translation;

    translation[0] = (inputItr->second)[9];
    translation[1] = (inputItr->second)[10];
    translation[2] = (inputItr->second)[11];


    typedef TransformType::VersorType RotationType;
    RotationType rotation;

    itk::Matrix<double,3,3> m;

    m(0,0) = (inputItr->second)[0];
    m(0,1) = (inputItr->second)[1];
    m(0,2) = (inputItr->second)[2];
    m(1,0) = (inputItr->second)[3];
    m(1,1) = (inputItr->second)[4];
    m(1,2) = (inputItr->second)[5];
    m(2,0) = (inputItr->second)[6];
    m(2,1) = (inputItr->second)[7];
    m(2,2) = (inputItr->second)[8];

    rotation.Set(m);

    // report error value
    typedef TransformType::ErrorType  ErrorType;
    const int transformationError = 12;
    ErrorType errorValue = (inputItr->second)[transformationError];

    transform.SetToIdentity(this->GetValidityTime());
    transform.SetTranslationAndRotation(translation, rotation, errorValue,
                                        this->GetValidityTime());
    // set the raw transform
    this->SetTrackerToolRawTransform(
      trackerToolContainer[inputItr->first], transform );

    this->SetTrackerToolTransformUpdate(
      trackerToolContainer[inputItr->first], true );

    ++inputItr;

    }// while

  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Update the shared memory buffer and the transforms.  This function
 *  is called by the thread that communicates with the tracker while
 *  the tracker is in the Tracking state. */
AtracsysEasyTrackTracker::ResultType AtracsysEasyTrackTracker
::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG,
       "igstk::AtracsysEasyTrackTracker::InternalThreadedUpdateStatus called ...\n");

  // Send the commands to the device that will get the transforms
  // for all of the tools.
  // Lock the buffer that this method shares with InternalUpdateStatus
  m_BufferLock->Lock();

  // call the ETK processing function
  apiProcess(m_SpDevice, 10);

  // unlock the buffer
  m_BufferLock->Unlock();

  // release the scheduler to update
  igstk::PulseGenerator::Sleep(1);

  return SUCCESS;
}

AtracsysEasyTrackTracker::ResultType
AtracsysEasyTrackTracker::
AddTrackerToolToInternalDataContainers( const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG,
    "igstk::AtracsysEasyTrackTracker::AddTrackerToolFromInternalDataContainers "
                 "called ...\n");

  if ( trackerTool == NULL )
    {
    return FAILURE;
    }

  const std::string trackerToolIdentifier =
                    trackerTool->GetTrackerToolIdentifier();

  std::vector< double > transform;
  transform.push_back( 1.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 1.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 1.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 1.0 );

  this->m_ToolTransformBuffer[ trackerToolIdentifier ] = transform;

  this->m_ToolStatusContainer[ trackerToolIdentifier ] = 0;

  return SUCCESS;
}


AtracsysEasyTrackTracker::ResultType
AtracsysEasyTrackTracker::
RemoveTrackerToolFromInternalDataContainers
( const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG,
    "igstk::AtracsysEasyTrackTracker::RemoveTrackerToolFromInternalDataContainers "
                 "called ...\n");

  const std::string trackerToolIdentifier =
                      trackerTool->GetTrackerToolIdentifier();

  // remove the tool from the Transform buffer container
  this->m_ToolTransformBuffer.erase( trackerToolIdentifier );
  this->m_ToolStatusContainer.erase( trackerToolIdentifier );

  return SUCCESS;
}

/** Print Self function */
void AtracsysEasyTrackTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Number of tools: " << this->m_NumberOfTools << std::endl;
}

void AtracsysEasyTrackTracker::ErrorCallback( unsigned uGravity,
                                       char *strDescription )
{
  AtracsysEasyTrackTracker::m_HasError = true;
  AtracsysEasyTrackTracker::m_ErrorCode = uGravity;
  AtracsysEasyTrackTracker::m_ErrorString = strDescription;
}


void AtracsysEasyTrackTracker::PnpMarkerCallback( glbDevice *pDevice,
                                           unsigned uMarkerID,
                                           unsigned uModelID,
                                           char *strMarkerName,
                                           unsigned uStatus )
{
  if (!m_SpMarkers [uMarkerID])
    m_SpMarkers [uMarkerID] = new PnpMarker;

  m_SpMarkers [uMarkerID]->muModel = uModelID;
  m_SpMarkers [uMarkerID]->muStatus = uStatus;
  if (strMarkerName)
    strcpy (m_SpMarkers [uMarkerID]->mstrName, strMarkerName);
}

void AtracsysEasyTrackTracker::MarkerCallback( glbDevice *pDevice,
                      unsigned uMarkerID,
                            double *adMatrix33,
                            double *adVector3,
                            double *pdError,
                            unsigned uExtended )
{
 if( m_TrackerObject )
    m_TrackerObject->ObjectMarkerCallback(
                          uMarkerID, adMatrix33, adVector3, pdError );
}

void AtracsysEasyTrackTracker::ObjectMarkerCallback( unsigned uMarkerID,
                                  double *adMatrix33,
                                  double *adVector3,
                                  double *pdError )
{
  char strMarkerID[128];
  sprintf(strMarkerID, "%d", uMarkerID);

  TrackerToolsContainerType trackerToolContainer =
    this->GetTrackerToolContainer();

  if( ! trackerToolContainer.count(strMarkerID) )
    return;

  if (adMatrix33 && adVector3 && pdError){
    // has valid measurement, set data in tool buffer
    this->m_ToolStatusContainer[strMarkerID] = 1;
    std::vector<double> transform;

    transform.push_back( adMatrix33[0] );
    transform.push_back( adMatrix33[1] );
    transform.push_back( adMatrix33[2] );
    transform.push_back( adMatrix33[3] );
    transform.push_back( adMatrix33[4] );
    transform.push_back( adMatrix33[5] );
    transform.push_back( adMatrix33[6] );
    transform.push_back( adMatrix33[7] );
    transform.push_back( adMatrix33[8] );
    transform.push_back( adVector3[0] );
    transform.push_back( adVector3[1] );
    transform.push_back( adVector3[2] );
    transform.push_back( *pdError );

    this->m_ToolTransformBuffer[strMarkerID] = transform;
    }
  else {
    this->m_ToolStatusContainer[strMarkerID] = 0;
    }
}

void AtracsysEasyTrackTracker::ReportError()
{
  printf("AtracsysEasyTrackTracker error: (%u): %s", m_ErrorCode, m_ErrorString );
}

} // end of namespace igstk
