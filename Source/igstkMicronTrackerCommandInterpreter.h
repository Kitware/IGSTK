/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMicronTrackerCommandInterpreter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkMicronTrackerCommandInterpreter_h
#define __igstkMicronTrackerCommandInterpreter_h

// Headers from Utilities/MicronTrackerFiles.  These contain
// classes that are meant to simplify the use of the MicronTracker.
// It is, in fact, possible to directly control the MicronTracker
// without these classes, by doing direct function calls to the
// MTC dll.
#include "UtilityFunctions.h"
#include "Persistence.h"
#include "Cameras.h"
#include "Markers.h"
#include "Collection.h"
#include "MTVideo.h"

// The header for the MicronTracker DLL
#include "MTC.h"

#include "igstkObject.h"


namespace igstk
{
/** \class MicronTrackerCommandInterpreter
 *  \brief Command layer for the Claron MicronTracker.
 *
 *  This class provides a command-level interface to the
 *  Claron MicronTracker, that is, it is used by the IGSTK
 *  MicronTracker object to control and receive data from
 *  the MicronTracker via the DLL that comes with the
 *  MicronTracker drivers.
 *
 *  Though it is not implemented yet as such, it should be a
 *  goal that a single MicronTrackerCommandInterpreter can be
 *  shared between a MicronTracker object, which receives
 *  tracking data, and a MicronTrackerVideoGrabber object,
 *  which receives video.
 *
 *  \ingroup Tracking
 */

class MicronTrackerCommandInterpreter : public Object
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( MicronTrackerCommandInterpreter, Object )

protected:

  /** Device error codes
   *
   *  These error codes can be set as a result of a successful
   *  or failed command to the MicronTracker.
   */
  typedef enum
  {
    MTI_FAIL                      = 0,   /**< Command faulure */
    MTI_SUCCESS                   = 1,   /**< Command success */
    MTI_CAMERA_NOT_FOUND          = 2,   /**< Camera not found */
    MTI_GRAB_FRAME_ERROR          = 3,   /**< Error grabbing video frame */
    MTI_MARKER_CAPTURED           = 4,   /**< Marker successfully captured */
    MTI_NO_MARKER_CAPTURED        = 5,   /**< Marker not captured */
    MTI_CAMERA_INDEX_OUT_OF_RANGE = 6,   /**< Bad camera index */
  } ErrorCodeType;


  /** Constructor */
  MicronTrackerCommandInterpreter();

  /** Destructor */
  ~MicronTrackerCommandInterpreter();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Set some preliminary variables. This function should be called
   *  before calling any other functions. Returns 1 if successful,
   *  0 if not. */
  int Init();

  /** No other function should be called after calling this function,
   *  unless the Init() is called again. */  
  void End();

  /** Get the current directory in which the micron tracker configuration
   *  files are stored. */
  const char *GetMicronTrackerDirectory();

  /* Camera setup methods */

  /** Instantiates an object of type CurrentCameras and calls the
   *  SetupCameras method on that object.  Checks for the presence
   *  of any cameras. Returns 1 if finds at least one camera.
   *  If no camera is found returns 0. */
  int SetupCameras();

  /** Detaches the already attached cameras. It is safe to call this
   *  method even if no camera(s) is attached as it checks the status
   *  of the camera(s) first. */
  void DetachCameras();

  /* Camera information methods */

  /** Returns the serial number of the specified camera.  The first
   *  camera has an index of zero. */
  int GetSerialNumber(int index);

  /** Returns the X resolution of the camera. */
  int GetXResolution(int index);

  /** Returns the Y resolution of the camera. */
  int GetYResolution(int index);

  /** Returns the number of attached cameras. */
  int GetNumberOfCameras();

  /* Camera shutter methods */

  /** Sets the shutter opening time in the index_th camera. 
   *  When this method is called the AutoExposure in the camera
   *  is set to false automatically. */
  int SetShutterTime(double n, int index);

  /** Gets the shutter opening time in the index_th camera. */
  double GetShutterTime(int index);

  /** Gets the minimum shutter opening time in the index_th camera. */
  double GetMinShutterTime(int index);

