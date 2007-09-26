/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMicronTracker.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

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

#define DEBUGMTC

#include "igstkMicronTracker.h"

#include <sstream>

namespace igstk
{

/** Constructor: Initializes all internal variables. */
MicronTracker::MicronTracker(void):m_StateMachine(this)
{
  m_NumberOfTools = 0;

  this->SetThreadingEnabled( true );

  // Lock for the data buffer that is used to transfer the
  // transformations from thread that is communicating
  // with the tracker to the main thread.
  m_BufferLock = itk::MutexLock::New();

  //instantiate Persistance object
  m_Persistence = new Persistence();

  //instantiate Markers object
  m_Markers = new Markers();

  //instantiate cameras
  m_Cameras = new Cameras();

  // initialize selected camera
  m_SelectedCamera = NULL;

}

/** Destructor */
MicronTracker::~MicronTracker(void)
{
  if ( m_Cameras != NULL )
    {
    delete m_Cameras;
    }

  if ( m_Markers != NULL )
    {
    delete m_Markers;
    }

  if ( m_Persistence != NULL )
    {
    delete m_Persistence;
    }
}

/** Specify camera calibration directory */
void MicronTracker::SetCameraCalibrationFilesDirectory( std::string fileName )
{
  igstkLogMacro( DEBUG,
                "MicronTracker::SetCameraCalibrationFilesDirectory called..\n");

  m_CalibrationFilesDirectory = fileName;
}

/** Specify the initialization filename */
void MicronTracker::SetInitializationFile( std::string fileName )
{
  igstkLogMacro( DEBUG,
                "MicronTracker::SetInitializationFileDirectoryPath called..\n");

  m_InitializationFile = fileName;
}

/** Load marker template  */
void
MicronTracker::LoadMarkerTemplate( std::string filename )
{
  igstkLogMacro( DEBUG,
                "MicronTracker::LoadMarkerTemplate called..\n");

  // clear templates already loaded
  this->m_Markers->clearTemplates();

  this->m_MarkerTemplateDirectory = filename;

  if ( m_MarkerTemplateDirectory == "" )
    {
    std::cerr << "Marker template directory name is empty string " << std::endl;
    return ;
    }
 
  char * markerTemplateDirectory = 
            const_cast< char *> ( m_MarkerTemplateDirectory.c_str() );
 
  Markers_LoadTemplates( markerTemplateDirectory );
}

MicronTracker::ResultType MicronTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "MicronTracker::InternalOpen called ...\n");

  /* Initialization involves two steps
   * 1) Set algorithm and camera attributes
   * 2) Attach the cameras
   */

  if ( ! this->Initialize() )
      {
      std::cerr << "Error initializing" << std::endl;
      return FAILURE;
      }

  if ( ! this->SetUpCameras() )
      {
      std::cerr << "Error setting up cameras " << std::endl;
      return FAILURE;
      }
  return SUCCESS;
}

/** Initialize camera and algorithm attributes such as 
 *  Frame interleave, template matching tolerance, extrapolate frame etc*/
bool MicronTracker::Initialize()
{
  igstkLogMacro( DEBUG, "MicronTracker::Initialize called ...\n");

  bool result = true;

  if ( m_InitializationFile == "" )
    {
    std::cerr << "Initialization file (.ini ) is not set" << std::endl;
    return FAILURE;
    }
  char * initializationFilename = 
            const_cast< char *> ( m_InitializationFile.c_str() );
  this->m_Persistence->setPath( initializationFilename );

  std::cout << "Initialization file: " << initializationFilename << std::endl;

  this->m_Persistence->setSection ("General");

  //Setting the TemplateMatchToleranceMM property in the Markers object
  double defaultTempMatchToleranceMM = 1.0;
  double defaultLightCoolness = 0.1;

  this->m_Markers->setTemplateMatchToleranceMM( 
      this->m_Persistence->retrieveDouble(
         "TemplateMatchToleranceMM", defaultTempMatchToleranceMM) );
  
  bool defaultSmallerXPFootprint = true;
  int  defaultExtrapolatedFrames = 5;

  bool SmallerXPFootprint = 
            (bool)(this->m_Persistence->retrieveInt(
                       "DetectSmallMarkers", defaultSmallerXPFootprint));

  int ExtrapolatedFrames = this->m_Persistence->retrieveInt(
                        "ExtrapolatedFrames", defaultExtrapolatedFrames);

  this->m_Markers->setSmallerXPFootprint(SmallerXPFootprint);
  this->m_Markers->setExtrapolatedFrames(ExtrapolatedFrames);

  //could have been clipped
  this->m_Persistence->saveInt("ExtrapolatedFrames", 
                               this->m_Markers->getExtrapolatedFrames());

  /* Markers directory might need to be created: TODO!!! */
  
  return result;
}

