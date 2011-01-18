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

//
// These header files can be found in the Utilities/MicronTracker subdirectory.
//
#include "Markers.h"
#include "Marker.h"
#include "Persistence.h"
#include "Cameras.h"
#include "MCamera.h"
#include "Facet.h"
#include "Xform3D.h"
#include "MTC.h"


#include <math.h>

#include "igstkMicronTracker.h"
#include "igstkMicronTrackerTool.h"
#include <itksys/SystemTools.hxx>

#include <sstream>

namespace igstk
{

//Initialize static variables
std::map< unsigned int, std::string> MicronTracker::m_ErrorCodeContainer;
bool MicronTracker::m_ErrorCodeListCreated = false;

/** Constructor: Initializes all internal variables. */
MicronTracker::MicronTracker(void):m_StateMachine(this)
{
  this->m_NumberOfTools = 0;

  this->SetThreadingEnabled( true );

  // Lock for the data buffer that is used to transfer the
  // transformations from thread that is communicating
  // with the tracker to the main thread.
  m_BufferLock = itk::MutexLock::New();

  //instantiate Persistance object
  this->m_Persistence = new Persistence();

  //instantiate Markers object
  this->m_Markers = new Markers();

  //instantiate cameras
  this->m_Cameras = new Cameras();

  // initialize selected camera
  this->m_SelectedCamera = NULL;

  // Camera light coolness
  this->m_CameraLightCoolness = 0.1;

  // Create error code list if it hasn't been
  // created.
  //
  if ( ! m_ErrorCodeListCreated )
    {
    this->CreateErrorCodeList();
    m_ErrorCodeListCreated = true;
    }
}

/** Destructor */
MicronTracker::~MicronTracker(void)
{
  if ( this->m_Cameras != NULL )
    {
    delete this->m_Cameras;
    }

  if ( this->m_Markers != NULL )
    {
    delete this->m_Markers;
    }

  if ( this->m_Persistence != NULL )
    {
    delete this->m_Persistence;
    }
}

/** Create a map data structure containing MTC error code and description */
void MicronTracker::CreateErrorCodeList()
{
  ErrorCodeContainerType & ecc = m_ErrorCodeContainer;

  ecc[0]  = "OK";
  ecc[1]  = "Invalid object handle";
  ecc[2]  = "Reentrant access - library is not thread-safe";
  ecc[3]  = "Internal MicronTracker software error";
  ecc[4]  = "Null pointer parameter";
  ecc[5]  = "Out of memory";
  ecc[6]  = "Parameter out of range";
  ecc[7]  = "String parameter too long";
  ecc[8]  = "Insufficient space allocated by the client to the output buffer";
  ecc[9]  = "Camera not initialized";
  ecc[10] = "Camera already initialized - cannot be initialized twice";
  ecc[11] = "Camera initialization failed";
  ecc[12] = "MTC is incompatible with a software module it calls";
  ecc[13] = "Calibration file error: unrecognized camera model";
  ecc[14] = "Path not set";
  ecc[15] = "Cannot access the directory specified";
  ecc[16] = "Write to file failed";
  ecc[17] = "Invalid Index parameter";
  ecc[18] = "Invalid SideI parameter";
  ecc[19] = "Invalid Divisor parameter";
  ecc[20] = "Attempting to access an item of an empty IntCollection";
  ecc[21] = "Insufficient samples";
  ecc[22] = "Insufficient samples that fit within the acceptance tolerance";
  ecc[23] = "Odd number of vector samples";
  ecc[24] = "Less than 2 vectors";
  ecc[25] = "More than maximum vectors per facet";
  ecc[26] = "Error exceeds tolerance";
  ecc[27] = "Insufficient angle between vectors";
  ecc[28] = "First vector is shorter than the second";
  ecc[29] = "Vector lengths are too similar";
  ecc[30] = "Template vector has 0 length";
  ecc[31] = "The template has not been created or loaded";
  ecc[32] = "Template file is corrupt";
  ecc[33] = "Maximum number of marker templates allowed exceeded";
  ecc[34] = "Geometries of different facets are too similar";
  ecc[35] = "Noncompliant facet definition";
  ecc[36] = "The SampledVectorPairsCollection contains non-Vector handles";
  ecc[37] = "Empty pixels buffer";
  ecc[38] = "Dimensions do not match";
  ecc[39] = "File open failed";
  ecc[40] = "File read failed";
  ecc[41] = "File write failed";
  ecc[42] = "Cannot open calibration file "
            "(typically named [driver]_[ser num].calib";
  ecc[43] = "Not a calibration file";
  ecc[44] = "Calibration file contents corrupt";
  ecc[45] = "Calibration file was not generated from this camera";
  ecc[46] = "Calibration file not loaded";
  ecc[47] = "Incorrect file version";
  ecc[48] = "Input image location is out of bounds of the measurement volume";
  ecc[49] = "Input image locations do not triangulate to a valid 3-D point";
  ecc[50] = "Transform between coordinate spaces is unknown";
  ecc[51] = "The given camera object was not found in the cameras array";
  ecc[52] = "Feature Data unavailable for the current frame";
  ecc[53] = "Feature Data is corrupt or incompatible with the current version";
  ecc[54] = "XYZ position is outside of calibrated field of view";
  ecc[55] = "Grab frame error";
}

std::string
MicronTracker::GetErrorDescription( unsigned int code )
{
  if ( code <= 55 )
    {
    return MicronTracker::m_ErrorCodeContainer[code];
    }
  else
    {
    return "Unknown error code";
    }
}

/** Load marker templates  */
bool
MicronTracker::LoadMarkerTemplates( void )
{
  igstkLogMacro( DEBUG,
                "igstk::MicronTracker::LoadMarkerTemplate called..\n");

  // clear templates already loaded
  this->m_Markers->clearTemplates();

  if( this->m_MarkerTemplatesDirectory == "" ||
    !itksys::SystemTools::FileExists( 
     this->m_MarkerTemplatesDirectory.c_str() ) )
    {
    igstkLogMacro( CRITICAL, "Marker templates directory is not properly set");
    return FAILURE;
    }

  // MTC library has poor const-correctness
  char * markerTemplateDirectory =
            const_cast< char *> ( this->m_MarkerTemplatesDirectory.c_str() );

  unsigned int  status = Markers_LoadTemplates( markerTemplateDirectory );

  if ( status != 0 )
    {
    igstkLogMacro( CRITICAL, "Error loading the marker templates: "
                   << MicronTracker::GetErrorDescription( status ));
    return FAILURE;
    }

  return SUCCESS;
}

MicronTracker::ResultType MicronTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::MicronTracker::InternalOpen called ...\n");

