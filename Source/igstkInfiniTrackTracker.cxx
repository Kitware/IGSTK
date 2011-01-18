/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkInfiniTrackTracker.cxx
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

#include "igstkInfiniTrackTracker.h"
#include "itksys/SystemTools.hxx"

#include "itkLoggerBase.h"

#include "itkInterface.h"
#include "itkHelper.h"

#include "itkMatrix.h"
#include "iostream"
#include "fstream"

namespace igstk
{

void deviceEnumerator (uint64 u64DeviceSerialNumber, void* pUserData)
{
  *((uint64*) pUserData) = u64DeviceSerialNumber;
}

/** Constructor */
InfiniTrackTracker::InfiniTrackTracker (void) :
m_StateMachine(this)
, m_Handle (NULL)
, m_u64DeviceSerialNumber (0)
, m_BufferLock (NULL)
, m_pvecMarkerPos (NULL)
, m_iProcessed (-1)
, m_iAvailable (-1)
, m_iInAcquisition (-1)
, m_vecTrackerToolID ()
{
  this->m_pvecMarkerPos = new std::vector<itkMarkerPos*>[3]; 

  this->m_vecTrackerToolID.clear ();

  // Lock for the data buffer that is used to transfer the
  // transformations from thread that is communicating
  // with the tracker to the main thread.
  this->m_BufferLock = itk::MutexLock::New();
}

/** Destructor */
InfiniTrackTracker::~InfiniTrackTracker(void)
{
  for (size_t t = 0; t < this->m_pvecMarkerPos [0].size (); t++)
    {
    delete this->m_pvecMarkerPos [0][t];
    }

  for (size_t t = 0; t < this->m_pvecMarkerPos [1].size (); t++)
    {
    delete this->m_pvecMarkerPos [1][t];
    }

  for (size_t t = 0; t < this->m_pvecMarkerPos [2].size (); t++)
    {
    delete this->m_pvecMarkerPos [2][t];
    }

  if (this->m_pvecMarkerPos)
    {
    delete [] this->m_pvecMarkerPos;
    }
  
  this->m_vecTrackerToolID.clear ();
}

/** The "InternalOpen" method opens communication with a tracking device. */
InfiniTrackTracker::ResultType InfiniTrackTracker::InternalOpen (void)
{
  igstkLogMacro(DEBUG, "igstk::InfiniTrackTracker::InternalOpen called ...\n")

  this->m_Handle = itkInitializeDriver ();

  if(!this->m_Handle)
    {
    igstkLogMacro(CRITICAL, itkLastErrorStr ())
    return FAILURE;
    }

  if (itkScanDevices((itkHandle)this->m_Handle, 
      deviceEnumerator,
      &this->m_u64DeviceSerialNumber))
    {
    igstkLogMacro(CRITICAL, itkLastErrorStr ())
    return FAILURE;
    }

  if (this->m_u64DeviceSerialNumber == 0)
    {
    igstkLogMacro(CRITICAL, "No device detected")
    return FAILURE;
    }


  return SUCCESS;
}

/** The "InternalClose" method closes communication with a tracking device. */
InfiniTrackTracker::ResultType InfiniTrackTracker::InternalClose( void )
{  
  igstkLogMacro(DEBUG, "igstk::InfiniTrackTracker::InternalClose called ...\n")

  if (this->m_Handle)
    {
    itkCloseDriver ((itkHandle)this->m_Handle);
    m_Handle = NULL;
    }

  return SUCCESS;
}

/** The "InternalReset" method resets tracker to a known configuration. */
InfiniTrackTracker::ResultType InfiniTrackTracker::InternalReset( void )
{
  igstkLogMacro(DEBUG, "igstk::InfiniTrackTracker::InternalReset called ...\n")

  itkOption option;

  if (itkSetOption ((itkHandle)this->m_Handle,
                   this->m_u64DeviceSerialNumber,
                   "RESET", &option))
    {
    igstkLogMacro(CRITICAL, itkLastErrorStr ())
    return FAILURE;
    }

  return SUCCESS;
}

/** The "InternalStartTracking" method starts tracking. */
InfiniTrackTracker::ResultType InfiniTrackTracker::InternalStartTracking( void )
{
  igstkLogMacro(DEBUG, 
         "igstk::InfiniTrackTracker::InternalStartTracking called ...\n")

  if (itkStartAcquisition ((itkHandle)this->m_Handle,
                            this->m_u64DeviceSerialNumber))
    {
    igstkLogMacro(CRITICAL, itkLastErrorStr ())
    return FAILURE;
    }

  return SUCCESS;
}

/** The "InternalStopTracking" method stops tracking. */
InfiniTrackTracker::ResultType InfiniTrackTracker::InternalStopTracking( void )
{
  igstkLogMacro(DEBUG, 
      "igstk::InfiniTrackTracker::InternalStopTracking called ...\n")

  if (itkStopAcquisition ((itkHandle)this->m_Handle, 
              this->m_u64DeviceSerialNumber))
    {
    igstkLogMacro(CRITICAL, itkLastErrorStr ())
    return FAILURE;
    }

  return SUCCESS;
}

/** The "InternalUpdateStatus" method updates tracker status. */
InfiniTrackTracker::ResultType InfiniTrackTracker::InternalUpdateStatus( void )
{
  igstkLogMacro(DEBUG, 
               "igstk::InfiniTrackTracker::InternalUpdateStatus called ...\n")

  this->m_BufferLock->Lock ();
  setNextArrayForUser ();
  this->m_BufferLock->Unlock ();

  if (m_iProcessed < 0)
  {
    igstkLogMacro(WARNING, "No frame available")
    return FAILURE;
  }

  TrackerToolsContainerType trackerToolContainer = GetTrackerToolContainer();

  for (size_t t = 0; t < this->m_pvecMarkerPos [m_iProcessed].size (); t++)
    {
    itkMarkerPos* pMarker = this->m_pvecMarkerPos [m_iProcessed] [t];

    std::string tool = "";

    for (size_t u = 0; u < this->m_vecTrackerToolID.size (); u++)
      {
      if (this->m_vecTrackerToolID [u].m_u32GeometryID == 
              pMarker->mu32GeometryID)
        {
        tool = this->m_vecTrackerToolID [u].m_TrackerToolName;
        }
      }

    if (tool == "")
      {
      continue;
      }

    // report to the tracker tool that the tracker is Visible
    this->ReportTrackingToolVisible(trackerToolContainer[tool]);

    // create the transform
    TransformType transform;

    typedef TransformType::VectorType TranslationType;
    TranslationType translation;

    translation[0] = pMarker->maf64Translation [0];
    translation[1] = pMarker->maf64Translation [1];
    translation[2] = pMarker->maf64Translation [2];

    typedef TransformType::VersorType RotationType;
    RotationType rotation;

    itk::Matrix<double> matrix;

    matrix [0][0] = pMarker->maf64Rotation [0][0];
    matrix [0][1] = pMarker->maf64Rotation [0][1];
    matrix [0][2] = pMarker->maf64Rotation [0][2];
    matrix [1][0] = pMarker->maf64Rotation [1][0];
    matrix [1][1] = pMarker->maf64Rotation [1][1];
    matrix [1][2] = pMarker->maf64Rotation [1][2];
    matrix [2][0] = pMarker->maf64Rotation [2][0];
    matrix [2][1] = pMarker->maf64Rotation [2][1];
    matrix [2][2] = pMarker->maf64Rotation [2][2];

    rotation.Set (matrix);

    // report error value
    // Get error value from the tracker.
    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = pMarker->mf64RegistrationMeanError;

    long lTime = this->GetValidityTime ();

    transform.SetToIdentity(this->GetValidityTime());
    transform.SetTranslationAndRotation(translation, 
                                        rotation, 
                                        errorValue,
                                        this->GetValidityTime());

    // set the raw transform
    this->SetTrackerToolRawTransform(trackerToolContainer [tool], transform );

    this->SetTrackerToolTransformUpdate(trackerToolContainer [tool], true );
    }

  return SUCCESS;
}


/** The "InternalThreadedUpdateStatus" method updates tracker status. */
InfiniTrackTracker::ResultType 
InfiniTrackTracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro(DEBUG, 
    "igstk::InfiniTrackTracker::InternalThreadedUpdateStatus called ...\n")