bool MicronTracker::SetUpCameras()
{
  igstkLogMacro( DEBUG, "MicronTracker::SetUpCameras called ...\n");

  bool result = true;

  this->m_Cameras->SetCameraCalibrationFilesDirectory( this->m_CalibrationFilesDirectory );
  int success = this->m_Cameras->AttachAvailableCameras();

  if ( success )
    {
    std::cerr << " No camera available or missing calibration file in:\t " 
              << this->m_CalibrationFilesDirectory << endl;
    std::cerr << "Error returned: " << MTLastErrorString() << std::endl; 
    result = false;
    }
  else
    {
    if ( this->m_Cameras->getCount() >= 1 ) 
      {
      this->m_SelectedCamera = this->m_Cameras->m_vCameras[0];
      } 
    }

  return result;
}

/** Detach camera . */
MicronTracker::ResultType MicronTracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "MicronTracker::InternalClose called ...\n");  
  m_Cameras->Detach();
 
  //TODO: parse detach procedure result 
  return SUCCESS;
}

/** Activate the tools attached to the tracking device. */
MicronTracker::ResultType MicronTracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, "MicronTracker::InternalActivateTools called ...\n");

  // There is no need to invoke a special command to activate the tools 

  return SUCCESS;
}

/** Deactivate the tools attached to the tracking device. */
MicronTracker::ResultType MicronTracker::InternalDeactivateTools( void )
{
  igstkLogMacro( DEBUG, "MicronTracker::InternalDeactivateTools called ...\n");

  // Communicate with the device to deactivate all tools (depending
  // on the tracker, there might not be anything to do here).

  // Return SUCCESS or FAILURE depending on whether communication
  // was successfully opened, without error
  return SUCCESS;
}

/** Put the tracking device into tracking mode. */
MicronTracker::ResultType MicronTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, "MicronTracker::InternalStartTracking called ...\n");  

  // Report errors, if any, and return SUCCESS or FAILURE
  // (the return value will be used by the superclass to
  //  set the appropriate input to the state machine) 
  return SUCCESS; 
}

/** Take the tracking device out of tracking mode. */
MicronTracker::ResultType MicronTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, "MicronTracker::InternalStopTracking called ...\n");

  // Send the command to stop tracking.

  // Report errors, if any, and return SUCCESS or FAILURE
  // (the return value will be used by the superclass to
  //  set the appropriate input to the state machine) 
  return SUCCESS; 
}

/** Reset the tracking device to put it back to its original state. */
MicronTracker::ResultType MicronTracker::InternalReset( void )
{
  // Send the command to reset.
  
  // Report errors, if any, and return SUCCESS or FAILURE
  // (the return value will be used by the superclass to
  //  set the appropriate input to the state machine) 
  return SUCCESS;
}

/** Update the status and the transforms for all TrackerTools. */
MicronTracker::ResultType MicronTracker::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG, "MicronTracker::InternalUpdateStatus called ...\n");

  // This method and the InternalThreadedUpdateStatus are both called
  // continuously in the Tracking state.  This method is called from
  // the main thread, while InternalThreadedUpdateStatus is called
  // from the thread that actually communicates with the device.
  // A shared memory buffer is used to transfer information between
  // the threads, and it must be locked when either thread is
  // accessing it.
  m_BufferLock->Lock();

  /* From the transform buffer information, create transform objects 
    
    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = m_TransformBuffer[port][7];

    transform.SetToIdentity(this->GetValidityTime());
    transform.SetTranslationAndRotation(translation, rotation, errorValue,
                                        this->GetValidityTime());

    this->SetToolTransform(port, 0, transform);
  */


  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Update the shared memory buffer and the transforms.  This function
 *  is called by the thread that communicates with the tracker while
 *  the tracker is in the Tracking state. */
MicronTracker::ResultType MicronTracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "MicronTracker::InternalThreadedUpdateStatus "
                 "called ...\n");
  std::cout << "InternalThreadedUpdateStatus called... " << std::endl;

  // Send the commands to the device that will get the transforms
  // for all of the tools.
  // Lock the buffer that this method shares with InternalUpdateStatus
  m_BufferLock->Lock();

  // Grab frame
  if ( ! m_Cameras->grabFrame( m_SelectedCamera ) )
    {
    std::cerr << "Error grabbing a frame" << std::endl;
    m_BufferLock->Unlock();
    return FAILURE;
    }

  // process frame
  int completionCode = m_Markers->processFrame( m_SelectedCamera ); 

  if ( completionCode != 0 ) 
    {
    std::cout << "Error in processing frame " << std::endl;
    m_BufferLock->Unlock();
    return FAILURE;
    }

#ifdef DEBUGMTC 
    unsigned char **laddr;
    unsigned char **raddr;
    m_SelectedCamera->getImages( &laddr, &raddr);
    int width = m_SelectedCamera->getXRes();
    int height = m_SelectedCamera->getYRes();

    unsigned int size = width*height;
    const char* copyPixels = (char *) laddr;

    unsigned int counter = 0; 
    std::stringstream leftImageNameStream;
    leftImageNameStream << "dataLeft" << counter << ".bin";
    
    std::string leftImageName = leftImageNameStream.str(); 
    ofstream myFile (leftImageName.c_str(), ios::out | ios::binary);
    myFile.write (copyPixels, size );
    myFile.close();

    std::stringstream rightImageNameStream;
    rightImageNameStream << "dataRight" << counter << ".bin";
    
    std::string rightImageName = rightImageNameStream.str(); 
    const char* copyPixelsRight = (char *) raddr;
    ofstream myFileRight (rightImageName.c_str(), ios::out | ios::binary);
    myFileRight.write (copyPixelsRight, size );
    myFileRight.close();