  /** Gets the maximum shutter opening time in the index_th camera. */
  double GetMaxShutterTime(int index = -1);

  /* Camera exposure methods */

  /** Gets the gain of the index_th camera. */
  double GetGain(int index);

  /** Sets the gain of the index_th camera. */
  int SetGain(double n, int index);

  /** Get the minimum gain of the index_th camera. */
  double GetMinGain(int index);

  /** Get the maximum gain of the index_th camera. */
  double GetMaxGain(int index);

  /** Get the gain in decibels of the the index_th camera. */
  double GetDBGain(int index);

  /** Get the exposure of the the index_th camera. */
  double GetExposure(int index);

  /** Set the exposure of the index_th camera. */
  int SetExposure(double n, int index);

  /** Get the minimum exposure of the the index_th camera. */
  double GetMinExposure(int index);

  /** Get the maximum exposure of the the index_th camera. */
  double GetMaxExposure(int index);

  /** Get the colour temperature ("light coolness") used by the camera.
   *  This will be automatically adjusted if a CoolCard is placed in
   *  the camera's field of view. */
  double GetLightCoolness(int index);

  /** Sets the AutoExposure property of the index_th camera.
   *  When set to true, the exposure is automatically adjusted to
   *  maintain a good distribution of grey levels in the image.
   *  If set to false, exposure is not automatically adjusted. */
  int SetCamAutoExposure(int n, int index);
  
  /** Gets the AutoExposure property of the index_th camera.
   *  See also SetCamAutoExposure(). */
  int GetCamAutoExposure(int index);

  /* Camera selection */

  /** Selects the index_th camera. Returns 0 if successful, -1 if not. */
  int SelectCamera(int index);

  /** Returns the index of the current camera. */
  int  GetCurrentCameraIndex();

  /* Grab a frame from the camera and process it. */

  /** Grabs the frame of the index_th cameras. */
  int GrabFrame(int index);

  /** Invokes processing of the most recent grabbed frame. This process
   * involves recognizing the identified vectors. */
  int ProcessFrame();

  /* Adding or changing templates */

  /** Initiates collecting new samples.
   *  if successfully detects two new vectors return 0.
   *  if more than two vectors are detected returns 1.
   *  if less than two vectros are detected returns -1.
   *  In the case of adding an additional facet to a known facet: 
   *  Returns 99 if no known facet is being detected. */
  int CollectNewSamples(int collectingAdditionalFacet);

  /** Stops collecting the samples from the new marker and creates
   *  a new marker with the name of 'templateName' and jitter value
   *  of 'jitterValue. If successful returns 0, otherwise returs -1. */
  int StopSampling(char* templateName, double jitterValue);

  /** Saves the template with name 'templName' in the Marker folder in the 
   *  current directory. Returns 0 if successful, -1 if not. */
  int SaveMarkerTemplate(char* templName);

  /** Deletes the previous samples received. */
  void ResetSamples();

  /* Get the latest frame (or general frame) information. */

  /** Returns the time of the latest frame of the index_th camera.
   *  The time is the time elapsed in msecs since the MicronTracker
   *  has started. If not successful, returns -1. */
  double GetLatestFrameTime(int index);

  /** Gets the histogram of the pixel values of the latest frame of the
   *  index_th camera.  If index is -1 then the histogram of the pixel
   *  values of the current camera will be calculted.
   *  The histogram array is copied into the aPixHist array.
   *  Returns 1 if successful, 0 if not.
   *  If subSampleRate is set to values > 1 then the speed of
   *  computation increases. */
  int GetLatestFramePixHistogram(long* &aPixHist, int subSampleRate,
                                   int index);

  /** Returns the number of frames grabbed by the index_th camera.
   *  Increments by one every time a frame is grabbed. If index is -1
   *  then returns the number of frames grabbed by the default camera.
   *  If no index is passed the default value is -1. Returns -1 if not
   *  successful. */
  int GetNumberOfFramesGrabbed(int index);

  /** Returns the BitsPerPixel of the index_th camera. */
  int GetBitsPerPixel(int index);
  
