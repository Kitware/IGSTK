/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*      Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*      Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2003
*
***************************************************************/
#include "Markers.h"
#include "Marker.h"
#include "MCamera.h"

/****************************/
/** Constructor */
Markers::Markers()
{
  //this->m_handle = Markers_New();
  this->ownedByMe = TRUE;
}

/****************************/
/** Destructor */
Markers::~Markers()
{
  //if(this->m_handle != 0 && this->ownedByMe)
    //Markers_Free(this->m_handle);
}

#if 0 // Claudio
/****************************/
/** Restore a Marker template and add it to the know Markers. If successful returns 0. Otherwise non-zero value is returned. */
int Markers::restoreTemplate( int pHandle, char* nameInP)
{
  int result;
  result = Markers_RestoreTemplate(this->m_handle, pHandle, nameInP);
  return result;
}
#endif

/****************************/
/** Store a Marker template If successful returns 0. Otherwise non-zero value is returned. */
int Markers::storeTemplate( int idx, int pHandle, char* nameInP)
{
  int result, tHandle;
  result = Markers_TemplateItemGet(idx, &tHandle);
  if (result != mtOK) return result;
  result = Marker_StoreTemplate(tHandle, pHandle, nameInP);
  return result;
}


/****************************/
/** Add a template to the markers. If successful returns 0. Otherwise non-zero value is returned. */
int Markers::addTemplate(int markerHandle)
{
  int result;
  result = Markers_AddTemplate( markerHandle);
  return result;
}

/****************************/
/** Clears the templates of this marker. Returns 0 if successful. Otherwise a non_zero value is returned. */
int Markers::clearTemplates()
{
  int result;
  result = Markers_ClearTemplates();
  return result;
}


/****************************/
/** Return the handle to a collection of identified markers by the most recent processed frame. ?? */
int Markers::identifiedMarkers(MCamera *cam)
{
  int identifiedHandle = Collection_New();
  int camHandle;
  if (cam == NULL) {
    camHandle = NULL;
  } else {
    camHandle = cam->Handle();
  }
  Markers_IdentifiedMarkersGet(camHandle, identifiedHandle );
  return identifiedHandle;
}

/****************************/
/** Return the handle to a collection of unidentified vestors by the most recent processed frame. */
int Markers::unidentifiedVectors(MCamera *cam)
{
  int unidentifiedHandle = Collection_New();
  int camHandle;
  if (cam == NULL) {
    camHandle = NULL;
  } else {
    camHandle = cam->Handle();
  }
  Markers_UnidentifiedVectorsGet(camHandle, unidentifiedHandle);
  return unidentifiedHandle;
}

/****************************/
/** Reutrn the value for the predictive frames interleave */
int Markers::getPredictiveFramesInterleave()
{
  int level ;
  Markers_PredictiveFramesInterleaveGet( &level); // Claudio
  return level;
}

/****************************/
/** Set the value of the predictive frames interleave. */
void Markers::setPredictiveFramesInterleave(int level)
{
  Markers_PredictiveFramesInterleaveSet( level); // Claudio
}

/****************************/
/** Return the value for the extrapolated frames */
int Markers::getExtrapolatedFrames()
{
  int result;
  Markers_ExtrapolatedFramesGet(&result);
  return result;
}

/****************************/
/** Set the value of the extrapolated frames */
void Markers::setExtrapolatedFrames(int newval)
{
  Markers_ExtrapolatedFramesSet(newval);
}

/****************************/
/** Return the value for the smaller XP footprint flag */
bool Markers::getSmallerXPFootprint()
{
  bool result;
  result = Markers_SmallerXPFootprint();
  return result;
}

/****************************/
/** Set the value of the smaller XP footprint flag */
void Markers::setSmallerXPFootprint(bool newval)
{
  Markers_SmallerXPFootprintSet(newval);
}

/****************************/
/** Return the value of the templateMatchToleranceMM */
double Markers::getTemplateMatchToleranceMM()
{
  double toleranceVal = 0;
  Markers_TemplateMatchToleranceMMGet( &toleranceVal);
  return toleranceVal;
}

/****************************/
/** Set the value of the templateMatchToleranceMM */
void Markers::setTemplateMatchToleranceMM(double newVal)
{
  Markers_TemplateMatchToleranceMMSet( newVal);
}

/****************************/
/** Returns the number of templates. */
int Markers::getTemplateCount()
{
  return Markers_TemplatesCount();
}


/****************************/

void Markers::setAutoAdjustCam2CamRegistration( bool newVal)
{
  Markers_AutoAdjustCam2CamRegistrationSet( newVal);
}

/****************************/
/**  */
bool Markers::getAutoAdjustCam2CamRegistration()
{
  bool val ;
  Markers_AutoAdjustCam2CamRegistrationGet( &val);
  return val;
}


/****************************/
/** Returns the handle to the template item with the index number of idx . */
int Markers::getTemplateItem(int idx)
{
  int result = 0;
  Markers_TemplateItemGet( idx, &result);
  return result;
}

/****************************/
/** Returns the name of the template item with the index number of idx. */
mtCompletionCode Markers::getTemplateItemName(int idx, std::string &templateName)
{
  int markerHandle = this->getTemplateItem(idx);
  
  memset((void *)tempString, 0 , sizeof(tempString));
  int b;
  mtCompletionCode status = Marker_NameGet( markerHandle, tempString, sizeof(tempString), &b);
  if ( status == mtOK) {
    tempString[b] = '\0';
  }
  templateName = std::string(tempString);

  return status;
}

/****************************/
/** Returns the name of the template item with the handle of handle. */
mtCompletionCode Markers::getTemplateItemHName(int handle, std::string &templateName)
{
  memset((void *)tempString, 0 , sizeof(tempString));
  int b;
  mtCompletionCode status = Marker_NameGet( handle, tempString, sizeof(tempString), &b);
  if ( status == mtOK) {
    tempString[b] = '\0';
  }
  templateName = std::string(tempString);
  return status;
}

/****************************/
/** */
int Markers::setTemplateItemName(int idx, char* name)
{
  int markerHandle = this->getTemplateItem(idx);
  return Marker_NameSet( markerHandle, name); 
}

/****************************/
/** Processes the current frame of the default camera. */
int Markers::processFrame(MCamera *cam)
{
  int camHandle;
  if (cam == NULL) {
    camHandle = NULL;
  } else {
    camHandle = cam->Handle();
  }

  return Markers_ProcessFrame( camHandle);
}