  this->m_BufferLock->Lock ();
  setNextArrayForAcquisition ();
  this->m_BufferLock->Unlock ();

  for (size_t t = 0; t < this->m_pvecMarkerPos [m_iInAcquisition].size (); t++)
    {
    delete this->m_pvecMarkerPos [m_iInAcquisition][t];
    }

  this->m_pvecMarkerPos [m_iInAcquisition].clear ();

  uint32 uNoFrame;

  // Declaration of the image structure
  if (itkProcess ((itkHandle)this->m_Handle, 
                 this->m_u64DeviceSerialNumber,
                 &uNoFrame))
    {
    if (itkLastError () > 0)
      {
      igstkLogMacro(CRITICAL, itkLastErrorStr ())
      }
    else
      {
      igstkLogMacro(DEBUG, itkLastErrorStr ())
      }

    return FAILURE;
    } 

  for (size_t t = 0; t < this->m_vecTrackerToolID.size (); t++)
    {
    uint32 u32ID = this->m_vecTrackerToolID [t].m_u32GeometryID;

    itkMarkerPos* pMarkerPosition = new itkMarkerPos;

    if (!itkGetMarkerPos((itkHandle)this->m_Handle, 
        this->m_u64DeviceSerialNumber, u32ID, 
        pMarkerPosition))
      {
      this->m_pvecMarkerPos [m_iInAcquisition].push_back (pMarkerPosition);
      }
    else
      {
      delete pMarkerPosition;
      }
    }

