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

#define PI atan(1.0) * 4

#include "igstkMicronTracker.h"

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
  this->m_MarkerTemplateDirectory = filename;

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
  bool result = true;

  char * initializationFilename = 
            const_cast< char *> ( m_InitializationFile.c_str() );
  this->m_Persistence->setPath( initializationFilename );
  this->m_Persistence->setSection ("General");

  //Setting the TemplateMatchToleranceMM property in the Markers object
  double defaultTempMatchToleranceMM = 1.0;
  double defaultLightCoolness = 0.1;

  this->m_Markers->setTemplateMatchToleranceMM( this->m_Persistence->retrieveDouble("TemplateMatchToleranceMM", defaultTempMatchToleranceMM) );
  
  bool defaultSmallerXPFootprint = true;
  int defaultExtrapolatedFrames = 5;

  bool SmallerXPFootprint = (bool)(this->m_Persistence->retrieveInt("DetectSmallMarkers", defaultSmallerXPFootprint));
  int ExtrapolatedFrames = this->m_Persistence->retrieveInt("ExtrapolatedFrames", defaultExtrapolatedFrames);

  this->m_Markers->setSmallerXPFootprint(SmallerXPFootprint);
  this->m_Markers->setExtrapolatedFrames(ExtrapolatedFrames);

  //could have been clipped
  this->m_Persistence->saveInt("ExtrapolatedFrames", this->m_Markers->getExtrapolatedFrames());

  /* Markers directory might need to be created: TODO!!! */
  
  return result;
}

bool MicronTracker::SetUpCameras()
{
  bool result = true;

  this->m_Cameras->SetCameraCalibrationFilesDirectory( this->m_CalibrationFilesDirectory );
  int success = this->m_Cameras->AttachAvailableCameras();

  if ( success )
    {
    std::cerr << " No camera available or missing calibration file. " 
              << " Please also check that MTHome system environment variable is set " << endl;
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

  // Send the commands to the device that will get the transforms
  // for all of the tools.
  // Lock the buffer that this method shares with InternalUpdateStatus
  m_BufferLock->Lock();

  // process frame
  m_Markers->processFrame( NULL );

  // process identified markers
  Collection* markersCollection = new Collection(this->m_Markers->identifiedMarkers(this->m_SelectedCamera));
  if (markersCollection->count() == 0) 
    {
    delete markersCollection; 
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
        Facet* f = new Facet(facetsCollection->itemI(facetNum));
        /* Get the x-points. The x-points are returned as a four dimensional array
           [Long/Short vector][L/R][base/head][X/Y] */
        double LS_LR_BH_XY[2][2][2][2];
        f->getXpoints(this->m_SelectedCamera, (double *)LS_LR_BH_XY);

        char caption[255];
          if (facetsCollection->count() > 1) {
            sprintf(caption,"%d.%s/%d",markerNum, marker->getName(), facetNum);
          } else {
            sprintf(caption,"%d.%s",markerNum, marker->getName());
          }

        std::cout << caption <<  std::endl;

        delete f;
        }
      delete facetsCollection;
      }
    delete marker;
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

    Xform3D* Mi2Cam = NULL;
    Xform3D* Mj2Cam = NULL;
    for(int i=1; i<markersCollection->count(); i++)
      {
      Marker* m1 = new Marker(markersCollection->itemI(i));
      Mi2Cam = m1->marker2CameraXf(this->m_SelectedCamera->Handle());
      for (int j=i+1; j<markersCollection->count()+1; j++)
        {
        Marker* m2 = new Marker(markersCollection->itemI(j));
        Mj2Cam = m2->marker2CameraXf(this->m_SelectedCamera->Handle());
        if(Mi2Cam != NULL || Mj2Cam != NULL)
          {
          Xform3D* Xf;
          double XUnitV[3] = {0};
          double angleCos;
          double angleRads;

          double vec1[3];
          Mi2Cam->getShiftVector(vec1);
          double vec2[3];
          Mj2Cam->getShiftVector(vec2);
          double distance = this->FindDistance( vec1, vec2 );
            
          char s[300];
          char buffer[100];
          sprintf(s, "%d", i);
          strcat(s, "-");
          sprintf(buffer, "%d", i+1);
          strcat(s, buffer);
          strcat(s, ": ");
          sprintf(buffer, "%.2f", distance);
          strcat(s, buffer);
          strcat(s, " mm / ");
            
          XUnitV[0] = 1;
          double XVect1[3];
          Mi2Cam->getRotateVector(XVect1, XUnitV);
          double XVect2[3];
          Mj2Cam->getRotateVector(XVect2, XUnitV);
          angleCos = this->EvaluteDotProduct(XVect1, XVect2);
          angleRads = ACOS(angleCos);
            
          sprintf(buffer, "%.1f", angleRads * 180 / PI);
          strcat(s, buffer);
#ifdef WIN32
          strcat(s, "°");
#else
          strcat(s, " deg");
#endif
           std::cout << "Angle=" << s << std::endl;
           }
        delete m2;
        }
        delete Mj2Cam;
        delete m1;
        delete Mi2Cam;
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

double MicronTracker::FindDistance(double* v1, double* v2)
{
  double acc = 0.0;
  for (int i=0; i< 3; i++)
    acc = acc + ( (v1[i] - v2[i]) * (v1[i] - v2[i]) );
  return sqrt(acc);
}

/****************************/
double MicronTracker::EvaluteDotProduct(double* v1, double* v2)
{
  double result = 0;
  for (int i=0; i<3; i++)
    result += v1[i]*v2[i];
  return result;
}

double MicronTracker::ACOS(double x)
{
  // REMOVE THIS METHOD...LATER ON..it is not needed
  if ( x == 1 )
    {
    return 0;
    }
  else if ( x == -1)
    {
    return PI / 2.0;
    }
  else 
    {
    return atan((double) ( -x / sqrt(-x * x + 1)) + 2 * atan(1.0));
    }
}

/** Print Self function */
void MicronTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Number of tools: " << m_NumberOfTools << std::endl;
}

} // end of namespace igstk
