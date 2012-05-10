/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkAxios3DTracker.h,v $
  Language:  C++
  Date:      $Date: 2011-03-17 20:12:26 $
  Version:   $Revision: 1.00 $

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

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

#include "igstkAxios3DTracker.h"
#include "itksys/SystemTools.hxx"
#include "itkLoggerBase.h"
#include "itkMatrix.h"
#include "iostream"
#include "fstream"

/**----------------------------------------------------------------------------
*   HandleMetroError
*  ----------------------------------------------------------------------------
*  The "HandleMetroError" function for error-handling
*  ----------------------------------------------------------------------------
*/
std::string HandleMetroError(metro_lib::MetroError & e)
{
  std::stringstream str;
  str  << "\n"
    << ": ERROR: \n"
    << e.GetMergedInfoString()<<"\n";

  return str.str().c_str();
}

namespace igstk
{

/** Constructor */
Axios3DTracker::Axios3DTracker (void) :
m_StateMachine(this), m_MetrologySystems(0), m_Metrology(0)
,m_Tracking(false)
{
  m_BufferLock = itk::MutexLock::New();
  metro_lib::MetroUtils::AddReceiver(&m_Mwr);

  CreateObject();
}

/** Desctructor */
Axios3DTracker::~Axios3DTracker (void)
{
}


/**----------------------------------------------------------------------------
*   RequestLoadLocatorsXML
*  ----------------------------------------------------------------------------
*  Load all locators that are defined in the given xml-file.
*  ----------------------------------------------------------------------------
*/
Axios3DTracker::ResultType Axios3DTracker
::RequestLoadLocatorsXML(const std::string & file)
{
  igstkLogMacro(DEBUG,
               "igstk::Axios3DTracker::RequestLoadLocatorXML called ...\n")

  try
  {
    metro_lib::LocatorAdministrator::LoadLocatorsFromFile(file.c_str());
    igstkLogMacro(INFO, "OK\n" );

  }
  catch(metro_lib::MetroError & e)
  {
    igstkLogMacro(WARNING, HandleMetroError(e));
    return FAILURE;
  }

  igstkLogMacro(INFO, m_Mwr.GetText());
  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   DeleteLocators
*  ----------------------------------------------------------------------------
*  deleting all loaded locator
*  ----------------------------------------------------------------------------
*/
Axios3DTracker::ResultType Axios3DTracker::DeleteLocators()
{
  igstkLogMacro(DEBUG,
               "igstk::Axios3DTracker::DeleteLocators called ...\n")

  try
  {
    igstkLogMacro(INFO, "Deleting Locators \n");

    metro_lib::LocatorAdministrator::DeleteAllLocators();
  }
  catch(metro_lib::MetroError & e)
  {
    igstkLogMacro(WARNING, HandleMetroError(e));
    return FAILURE;
  }

  igstkLogMacro(INFO, m_Mwr.GetText());
  return SUCCESS;
}

/** ----------------------------------------------------------------------------
*  Locks the Image Access\n
*  for synchronism reasons, you have to "lock" the image access for
*  measuring points and locators\n
*  Once the system is locked, you can measure all points "MeasurePoints"
*  and different locators "MeasureLocators" one after the other
*  - but in the same picture data \n
*  Every call of "Lock" gets you fresh data from the measurement system.
*  \returns the ID of the Lock
*  ----------------------------------------------------------------------------
*/
Axios3DTracker::ResultType Axios3DTracker::Lock()
{
  igstkLogMacro(DEBUG,
               "igstk::Axios3DTracker::Lock called ...\n")

  std::stringstream str;

  if (m_Metrology == NULL)
  {
    igstkLogMacro( CRITICAL, "Metrology is not defined" );
    return FAILURE;
  }

  try
  {
    igstkLogMacro(INFO, "Lock: " );

    str<< m_Metrology->Lock();
    // give the device time to capture images from each camera and
    // store them for further processing (i.e. pose estimation)
    Sleep(10);
    igstkLogMacro(INFO,  "Ok" << "\n" );

  }
  catch(metro_lib::MetroError & e)
  {
    str  << HandleMetroError(e);
  }

  str << m_Mwr.GetText();

  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   GetStringLocatorPointResult
*  ----------------------------------------------------------------------------
*  Returns information if each function is true
*  ----------------------------------------------------------------------------
*/
std::string GetStringLocatorPointResult
(
  const metro_lib::LocatorPointResult & point
)
{
  std::stringstream str;

  unsigned id = point.GetId();

  str << "\n\nPoint-ID: " << id << "\n";

  bool b = point.HasImgRes();
  str << "Has Point picturepoint-elements: " << b << "\n";
  if ( b )
  {
    //Point OK
    bool pok = point.IsOk();
    str << "Is Point ok: " << pok << "\n";

    //Point visible
    bool vis = point.IsVisible();
    str << "Is Point visible: " << vis << "\n";

    //Point in measurement volume
    bool inMeasVolume = point.IsInMeasurementVolume();
    str << "Is Point in the measuring volume: " << inMeasVolume << "\n";

    //global coordinates
    std::vector<double> global;
    global.resize(3);

    point.GetCoordinate3D
    (
      global[0],
      global[1],
      global[2]
    );

    str << "global coordinates:\n"
      << global[0] << ", "
      << global[1] << ", "
      << global[2] << "\n\n";

    double rms_img = point.GetImgRms();
    str << "RMS image point: " << rms_img << "\n";

    bool a = point.Has3DRms();
    str << "has point 3D RMS: " << a << "\n";
    if ( a )
    {
      double img3DRms = point.GetRms3D();
      str << "3D-Rms: " << img3DRms << "\n";
      bool aok = point.Is3DRmsOk();
      str << "Is 3D Rms OK? - " << aok << "\n";
    }

  }

  b = point.IsUsedForLocPosition();
  str << "IsUsedForLocPosition: " << b << "\n";

  b = point.IsTargetNormalInRange();
  str << "IsTargetNormalInRange: " << b << "\n";

  b = point.HasTargetNormalState();
  str << "HasTargetNormalState: " << b << "\n";

  b = point.HasLocalTransformedCoordinate();
  str << "Has point local coordinates" << b << "\n";

  if ( b )
  {
    // if yes output
    std::vector<double> local_coord;
    local_coord.resize(3);

    point.GetLocalTransformed3DCoordinate
    (
      local_coord[0],
      local_coord[1],
      local_coord[2]
    );

    str << "local position:"
      << local_coord[0] << ", "
      << local_coord[1] << ", "
      << local_coord[2] << "\n";
  }

  // Virtual Points are locator points that have not been measured
  b = point.HasVirtualCoordinate();
  str << "Has Point virtual coordinates: " << b << "\n";

  if ( b )
  {
    std::vector<double> virt_coord;
    virt_coord.resize(3);

    point.GetVirtual3DCoordinate
    (
      virt_coord[0],
      virt_coord[1],
      virt_coord[2]
    );

    str << "virtual position:"
      << virt_coord[0] << ", "
      << virt_coord[1] << ", "
      << virt_coord[2] << "\n";
  }

  return str.str();
}

/**----------------------------------------------------------------------------
*   MeasurePoints
*  ----------------------------------------------------------------------------
*  Returns all measured points\n
*  system must be locked to use this function!
*  ----------------------------------------------------------------------------
*/
Axios3DTracker::ResultType Axios3DTracker::MeasurePoints(bool extended_protocol)
{

  igstkLogMacro(DEBUG,
               "igstk::Axios3DTracker::MeasurePoints called ...\n")

  std::stringstream s;
  // without lock it's not possible to measure points and locators
  s << Lock();

  if (m_Metrology == NULL)
  {
    igstkLogMacro( CRITICAL, "Metrology is not defined" )
    return FAILURE;
  }

  try
  {
    igstkLogMacro( INFO, "MeasurePoints:" );

    metro_lib::PointResultList pts = m_Metrology->MeasurePoints();

    unsigned size = pts.Size();
    igstkLogMacro( INFO, "Size: " << size << " " );
    if (extended_protocol)
    {
      igstkLogMacro( INFO, "\n" );
    }

    for ( unsigned i = 0; i < size; ++i )
    {
      metro_lib::PointResult p
        = pts.GetElement(i);

      // is the locator ok
      bool ok = p.IsOk();

      if (extended_protocol)
      {
        igstkLogMacro( INFO, "IsOk: " << ok << "\n");
      }

      unsigned id = p.GetId();

      if (extended_protocol)
      {
        igstkLogMacro( INFO, "Id: "<< id <<"\n" );
      }
      std::vector<double> point;
      point.resize(3);
      p.GetCoordinate3D
      (
        point[0],
        point[1],
        point[2]
      );

      if (extended_protocol)
        {
        igstkLogMacro( INFO, "Coordinate3D: ");
        }

      igstkLogMacro( INFO, point[0]<<" " );
      igstkLogMacro( INFO, point[1]<<" " );
      igstkLogMacro( INFO, point[2]<<"; " );
      if (extended_protocol)
      {
        igstkLogMacro( INFO, "\n" );

        if(p.Has3DRms())
        {
          double d = p.GetRms3D();
          igstkLogMacro( INFO, "Rms3D: "<< d <<"\n" );

          unsigned int ac = p.GetRms3DAccClass();
          igstkLogMacro( INFO, "Rms3DAccClass: "<< ac <<"\n" );
        }

        if(p.HasImgRes())
        {
          double d = p.GetImgRms();
          igstkLogMacro( INFO, "ImgRms: "<< d <<"\n" );

          unsigned int ac = p.GetImgAccClass();
          igstkLogMacro( INFO, "ImgAccClass: "<< ac <<"\n" );
        }

        if(p.HasImgRes())
        {
          double d = p.GetImgRms();
          igstkLogMacro( INFO, "ImgRms: "<< d <<"\n" );

          unsigned int ac = p.GetImgAccClass();
          igstkLogMacro( INFO, "ImgAccClass: "<< ac <<"\n" );
        }
        int i = p.GetAccClass();
        igstkLogMacro( INFO, "AccClass: "<< i <<"\n" );

        bool b1 = p.IsInMeasurementVolume();
        igstkLogMacro( INFO, "IsInMeasurementVolume: "<< b1 <<"\n" );

        if(p.HasImgRes())
        {
          double rms_img = p.GetImgRms();
          igstkLogMacro( INFO, "ImgRms: "<< rms_img <<"\n" );
        }

        igstkLogMacro( INFO, "Warnings ");
        unsigned num_warn = p.NumberOfWarnings();
        for (unsigned i_warn = 0; i_warn < num_warn; ++i_warn)
        {
          igstkLogMacro( INFO, p.WarningFromIndex(i_warn) );
          igstkLogMacro( INFO, " " );
        }
        igstkLogMacro( INFO, "\n");

      }
    }
    igstkLogMacro( INFO, "\n");
  }
  catch(metro_lib::MetroError & e)
  {
    igstkLogMacro( WARNING, HandleMetroError(e) );
    return FAILURE;
  }
  catch(...)
  {
    // error handling
    igstkLogMacro( WARNING, "\n" << "ERROR: Unknown" << "\n" );
    return FAILURE;
  }

  igstkLogMacro( INFO, m_Mwr.GetText() );
  return SUCCESS;
}

/**----------------------------------------------------------------------------
*  MeasureLocator
*  ----------------------------------------------------------------------------
*  \brief  locator measurement with extended protocol of data
*  ----------------------------------------------------------------------------
*/
Axios3DTracker::ResultType Axios3DTracker::MeasureLocator
(
  const std::string & loc_id,
  bool extended_protocol
)
{

  igstkLogMacro(DEBUG,
               "igstk::Axios3DTracker::MeasureLocator called ...\n")

  std::stringstream s;
   // create the transform
  TransformType transform;
  // without lock it's not possible to measure points and locators
  s << Lock();

  if (m_Metrology == NULL)
  {
    igstkLogMacro(CRITICAL, "Metrology not defined")
    return FAILURE;
  }

  try
  {
    if (extended_protocol)
    {
      igstkLogMacro( INFO, "\n ---- Locator Measurement ---- "<<loc_id<<"\n" );
    }

    metro_lib::LocatorResult loc_res = m_Metrology->MeasureLocator(
                                                            loc_id.c_str()  );

    bool visible = loc_res.IsVisible();

    igstkLogMacro( INFO, "Locator IsVisible: " << visible << " - " );


    std::string loc_name = loc_res.GetId();
    if (extended_protocol)
    {
      igstkLogMacro( INFO, "Locator ID: " << loc_name );
    }

    // if locator is visible
    if ( visible )
    {
      // is the locator OK?
      if (extended_protocol)
      {
        igstkLogMacro( INFO, "IsOk: " << loc_res.IsOk() << "\n" );

        // rms_adjustment
        double rms_adjustment = loc_res.GetRmsAdjustment();
        igstkLogMacro( INFO, "rms_adjustment: " << rms_adjustment << "\n" );

        // rms_transformation
        double rms_trans = loc_res.GetRmsTransformation();
        igstkLogMacro( INFO, "rms_transformation: " << rms_trans << "\n" );

        // RMS image measurement
        double rms_imgMeas = loc_res.GetRmsImgMeasurement();
        igstkLogMacro( INFO, "RMS image measurement: " << rms_imgMeas << "\n" );

        // visible points
        unsigned int noOfVPts = loc_res.GetNumberOfVisiblePts();
        igstkLogMacro( INFO, "visible points: " << noOfVPts << "\n" );

        // used points
        unsigned int noOfUPts = loc_res.GetNumberOfUsedPts();
        igstkLogMacro( INFO, "used points: " << noOfVPts << "\n" );

        unsigned int noOfWarns = loc_res.GetNumberOfWarnings();

        if ( noOfWarns > 0 )
          {
          igstkLogMacro( INFO, "warnings: \n" );

          for (unsigned int i = 0; i<noOfWarns; ++i)
          {
            unsigned int w = loc_res.GetWarningFromIndex(i);
            igstkLogMacro( INFO, "Warning: " << w << "\n" );
          }
        }
        else
        {
        igstkLogMacro( INFO, "locator has no warnings!\n\n" );
        }
        //get the accuracy class
        bool accClass = loc_res.HasAccClass();

        if ( accClass )
        {
          unsigned int aClass = loc_res.GetAccClass();
          igstkLogMacro( INFO, "accuracy class: " << aClass << "\n" );
        }
        else
        {
          igstkLogMacro( INFO, "no accuracy class!\n" );
        }

        bool allAccClass = loc_res.HasAllAccClasses();

        if ( allAccClass )
        {
          unsigned int transformation =
            loc_res.GetAccClassTransformation();
          double dtransformation = loc_res.GetRmsTransformation();
          igstkLogMacro( INFO, "accuracy transformation: class "
            << transformation << " val "
            << dtransformation << "\n" );
          unsigned int adjustment = loc_res.GetAccClassAdjustment();
          double dadjustment = loc_res.GetRmsAdjustment();
          igstkLogMacro( INFO, "accuracy adjustment: class "
            << adjustment << " val " << dadjustment << "\n" );
          unsigned int imgMeas = loc_res.GetAccClassImgMeasurement();
          double dimgMeas = loc_res.GetRmsImgMeasurement();
          igstkLogMacro( INFO, "accuracy image measurement: class "
            << imgMeas << " val " << dimgMeas << "\n" );
        }
        else
        {
          igstkLogMacro( INFO, "no accuracy class!\n" );
        }
      }

      // translation of the origin of locator definition
      // in the coordinate system of the measurement system
      std::vector<double> trans;
      trans.resize(3);

      loc_res.GetTranslation
      (
        trans[0],
        trans[1],
        trans[2]
      );

      igstkLogMacro( INFO, "translation: "
        << trans[0] << ", "
        << trans[1] << ", "
        << trans[2] << " " );

      if (extended_protocol)
      {
        igstkLogMacro( INFO, "\n" );
      }

      // RotationAsMatrix
      std::vector<double> rotMatrix;
      rotMatrix.resize(9);

      loc_res.GetRotationAsMatrix
      (
        rotMatrix[0],
        rotMatrix[1],
        rotMatrix[2],
        rotMatrix[3],
        rotMatrix[4],
        rotMatrix[5],
        rotMatrix[6],
        rotMatrix[7],
        rotMatrix[8]
      );

      igstkLogMacro( INFO,  "rotation as matrix: "
        << rotMatrix[0] << " "
        << rotMatrix[1] << " "
        << rotMatrix[2] << " | "
        << rotMatrix[3] << " "
        << rotMatrix[4] << " "
        << rotMatrix[5] << " | "
        << rotMatrix[6] << " "
        << rotMatrix[7] << " "
        << rotMatrix[8] << " " );

      if (extended_protocol)
      {
        igstkLogMacro( INFO,  "\n\n" );
      }

      // RotationAlgebraic
      std::vector<double> rotAlg;
      rotAlg.resize(4);

      loc_res.GetRotationAlgebraic
      (
        rotAlg[0],
        rotAlg[1],
        rotAlg[2],
        rotAlg[3]
      );

      typedef TransformType::VectorType TranslationType;
      TranslationType translation;

      translation[0] = trans[0];
      translation[1] = trans[1];
      translation[2] = trans[2];

      typedef TransformType::VersorType RotationType;
      RotationType rotation;
      rotation.Set(rotAlg[0],
            rotAlg[1],
            rotAlg[2],
            rotAlg[3]);

      // report error value
      // Get error value from the tracker.
      typedef TransformType::ErrorType  ErrorType;

      ErrorType errorValue = 0;

      long lTime = this->GetValidityTime ();

      transform.SetToIdentity(this->GetValidityTime());
      transform.SetTranslationAndRotation(translation,
                        rotation,
                        errorValue,
                        this->GetValidityTime());

      if (extended_protocol)
        {
        igstkLogMacro( INFO,  "rotation algebraic:\n"
          << rotAlg[0] << "\n"
          << rotAlg[1] << "\n"
          << rotAlg[2] << "\n"
          << rotAlg[3] << "\n\n" );

      // measured and virtual points
      metro_lib::LocatorPointResultList list
        = loc_res.GetMeasuredAndVirtualPts();

      // number of points
      unsigned no_of_pts = list.Size();

      igstkLogMacro( INFO, "number of locator points: "
        << no_of_pts << "\n" );

      igstkLogMacro( INFO, "\n -- locatorpoints -- \n\n" );

        // data of the points
        for ( unsigned int i = 0; i < no_of_pts; ++i )
        {
          // get point
          metro_lib::LocatorPointResult point
            = list.GetElement(i);

          igstkLogMacro( INFO, GetStringLocatorPointResult(point) );
        }
      }
     }

    LocatorResult lockResult(transform, visible);
    m_LocatorResultsContainer[loc_id.c_str()] = lockResult;
  }
  catch(metro_lib::MetroError & e)
  {
    igstkLogMacro( WARNING, HandleMetroError(e) );
    return FAILURE;
  }

  igstkLogMacro( INFO, m_Mwr.GetText() );
  return SUCCESS;
}


/**----------------------------------------------------------------------------
*   GetLocatorNames
*  ----------------------------------------------------------------------------
*  get the locator definitions
*  ----------------------------------------------------------------------------
*/
std::list<std::string> Axios3DTracker::GetLocatorNames()
{
  igstkLogMacro(DEBUG,
               "igstk::Axios3DTracker::GetLocatorNames called ...\n")

  std::list<std::string> loc_list;
  unsigned int num
  = metro_lib::LocatorAdministrator::NumberOfLoadedLocators();

  for (unsigned int ui = 0; ui < num; ++ui)
  {
    metro_lib::Locator loc
      = metro_lib::LocatorAdministrator::GetLocatorByIndex(ui);
    loc_list.push_back(loc.GetId());
  }
  return loc_list;
}

/**----------------------------------------------------------------------------
*   CreateObject
*  ----------------------------------------------------------------------------
*  create Object
*  ----------------------------------------------------------------------------
*/
Axios3DTracker::ResultType Axios3DTracker::CreateObject()
{
  igstkLogMacro(DEBUG,
               "igstk::Axios3DTracker::CreateObject called ...\n")

   std::stringstream str;

  if (m_MetrologySystems)
  {
    return SUCCESS;
  }

  // an exception may occur here, when the m_Metrology-DLL could not be found!
  try
  {
    m_MetrologySystems = new metro_lib::MetrologySystems();
    igstkLogMacro(INFO, "Object created!\n" );
  }
  catch(metro_lib::MetroError & e)
  {
    igstkLogMacro(WARNING, HandleMetroError(e) );
    return FAILURE;
  }
  catch(MetrologyDLLLoadException & e)
  {
    igstkLogMacro(WARNING,
    "MetrologyDLLLoadException! " << "\tFile " << e.GetFileName()
    << "\tFunction " << e.GetFunctionName() );
    return FAILURE;
  }

  igstkLogMacro(INFO, m_Mwr.GetText() );
  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   InternalOpen
*  ----------------------------------------------------------------------------
*  The "InternalOpen" method opens communication with a tracking device.
*  ----------------------------------------------------------------------------
*/
Axios3DTracker::ResultType Axios3DTracker::InternalOpen (void)
{
    igstkLogMacro(DEBUG, "igstk::Axios3DTracker::InternalOpen called ...\n")

  if(m_VirtualMode==1)
  {
    igstkLogMacro(INFO, "in Virtual");
    std::stringstream str;

    try
    {
      if (m_Metrology)
      {
        delete m_Metrology;
        m_Metrology = 0;
      }

      igstkLogMacro(INFO, "Initialization Virtual\n" );
      m_Metrology = new metro_lib::Metrology
      (
        m_MetrologySystems->InitVirtualSystem
        (
          m_Path, 1
        )
      );

      metro_lib::MeasurementSystemInfo msi = m_Metrology->GetSystemInfo();

      igstkLogMacro( INFO, "SystemName: " << msi.GetSystemName() );
      igstkLogMacro( INFO, " - SerialNumber: " <<  msi.GetSerialNumber() );
      igstkLogMacro( INFO, " - Revision: " << msi.GetRevision() );
      igstkLogMacro( INFO, " - Configuration: " << msi.GetConfiguration() );
      igstkLogMacro( INFO, " - VirtualSystem: " << msi.Virtual()<< "\n" );

      igstkLogMacro( INFO, "\n" << "Initialization finished\n" );
    }

    catch(metro_lib::MetroError & e)
    {

      if(e.HasErrors())
      {
        if (m_Metrology)
        {
          delete m_Metrology;
          m_Metrology = 0;
        }
      }
      igstkLogMacro( INFO, HandleMetroError(e) );
    }

      igstkLogMacro( INFO, m_Mwr.GetText() );
  }
  else
  {

    try
    {
      if (m_Metrology)
      {
        delete m_Metrology;
        m_Metrology = 0;
      }

      igstkLogMacro( INFO, "Initialization " );

      metro_lib::MeasurementSystemInfoList msil
        = m_MetrologySystems->FindMeasurementSystems();

      unsigned int size = msil.Size();

      igstkLogMacro( INFO, "Number of systems: " << size << "\n" );

      std::list<std::string> lst;

      metro_lib::MeasurementSystemInfo  msi = msil.GetElement(0);

      for(unsigned int i=0; i<size; ++i)
      {
        igstkLogMacro( INFO, "SystemName: " << msi.GetSystemName() );
        igstkLogMacro( INFO, " - SerialNumber: " <<  msi.GetSerialNumber() );
        igstkLogMacro( INFO, " - Revision: " << msi.GetRevision() );
        igstkLogMacro( INFO, " - Configuration: " << msi.GetConfiguration() );
        igstkLogMacro( INFO, " - VirtualSystem: " << msi.Virtual()<< "\n" );

        std::stringstream strm;
        strm << msi.GetSystemName() << " | ";
        strm << msi.GetRevision() << " | ";
        strm << msi.GetSerialNumber() << " | ";
        strm << msi.GetConfiguration();

        lst.push_back(strm.str());
      }

      if (size > 0)
        {

        int index=0;

        if (index < 0)
        {
          igstkLogMacro( WARNING, "No System Chosen\n" );
          igstkLogMacro( WARNING, "Initialization finished\n" );
          return FAILURE;
        }

        m_Metrology = new metro_lib::Metrology
        (
          m_MetrologySystems->InitSystem(msil.GetElement(index))
        );

        }

      igstkLogMacro( INFO, "Initialization finished\n" );

      }
    catch(metro_lib::MetroError & e)
      {
      std::cout << HandleMetroError(e);
      igstkLogMacro( INFO, HandleMetroError(e) );
    }

    igstkLogMacro( INFO, m_Mwr.GetText() );
  }

  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   InternalClose
*  ----------------------------------------------------------------------------
*  The "InternalClose" method closes communication with a tracking device.
*  ----------------------------------------------------------------------------
*/
Axios3DTracker::ResultType Axios3DTracker::InternalClose( void )
{
  igstkLogMacro(DEBUG, "igstk::Axios3DTracker::InternalClose called ...\n")

  try
  {
    if(m_Metrology == NULL)
    {
      igstkLogMacro(CRITICAL, "Metrology is not defined")
      return FAILURE;
    }

    igstkLogMacro( INFO, "Closing Systems\n" );

    m_Metrology->Delete();
    delete m_Metrology;
    m_Metrology = 0;
    }
  catch(metro_lib::MetroError & e)
    {
    igstkLogMacro( CRITICAL, HandleMetroError(e) );
    return FAILURE;
    }

  igstkLogMacro( INFO, m_Mwr.GetText() );

  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   InternalReset
*  ----------------------------------------------------------------------------
*  The "InternalReset" method resets tracker to a known configuration.
*  ----------------------------------------------------------------------------
*/
Axios3DTracker::ResultType Axios3DTracker::InternalReset( void )
{
  igstkLogMacro(DEBUG, "igstk::Axios3DTracker::InternalReset called ...\n")

  if (m_Metrology == NULL)
  {
    igstkLogMacro(CRITICAL, "Metrology is not defined")
    return FAILURE;
    }

  try
    {
    igstkLogMacro( INFO, "ReInit:\n" );

    m_Metrology->ReInit();
    igstkLogMacro( INFO, m_Mwr.GetText() );
    }
  catch(metro_lib::MetroError & e)
    {
    igstkLogMacro( WARNING, HandleMetroError(e) );
    return FAILURE;
    }

  igstkLogMacro( INFO,  m_Mwr.GetText() );

  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   InternalStartTracking
*  ----------------------------------------------------------------------------
*  The "InternalStartTracking" method starts tracking.
*  ----------------------------------------------------------------------------
*/
Axios3DTracker::ResultType Axios3DTracker::InternalStartTracking( void )
{
  igstkLogMacro(DEBUG,
         "igstk::Axios3DTracker::InternalStartTracking called ...\n")

  if (m_Tracking)
    {
    return SUCCESS;
    }

  m_LoadedLocators = GetLocatorNames();
  std::cout << "loaded locators count: " << m_LoadedLocators.size() << std::endl;

  m_Tracking = true;

  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   InternalStopTracking
*  ----------------------------------------------------------------------------
*  The "InternalStopTracking" method stops tracking.
*  ----------------------------------------------------------------------------
*/
Axios3DTracker::ResultType Axios3DTracker::InternalStopTracking( void )
{
  igstkLogMacro(DEBUG,
      "igstk::Axios3DTracker::InternalStopTracking called ...\n")

    m_Tracking = false;

  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   InternalUpdateStatus
*  ----------------------------------------------------------------------------
*  The "InternalUpdateStatus" method updates tracker status.
*  ----------------------------------------------------------------------------
*/
Axios3DTracker::ResultType Axios3DTracker::InternalUpdateStatus( void )
{
    igstkLogMacro(DEBUG,
               "igstk::Axios3DTracker::InternalUpdateStatus called ...\n")

  this->m_BufferLock->Lock ();

  TrackerToolsContainerType trackerToolContainer = GetTrackerToolContainer();
  try{
    std::list<std::string>::const_iterator it = m_LoadedLocators.begin();
    for (; it!= m_LoadedLocators.end(); ++it)
      {
      LocatorResult lockResult = m_LocatorResultsContainer[*(it)];

      if(lockResult.m_IsVisible && trackerToolContainer[*(it)])
        {
        // report to the tracker tool that the tracker is Visible
      this->ReportTrackingToolVisible(trackerToolContainer[*(it)]);

        // set the raw transform
        this->SetTrackerToolRawTransform(
                          trackerToolContainer [*(it)], lockResult.m_Transform );

        this->SetTrackerToolTransformUpdate(
                                          trackerToolContainer [*(it)], true );
        }
      }

    this->m_BufferLock->Unlock ();
    }
  catch(metro_lib::MetroError & e)
    {
    igstkLogMacro( WARNING, HandleMetroError(e) );
    return FAILURE;
    }

  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   InternalThreadedUpdateStatus
*  ----------------------------------------------------------------------------
*  The "InternalThreadedUpdateStatus" method updates tracker status.
*  ----------------------------------------------------------------------------
*/
Axios3DTracker::ResultType
Axios3DTracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro(DEBUG,
    "igstk::Axios3DTracker::InternalThreadedUpdateStatus called ...\n")

  std::list<std::string>::const_iterator it = m_LoadedLocators.begin();
  this->m_BufferLock->Lock();

  for (; it!= m_LoadedLocators.end(); ++it)
    {
    MeasureLocator(*(it),false);
    }

  this->m_BufferLock->Unlock();

  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   PrintSelf
*  ----------------------------------------------------------------------------
*  Print the object information in a stream.
*  ----------------------------------------------------------------------------
*/
void Axios3DTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  igstkLogMacro(DEBUG,
               "igstk::Axios3DTracker::PrintSelf called ...\n")

  Superclass::PrintSelf(os, indent);
}

/**----------------------------------------------------------------------------
*   VerifyTrackerToolInformation
*  ----------------------------------------------------------------------------
*  Verify if a tracker tool information is correct before attaching
*  ----------------------------------------------------------------------------
*/
Axios3DTracker::ResultType
Axios3DTracker::VerifyTrackerToolInformation (
     const TrackerToolType* trackerTool)
{
  igstkLogMacro(DEBUG,
    "igstk::Axios3DTracker::VerifyTrackerToolInformation called ...\n")

  if ( trackerTool == NULL )
  {
    igstkLogMacro(CRITICAL, "TrackerTool is not defined")
    return FAILURE;
    }

  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   ValidateSpecifiedFrequency
*  ----------------------------------------------------------------------------
*  The "ValidateSpecifiedFrequency" method checks if the specified
*  ----------------------------------------------------------------------------
*/
Axios3DTracker::ResultType
Axios3DTracker::ValidateSpecifiedFrequency( double frequencyInHz )
{
  igstkLogMacro(DEBUG,
   "igstk::Axios3DTracker::ValidateSpecifiedFrequency called ...\n")

  // manufacturer info: 90 images per second
  const double MAXIMUM_FREQUENCY = 90;
  if ( frequencyInHz < 0.0 || frequencyInHz > MAXIMUM_FREQUENCY )
    {
    return FAILURE;
    }
  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   RemoveTrackerToolFromInternalDataContainers
*  ----------------------------------------------------------------------------
*  This method will remove entries of the traceker tool
*   from internal data containers
*  ----------------------------------------------------------------------------
*/
Axios3DTracker::ResultType
Axios3DTracker::RemoveTrackerToolFromInternalDataContainers(
 const TrackerToolType * trackerTool )
{
  igstkLogMacro(DEBUG,
   "igstk::Axios3DTracker::RemoveTrackerToolFromInternalDataContainers \
   called ...\n")
  // purposely not implemented
  // adding/removing locators (rigid-bodoies) is handled by
  // this->RequestLoadLocatorsXML(...)
  return SUCCESS;
}

/**----------------------------------------------------------------------------
*   AddTrackerToolToInternalDataContainers
*  ----------------------------------------------------------------------------
*  Add tracker tool entry to internal containers
*  ----------------------------------------------------------------------------
*/
Axios3DTracker::ResultType
Axios3DTracker::AddTrackerToolToInternalDataContainers (
   const TrackerToolType * trackerTool)
{
  igstkLogMacro(DEBUG,
  "igstk::Axios3DTracker::AddTrackerToolToInternalDataContainers \
   called ...\n")
  // purposely not implemented
  // adding/removing new locators (rigid-bodoies) is handled by
  // this->RequestLoadLocatorsXML(...)
  return SUCCESS;
}

} // end of namespace igstk
