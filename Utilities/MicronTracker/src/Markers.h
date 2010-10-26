/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*      Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*      Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*    Modified by:
*      Gregory Bootsma, Princess Margaret Hospital - Toronto - Ontario
*
*     Copyright Claron Technology 2000-2003
*
***************************************************************/
#ifndef __MARKERS_H__
#define __MARKERS_H__

#include "MTC.h"
#include "UtilityFunctions.h"
#include "Collection.h"
#include "Marker.h"
#include "MCamera.h"
#include <string.h>

class Markers
{
public:
  Markers();
  ~Markers();
  inline int getHandle(){ return m_handle; };


  int addTemplate(int markerHandle);
  int clearTemplates();
  int identifiedMarkers(MCamera *cam);
  int unidentifiedVectors(MCamera *cam);

  void setPredictiveFramesInterleave(int level);
  int getPredictiveFramesInterleave();
  void setTemplateMatchToleranceMM(double newval);
  double getTemplateMatchToleranceMM();
  void setExtrapolatedFrames(int newval);
  int getExtrapolatedFrames();
  void setSmallerXPFootprint(bool newval);
  bool getSmallerXPFootprint();

  int processFrame(MCamera *cam);
  int getTemplateCount();
  int getTemplateItem(int idx);
  mtCompletionCode getTemplateItemName(int idx, std::string &templateName);
  mtCompletionCode getTemplateItemHName(int handle, std::string &templateName);
  int setTemplateItemName(int idx, char* name);
  int storeTemplate( int idx, int pHandle, char* nameInP);
  int restoreTemplate(int pHandle, char* nameInP);
  void setAutoAdjustCam2CamRegistration(bool newVal);
  bool getAutoAdjustCam2CamRegistration();
  

private:
  int m_handle;
  bool ownedByMe;
  char tempString[400];

};

#endif
