/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    NeedleBiopsy.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __NeedleBiopsy_h
#define __NeedleBiopsy_h

#include "NeedleBiopsyGUI.h"

#include "igstkCTImageReader.h"
#include "igstkCTImageSpatialObjectRepresentation.h"
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

/** \class NeedleBiopsy
* 
* \brief Implementation class for NeedleBiopsyGUI.
*
* This class implements the main application.
*
*/

class NeedleBiopsy : public NeedleBiopsyGUI
{
public:

  /** Typedefs */
  igstkStandardClassBasicTraitsMacro( NeedleBiopsy, NeedleBiopsyGUI );
  
  igstkLoggerMacro();

  /** typedef for ImageReaderType */
  typedef igstk::CTImageReader                        ImageReaderType;
  typedef ImageReaderType::ImageSpatialObjectType     ImageSpatialObjectType;
  typedef ImageSpatialObjectType::IndexType           IndexType;

  /** typedef for ImageRepresentationType */
  typedef igstk::CTImageSpatialObjectRepresentation   ImageRepresentationType;

  /** typedef for RegistrationType */
  typedef igstk::Landmark3DRegistration               RegistrationType;
  typedef RegistrationType::LandmarkPointContainerType  
                                                   LandmarkPointContainerType;  

  /** Public request methods from the GUI */
  int  RequestLoadImage();
  void ChangeSelectedTPlanPoint();
  void RequestConnectToTracker();
  void RequestDisconnetTracker();
  void ChangeActiveTrackerTool();
  void RequestRegistration();
  void RequestStartTracking();
  void RequestStopTracking();
  void SetTrackerFiducialPoint();

  /** Define observers for event communication */
  igstkObserverObjectMacro( CTImage,igstk::CTImageReader::ImageModifiedEvent,
                                                 igstk::CTImageSpatialObject);

  igstkObserverMacro( Registration, igstk::CoordinateSystemTransformToEvent,
    igstk::CoordinateSystemTransformToResult );

  igstkObserverMacro( RegistrationError, igstk::DoubleTypeEvent, double );

  igstkObserverMacro( SliceBounds, igstk::IntegerBoundsEvent, 
                                  igstk::EventHelperType::IntegerBoundsType );
  NeedleBiopsy();
  virtual ~NeedleBiopsy();

private:

  NeedleBiopsy(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented
  
  /** DICOM image reader */
  ImageReaderType::Pointer                              m_ImageReader;
  std::string                                           m_ImageDir;
  std::string                                           m_PlanFilename;
  igstk::TreatmentPlan                                * m_Plan;

  /** Pointer to the CTImageSpatialObject */
  ImageSpatialObjectType::Pointer                       m_ImageSpatialObject;

  /** Define a initial world coordinate system */
  igstk::AxesObject::Pointer                            m_WorldReference;

  /** Slice representations of the image in View2D and View3D */
  std::vector< ImageRepresentationType::Pointer >       m_ImageRepresentation;

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

  /** Ellipsoid spatial object, used to represent 
   *  the landmark point, tip of the probe. */
  typedef igstk::EllipsoidObject                  EllipsoidType;
  typedef igstk::EllipsoidObjectRepresentation    EllipsoidRepresentationType;
  EllipsoidType::Pointer                          m_NeedleTip;
  EllipsoidRepresentationType::Pointer            m_NeedleTipRepresentation;

  /** Objects for path planning and fiducial selection */
  EllipsoidType::Pointer                          m_TargetPoint;
  EllipsoidRepresentationType::Pointer            m_TargetRepresentation;
  EllipsoidType::Pointer                          m_EntryPoint;
  EllipsoidRepresentationType::Pointer            m_EntryRepresentation;  
  EllipsoidType::Pointer                          m_FiducialPoint;
  EllipsoidRepresentationType::Pointer            m_FiducialRepresentation;  
  
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
  CylinderRepresentationType::Pointer             m_NeedleRepresentation;

  /** Annotation is used for displaying 2D texts on View */
  igstk::Annotation2D::Pointer                    m_Annotation;

  /** File for logger */
  std::ofstream                                   m_LogFile;

  /** Utility functions, conversion between points and transform */
  inline 
  igstk::Transform 
  PointToTransform( ImageSpatialObjectType::PointType point)
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
  ImageSpatialObjectType::PointType 
  TransformToPoint( igstk::Transform transform)
  {
    ImageSpatialObjectType::PointType point;
    for (unsigned int i=0; i<3; i++)
      {
      point[i] = transform.GetTranslation()[i];
      }
    return point;
    }

  /** Internal functions */
  void ReadTreatmentPlan();
  void WriteTreatmentPlan();
  void ConnectImageRepresentation();
  void UpdateTrackerAndTrackerToolList();
  void UpdateFiducialPoint();
  void UpdatePath();  
  void ResliceImage( IndexType index ); 

  /** Callback functions for picking and reslicing image events. */
  void Picking( const itk::EventObject & event );
  void ResliceImage( const itk::EventObject & event );
  void RequestInitializeTracker( const itk::EventObject & event );
  void Tracking( const itk::EventObject & event ); 
  void NullAction(const itk::EventObject & itkNotUsed(event) ){};

};

#endif
