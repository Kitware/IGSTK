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

//#include "igstkDICOMGenericImageReader.h" // Not yet functional
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

#include "PolarisTrackerConfigurationGUI.h"
#include "igstkTrackerInitializer.h"

class NeedleBiopsy : public NeedleBiopsyGUI
{
public:
  /** Typedefs */
  igstkStandardClassBasicTraitsMacro( NeedleBiopsy, NeedleBiopsyGUI );
  
  igstkLoggerMacro();

  /** typedef for ImageReaderType */
  //typedef igstk::DICOMGenericImageReader                ImageReaderType;
  typedef igstk::CTImageReader                          ImageReaderType;
  typedef ImageReaderType::ImageSpatialObjectType       ImageSpatialObjectType;
  typedef ImageSpatialObjectType::IndexType             IndexType;

  /** typedef for ImageRepresentationType */
  typedef igstk::CTImageSpatialObjectRepresentation     ImageRepresentationType;

  /** typedef for RegistrationType */
  typedef igstk::Landmark3DRegistration                 RegistrationType;
  //typedef RegistrationType::TransformType               TransformType;
  typedef RegistrationType::LandmarkPointContainerType  
                                                    LandmarkPointContainerType;  

  /** Public request methods from the GUI. */
  int  RequestLoadImage();
  void ChangeSelectedTPlanPoint();
  void RequestConnectToTracker();
  
  void RequestRegistration();

  igstkObserverObjectMacro( CTImage,igstk::CTImageReader::ImageModifiedEvent,
                                                   igstk::CTImageSpatialObject);

  igstkObserverMacro( Registration, igstk::CoordinateSystemTransformToEvent,
    igstk::CoordinateSystemTransformToResult );

  igstkObserverMacro( RegistrationError, igstk::DoubleTypeEvent, double );

  igstkObserverMacro( SliceBounds, igstk::IntegerBoundsEvent, 
                                    igstk::EventHelperType::IntegerBoundsType );

  /*
    igstkObserverMacro( Reslicing, igstk::QuadrantViews::ReslicingEvent, 
                                      igstk::QuadrantViews::SliceNumberType );*/
                                        
  
  NeedleBiopsy();
  virtual ~NeedleBiopsy();

private:

  NeedleBiopsy(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented
  
  /** DICOM image reader */
  ImageReaderType::Pointer            m_ImageReader;
  std::string                         m_ImageDir;
  std::string                         m_PlanFilename;
  igstk::TreatmentPlan              * m_Plan;

  /** Pointer to the CTImageSpatialObject */
  ImageSpatialObjectType::Pointer     m_ImageSpatialObject;

  /** Define a initial world coordinate system */
  igstk::AxesObject::Pointer          m_WorldReference;

  /** Slice representations of the image in View2D and View3D */
  std::vector< ImageRepresentationType::Pointer > m_ImageRepresentation;

  /** Landmark registration and its landmark points container */
  RegistrationType::Pointer           m_LandmarkRegistration;
  LandmarkPointContainerType          m_ImageLandmarksContainer;
  LandmarkPointContainerType          m_TrackerLandmarksContainer;

  /** To store the landmark registration result transform */
  igstk::Transform                    m_ImageToTrackerTransform;  
  
  igstk::Tracker::Pointer             m_Tracker;
  igstk::TrackerConfigurationGUIBase  * m_TrackerConfigurationGUI;

  /** Observer type for loaded event, 
   *  the callback can be set to a member function. */
  typedef itk::ReceptorMemberCommand < Self > LoadedObserverType;
  LoadedObserverType::Pointer               m_ViewPickerObserver;
  LoadedObserverType::Pointer               m_ViewResliceObserver;
  LoadedObserverType::Pointer               m_TrackerConfigurationObserver;

  /** Ellipsoid spatial object, used to represent 
   *  the landmark point, tip of the probe. */
  typedef igstk::EllipsoidObject                  EllipsoidType;
  typedef igstk::EllipsoidObjectRepresentation    EllipsoidRepresentationType;
  EllipsoidType::Pointer                          m_NeedleTip;
  EllipsoidRepresentationType::Pointer            m_NeedleTipRepresentation;

  /** Objects for path planning */
  EllipsoidType::Pointer                          m_TargetPoint;
  EllipsoidRepresentationType::Pointer            m_TargetRepresentation;
  EllipsoidType::Pointer                          m_EntryPoint;
  EllipsoidRepresentationType::Pointer            m_EntryRepresentation;  
  EllipsoidType::Pointer                          m_FiducialPoint;
  EllipsoidRepresentationType::Pointer            m_FiducialRepresentation;  
    
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

  //std::vector <igstk::Annotation2D::Pointer >     m_Annotation;
  igstk::Annotation2D::Pointer     m_Annotation;


  /** Utility functions */
  inline igstk::Transform PointToTransform( ImageSpatialObjectType::PointType point)
  {
    igstk::Transform transform;
    igstk::Transform::VectorType translation;
    for (int i=0; i<3; i++)
    {
      translation[i] = point[i];
    }
    transform.SetTranslation( translation, 0.1, igstk::TimeStamp::GetLongestPossibleTime() );
    return transform;
  }

  inline ImageSpatialObjectType::PointType TransformToPoint( igstk::Transform transform)
  {
    ImageSpatialObjectType::PointType point;
    for (int i=0; i<3; i++)
    {
      point[i] = transform.GetTranslation()[i];
    }
    return point;
  }

  void ReadTreatmentPlan();
  void WriteTreatmentPlan();
  void ConnectImageRepresentation();

  /** Callback functions for picking and reslicing image events. */
  void Picking( const itk::EventObject & event );
  void ResliceImage( const itk::EventObject & event );
  void RequestInitializeTracker( const itk::EventObject & event );

  void UpdateFiducialPoint();
  void UpdatePath();  
  void ResliceImage( IndexType index );  
  void NullAction(const itk::EventObject & event ){};

};

#endif