  /* Initialization involves two steps
   * 1) Loading marker templates
   * 2) Setting algorithm and camera attributes
   * 3) Attaching the cameras
   */

  if( ! this->LoadMarkerTemplates() )
    {
    igstkLogMacro( CRITICAL, "Error loading marker templates");
    return FAILURE;
    }

  if( ! this->Initialize() )
    {
    igstkLogMacro( CRITICAL, "Error initializing");
    return FAILURE;
    }

  return SUCCESS;
}

/** Initialize camera and algorithm attributes such as Frame interleave,
 * template matching tolerance, extrapolate frame, etc. */
bool MicronTracker::Initialize( void )
{
  igstkLogMacro( DEBUG, "igstk::MicronTracker::Initialize called ...\n");

  bool result = true;
  if( this->m_InitializationFile == "" ||
      !itksys::SystemTools::FileExists( this->m_InitializationFile.c_str() ) )
    {
    igstkLogMacro( CRITICAL, "Initialization file (.ini ) is not properly set");
    return FAILURE;
    }
  char * initializationFilename =
            const_cast< char *> ( this->m_InitializationFile.c_str() );
  this->m_Persistence->setPath( initializationFilename );
  std::string sectionName = "General";

  // MTC library has poor const-correctness
  this->m_Persistence->setSection ( const_cast< char *>( sectionName.c_str() ));

  // The LightCoolness property provides an indication to the camera of how the
  // illumination spectrum is distributed, allowing it to correct for the
  // difference between the calibration light spectrum and the measurement
  // light spectrum.  The LightCoolness value indicates the balance between
  // reds (warm) and blue (cool) colors in the lighting.
  const double defaultLightCoolness = 0.1;

  // light coolness value will be set on the camera after it is attached.
  std::string doubleName = "LightCoolness";
  // MTC library has poor const-correctness
  this->m_CameraLightCoolness =
      this->m_Persistence->retrieveDouble(
         const_cast< char *>( doubleName.c_str() ), defaultLightCoolness);

  // PredictiveFramesInterleave controls the number of predictive-only matching
  // frames inserted between each subsequent full-matching frames. A valued of 0
  // means that all frames are full-matching (require more computing resource).
  // A value of 3, for example, means that 3 out of every 4 frames would be
  // predictive-only.
  //

  const int defaultFrameInterleave = 0;
  std::string intName = "PredictiveFramesInterleave";
  // MTC library has poor const-correctness
  this->m_Markers->setPredictiveFramesInterleave(
    this->m_Persistence->retrieveInt( const_cast< char * >( intName.c_str() ),
      defaultFrameInterleave) );

  // Sets the template match tolerance (in millimeters).  This tolerance
  // determines the sensitivity of the MicronTracker to deviations between the
  // measured relative positions of XPoints and their relative positions as
  // described in the facet templates . The lower the tolerance, the more
  // likely it is tracking would be momentarily lost under some circumstances
  // (poor lighting, large distance), but the more closely the geometry of
  // different markers can be without the possibility of accidentally confusing
  // them.
  //

  const double defaultTempMatchToleranceMM = 1.0;
  doubleName = "TemplateMatchToleranceMM";
  // MTC library has poor const-correctness
  this->m_Markers->setTemplateMatchToleranceMM(
      this->m_Persistence->retrieveDouble(
         const_cast< char * >( doubleName.c_str() ),
         defaultTempMatchToleranceMM) );

  // Selects which image footprint size to use for the XPoint detection
  // algorithm.  Smaller XPoint footprint is 9 pixels in diameter, while the
  // regular size is 11.  Using the smaller footprint allows to detect smaller
  // markers at larger distances from the camera, at the expense of reduced
  // accuracy near the far end of the FOM.
  const bool defaultSmallerXPFootprint = true;
  intName = "DetectSmallMarkers";
  // MTC library has poor const-correctness
  const bool SmallerXPFootprint =
            (bool)(this->m_Persistence->retrieveInt(
               const_cast< char * >( intName.c_str() ),
               defaultSmallerXPFootprint));
  this->m_Markers->setSmallerXPFootprint(SmallerXPFootprint);

  // ExtrapolatedFrames controls the number of future frames for which the
  // markers, which have been identified in the current frame, are remembered.
  // The predicted locations of the remembered markers can then be reported if
  // not found in the future frames. Setting this property to 0 disables
  // extrapolation.
  const int defaultExtrapolatedFrames = 5;
  intName = "ExtrapolatedFrames";
  // MTC library has poor const-correctness
  const int ExtrapolatedFrames = this->m_Persistence->retrieveInt(
    const_cast< char * >( intName.c_str() ), defaultExtrapolatedFrames);
  this->m_Markers->setExtrapolatedFrames(ExtrapolatedFrames);

  return result;
}

