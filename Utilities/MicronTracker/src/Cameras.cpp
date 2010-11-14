/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*
*     Written by:
*      Shi Sherebrin , Robarts Research Institute - London- Ontario , www.robarts.ca
*      Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*      Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto- Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2003
*
***************************************************************/

#include "string.h"
#include "Cameras.h"
#include "MCamera.h"
#include "UtilityFunctions.h"
#include "MTC.h"
#include "MTVideo.h"
#include <stdio.h>

/****************************/
/** Constructor */
Cameras::Cameras()
{
  this->ownedByMe = TRUE;
  this->mCurrCam = NULL;
  this->mFailedCam = NULL;
  Cameras_HistogramEqualizeImagesSet(true);
// error handling here
}

/****************************/
/** Destructor */
Cameras::~Cameras()
{
  // Clear all previously connected camera
  vector<MCamera *>::iterator camsIterator;
  for (camsIterator = m_vCameras.begin(); camsIterator != m_vCameras.end();
         camsIterator++)
    {
    free (*camsIterator);
  }
  if (mCurrCam != NULL) free(mCurrCam);
  if (mFailedCam != NULL) free(mFailedCam);


}

/****************************/
/** */
bool Cameras::getHistogramEqualizeImages()
{
  bool R;
  Cameras_HistogramEqualizeImagesGet(&R);
  return R;
}

/****************************/
int Cameras::setHistogramEqualizeImages(bool on_off)
{
  int result = Cameras_HistogramEqualizeImagesSet(on_off);
  return result == mtOK ? result : -1;
}

/****************************/
/** Returns the camera with the index of /param index. */
MCamera* Cameras::getCamera(int index)
{
  return this->m_vCameras[index];
}

bool Cameras::grabFrame(MCamera *cam)
{
  bool r = true;
  if (NULL == cam)
    {
    // grab from all cameras
    vector<MCamera *>::iterator camsIterator;
    for (camsIterator = m_vCameras.begin(); camsIterator != m_vCameras.end();
       camsIterator++)
    {
      if (false == (*camsIterator)->grabFrame())
            {
        mFailedCam = *camsIterator;
        r = false;
        break;
      }
    }
  }
    else
    {
    if (false == cam->grabFrame())
        {
      mFailedCam = cam;
      r = false;
    }
  }
  return r;
}


void Cameras::Detach()
{
  MTexit();
}


int Cameras::getMTHome (  char *sMTHome, int size )
    {
#ifdef _WIN32
    LONG err;
    HKEY key;
    char *mfile = "MTHome";
    DWORD value_type;
    DWORD value_size = size;

    /* Check registry key to determine log file name: */
    if ( (err = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", 0,
      KEY_QUERY_VALUE, &key)) != ERROR_SUCCESS ) {
    return(-1);
  }

    if ( RegQueryValueEx( key,
      mfile,
      0,  /* reserved */
      &value_type,
      (unsigned char*)sMTHome,
      &value_size ) != ERROR_SUCCESS || value_size <= 1 ){
    /* size always >1 if exists ('\0' terminator) ? */
    return(-1);
  }
#else
  char *localNamePtr = getenv("MTHome");
  if ( localNamePtr) {
    strncpy(sMTHome, localNamePtr, size-1);
    sMTHome[size] = '\0';
  } else {
    //sprintf(sMTHome,"/Developer/MicronTracker");
    return(-1);
  }


#endif
    return(0);
    }

 void Cameras::SetCameraCalibrationFilesDirectory( std::string directory )
 {
   this->CalibrationDir = directory;
 }


int Cameras::AttachAvailableCameras()
{
//  char calibrationDir[512];
  int result = 0;

  char * calibrationDir = const_cast< char * >(this->CalibrationDir.c_str());
// This is commented out, using SetCameraCalibrationFilesDirectory() to set calibration file directory

/*       if ( getMTHome (calibrationDir, sizeof(calibrationDir)) < 0 ) {
    // No Environment
    return result;
  } else {
    sprintf(calibrationDir,"%s/CalibrationFiles",calibrationDir);
  }
#if 0
  // Clear all previously connected camera
  vector<MCamera *>::iterator camsIterator;
  for (camsIterator = m_vCameras.begin(); camsIterator != m_vCameras.end();
         camsIterator++)
    {
    free (*camsIterator);
  }
  if (mCurrCam != NULL) free(mCurrCam);
  if (mFailedCam != NULL) free(mFailedCam);
#endif */

  result = Cameras_AttachAvailableCameras( calibrationDir);

  if ( result != mtOK) return result;
  // Number of the attached cameras
  this->m_attachedCamNums = Cameras_Count();

  if (this->m_attachedCamNums <=0) return -1;

  int camHandle;
  // Populate the array of camera that are already attached
  for (int c=0; c < this->m_attachedCamNums; c++)
  {
    if ( c > MaxCameras) break;
    Cameras_ItemGet( c , &camHandle);
    m_vCameras.push_back( new MCamera(camHandle));
  }

  return result;
}