  /** Check for hazard codes on the lastest frame
   *  (e.g. thermal instability) */
  MeasurementHazardCode GetLatestFrameHazardCode();

  /* Template loading methods */

  /** Loads the marker templates and inserts the result in the
   *  passed arguments:
   *   \param tmplsName Holds the name of the templates.
   *   \param tmplsError Holds text for any errors encountered.
   *   \param tmplsWarn Holds the text for any warnings encountered. */
  int RefreshTemplates(vector<string> &tmplsName,
                       vector<string> &tmplsError);

  /** Sets the match tolerance of the marker templates:
   *  \param matchTolerance in mm. */
  int SetTemplMatchTolerance(double matchTolerance);

  /** Gets the match tolerance of the marker templates. */
  double GetTemplMatchTolerance();

  /** Gets the default value for match tolerance of the marker templates. */
  double GetTemplMatchToleranceDefault();

  /* Get and set properties of some of the loaded templates.
   * IMPORTANT NOTE: All the following methods should be called only after
   * the templates have been loaded (i.e. after the RefreshTemplates ) has
   * been called by the client. */

  /** Get the number of all the loaded templates in the markers list.
   *  Important: This method should not be called before callng the
   *  RefreshTemplates method (see above). */
  int GetNumberOfLoadedTemplates();

  /** Set the name of the template 'index' in the templates list. */
  int SetTemplateName(int index, char* templName);

  /** Get the name of the template 'index' in the template list. */
  char* GetTemplateName(int index);

  /** Get the name of the template 'index' in the list of the
   *  identified markers.  If index is out of range returns an
   *  empty string. */
  char* GetIdentifiedTemplateName(int index);

  /** Delete the template 'index' in the list of the loaded templates. */
  int DeleteTemplate(int index);

  /* Predictive frames settings. */

  /** Set the predicted frames interleave.  This is the number of
   *  predictive-only frames between the pairs of comprehensive
   *  frames. */
  int SetPredictiveFramesInterleave(int predictiveInterleave);

  /** Get the predicted frames interleave.  This is the number of
   *  predictive-only frames between the pairs of comprehensive
   *  frames. */
  int GetPredictiveFramesInterleave();

  /* Marker information */

  /** Returns the number of loaded marker templates. */
  int  GetNumberOfIdentifiedMarkers();
  
  /** Inserts the number of identified facets in each marker in
   *  the NumberOfFacets vector. So the first element of this vector would
   *  be the number of identified facets in the first marker, the
   *  second element of the vector would be the number of identified
   *  facets in the second marker and so on. */
  int GetNumberOfFacetsInMarker(int markerIndex);

  /** Inserts the number of total facets in each marker in the
   *  NumberOfFacets vector. So the first element of this vector would
   *  be the number of total facets in the first marker, the 
   *  second element of the vector would be the number of total
   *  facets in the second marker and so on. */
  int GetNumberOfTotalFacetsInMarker(int markerIndex);

  /** Returns the number of unidentified vectors. */
  int GetNumberOfUnidentifiedMarkers();

  /* Get the rotation and translation matrix of a marker. */

  /** Inserts the Rotation Matrix for markerIndex_th marker into
   *  \param vRotation.
   *  The resulted vector  has nine elements with this order: 
   *  R11, R12, R13, R21, R22, R23, R31, R32, R33
   *  where the first digit after R represents row and the second
   *  digit represents column.
   *  This rotation matrix represents the center of the long vector
   *  of the marker.  Markers' index start at 0. */
  void GetRotations(vector<double> &vRotations, int markerIndex);

  /** Inserts the Translation (offset) of the markerIndex_th marker
   *  into \param vTranslation.
   *  This offset  is in fact the offset of the center of the long
   *  vector of the marker. Markers' index start at 0. */
  void GetTranslations(vector<double> &vTranslations, int markerIndex);

  /** Status information */

  /** Tracked tool status */
  int  GetStatus();