bool MicronTracker::SetUpCameras( void )
{
  igstkLogMacro( DEBUG, "igstk::MicronTracker::SetUpCameras called ...\n");

  bool result = true;

  if ( this->m_CameraCalibrationFilesDirectory == "" ||
       !itksys::SystemTools::FileExists(
       this->m_CameraCalibrationFilesDirectory.c_str() ) )
    {
    igstkLogMacro( CRITICAL,
      "Camera calibration directory is not properly set");
    return FAILURE;
    }

  this->m_Cameras->SetCameraCalibrationFilesDirectory(
    this->m_CameraCalibrationFilesDirectory );

  int success = this->m_Cameras->AttachAvailableCameras();

  if ( success )
    {
    igstkLogMacro(CRITICAL,
      " No camera available or missing calibration file in:\t "
      << this->m_CameraCalibrationFilesDirectory);

    igstkLogMacro(CRITICAL, "MTC Error returned: " << MTLastErrorString());

    result = false;
    }
  else
    {
    //This tracker implementation only handles a single camera. However, MTC
    //library provides
    if ( this->m_Cameras->getCount() >= 1 )
      {
      this->m_SelectedCamera = this->m_Cameras->m_vCameras[0];
      // set the camera Light coolness parameter
      this->m_SelectedCamera->setLightCoolness( this->m_CameraLightCoolness );
      }
    }

  return result;
}

