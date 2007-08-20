/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMicronTrackerCommandInterpreter.cxx
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
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkMicronTrackerCommandInterpreter.h"

#define MT_OK 0

namespace igstk
{

/** Constructor (initializes MicronTrackerCommandInterpreter values) */
MicronTrackerCommandInterpreter::MicronTrackerCommandInterpreter()
{
}

/** Destructor */
MicronTrackerCommandInterpreter::~MicronTrackerCommandInterpreter()
{
}

/** Print Self function */
void MicronTrackerCommandInterpreter::PrintSelf( std::ostream& os, 
                                                 itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

//------------------------------------------
int MicronTrackerCommandInterpreter::Init()
{
  m_ErrorString = "No error!";

  m_Markers = new Markers();
  m_Persistance = new Persistence();
  this->InitializeInitFileAccess();
  m_TempMarkerForAddingFacet = 0;
  
  return 1;
}

//------------------------------------------
void MicronTrackerCommandInterpreter::End()
{
  this->DetachCameras();

}

//------------------------------------------
const char *MicronTrackerCommandInterpreter::GetMicronTrackerDirectory()
{
#ifdef _WIN32

  // Temporary storage for the directory
  const DWORD size = 1024;
  char value[size];

  // Get the registry value associated with the "MTHome" key
  LONG errorValue = ERROR_SUCCESS;
  HKEY key;
  const char *mfile = "MTHome";
  DWORD valueType;
  DWORD valueSize = size;

  /* Check registry key to determine log file name: */
  errorValue = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                            "SYSTEM\\CurrentControlSet\\"
                            "Control\\Session Manager\\Environment",
                            0,
                            KEY_QUERY_VALUE,
                            &key);

  if (errorValue != ERROR_SUCCESS)
    {
    // report an error here
    return ".";
    }
  else
    {
    errorValue = RegQueryValueEx(key,
                                 mfile,
                                 0,        // reserved
                                 &valueType,
                                 (unsigned char*)value,
                                 &valueSize);

    if (errorValue != ERROR_SUCCESS || valueSize <= 1 || valueSize > size)
      {
      return ".";
      }
    }
#else

  // On UNIX and Linux, check for environment variable
  char *directoryName = getenv("MTHome");
  if (directoryName)
    {
    m_MicronTrackerDirectory = directoryName;
    }
  else
    {
    // report an error here
    return ".";
    }

#endif

  return m_MicronTrackerDirectory.c_str();
}

//------------------------------------------
void MicronTrackerCommandInterpreter::InitialInitFileAccess()
{
  std::string micronTrackerDirectory = this->GetMicronTrackerDirectory();
  std::string initFileName = micronTrackerDirectory;
#ifdef WIN32
  initFileName += "\\MicronTracker Demo.ini";
#else
  initFileName += "/MicronTracker Demo.ini";
#endif

  m_Persistence->setPath(micronTrackerDirectory);
  m_Persistence->setSection("General");
  
  // Setting the FrameInterleave property in the Markers object
  int defaultFrameInterleave = 0;
  m_Markers->setPredictiveFramesInterleave(
    m_Persistence->retrieveInt("PredictiveFramesInterleave",
                               defaultFrameInterleave) );
  
  //Setting the TemplateMatchToleranceMM property in the Markers object
  double defaultTempMatchToleranceMM = 1.0;
  m_Markers->setTemplateMatchToleranceMM(
    m_Persistence->retrieveDouble("TemplateMatchToleranceMM",
                                  defaultTempMatchToleranceMM) );
}

//------------------------------------------
void MicronTrackerCommandInterpreter::UpdateInitFile()
{
  m_Persistence->saveDouble( "TemplateMatchToleranceMM",
                             m_Markers->getTemplateMatchToleranceMM() );
  m_Persistence->saveInt( "PredictiveFramesInterleave",
                          m_Markers->getPredictiveFramesInterleave() );
}

//------------------------------------------
int MicronTrackerCommandInterpreter::SetupCameras()
{
  



  m_Cameras = new Cameras();
  int result = m_Cameras->AttachAvailableCameras();
  
  if (result == MT_OK &&  m_Cameras->getCount() >= 1 )
    {
      // Set the current camera to the first in the list. 
      m_CurrentCameraIndex = 0;
      m_CurrentCamera = m_Cameras->getCamera(m_CurrentCameraIndex);
      m_CameraIsAttached = true;
      
      return 1;
    }
  else
    {
      m_ErrorString = this->HandleErrors(result);
      return 0;
    }
}

//------------------------------------------
void MicronTrackerCommandInterpreter::DetachCameras()
{
  if ( m_CameraIsAttached )
    {
    m_CameraIsAttached = false;
    MTexit();     
    }
}

//------------------------------------------
// Checks to see if the passed argument is within the camera index,
// i.e. it is not less than 0 or more than the number of attached
// camera (minus 1, of course). 

bool MicronTrackerCommandInterpreter::CheckCameraIndex(int id)
{
  if (id > -1 && id < m_Cameras->getCount())
    {
    return true;
    }
  else
    {
    m_ErrorString = "Camera index out of bound.";
    return false;
    }
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetSerialNumber(int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->getSerialNum();
    }
  
  else if (!this->CheckCameraIndex(index))
    {
    return -1;
    }
  else
    {
    return m_Cameras->getCamera(index)->getSerialNum();
    }
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetXResolution(int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->getXRes();
    }
  else if (!this->CheckCameraIndex(index))
    {
    return -1;
    }
  else
    {
    return m_Cameras->getCamera(index)->getXRes();
    }
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetYResolution(int index)
{
  if (index == -1)
    {
      return m_CurrentCamera->getYRes();
    }
  else if (!this->CheckCameraIndex(index))
    {
      return -1;
    }
  else
    {
      return m_Cameras->getCamera(index)->getYRes();
    }
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetNumberOfCameras()
{
  return m_Cameras->getCount();
}
//------------------------------------------
int MicronTrackerCommandInterpreter::RefreshTemplates(vector<string> &tmplsName,
                                               vector<string> &tmplsError)
{
  mtUtils::getFileNamesFromDirectory( tmplsName,"Markers", true);
  int result = m_Markers->clearTemplates();
  std::cout << "Cleared templates..." << std::endl;
  char currentFolderPath[MT_MAX_STRING_LENGTH];
  mtCompletionCode st;
  mtUtils::getCurrPath(currentFolderPath);
  
#ifdef WIN32
  strcat(currentFolderPath, "\\Markers\\");
#else
  strcat(currentFolderPath, "/Markers/");
#endif
  if (0 == result)
    {
      st = Markers_LoadTemplates(currentFolderPath);
    }
  return (int)st;
}
//------------------------------------------
int MicronTrackerCommandInterpreter::GrabFrame(int index)
{
  int result = -1;
  if (index == -1) // From all the cameras
    {
      result = (true == m_Cameras->grabFrame()) ? 0 : -1;
    }
  else if (false == this->CheckCameraIndex(index)) // Camera index out of range
    {
      result = -1;
    }
  else // From one camera
    {
      result = (true == m_Cameras->grabFrame(m_Cameras->getCamera(index))) ? 0 : -1;
    }

  return result;
}

//------------------------------------------
int MicronTrackerCommandInterpreter::ProcessFrame()
{
  int result = m_Markers->processFrame(m_CurrentCamera);
  if (result != MT_OK)
    {
      result = -1;
      m_ErrorString = this->HandleErrors(result);
    }
  return result;
}

//------------------------------------------
int MicronTrackerCommandInterpreter::CollectNewSamples(int collectingAdditionalFacet)
{
  int result = 0;
  Collection* col = new Collection( m_Markers->unidentifiedVectors(m_CurrentCamera) );
  m_isAddingAdditionalFacet = collectingAdditionalFacet;
  if (col->count() == 2 || collectingAdditionalFacet == 1)
    {
      m_sampleVectors.push_back(col);
      result = 0;
      //                m_collectedSampleFrames++;
      // Additional facets
      if (m_isAddingAdditionalFacet)
        {
          Collection* markersCollection = new Collection(m_Markers->identifiedMarkers(m_CurrentCamera));
          m_TempMarkerForAddingFacet = new Marker(markersCollection->itemI(1));
          Collection* identifiedFacetsCol = new Collection(m_TempMarkerForAddingFacet->getTemplateFacets());
          if (identifiedFacetsCol->count() > 0)
            {        
              // Also compute and save the xform of facet1 to sensor
              // Note (Programming Note): The following if block has been commented out because it
              // relates to registering a marker with two cameras. As soon as the second camera is 
              // in, we can start working on it.
              //if (m_CurrMarker->marker2CameraXf(m_CurrentCamera->getHandle() != (Xform3D*)NULL)
              this->facet1ToCameraXfs.push_back(m_TempMarkerForAddingFacet->marker2CameraXf(m_CurrentCamera->getHandle()));
              
              //else // Seen by a camera not registered with this one
              //{
              //        this->collectingStatus->label("Cannot sample: unregistered camera");
              //        this->sampleVectors.pop_back();
              //        m_collectedSampleFrames--;
              //}
              return result;
            }
          

          else // Not seeing a known facet
            {
              //this->collectingStatus->label("Cannot sample: no known facet!");
              m_sampleVectors.pop_back();
              delete identifiedFacetsCol;
              return 99;                                
            }
          delete identifiedFacetsCol;
          //delete markersCollection;
        }
      
      //                char buffer[255];
      //                sprintf(buffer, "% d", m_collectedSampleFrames);
      //                //strcat("Collected: ", buffer);
      //                this->collectingNum->label(buffer);
    }
  else if(col->count() < 2) 
    {
      result = -1;
    }
  else if(col->count() > 2)
    {
      result = 1;
    }
  
  //        delete col;
  return result;
}

//------------------------------------------
int MicronTrackerCommandInterpreter::StopSampling(char* templateName, double jitterValue)
{        
  string errorsString;
  Facet* f = new Facet();
  vector<Vector*> vectorPair;
  int result = 0;
  if (f->setVectorsFromSample(m_sampleVectors, errorsString))
    {
      if (m_isAddingAdditionalFacet)
        {
          Collection* markersCollection = new Collection(m_Markers->identifiedMarkers(m_CurrentCamera));
          //m_TempMarkerForAddingFacet->setHandle(markersCollection->itemI(1));
          Collection* identifiedFacetsCol = new Collection(m_TempMarkerForAddingFacet->getTemplateFacets());
          vector<Xform3D*> facet1ToNewFacetXfs;
          Xform3D* facet1ToNewFacetXf;// = new Xform3D();
          
          for (int i=0; i<m_sampleVectors.size(); i++)
            {
              vectorPair.clear();
              vectorPair.push_back(new Vector( m_sampleVectors[i]->itemI(0) ));
              vectorPair.push_back(new Vector( m_sampleVectors[i]->itemI(1) ));
              //Programming Note: The following line is crashing in the Facet.cpp when
              //it makes a call to the DLL. As a result no registration of multi-facet markers can be made at this point. June 7, 2004
              if (f->identify(m_CurrentCamera, vectorPair, 1)) // Then the sample matches the template
                {
                  // Compute the xform between the first marker facet and the new one
                  facet1ToNewFacetXf = this->facet1ToCameraXfs[i]->concatenate(f->getFacet2CameraXf(m_CurrentCamera)->inverse());
                  facet1ToNewFacetXfs.push_back(facet1ToNewFacetXf);
                }
   
            }
          // Combine the transforms accumulated to a new one and save it with the facet in the marker
          facet1ToNewFacetXf = facet1ToNewFacetXfs[1];
          for (i=2; i<facet1ToNewFacetXfs.size(); i++)
            {
              facet1ToNewFacetXf->inBetween(facet1ToNewFacetXfs[i], 1);// will result in equal contribution by all faces
            }

                        m_TempMarkerForAddingFacet->addTemplateFacet(f, facet1ToNewFacetXf);


        }
      else
        {
          m_CurrMarker = new Marker();
          Xform3D* Xf = new Xform3D();
          m_CurrMarker->setName(templateName);
          result = m_CurrMarker->addTemplateFacet(f, Xf);
          if(result != MT_OK)
            {
              m_ErrorString = "Error occured creating the new marker!";
              result = -1;
            }
        }
    }
  else
    {
      m_ErrorString = "Error occured creating the new marker!";
      result = -1;
    }
  return result;
}

//------------------------------------------
int MicronTrackerCommandInterpreter::SaveMarkerTemplate(char* templName)
{
  char currDir[255];
  mtUtils::getCurrPath(currDir);
#if(WIN32)
  strcat(currDir, "\\Markers\\");
#else
  strcat(currDir, "/Markers/");
#endif
  strcat(currDir, templName);
  Persistence* newPersistence = new Persistence();
  newPersistence->setPath(currDir);
  int storeResult = m_CurrMarker->storeTemplate(newPersistence, "");//(char*)(name.c_str()) );
  if (storeResult != MT_OK)
    {
      m_ErrorString = "Error in saving the current template";
      return -1;
    }
  else
    {
      mtResetSamples();
      return 0;
    }
}

//------------------------------------------
void MicronTrackerCommandInterpreter::ResetSamples()
{
  for (int i=0; i<m_sampleVectors.size(); i++)
    {
      delete m_sampleVectors[i];
    }
  m_sampleVectors.clear();
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetNumberOfLoadedTemplates()
{
  return m_Markers->getTemplateCount();
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetNumberOfIdentifiedMarkers()
{
  return m_NumberOfIdentifiedMarkers;
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetNumberOfUnidentifiedMarkers()
{
  return m_NumberOfUnidentifiedMarkers;
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetNumberOfFacetsInMarker(int markerIndex)
{
  return m_NumberOfFacetsInEachMarker[markerIndex];
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetNumberOfTotalFacetsInMarker(int markerIndex)
{
  return m_NumberOfTotalFacetsInEachMarker[markerIndex];
}

//------------------------------------------
void MicronTrackerCommandInterpreter::FindIdentifiedMarkers()
{
  Collection* markersCollection = new Collection(m_Markers->identifiedMarkers(m_CurrentCamera));
  m_NumberOfIdentifiedMarkers = markersCollection->count();
  if (m_NumberOfIdentifiedMarkers == 0)
    {
      m_markerStatus = MTI_NO_MARKER_CAPTURED;
      delete markersCollection; 
      return;
    }
  m_Rotations.clear();
  m_Translations.clear();
  m_IdentifiedMarkersXPoints.clear();
  for (int i=0;i<m_IdentifiedMarkersName.size();i++)
    {
      m_IdentifiedMarkersName[i].erase();
    }
  
  m_IdentifiedMarkersName.resize(0);
  m_IdentifiedMarkersName.clear();
  m_NumberOfFacetsInEachMarker.clear();
  m_NumberOfTotalFacetsInEachMarker.clear();
  
  m_markerStatus = MTI_MARKER_CAPTURED;
  int markerNum = 1;
  int facetNum = 1;
  
  for (markerNum = 1; markerNum <= markersCollection->count(); markerNum++)
    {
      //m_CurrTempMarker->setHandle(markersCollection->itemI(markerNum));
      Marker* marker = new Marker (markersCollection->itemI(markerNum));
      //Collection* totalFacetsCollection = new Collection(marker->getTemplateFacets());
      Collection* totalFacetsCollection = new Collection(marker->getTemplateFacets());
      m_IdentifiedMarkersName.push_back(marker->getName());
      
      if (marker->wasIdentified(m_CurrentCamera) != 0)
        {
          Collection* facetsCollection = new Collection(marker->identifiedFacets(m_CurrentCamera));
          
          // Adjust the colour temperature if we see a CoolCard marker
          if ((0 == strncmp(marker->getName(), "COOL", 4)) ||  
              (0 == strncmp(marker->getName(), "cool", 4)) ||
              (0 == strncmp(marker->getName(), "Cool", 4)) )
            {
              Facet* f = new Facet(facetsCollection->itemI(1));
              Vector* ColorVector = (f->IdentifiedVectors())[0];
              m_CurrentCamera->AdjustCoolnessFromColorVector(ColorVector->Handle());
              delete f;
            }
          vXPointsTemp.clear();
          for (facetNum = 1; facetNum <= facetsCollection->count(); facetNum++)
            {
              Facet* f = new Facet(facetsCollection->itemI(facetNum));
              // get Xpoints and then draw on each image if enabled        
              f->getXpoints(m_CurrentCamera, (double *)LS_LR_BH_XY);
              
              // Have to push_back the elements one by one. If pass the address of the whole
              // array, the points will be deleted if the facet is deleted.
                    
              // Left, Long vector
              vXPointsTemp.push_back(LS_LR_BH_XY[0][0][0][0]);
              vXPointsTemp.push_back(LS_LR_BH_XY[0][0][0][1]);
              vXPointsTemp.push_back(LS_LR_BH_XY[0][0][1][0]);
              vXPointsTemp.push_back(LS_LR_BH_XY[0][0][1][1]);
                            
              // Right, Long vector
              vXPointsTemp.push_back(LS_LR_BH_XY[0][1][0][0]);
              vXPointsTemp.push_back(LS_LR_BH_XY[0][1][0][1]);
              vXPointsTemp.push_back(LS_LR_BH_XY[0][1][1][0]);
              vXPointsTemp.push_back(LS_LR_BH_XY[0][1][1][1]);
              
              // Left, short vector
              vXPointsTemp.push_back(LS_LR_BH_XY[1][0][0][0]);
              vXPointsTemp.push_back(LS_LR_BH_XY[1][0][0][1]);
              vXPointsTemp.push_back(LS_LR_BH_XY[1][0][1][0]);
              vXPointsTemp.push_back(LS_LR_BH_XY[1][0][1][1]);

              // Right, Short vector
              vXPointsTemp.push_back(LS_LR_BH_XY[1][1][0][0]);
              vXPointsTemp.push_back(LS_LR_BH_XY[1][1][0][1]);
              vXPointsTemp.push_back(LS_LR_BH_XY[1][1][1][0]);
              vXPointsTemp.push_back(LS_LR_BH_XY[1][1][1][1]);
              
              delete f;
              
            } // End of the for loop for the facets.
          m_IdentifiedMarkersXPoints.push_back( vXPointsTemp );
          m_NumberOfFacetsInEachMarker.push_back(facetsCollection->count());
          m_NumberOfTotalFacetsInEachMarker.push_back(totalFacetsCollection->count());
          delete facetsCollection;
          

          /***********************************/
          /*VERY IMPORTANT PROGRAMMING NOTE                                       
            Deleting the totalFacetsCollection here causes the identified facets to be deleted
            and hence not shown as identified on the GUI side. Obviously on the other hand, not deleting this
            object results in memory leak. Should be fixed soon! */
          /***********************************/
          //                        delete totalFacetsCollection;
          Xform3D* Marker2CurrentCameraeraXf = marker->marker2CameraXf(m_CurrentCamera->getHandle());
          // Find the translations and push them in a 2 temporary vector and then push that temp vector into a 
          // 2 dimensional vector.
          vector<double> vTransTemp;
          vTransTemp.clear();
          for (int i = 0 ; i < 3; i++)
            {
              vTransTemp.push_back(Marker2CurrentCameraeraXf->getShift(i));
            }
          vTransTemp.push_back(1);
          m_Translations.push_back(vTransTemp);
          // Find the rotations and push them in a 2 temporary vector and then push that temp vector into a 
          // 2 dimensional vector.
          
          double vR[3][3];
          
          vector<double> vRotTemp;
          // problem lies here !
          Xform3D_RotMatGet(Marker2CurrentCameraeraXf->getHandle(), reinterpret_cast<double *>(vR[0]));
         //Marker2CurrentCameraeraXf->getRotationMatrix(reinterpret_cast<double *>(vR[0]));
          for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                  { 
                  vRotTemp.push_back( vR[j][k] );
                    }
            }
          
          m_Rotations.push_back(vRotTemp);/**/
          delete Marker2CurrentCameraeraXf;
          
          
        } // End of if (m_CurrTempMarker->wasIdentified...)
      

    } // End of the for loop for the markers.
  delete markersCollection;
}





//------------------------------------------
void MicronTrackerCommandInterpreter::GetTranslations(vector<double> &vTranslations, int markerIndex)
{        
  vTranslations = m_Translations[markerIndex];        
}

//------------------------------------------
void MicronTrackerCommandInterpreter::GetRotations(vector<double> &vRotations, int markerIndex)
{
  vRotations = m_Rotations[markerIndex];
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetStatus()
{
  return m_markerStatus;
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetMarkerStatus(int loadedMarkerIndex, int* identifiedMarkerIndex)
{
  // Safety check. If the request marke index is greater than the identified markers,
  // return NO_MARKER_CAPTURED
  if (loadedMarkerIndex > this->GetLoadedTemplatesNum() || loadedMarkerIndex < 0)
    {
      return MTI_NO_MARKER_CAPTURED;
    }
  char* markerName = this->GetTemplateName(loadedMarkerIndex);
  for (int i=0; i< m_NumberOfIdentifiedMarkers; i++)
    {
      if (markerName == m_IdentifiedMarkersName[i])
        {
          *identifiedMarkerIndex = i;
          return MTI_MARKER_CAPTURED;
        }
    }
  return MTI_NO_MARKER_CAPTURED;
}

//------------------------------------------
int MicronTrackerCommandInterpreter::SelectCamera(int index)
{
  if (!this->CheckCameraIndex(index))
    {
      m_CurrentCamera = m_Cameras->getCamera(index);
    }
  if (m_CurrentCamera != NULL)
    {
      m_CurrentCameraIndex = index;
      return 0;
    }
  else
    {
      return -1;
    }
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetCurrentCameraIndex()
{
  return m_CurrentCameraIndex;
}

//------------------------------------------
int MicronTrackerCommandInterpreter::SetTemplMatchTolerance(double matchTolerance)
{
  m_Markers->setTemplateMatchToleranceMM(matchTolerance);
  return 1;
}

//------------------------------------------
double MicronTrackerCommandInterpreter::GetTemplMatchTolerance()
{
  return m_Markers->getTemplateMatchToleranceMM();
}

//------------------------------------------
double MicronTrackerCommandInterpreter::GetTemplMatchToleranceDefault()
{
  double defToleranceVal = 0;
  Markers_TemplateMatchToleranceMMDefaultGet(&defToleranceVal);
  return defToleranceVal;
  //return m_Markers->getDefaultTemplateMatchToleranceMM();
}

//------------------------------------------
int MicronTrackerCommandInterpreter::SetPredictiveFramesInterleave(int predictiveInterleave)
{
  m_Markers->setPredictiveFramesInterleave(predictiveInterleave);
  return 1;
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetPredictiveFramesInterleave()
{
  return m_Markers->getPredictiveFramesInterleave();
}

//------------------------------------------
int MicronTrackerCommandInterpreter::SetShutterTime(double n, int index)
{
  if (index == -1)
    {
      return m_CurrentCamera->setShutterTime(n);
    }
  else if(!this->CheckCameraIndex(index))
    {
      return -1;
    }

  else
    {
      return m_Cameras->getCamera(index)->setShutterTime(n);
    }
}

//------------------------------------------
double MicronTrackerCommandInterpreter::GetShutterTime(int index)
{
  if (index == -1)
    {
      return m_CurrentCamera->getShutterTime();
    }
  else if(!this->CheckCameraIndex(index))
    {
      return -1;
    }
  else
    {
      return m_Cameras->getCamera(index)->getShutterTime();
    }
}

//------------------------------------------
double MicronTrackerCommandInterpreter::GetMinShutterTime(int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->getMinShutterTime();
    }
  else if(!this->CheckCameraIndex(index))
    {
    return -1;
    }
  else
    {
    return m_Cameras->getCamera(index)->getMinShutterTime();
    }
}

//------------------------------------------
double MicronTrackerCommandInterpreter::GetMaxShutterTime(int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->getMaxShutterTime();
    }
  else if(!this->CheckCameraIndex(index))
    {
    return -1.0;
    }

  else
    {
    return m_Cameras->getCamera(index)->getMaxShutterTime();
    }
}

//------------------------------------------
double MicronTrackerCommandInterpreter::GetGain(int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->getGain();
    }
  else if(!this->CheckCameraIndex(index))
    {
    return -1;
    }
  else
    {
    return m_Cameras->getCamera(index)->getGain();
    }
}

//------------------------------------------
int MicronTrackerCommandInterpreter::SetGain(double n, int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->setGain(n);
    }
  else if(!this->CheckCameraIndex(index))
    {
    return -1;
    }
  else
    {
    return m_Cameras->getCamera(index)->setGain(n);
    }
}

//------------------------------------------
double MicronTrackerCommandInterpreter::GetMinGain(int index)
{
  if (index == -1)
    {
      return m_CurrentCamera->getMinGain();
    }
  else if(!this->CheckCameraIndex(index))
    {
    return -1;
    }
  else
    {
    return m_Cameras->getCamera(index)->getMinGain();
    }
}

//------------------------------------------
double MicronTrackerCommandInterpreter::GetMaxGain(int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->getMaxGain();
    }
  else if(!this->CheckCameraIndex(index))
    {
    return -1;
    }
  else
    {
    return m_Cameras->getCamera(index)->getMaxGain();
    }
}

//------------------------------------------
double MicronTrackerCommandInterpreter::GetDBGain(int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->getDBGain();
    }
  else if(!this->CheckCameraIndex(index))
    {
    return -1;
    }
  else
    {
    return m_Cameras->getCamera(index)->getDBGain();
    }
}

//------------------------------------------
double MicronTrackerCommandInterpreter::GetExposure(int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->getExposure();
    }
  else if(!this->CheckCameraIndex(index))
    {
    return -1;
    }
  else
    {
    return m_Cameras->getCamera(index)->getExposure();
    }
}

//------------------------------------------
int MicronTrackerCommandInterpreter::SetExposure(double n, int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->setExposure(n);
    }
  else if(!this->CheckCameraIndex(index))
    {
    return -1;
    }
  else
    {
    return m_Cameras->getCamera(index)->setExposure(n);
    }
}

//------------------------------------------
double MicronTrackerCommandInterpreter::GetMinExposure(int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->getMinExposure();
    }
  else if(!this->CheckCameraIndex(index))
    {
    return -1;
    }
  else
    {
    return m_Cameras->getCamera(index)->getMinExposure();
    }
}

//------------------------------------------
double MicronTrackerCommandInterpreter::GetMaxExposure(int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->getMaxExposure();
    }
  else if(!this->CheckCameraIndex(index))
    {
    return -1.0;
    }
  else
    {
    return m_Cameras->getCamera(index)->getMaxExposure();
    }
}

//------------------------------------------
double MicronTrackerCommandInterpreter::GetLightCoolness(int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->getLightCoolness();
    }
  else if(!this->CheckCameraIndex(index))
    {
    return -1;
    }
  else
    {
    return m_Cameras->getCamera(index)->getLightCoolness();
    }
}

