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
#include "Marker.h"

#include <string>

/****************************/
/** Constructor */
MTMarker::Marker::Marker(int h)
{
  // If a handle is provided to this class, don't create a new one
  if (h != 0)
    this->m_handle = h;
  else
    this->m_handle = Marker_New();
  this->ownedByMe = TRUE;
}

/****************************/
/** Destructor */
MTMarker::Marker::~Marker()
{
  if (this->m_handle != 0 && this->ownedByMe)
    Marker_Free(this->m_handle);
}

/****************************/
/** Return a handle to the Collection of the identified facets */
int MTMarker::Marker::identifiedFacets(MCamera *cam)
{
  int camHandle;
  if (cam == NULL) {
    camHandle = NULL;
  } else {
    camHandle = cam->Handle();
  }
  int identifiedHandle = Collection_New();
  Marker_IdentifiedFacetsGet(this->m_handle, camHandle, true, identifiedHandle);
  return identifiedHandle;
}


/****************************/
/** Return a handle to the Collection of the facets */
int MTMarker::Marker::getTemplateFacets()
{
  int templateFacetsColl = 0;
  Marker_TemplateFacetsGet(this->m_handle, &templateFacetsColl);
  return templateFacetsColl;
}
/****************************/
/** */
bool MTMarker::Marker::wasIdentified(MCamera *cam)
{
  int camHandle;
  bool result;
  if (cam == NULL) {
    camHandle = NULL;
  } else {
    camHandle = cam->Handle();
  }
  
  Marker_WasIdentifiedGet(this->m_handle, camHandle, &result);
  return result;
}

/****************************/
/** */
Xform3D* MTMarker::Marker::marker2CameraXf(int camHandle)
{
  Xform3D* xf = new Xform3D;
  int identifyingCamHandle;
  int result = Marker_Marker2CameraXfGet(this->m_handle, camHandle, xf->getHandle(), &identifyingCamHandle);

  // if the result is ok then return the handle, otherwise return NULL
  if (result == mtOK)
    return xf;
  else
    return NULL;
}

/****************************/
/** */
Xform3D* MTMarker::Marker::tooltip2MarkerXf()
{
  Xform3D* xf = new Xform3D;
  int result = Marker_Tooltip2MarkerXfGet(this->m_handle, xf->getHandle());

  // if the result is ok then return the handle, otherwise return NULL
  if (result == mtOK)
    return xf;
  else
    return NULL;
}

/****************************/
/** Returns the name of the marker. */
char* MTMarker::Marker::getName()
{
  int size;
  Marker_NameGet(this->m_handle, m_MarkerName, sizeof(m_MarkerName), &size);
  m_MarkerName[size] = '\0';
  return m_MarkerName;
}

/****************************/
/** Sets the name of the marker */
void MTMarker::Marker::setName(char* name)
{
  Marker_NameSet(this->m_handle, name);
}

/****************************/
/** */
int MTMarker::Marker::addTemplateFacet(Facet* newFacet, Xform3D* facet1ToNewFacetXf)
{
  int result = Marker_AddTemplateFacet(this->m_handle, newFacet->getHandle(), facet1ToNewFacetXf->getHandle() );
  return result;
}

/****************************/
/** */
bool MTMarker::Marker::validateTemplate(double positionToleranceMM, string complString)
{
  Collection* facetsColl = new Collection(this->getTemplateFacets());
  
  for(int k=1; k < facetsColl->count(); k++)
  {
    Facet* f = new Facet(facetsColl->itemI(k));
    if (!f->validateTemplate(positionToleranceMM, complString))
      return false;
  }

  vector<Vector*> vs;
  for (int fi=1; fi < facetsColl->count()-1; fi++)
  {
    Facet* Fti = new Facet(fi);
    for(int fj= fi+1; fj < facetsColl->count(); fj++)
    {
      Facet* Ftj = new Facet(fj);
      vs = Ftj->TemplateVectors();
      if (Fti->identify(NULL, vs, 2*positionToleranceMM))
        return false; //mtDifferentFacetsGeometryTooSimilar
    }
  }
/*
  'Check that two facets cannot be confused with each other, ie
  'their positions are at least twice the tolerance
  Dim fi, fj, Fti As Facet, Ftj As Facet, Vs(2) As Vector
  For fi = 1 To TemplateFacets.Count - 1
    Set Fti = TemplateFacets(fi)
    For fj = fi + 1 To TemplateFacets.Count
      Set Ftj = TemplateFacets(fj)
      Set Vs(0) = Ftj.TemplateVectors(1)
      Set Vs(1) = Ftj.TemplateVectors(2)
      If Fti.Identify(Nothing, Vs, 2, 2 * PositionToleranceMM) Then
        ValidateTemplate = False 'mtDifferentFacetsGeometryTooSimilar
        If Not IsMissing(OutCompletionString) Then
          OutCompletionString = "geometry of facets " & fi & " and " & fj & " too similar"
        End If
        Exit Function
      End If
    Next
  Next
  ValidateTemplate = True
End Function
  */
  return true;
}

/****************************/
int MTMarker::Marker::storeTemplate(Persistence* p, const char* name)
{
  int result = Marker_StoreTemplate(this->m_handle, p->getHandle(), name);
  return result;
}

#if 0
/****************************/
/** Set the JitterFilteringFracetion*/
void MTMarker::Marker::setJitterFilteringFraction(double newVal)
{
  Marker_JitterFilteringFractionSet(this->m_handle, newVal);
}

/****************************/
/** Return the JitterFilteringFraction. */
double MTMarker::Marker::getJitterFilteringFraction()
{
  double jitterFilteringFraction = 0;
  Marker_JitterFilteringFractionGet(this->m_handle, &jitterFilteringFraction);
  return jitterFilteringFraction;
}

#endif
