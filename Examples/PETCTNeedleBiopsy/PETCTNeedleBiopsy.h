/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    PETCTNeedleBiopsy.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __PETCTNeedleBiopsy_h
#define __PETCTNeedleBiopsy_h

#include "PETCTNeedleBiopsyGUI.h"

#include "igstkPETImageReader.h"
#include "igstkPETImageSpatialObject.h"

#include "igstkTransformObserver.h"
#include "igstkCTImageReader.h"
#include "igstkCTImageSpatialObjectRepresentation.h"
#include "igstkImageSliceRepresentation.h"
#include "igstkImageSliceRepresentationPlus.h"
#include "igstkTracker.h"
#include "igstkTrackerTool.h"
#include "igstkLandmark3DRegistration.h"

#include "igstkEllipsoidObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkTubeObject.h"
#include "igstkTubeObjectRepresentation.h"
#include "igstkCylinderObject.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkAnnotation2D.h"
#include "itkStdStreamLogOutput.h"
#include "igstkAxesObject.h"

#include "igstkTreatmentPlanIO.h"
#include "igstkTrackerConfigurationGUIBase.h"
#include "igstkTrackerInitializer.h"
#include "igstkCoordinateSystemTransformToResult.h"

#include "vtkSmartPointer.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkLookupTable.h"

/** \class PETCTNeedleBiopsy
*
* \brief Implementation class for PETCTNeedleBiopsyGUI.
*
* This class implements the main application.
*
*/

class PETCTNeedleBiopsy : public PETCTNeedleBiopsyGUI
{
public:

  /** Typedefs */
  igstkStandardClassBasicTraitsMacro( PETCTNeedleBiopsy, PETCTNeedleBiopsyGUI );

  igstkLoggerMacro();

  /** typedef for CTImageReaderType */
  typedef igstk::CTImageReader                          CTImageReaderType;
  typedef CTImageReaderType::ImageSpatialObjectType     CTImageSOType;
  typedef CTImageSOType::IndexType                      IndexType;

  /** typedef for PETImageReaderType */
  typedef igstk::PETImageReader                         PETImageReaderType;
  typedef PETImageReaderType::ImageSpatialObjectType    PETImageSOType;

  /** typedef for ImageRepresentationType */
  typedef igstk::ImageSliceRepresentationPlus<CTImageSOType, CTImageSOType>
                                                    CTCTRepresentationType;
  typedef igstk::ImageSliceRepresentationPlus<CTImageSOType, PETImageSOType>
                                                    CTPETRepresentationType;

  /** typedef for RegistrationType */
  typedef igstk::Landmark3DRegistration               RegistrationType;
  typedef RegistrationType::LandmarkPointContainerType
                                                   LandmarkPointContainerType;

  /** Public request methods from the GUI */
  int  RequestLoadCTImage( int ct );
  void ChangeSelectedCTImage ( int ct );
  void RequestCT2CTRegistration();
  void RequestChangeDisplayMode();
  void RequestChangeTransparency();
  void RequestChangeWindowLevel();
  //int  RequestLoadPETCTImage();
  int  RequestLoadPETImage();
  void ChangeSelectedCTImage();
  void ChangeSelectedTPlanPoint();
  void RequestConnectToTracker();
  //void RequestDisconnetTracker();
  //void ChangeActiveTrackerTool();
  void RequestRegistration();
  void RequestStartTracking();
  void RequestStopTracking();
  void SetTrackerFiducialPoint();

  /** Define observers for event communication */
  igstkObserverObjectMacro( CTImage,CTImageReaderType::ImageModifiedEvent,
                                                 CTImageSOType);
  igstkObserverObjectMacro( PETImage,PETImageReaderType::ImageModifiedEvent,
                                                 PETImageSOType);

  igstkObserverMacro( Registration, igstk::CoordinateSystemTransformToEvent,
    igstk::CoordinateSystemTransformToResult );

  igstkObserverMacro( RegistrationError, igstk::DoubleTypeEvent, double );

  PETCTNeedleBiopsy();
  virtual ~PETCTNeedleBiopsy();

private:

  PETCTNeedleBiopsy(const Self&); // purposely not implemented
  void operator=(const Self&);    // purposely not implemented

  /** DICOM image reader */
  CTImageReaderType::Pointer                            m_CTImageReader;
  PETImageReaderType::Pointer                           m_PETImageReader;
  std::string                                           m_ImageDir;
  std::string                                           m_PlanFilename;
  igstk::TreatmentPlan                                * m_Plan;
  std::string                                           m_PETCTPlanFilename;
  igstk::TreatmentPlan                                * m_PETCTPlan;
  std::string                                           m_CTPlanFilename;
  igstk::TreatmentPlan                                * m_CTPlan;
  CTImageSOType::PointType                              m_ImageCenter;

  /** Pointer to the CTImageSpatialObject */
  CTImageSOType::Pointer                                m_ImageSpatialObject;    //Temp holder
  CTImageSOType::Pointer                                m_CTImageSpatialObject;
  CTImageSOType::Pointer                                m_PETCTImageSpatialObject;
  CTImageSOType::Pointer                                m_FGImageSpatialObject;  //Temp holder
  PETImageSOType::Pointer                               m_PETImageSpatialObject;
  igstk::Transform                                      m_CT2CTTransform;

  /** Define a initial world coordinate system */
  igstk::AxesObject::Pointer                            m_WorldReference;