//------------------------------------------
int MicronTrackerCommandInterpreter::SetCamAutoExposure(int n, int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->setAutoExposure(n);
    }
  else if(!this->CheckCameraIndex(index))
    {
    return -1;
    }
  else
    {
    return m_Cameras->getCamera(index)->setAutoExposure(n);
    }
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetCamAutoExposure(int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->getAutoExposure();
    }
  else
    {
    return m_Cameras->getCamera(index)->getAutoExposure();
    }
}

//------------------------------------------
double MicronTrackerCommandInterpreter::GetLatestFrameTime(int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->getFrameTime();
    }
  else if(!this->CheckCameraIndex(index))
    {
    return -1;
    }
  else
    {
    return m_Cameras->getCamera(index)->getFrameTime();
    }
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetNumberOfFramesGrabbed(int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->getNumberOfFramesGrabbed();
    }
  else if (!this->CheckCameraIndex(index))
    {
    return -1;
    }
  else
    {
    return m_Cameras->getCamera(index)->getNumberOfFramesGrabbed();
    }
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetBitsPerPixel(int index)
{
  if (index == -1)
    {
    return m_CurrentCamera->getBitsPerPixel();
    }
  else if (!this->CheckCameraIndex(index))
    {
    return -1;
    }
  else
    {
    return m_Cameras->getCamera(index)->getBitsPerPixel();
    }
  return 0;
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetLatestFramePixHistogram(long* &aPixHist, int subSampleRate, int index )
{
  return        0;//m_CurrentCameras->getLatestFramePixHistogram(aPixHist, subSampleRate, index);
}

//------------------------------------------
mtMeasurementHazardCode MicronTrackerCommandInterpreter::GetLatestFrameHazardCode()
{
  return Camera_LastFrameThermalHazard(m_CurrentCamera->getHandle());
  // return m_CurrentCamera->getHazardCode();
}

//------------------------------------------
int MicronTrackerCommandInterpreter::SetTemplateName(int index, char* templName)
{
  int result = -1;
  // Check the index
  if (index < 0 || index > m_Markers->getTemplateCount() )
    {
    m_ErrorString = "Marker index out of range!";
    }
  char* oldN = this->GetTemplateName(index);
  result = m_Markers->setTemplateItemName(index, templName);
  result = m_Markers->storeTemplate(index, m_Persistence->getHandle(), NULL );
  result = this->RenameFile(oldN, templName, "Markers");
  if (result != MT_OK)
    {
    m_ErrorString = "Changing the name of the template was not successful.";
    }
  return result;
}

//------------------------------------------
char* MicronTrackerCommandInterpreter::GetTemplateName(int index)
{
  std::string s;
  
  m_Markers->getTemplateItemName(index, s);
  return ((char*)s.c_str());
}

//------------------------------------------
char* MicronTrackerCommandInterpreter::GetIdentifiedTemplateName(int index)
{
  // Check the index
  if (index > -1 && index < m_NumberOfIdentifiedMarkers)
    {
    return (char*)m_IdentifiedMarkersName[index].c_str();
    }
  else
    {
    return "";
    }
}

//------------------------------------------
int MicronTrackerCommandInterpreter::DeleteTemplate(int index)
{        
  int result = -1;
  if (index > -1 && index < m_Markers->getTemplateCount() )
    {
    char* templateName = this->GetTemplateName(index);//m_Markers->getTemplateItemName(index);
    result = this->RemoveFile(templateName, "Markers");
    if (result != 0)
      {
      m_ErrorString = "Could not delete the template!";
      }
    }
  else
    {
    m_ErrorString = "Marker index out of range!";
    }
  return result;
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetLeftRightImageArray(unsigned char** &leftImageArray, unsigned char** &rightImageArray, int index)
{
  bool result = 0;
  if (index == -1)
    {
    result = m_CurrentCamera->getImages(&leftImageArray, &rightImageArray);
    }
  else if (!this->CheckCameraIndex(index))
    {
    result = false;
    }
  else
    {
    result = m_Cameras->getCamera(index)->getImages(&leftImageArray, &rightImageArray);
    }
                
  return result ? 0 : -1;
}

//------------------------------------------
int MicronTrackerCommandInterpreter::GetLeftRightImageArrayHalfSize(
  unsigned char** &leftImageArray, 
  unsigned char** &rightImageArray, 
  int xResolution, int yResolution, int index)
{
  bool result = 0;
  if (index == -1)
    {
    result = m_CurrentCamera->getHalfSizeImages(
      &leftImageArray, &rightImageArray, xResolution, yResolution);
    }
  else if (!this->CheckCameraIndex(index))
    {
    result = false;
    }
  else
    {
    result = m_Cameras->getCamera(index)->getHalfSizeImages(
      &rightImageArray, &leftImageArray, xResolution, yResolution);
    }
  return result ? 0 : -1;
}

//------------------------------------------
void MicronTrackerCommandInterpreter::GetIdentifiedMarkersXPoints(
  double* &xPoints, int markerIndex)
{
  //Check the marker index
  if (markerIndex >= m_NumberOfIdentifiedMarkers-1)
    {
    markerIndex = m_NumberOfIdentifiedMarkers -1;
    }
  if(markerIndex < 0)
    {
    markerIndex = 0;
    }
  xPoints = &m_IdentifiedMarkersXPoints[markerIndex][0];
}

//------------------------------------------
void MicronTrackerCommandInterpreter::GetUnidentifiedMarkersEnds(
  double* &endPoints, int vectorIndex)
{        
  //Check the vector index
  if (vectorIndex >= m_NumberOfUnidentifiedMarkers-1)
    {
    vectorIndex = m_NumberOfUnidentifiedMarkers-1;
    }
  if (vectorIndex < 0)
    {
    vectorIndex = 0;
    }
  endPoints = &m_UnidentifiedMarkersEndPoints[vectorIndex][0];
}

//------------------------------------------
int MicronTrackerCommandInterpreter::RemoveFile(string fileName, char* dir)
{
  char currentFolderPath[255];
  mtUtils::getCurrPath(currentFolderPath);
  if ( dir != NULL )
    {
#if(WIN32)
    strcat(currentFolderPath, "\\");
    strcat(currentFolderPath, dir);
    strcat(currentFolderPath, "\\");
#else
    strcat(currentFolderPath, "/");
    strcat(currentFolderPath, dir);
    strcat(currentFolderPath, "/");
#endif
    }
  if (_chdir(currentFolderPath) == 0)
    {
    int result = remove(fileName.c_str());
    // Return to the parent directory
    _chdir("..");
    if (result == 0)
      {
      return 0; // successful
      }
    else // not successful
      {
      return -1;
      }
    }
  return -1;
}

//---------------------------------------------
int MicronTrackerCommandInterpreter::RenameFile(
  string oldName, string newName, char* dir)
{
  char currentFolderPath[255];
  mtUtils::getCurrPath(currentFolderPath);
  if ( dir != NULL )
    {
#if(WIN32)
    strcat(currentFolderPath, "\\");
    strcat(currentFolderPath, dir);
    strcat(currentFolderPath, "\\");
#else
    strcat(currentFolderPath, "/");
    strcat(currentFolderPath, dir);
    strcat(currentFolderPath, "/");
#endif
    }
  if (_chdir(currentFolderPath) == 0 )
    {
    int result = rename(oldName.c_str(), newName.c_str());
      
    // Return to the parent directory
    _chdir("..");
    if (result == 0 )
      {
      return 0; // successful
      }
    else // not successful
      {
      return -1;
      }
    }
  return -1;
}
//------------------------------------------
string MicronTrackerCommandInterpreter::HandleErrors(int errorNum)
{
  // Not complete yet

  /*mtOk =0,
    mtError,
    mtInvalidHandle,
    mtNullPointer,
    mtOutOfMemory,
    mtStringParamTooLong,
    mtPathNotSet,
    mtWriteToFileFailed,
    mtInvalidIndex,
    mtInvalidSideI,
    mtInvalidDivisor,
    mtEmptyCollection,
    mtInsufficientSamples,
    mtOddNumberOfSamples,
    mtLessThan2Vectors,
    mtMoreThanMaxVectorsPerFacet,
    mtErrorExceedsTolerance,
    mtInsufficientAngleBetweenVectors,
    mtFirstVectorShorterThanSecond,
    mtVectorLengthsTooSimilar,
    mtNullTemplateVector,
    mtDifferentFacetsGeometryTooSimilar,
    mtNoncompliantFacetDefinition,
    mtCollectionContainsNonVectorHandles,
    mtParameterOutOfRange,
    mtEmptyBuffer,
    mtPropertyNotSet,
    mtDimensionsDoNotMatch,
    mtOpenFileFailed,
    mtReadFileFailed,
    mtNotACalibrationFile,
    mtIncorrectFileVersion,
    mtCalibrationFileIncomplete,
    mtCameraInitializeFailed,
    mtGrabFrameError*/
  string error = "";
  switch (errorNum)
    {
    case 1:
      error = "Unknown error!";
      break;
    case 2:
      error = "Invalid handle!";
      break;
    case 3:
      error = "Null pointer!";
      break;
    case 4:
      error = "Out of memory";
      break;
    case 5:
      error = "String parameter is too long!";
      break;
    default:
      error = "Unknown error";
      break;
    }
  return error;
}