#endif
 
  // process identified markers
  Collection* markersCollection = new Collection(this->m_Markers->identifiedMarkers(this->m_SelectedCamera));
 
  std::cout << "\tNumber of identified markers: \t" <<  markersCollection->count() << std::endl;

  if (markersCollection->count() == 0) 
    {
    delete markersCollection; 
    m_BufferLock->Unlock();
    return FAILURE;
    }

  int markerNum = 1;
  int facetNum = 1;
  Marker* marker;
  for (markerNum = 1; markerNum <= markersCollection->count(); markerNum++)
    {
    marker = new Marker(markersCollection->itemI(markerNum));
    if (marker->wasIdentified(this->m_SelectedCamera) )
      {
      Collection* facetsCollection = new Collection(marker->identifiedFacets(this->m_SelectedCamera));
      for (facetNum = 1; facetNum <= facetsCollection->count(); facetNum++)
        {
        Facet* face = new Facet(facetsCollection->itemI(facetNum));
        /* Get the x-points. The x-points are returned as a four dimensional array
           [Long/Short vector][L/R][base/head][X/Y] */
        double LS_LR_BH_XY[2][2][2][2];
        face->getXpoints(this->m_SelectedCamera, (double *)LS_LR_BH_XY);

        char caption[255];
          if (facetsCollection->count() > 1) {
            sprintf(caption,"%d.%s/%d",markerNum, marker->getName(), facetNum);
          } else {
            sprintf(caption,"%d.%s",markerNum, marker->getName());
          }

        std::cout << caption <<  std::endl;

        delete face;
        }
      delete facetsCollection;
      }
    // DONOT invoke delete marker. This is a possible bug in Marker class.
    // Invoking the marker class destructor causes misidentification of the
    // markers in the subsequent frames. 
    //delete marker;
    }

  Xform3D* Marker2CurrCameraXf = NULL;
  for (markerNum = 1; markerNum <= markersCollection->count(); markerNum++)
    {
    marker = new Marker(markersCollection->itemI(markerNum));
    Marker2CurrCameraXf = marker->marker2CameraXf(this->m_SelectedCamera->Handle());
    // There may be a situation where marker was identified by another camera (not CurrCamera)
    // and the identifying camera is not registered with CurrCamera. In this case, the pose is
    // not known in CurrCamera coordinates and Marker2CurrCameraXf is Nothing.
    if(Marker2CurrCameraXf != NULL)
        {
        //Show the XYZ position of the Marker's origin.
        //string s = ".(";
        char buffer[3][100];
        char s[600];
        Xform3D* m2c;
        m2c = marker->marker2CameraXf(this->m_SelectedCamera->Handle());
        for (int i=0; i<3; i++)
        { 
          sprintf(buffer[i], "%.2f",m2c->getShift(i));
        }
        sprintf(s, "%d", markerNum);
        strcat(s, ". (");
        strcat(s, buffer[0]);
        strcat(s, ",");
        strcat(s, buffer[1]);
        strcat(s, ",");
        strcat(s, buffer[2]);
        strcat(s, ")");

        // If there's a tooltip, add it
        // Marker to tooltip is set using 
        // Marker_Tooltip2MarkerXfSet method or it can be 
        // specified in the marker template file in the
        // marker's coordinate system
        Xform3D* t2m; // tooltip to marker xform
        Xform3D* t2c; // tooltip to camera xform
        double svec[3];
        t2m = marker->tooltip2MarkerXf();
        t2m->getShiftVector(svec);
        if (svec[0] != 0 || svec[1] != 0 || svec[2] != 0) 
          { // non-null transform
          strcat(s, " tip(");
          t2c = t2m->concatenate(m2c);
          for (int i=0; i<3; i++) {
            sprintf(buffer[i], "%.2f", t2c->getShift(i));
          }
          strcat(s, buffer[0]);
          strcat(s, ",");
          strcat(s, buffer[1]);
          strcat(s, ",");
          strcat(s, buffer[2]);
          strcat(s, ")");
          delete t2c;
          }
        delete t2m;
        delete m2c;
        std::cout << "XYZ= " << s << std::endl;
        }

      delete Marker2CurrCameraXf;
    }

  delete markersCollection; 

  // Copy the transforms and any status information into the
  // buffer.

  /* TODO : create a data structure ( transform buffer ) that will be used to 
  store the position  on and pose information for the markers.  The data structure will be 
  a list vector of transform parameters. Each tracker tool ( marker ) 
  will have an entry. Get the identified makers list and for each marker
  run Maerk2CameraXfGet and get the pose information */  

  // unlock the buffer
  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Print Self function */
void MicronTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Number of tools: " << m_NumberOfTools << std::endl;
}

} // end of namespace igstk