  /** Slice representations of the image in View2D and View3D */
  std::vector< CTCTRepresentationType::Pointer >       m_CTCTImageRepresentation;
  std::vector< CTPETRepresentationType::Pointer >      m_CTPETImageRepresentation;
  vtkWindowLevelLookupTable *                          m_LUT;



  /** Landmark registration and its landmark points container */
  RegistrationType::Pointer                       m_LandmarkRegistration;
  LandmarkPointContainerType                      m_ImageLandmarksContainer;
  LandmarkPointContainerType                      m_TrackerLandmarksContainer;

  /** Pointers for TrackerInitializer and related objects */
  igstk::Tracker::Pointer                         m_Tracker;
  igstk::TrackerConfigurationGUIBase            * m_TrackerConfigurationGUI;
  igstk::TrackerInitializer                     * m_TrackerInitializer;
  std::vector< igstk::TrackerInitializer * >      m_TrackerInitializerList;
  std::vector< igstk::TrackerTool::Pointer >      m_TrackerToolList;
  igstk::TrackerTool::Pointer                     m_ActiveTool;

  /** Observer type for loaded event,
   *  the callback can be set to a member function. */
  typedef itk::ReceptorMemberCommand < Self > LoadedObserverType;
  LoadedObserverType::Pointer               m_ViewPickerObserver;
  LoadedObserverType::Pointer               m_ViewResliceObserver;
  LoadedObserverType::Pointer               m_TrackerConfigurationObserver;
  LoadedObserverType::Pointer               m_TrackerToolUpdateObserver;
  igstk::TransformObserver::Pointer         m_TransformObserver;

  /** Ellipsoid spatial object, used to represent
   *  the landmark point, tip of the probe. */
  typedef igstk::EllipsoidObject                  EllipsoidType;
  typedef igstk::EllipsoidObjectRepresentation    EllipsoidRepresentationType;
  EllipsoidType::Pointer                          m_NeedleTip;
  std::vector<EllipsoidRepresentationType::Pointer>  m_NeedleTipRepresentation;
  EllipsoidType::Pointer                          m_NeedleHub;
  EllipsoidRepresentationType::Pointer            m_NeedleHubRepresentation;

  /** Objects for path planning and fiducial selection */
  EllipsoidType::Pointer                          m_TargetPoint;
  EllipsoidType::Pointer                          m_TargetProjection;
  std::vector<EllipsoidRepresentationType::Pointer>     m_TargetRepresentation;

  EllipsoidType::Pointer                          m_EntryPoint;
  EllipsoidRepresentationType::Pointer            m_EntryRepresentation;
  EllipsoidType::Pointer                          m_FiducialPoint;
  std::vector<EllipsoidRepresentationType::Pointer>    m_FiducialRepresentation;

  /** Tube object represents the planned path */
  typedef igstk::TubeObject                       PathType;
  typedef igstk::TubeObjectRepresentation         PathRepresentationType;
  typedef igstk::TubeObject::PointType            TubePointType;
  PathType::Pointer                               m_Path;
  std::vector< PathRepresentationType::Pointer >  m_PathRepresentation;

  /** Cylinder spatial object, used to represent the probe */
  typedef igstk::CylinderObject                   CylinderType;
  typedef igstk::CylinderObjectRepresentation     CylinderRepresentationType;
  CylinderType::Pointer                           m_Needle;
  std::vector<CylinderRepresentationType::Pointer>      m_NeedleRepresentation;
  CylinderType::Pointer                           m_VirtualTip;
  std::vector<CylinderRepresentationType::Pointer>      m_VirtualTipRepresentation;

  /** Annotation is used for displaying 2D texts on View */
  igstk::Annotation2D::Pointer                    m_Annotation;

  /** File for logger */
  std::ofstream                                   m_LogFile;

  /** Utility functions, conversion between points and transform */
  inline
  igstk::Transform
  PointToTransform( CTImageSOType::PointType point)
  {
    igstk::Transform transform;
    igstk::Transform::VectorType translation;
    for (unsigned int i=0; i<3; i++)
      {
      translation[i] = point[i];
      }
    transform.SetTranslation( translation, 0.1,
                                igstk::TimeStamp::GetLongestPossibleTime() );
    return transform;
  }

  inline
  CTImageSOType::PointType
  TransformToPoint( igstk::Transform transform)
  {
    CTImageSOType::PointType point;
    for (unsigned int i=0; i<3; i++)
      {
      point[i] = transform.GetTranslation()[i];
      }
    return point;
    }

  /** Internal functions */
  void ReadTreatmentPlan();
  void WriteTreatmentPlan();
  void ConnectImageRepresentation( int pet);
  //void UpdateTrackerAndTrackerToolList();
  void UpdateFiducialPoint();
  void UpdatePath();
  void ResliceImage( IndexType index );
  void ObliqueResliceImage( igstk::Transform transform, double virtualTip );
  void ResetSliders();

  /** Callback functions for picking and reslicing image events. */
  void Picking( const itk::EventObject & event );
  void ResliceImage( const itk::EventObject & event );
  void UpdateVirtualTip( const itk::EventObject & event );
  void RequestInitializeTracker( const itk::EventObject & event );
  void Tracking( const itk::EventObject & event );
  void NullAction(const itk::EventObject & itkNotUsed(event) ){};

};

#endif