/** Verify tracker tool information. */
MicronTracker::ResultType
MicronTracker
::VerifyTrackerToolInformation( const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG,
    "igstk::MicronTracker::VerifyTrackerToolInformation called ...\n");

  // Verify that the template file for the marker is found
  unsigned int totalNumberOfTemplates = Markers_TemplatesCount();


  typedef igstk::MicronTrackerTool              MicronTrackerToolType;

  TrackerToolType * trackerToolNonConst =
          const_cast<TrackerToolType *>( trackerTool );

  MicronTrackerToolType * micronTrackerTool  =
        dynamic_cast< MicronTrackerToolType *> ( trackerToolNonConst );

  if ( micronTrackerTool == NULL )
    {
    igstkLogMacro ( DEBUG, "Tracker tool probably not Micron Tracker type " );
    return FAILURE;
    }

  for (unsigned int idx=1; idx <= totalNumberOfTemplates; idx++ )
    {
    std::string templateName;
    this->m_Markers->getTemplateItemName(idx, templateName );
    if( micronTrackerTool->GetMarkerName() == templateName )
      {
      return SUCCESS;
      }
    }

  igstkLogMacro( CRITICAL, "Tracker tool template NOT FOUND");
  return FAILURE;
}

MicronTracker::ResultType MicronTracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "igstk::MicronTracker::InternalClose called ...\n");
  return SUCCESS;
}

/** Put the tracking device into tracking mode. */
MicronTracker::ResultType MicronTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG,
    "igstk::MicronTracker::InternalStartTracking called ...\n");

  // Report errors, if any, and return SUCCESS or FAILURE
  // (the return value will be used by the superclass to
  //  set the appropriate input to the state machine)
  //
  
  if( ! this->SetUpCameras() )
    {
    igstkLogMacro( CRITICAL, "Error setting up cameras ");
    return FAILURE;
    }

  return SUCCESS;
}

/** Take the tracking device out of tracking mode. */
MicronTracker::ResultType MicronTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG,
    "igstk::MicronTracker::InternalStopTracking called ...\n");

  //Detach the camera
  this->m_Cameras->Detach();

  return SUCCESS;
}

/** Reset the tracking device to put it back to its original state. */
MicronTracker::ResultType MicronTracker::InternalReset( void )
{
  igstkLogMacro( DEBUG, "igstk::MicronTracker::InternalReset called ...\n");
  
  //Detach the camera
  this->m_Cameras->Detach();

  return SUCCESS;
}