  /** Finds the status of the loaded marker with the index of \param
   *  loadedMarkerIndex. The status of the marker can be one of the
   *  following two values:
   *  1. If the marker is indentified in the current camera frame the
   *     value will be mti_utils::MTI_MARKER_CAPTURED. 
   *  2. If the makrer is not identified in the current camera frame
   *     the value will be mti_utils::MTI_NO_MARKER_CAPTURED. 
   *  If the requested marker if found in the current camera frame,
   *  then it finds out what the index of the identified marker
   *  is in the list of the identified markers. Obviously this is
   *  different from the index of the marker in the list of the loaded
   *  markers.  This index will be returned via the second paramter
   *  \param identifiedMarkerIndex.  */
  int GetMarkerStatus(int loadedMarkerIndex, int* identifiedMarkerIndex);

  /** Find all identified markers. */
  void FindIdentifiedMarkers();

  /** Find all unidentified markers. */
  void FindUnidentifiedMarkers();

  /** Find the xpoints of the identified markers and retrieve them.
   *  Note: The elements in the returned  array are as follow (the
   *  array starts at 0):
   *  Index 0 and 1: X and Y of the Long Vector's Base of the Left Image.
   *  Index 2 and 3: X and Y of the Long Vector's Head of the Left Image.
   *  Index 4 and 5: X and Y of the Long Vector's Base of the Right Image.
   *  Index 6 and 7: X and Y of the Long Vector's Head of the Right Image.
   *  Index 8 and 9: X and Y of the Short Vector's Base of the Left Image.
   *  Index 10 and 11: X and Y of the Short Vector's Head of the Left Image.
   *  Index 12 and 13: X and Y of the Short Vector's Base of the Right Image.
   *  Index 14 and 15: X and Y of the Short Vector's Head of the Right
   *  Image. */
  void GetIdentifiedMarkersXPoints(double* &xPoints, int markerIndex);

  /** Find the end points of the unidentified markers and retrieve them.
   *  Note: The elements in the returned  array are as follow (the
   *  array starts at 0):
   *  Index 0 and 4: X and Y of the vector's Base of the Left Image. 
   *  Index 2 and 6: X and Y of the vector's Head of the left Image.
   *  Index 1 and 5: X and Y of the vector's Base of the right Image. 
   *  Index 3 and 7: X and Y of the vector's Head of the right Image. */
  void GetUnidentifiedMarkersEnds(double* &endPoints, int vectorIndex);

  /* Get the image data captured by the cameras. */

  /** Gets the images captured by the index_th camera. */
  int GetLeftRightImageArray(unsigned char** &leftImageArray,
                             unsigned char** &rightImageArray, int index);

  /** Get the images at reduced size. */
  int GetLeftRightImageArrayHalfSize(unsigned char** &leftImageArray,
                                     unsigned char** &rightImageArray, 
                                     int xResolution, int yResolution,
                                     int index);

  /** Get the stored error string */
  char *GetErrorString() { return m_ErrorString.c_str(); };

  /** Update the ini file */
  void UpdateInitFile();

protected:

  /** Platform-independent method to remove a file */
  int RemoveFile(string fileName, char* dir);

  /** Platform-independent method to rename a file */
  int RenameFile(string oldName, string newName, char* dir);

  /** Error-handling function */
  string HandleErrors(int errorNum);

  /** The micron tracker home directory. */
  std::string m_MicronTrackerDirectory;

  /** The string returned for the last error that occurred. */
  std::string m_ErrorString;

  /** Handle for each camera available to the system. */
  Cameras m_Cameras;

  /** Handle for the current camera. */
  MCamera m_CurrentCamera;

  /** The index of the current camera. */
  unsigned int m_CurrentCameraIndex;

  /** Handle for each marker registered with the system. */
  Markers m_Markers;

  /** The persistence object stores information between sessions. */ 
  Persistence m_Persistence;

  /** Whether a camera is attached. */
  bool m_CameraIsAttached;

  /** The Rotations */
  std::vector<double> m_Rotations;

  /** The Translations */
  std::vector<double> m_Translations;

private:

  MicronTrackerCommandInterpreter(const Self&);  //purposely not implemented
  void operator=(const Self&);       //purposely not implemented
};  


} // namespace igstk


#endif  // __igstk_MicronTrackerCommandInterpreter_h_
