/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    UltrasoundGuidedRFAImplementation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __UltrasoundGuidedRFAImplementation_h
#define __UltrasoundGuidedRFAImplementation_h

#if defined(_MSC_VER)
  // Warning about: identifier was truncated to '255' characters 
  // in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "igstkSandboxConfigure.h"
#include "igstkMR3DImageToUS3DImageRegistration.h"
#include "UltrasoundGuidedRFA.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkPolarisTracker.h"
#include "igstkFlockOfBirdsTracker.h"
#include "igstkUltrasoundProbeObjectRepresentation.h"
#include "igstkAxesObjectRepresentation.h"
#include "igstkMouseTracker.h"
#include "igstkMeshReader.h"
#include "igstkMeshObject.h"
#include "igstkMeshObjectRepresentation.h"
#include "igstkObliqueContourMeshObjectRepresentation.h"
#include "igstkContourVascularNetworkObjectRepresentation.h"
#include "igstkVascularNetworkReader.h"
#include "igstkVascularNetworkObject.h"
#include "igstkVascularNetworkObjectRepresentation.h"
#include "igstkMRImageReader.h"
#include "igstkMRImageSpatialObject.h"
#include "igstkMRImageSpatialObjectRepresentation.h"
#include "igstkUSImageReader.h"
#include "igstkUSImageObjectRepresentation.h"
#include "igstkObliqueImageSpatialObjectRepresentation.h"
#include "igstkUltrasoundImageSimulator.h"
#include "igstkView2D.h"
#include "igstkView3D.h"

#include "FL/Fl_File_Chooser.H"

#include "igstkSerialCommunication.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

//#define UGRFA_USE_FOB


class UltrasoundGuidedRFAImplementation : public UltrasoundGuidedRFA
{
public:
  /** Typedefs */
  igstkStandardClassBasicTraitsMacro( UltrasoundGuidedRFAImplementation, 
                                      UltrasoundGuidedRFA );
  igstkLoggerMacro();

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

  igstkObserverObjectMacro(MRImage,igstk::MRImageReader::ImageModifiedEvent,
                                                  igstk::MRImageSpatialObject)
  igstkObserverObjectMacro(USImage,igstk::USImageReader::ImageModifiedEvent,
                                                  igstk::USImageObject)

  igstkObserverMacro(USImageTransform, 
    igstk::CoordinateSystemTransformToEvent,
    igstk::CoordinateSystemTransformToResult)

  igstkObserverMacro(RegistrationTransform,
    igstk::CoordinateSystemTransformToEvent,
    igstk::CoordinateSystemTransformToResult )

public:

  typedef itk::StdStreamLogOutput       LogOutputType;

#ifdef UGRFA_USE_FOB
  typedef igstk::FlockOfBirdsTracker       TrackerType;
  typedef igstk::FlockOfBirdsTrackerTool   TrackerToolType;
  #define TRACKER_TOOL_PORT 0
#else
  typedef igstk::PolarisTracker            TrackerType;
  typedef igstk::PolarisTrackerTool        TrackerToolType;
  #define TRACKER_TOOL_PORT 0
#endif

  typedef igstk::MeshReader             LiverReaderType;
  typedef igstk::VascularNetworkReader  VascularNetworkReaderType;
  typedef igstk::VascularNetworkObject  VascularNetworkType;
  typedef igstk::VascularNetworkObjectRepresentation
                                        VascularNetworkRepresentationType;

  typedef igstk::MRImageSpatialObject               MRImageType;
  typedef igstk::MRImageSpatialObjectRepresentation MRImageRepresentationType;
  typedef igstk::MRImageReader                      MRImageReaderType;

  typedef igstk::UltrasoundImageSimulator<MRImageType> USSimulatorType;

  /** Registration typedefs */
  typedef igstk::MR3DImageToUS3DImageRegistration   RegistrationType;

  igstkObserverObjectMacro(SimulatedUSImage,
                       USSimulatorType::ImageModifiedEvent,igstk::USImageObject)


  typedef igstk::ObliqueImageSpatialObjectRepresentation<MRImageType>
                                             MRObliqueImageRepresentationType;

  typedef igstk::USImageObject                      USImageType;
  typedef igstk::USImageObjectRepresentation        USImageRepresentationType;
  typedef igstk::USImageReader                      USImageReaderType;

  typedef igstk::View2D                             View2DType;
  typedef igstk::View3D                             View3DType;

  typedef igstk::SerialCommunication                CommunicationType;

public:

  void EnableTracking();  
  void DisableTracking();
  void AddProbe( igstk::UltrasoundProbeObjectRepresentation 
                                                   * cylinderRepresentation );

  void AddAxes( igstk::AxesObjectRepresentation * cylinderRepresentation );

  void AttachObjectToTrack( igstk::SpatialObject * objectToTrack );

  void LoadLiverSurface();
  void LoadLiverVasculature();
  void LoadLiverImage();
  void Load2DUltrasound();
  void Quit();

  void SetSliceNumber(unsigned int value);

  void Randomize();

  bool HasQuitted() {return m_HasQuitted;}

  /** Methods for Converting Events into State Machine Inputs */
  igstkLoadedEventTransductionMacro( AxialSliceBounds,
                                     AxialBounds );

  UltrasoundGuidedRFAImplementation();
  ~UltrasoundGuidedRFAImplementation();

private:

  LogOutputType::Pointer                          m_LogOutput;
  TrackerType::Pointer                            m_Tracker;
  TrackerToolType::Pointer                        m_TrackerTool;
  CommunicationType::Pointer                      m_Communication;

  View2DType::Pointer                             m_View2D;
  View3DType::Pointer                             m_View3D;

  
  LiverReaderType::Pointer                        m_MeshReader;
  igstk::MeshObjectRepresentation::Pointer        m_LiverRepresentation;
  igstk::ObliqueContourMeshObjectRepresentation::Pointer 
                                                  m_ContourLiverRepresentation;
  
  igstk::ContourVascularNetworkObjectRepresentation::Pointer 
                                        m_ContourVascularNetworkRepresentation;

  VascularNetworkReaderType::Pointer          m_VascularNetworkReader;
  VascularNetworkRepresentationType::Pointer  m_VascularNetworkRepresentation;
  MRImageReaderType::Pointer                  m_MRImageReader;
  MRImageType::Pointer                        m_LiverMR;
  MRImageRepresentationType::Pointer          m_LiverMRRepresentation;
  MRObliqueImageRepresentationType::Pointer   m_ObliqueLiverMRRepresentation;
  //MRImageRepresentationType::Pointer m_ObliqueLiverMRRepresentation;

  USImageReaderType::Pointer         m_USImageReader;
  USImageType::Pointer               m_LiverUS;
  USImageRepresentationType::Pointer m_LiverUSRepresentation;

  bool                      m_Tracking;
  std::ofstream             m_LogFile;
  bool                      m_HasQuitted;

  USSimulatorType::Pointer          m_USSimulator;

  MRObliqueImageRepresentationType::PointType m_ObliquePoint;
  
  void SetAxialSliderBoundsProcessing();
  igstkDeclareInputMacro( AxialBounds );
  igstkDeclareStateMacro( Initial );

  RegistrationType::Pointer m_Registration;

  igstkObserverObjectMacro(VascularNetwork,
    igstk::VascularNetworkReader::VascularNetworkObjectModifiedEvent,
    igstk::VascularNetworkObject)
};


#endif