/** Update the status and the transforms for all TrackerTools. */
MicronTracker::ResultType MicronTracker::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG,
    "igstk::MicronTracker::InternalUpdateStatus called ...\n");

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

  unsigned int toolId = 0;

  while( inputItr != inputEnd )
    {
    // only report tools that are in view
    if (! this->m_ToolStatusContainer[inputItr->first])
      {
      igstkLogMacro( DEBUG, "igstk::MicronTracker::InternalUpdateStatus: " <<
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

    translation[0] = (inputItr->second)[0];
    translation[1] = (inputItr->second)[1];
    translation[2] = (inputItr->second)[2];

    typedef TransformType::VersorType RotationType;
    RotationType rotation;

    rotation.Set( (inputItr->second)[3],
                   (inputItr->second)[4],
                   (inputItr->second)[5],
                   (inputItr->second)[6]);

    // report error value
    // Get error value from the tracker. TODO
    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = 0.0;

    transform.SetToIdentity(this->GetValidityTime());
    transform.SetTranslationAndRotation(translation, rotation, errorValue,
                                        this->GetValidityTime());

    // set the raw transform
    this->SetTrackerToolRawTransform(
      trackerToolContainer[inputItr->first], transform );

    this->SetTrackerToolTransformUpdate(
      trackerToolContainer[inputItr->first], true );

    ++inputItr;
    ++toolId;
    }

  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Update the shared memory buffer and the transforms.  This function
 *  is called by the thread that communicates with the tracker while
 *  the tracker is in the Tracking state. */
MicronTracker::ResultType MicronTracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::MicronTracker::InternalThreadedUpdateStatus "
                 "called ...\n");

  // Send the commands to the device that will get the transforms
  // for all of the tools.
  //
  // Grab frame
  if ( ! this->m_Cameras->grabFrame( this->m_SelectedCamera ) )
    {
    igstkLogMacro( CRITICAL, "Error grabbing a frame");
    return FAILURE;
    }

  // process frame
  unsigned int completionCode =
    this->m_Markers->processFrame( m_SelectedCamera );

  if ( completionCode != 0 )
    {
    igstkLogMacro( CRITICAL, "Error processing a frame: "
     << MicronTracker::GetErrorDescription( completionCode ));
    return FAILURE;
    }

  // Lock the buffer to update the tool transform 
  m_BufferLock->Lock();
 
  // First, reset the status of all the tracker tools
  typedef TrackerToolTransformContainerType::const_iterator  InputConstIterator;
  InputConstIterator inputItr = this->m_ToolTransformBuffer.begin();
  InputConstIterator inputEnd = this->m_ToolTransformBuffer.end();

  while( inputItr != inputEnd )
    {
    this->m_ToolStatusContainer[inputItr->first] = 0;
    ++inputItr;
    }

  Collection* markersCollection =
    new Collection(this->m_Markers->identifiedMarkers(this->m_SelectedCamera));

  const unsigned int markersCollectionCount = 
      static_cast<unsigned int>( markersCollection->count() );

  if (markersCollectionCount == 0)
    {
    delete markersCollection;
    m_BufferLock->Unlock();
    return SUCCESS;
    }

  for(unsigned int markerNum = 1;
      markerNum <= markersCollectionCount; markerNum++)
    {
    MTMarker::Marker * marker;
    marker = new MTMarker::Marker(markersCollection->itemI(markerNum));
    if (marker->wasIdentified(this->m_SelectedCamera) )
      {
      //Get postion and pose information
      Xform3D* Marker2CurrCameraXf = NULL;
      Marker2CurrCameraXf =
        marker->marker2CameraXf(this->m_SelectedCamera->Handle());

      if(Marker2CurrCameraXf != NULL)
        {
        // Tooltip calibration information which could be available in the
        // marker template file will not be used here. If needed, the 
        // calibration transform should be set to the tracker tool using the
        // SetCalibrationTransform method in the trackertool and the Tracker
        // base class will computed the composition.

        // Add the translation and rotation to the transform buffer
        std::vector < double > transform;

        double translation[3];
        //the first three are translation
        translation[0] = Marker2CurrCameraXf->getShift(0);
        translation[1] = Marker2CurrCameraXf->getShift(1);
        translation[2] = Marker2CurrCameraXf->getShift(2);

        transform.push_back( translation[0] );
        transform.push_back( translation[1] );
        transform.push_back( translation[2] );

        //the next four are quaternion
        double quaternion[4];

        quaternion[0] = -1.0 * Marker2CurrCameraXf->getQuaternion(0);
        quaternion[1] = -1.0 * Marker2CurrCameraXf->getQuaternion(1);
        quaternion[2] = -1.0 * Marker2CurrCameraXf->getQuaternion(2);
        quaternion[3] = Marker2CurrCameraXf->getQuaternion(3);

        transform.push_back( quaternion[0] );
        transform.push_back( quaternion[1] );
        transform.push_back( quaternion[2] );
        transform.push_back( quaternion[3] );

        //
        // Check if a Tracker tool is added with this marker type
        //
        typedef TrackerToolTransformContainerType::iterator InputIterator;

        InputIterator markerItr =
          m_ToolTransformBuffer.find( marker->getName() );

        if( markerItr != this->m_ToolTransformBuffer.end() )
          {
          this->m_ToolTransformBuffer[ marker->getName() ] = transform;
          this->m_ToolStatusContainer[ marker->getName() ] = 1;
          }
        }
  
      }
    // DO NOT delete marker. This is a possible bug in Marker class.
    // Invoking the marker class destructor causes misidentification of the
    // markers in the subsequent frames.
    //
    //             "delete marker;"
    //
    }

  // unlock the buffer
  m_BufferLock->Unlock();
 
  delete markersCollection;

  return SUCCESS;
}