  this->m_BufferLock->Lock ();
  acquisitionFinished ();
  this->m_BufferLock->Unlock ();

  return SUCCESS;
}

/** Print the object information in a stream. */
void InfiniTrackTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if(!this->m_Handle || !this->m_u64DeviceSerialNumber)
    {
    return;
    }

  // Declaration of the device structure
  itkDevice device;

  // Retrieve information about the device
  if (itkGetDevice ((itkHandle)this->m_Handle, 
                   this->m_u64DeviceSerialNumber, 
                   &device))
    {
    return;
    }

  uint32 uLeft  = ((uint32)(device.mu64SerialNumber)),
         uRight = ((uint32)((device.mu64SerialNumber)>>32));

  char strSerial [100];

  sprintf (strSerial, "0x%X%X", uLeft, uRight);

  os << indent << "Tracker: "             << device.mstr64Tracker << std::endl;
  os << indent << "Model:"                << device.mstr64Model << std::endl;
  os << indent << "Device serial number:" << strSerial << std::endl;
}

/** Verify if a tracker tool information is correct before attaching */
InfiniTrackTracker::ResultType 
InfiniTrackTracker::VerifyTrackerToolInformation (
     const TrackerToolType* trackerTool)
{
  igstkLogMacro(DEBUG, 
     "igstk::InfiniTrackTracker::VerifyTrackerToolInformation called ...\n")

  if ( trackerTool == NULL )
    {
    igstkLogMacro(CRITICAL, "TrackerTool is not defined")
    return FAILURE;
    }

  if(this->m_MarkerTemplatesDirectory == "" || 
   !itksys::SystemTools::FileExists(this->m_MarkerTemplatesDirectory.c_str()))
    {
    igstkLogMacro(CRITICAL, "Marker templates directory is not properly set")
    return FAILURE;
    }

  TrackerToolType* pTrackerToolNonConst = 
             const_cast<TrackerToolType *>(trackerTool);

  InfiniTrackTrackerTool* pInfiniTrackTrackerTool 
           = dynamic_cast<InfiniTrackTrackerTool *> (pTrackerToolNonConst);

  if(!pInfiniTrackTrackerTool)
    {
    igstkLogMacro(CRITICAL, "Tracker tool is not a InfiniTrack tool")
    return FAILURE;
    }

  std::string strMarkerFileName = 
     this->m_MarkerTemplatesDirectory + "\\" + 
     pInfiniTrackTrackerTool->GetMarkerName () + 
     ".xml";

  if(!itksys::SystemTools::FileExists(strMarkerFileName.c_str()))
    {
    igstkLogMacro(CRITICAL, "Marker does not exist")
    return FAILURE;
    }

  itkMarker marker;
    
  if (ITK_OK != itkHelperLoadGeometry (strMarkerFileName.c_str (), &marker))
    {
    igstkLogMacro(CRITICAL, "File is not a InfiniTrack marker")
    return FAILURE;
    }
    
  return SUCCESS;
}

/** The "ValidateSpecifiedFrequency" method checks if the specified  */
InfiniTrackTracker::ResultType 
InfiniTrackTracker::ValidateSpecifiedFrequency( double frequencyInHz )
{
  igstkLogMacro(DEBUG, 
   "igstk::InfiniTrackTracker::ValidateSpecifiedFrequency called ...\n")

  return SUCCESS;
}

/** This method will remove entries of the traceker tool 
 * from internal data containers */
