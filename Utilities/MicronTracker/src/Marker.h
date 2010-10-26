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
#ifndef __MARKER_H__
#define __MARKER_H__

#include "MTC.h"
//#include <cstdlib>
#include "Collection.h"
#include "MCamera.h"
#include "Facet.h"
#include "Xform3D.h"
#include "Persistence.h"
using namespace MTCollection;


namespace MTMarker
{
class Marker
{
public:
  Marker(int h=0);
  ~Marker();
  inline int getHandle(){ return m_handle; };

  int getTemplateFacets();
  int restoreTemplate(int persistenceHandle, const char* name);
  
  char* getName();
  void setName(char* name);
  bool wasIdentified(MCamera *cam);
  Xform3D* marker2CameraXf(int camHandle);
  Xform3D* tooltip2MarkerXf();
  int identifiedFacets (MCamera *cam);
  // void setJitterFilteringFraction(double newVal); // Claudio
   // double getJitterFilteringFraction();
  int addTemplateFacet(Facet* newFacet, Xform3D* facet1ToNewFacetXf);
  bool validateTemplate(double positionToleranceMM, string complString);
  int storeTemplate(Persistence* p, const char* name);

private:
  int m_handle;
  bool ownedByMe;
  char m_MarkerName[400];

};
};

#endif