MicronTracker::ResultType
MicronTracker::
AddTrackerToolToInternalDataContainers( const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG,
    "igstk::MicronTracker::RemoveTrackerToolFromInternalDataContainers "
                 "called ...\n");

  if ( trackerTool == NULL )
    {
    return FAILURE;
    }

  const std::string trackerToolIdentifier =
                    trackerTool->GetTrackerToolIdentifier();

  std::vector< double > transform;
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 1.0 );

  this->m_ToolTransformBuffer[ trackerToolIdentifier ] = transform;
  this->m_ToolStatusContainer[ trackerToolIdentifier ] = 0;

  return SUCCESS;
}


MicronTracker::ResultType
MicronTracker::
RemoveTrackerToolFromInternalDataContainers
( const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG,
    "igstk::MicronTracker::RemoveTrackerToolFromInternalDataContainers "
                 "called ...\n");

  const std::string trackerToolIdentifier =
                      trackerTool->GetTrackerToolIdentifier();

  // remove the tool from the Transform buffer container
  this->m_ToolTransformBuffer.erase( trackerToolIdentifier );
  this->m_ToolStatusContainer.erase( trackerToolIdentifier );

  return SUCCESS;
}

/**The "ValidateSpecifiedFrequency" method checks if the specified
  * frequency is valid for the tracking device that is being used. */
MicronTracker::ResultType
MicronTracker::ValidateSpecifiedFrequency( double frequencyInHz )
{
  const double MAXIMUM_FREQUENCY = 50;
  if ( frequencyInHz < 0.0 || frequencyInHz > MAXIMUM_FREQUENCY )
    {
    return FAILURE;
    } 
  return SUCCESS;
}


/** Print Self function */
void MicronTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Number of tools: " << this->m_NumberOfTools << std::endl;
  os << indent << "Camera calibration files directory: "
               << this->m_CameraCalibrationFilesDirectory << std::endl;
  os << indent << "Initialization file: " 
               << this->m_InitializationFile << std::endl;
  os << indent << "Markers template directory: "
               << this->m_MarkerTemplatesDirectory << std::endl;
  os << indent << "Camera Light coolness value : "
               << this->m_CameraLightCoolness << std::endl;
}

} // end of namespace igstk