InfiniTrackTracker::ResultType 
InfiniTrackTracker::RemoveTrackerToolFromInternalDataContainers(
 const TrackerToolType * trackerTool )
{
  igstkLogMacro(DEBUG, 
   "igstk::InfiniTrackTracker::RemoveTrackerToolFromInternalDataContainers \
   called ...\n")

  TrackerToolType* pTrackerToolNonConst = 
     const_cast<TrackerToolType *>(trackerTool);
  InfiniTrackTrackerTool* pInfiniTrackTrackerTool = 
     dynamic_cast<InfiniTrackTrackerTool *> (pTrackerToolNonConst);
  std::string strMarkerFileName = this->m_MarkerTemplatesDirectory + "\\" + 
             pInfiniTrackTrackerTool->GetMarkerName () + ".xml";

  itkMarker marker;
    
  if (ITK_OK != itkHelperLoadGeometry (strMarkerFileName.c_str (), &marker))
    {
    igstkLogMacro(CRITICAL, itkLastErrorStr ())
    return FAILURE;
    }
    
  if (itkRemoveGeometry ((itkHandle)this->m_Handle, 
     this->m_u64DeviceSerialNumber, 
     marker.mu32GeometryID))
    {
    igstkLogMacro(CRITICAL, itkLastErrorStr ())
    return FAILURE;
    }

  for (size_t t = 0; t < m_vecTrackerToolID.size (); t++)
    {
    if (this->m_vecTrackerToolID [t].m_TrackerToolName == 
       pInfiniTrackTrackerTool->GetMarkerName ())
      {
      this->m_vecTrackerToolID.erase (this->m_vecTrackerToolID.begin () + t);
      break;
      }
    }

  return SUCCESS;
}

/** Add tracker tool entry to internal containers */
InfiniTrackTracker::ResultType 
InfiniTrackTracker::AddTrackerToolToInternalDataContainers (
   const TrackerToolType * trackerTool)
{
  igstkLogMacro(DEBUG, 
  "igstk::InfiniTrackTracker::AddTrackerToolToInternalDataContainers \
   called ...\n")

  TrackerToolType* pTrackerToolNonConst = 
                 const_cast<TrackerToolType *>(trackerTool);
  InfiniTrackTrackerTool* pInfiniTrackTrackerTool = 
                 dynamic_cast<InfiniTrackTrackerTool *> (pTrackerToolNonConst);
  std::string strMarkerFileName = this->m_MarkerTemplatesDirectory + "\\" + 
             pInfiniTrackTrackerTool->GetMarkerName () + 
             ".xml";

  itkMarker marker;

  if (ITK_OK != itkHelperLoadGeometry (strMarkerFileName.c_str (), &marker))
    {
    igstkLogMacro(CRITICAL, itkLastErrorStr ())
    return FAILURE;
    }
    
  if (itkSetGeometry ((itkHandle)this->m_Handle, 
       this->m_u64DeviceSerialNumber, &marker))
    {
    igstkLogMacro(CRITICAL, itkLastErrorStr ())
    return FAILURE;
    }

  TrackerToolIdentification TrackerID;

  TrackerID.m_TrackerToolName = pInfiniTrackTrackerTool->GetMarkerName ();
  TrackerID.m_u32GeometryID   = marker.mu32GeometryID;

  this->m_vecTrackerToolID.push_back (TrackerID);

  return SUCCESS;
}
      
/** Set the next available object array for acquisition */
void InfiniTrackTracker::setNextArrayForAcquisition ()
{
  if (this->m_iAvailable != 0 && this->m_iProcessed != 0)
    {
    this->m_iInAcquisition = 0;
    }
  else if (this->m_iAvailable != 1 && this->m_iProcessed != 1)
    {
    this->m_iInAcquisition = 1;
    }
  else
    {
    this->m_iInAcquisition = 2;
    }
}

/** Set the next object array for the user. True if a new array is available */
bool InfiniTrackTracker::setNextArrayForUser ()
{
  this->m_iProcessed = this->m_iAvailable;
  this->m_iAvailable = -1;

  if(this->m_iProcessed > -1)
    {
    return true;
    }
  else
    {
    return false;
    }
}

/** Called when the acquisition is finished */
void InfiniTrackTracker::acquisitionFinished ()
{
  this->m_iAvailable     = this->m_iInAcquisition;
  this->m_iInAcquisition = -1;
}

} // end of namespace igstk
