/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    UltrasoundGuidedRFAImplementation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

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
#include "UltrasoundGuidedRFA.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkAuroraTracker.h"
#include "igstkFlockOfBirdsTracker.h"
#include "igstkUltrasoundProbeObjectRepresentation.h"
#include "igstkAxesObjectRepresentation.h"
#include "igstkMouseTracker.h"
#include "igstkMeshReader.h"
#include "igstkMeshObject.h"
#include "igstkMeshObjectRepresentation.h"
#include "igstkContourMeshObjectRepresentation.h"
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

#include "FL/Fl_File_Chooser.H"

#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace igstk
{

class UltrasoundGuidedRFAImplementation : public UltrasoundGuidedRFA,
                                          public igstk::Object
{
public:
  /** Typedefs */
  igstkStandardClassBasicTraitsMacro( UltrasoundGuidedRFAImplementation, 
                                      UltrasoundGuidedRFA );
  igstkNewMacro( Self );

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

public:
    
  typedef itk::Logger                   LoggerType; 
  typedef itk::StdStreamLogOutput       LogOutputType;
  typedef igstk::FlockOfBirdsTracker    TrackerType;
  typedef igstk::MeshReader             LiverReaderType;
  typedef igstk::VascularNetworkReader  VascularNetworkReaderType;
  typedef igstk::VascularNetworkObject  VascularNetworkType;
  typedef igstk::VascularNetworkObjectRepresentation
                                        VascularNetworkRepresentationType;

  typedef igstk::MRImageSpatialObject               MRImageType;
  typedef igstk::MRImageSpatialObjectRepresentation MRImageRepresentationType;
  typedef igstk::MRImageReader                      MRImageReaderType;

  typedef igstk::ObliqueImageSpatialObjectRepresentation<MRImageType>
                                             MRObliqueImageRepresentationType;

  typedef igstk::USImageObject                      USImageType;
  typedef igstk::USImageObjectRepresentation        USImageRepresentationType;
  typedef igstk::USImageReader                      USImageReaderType;

#ifdef WIN32
  typedef igstk::SerialCommunicationForWindows  CommunicationType;
#else
  typedef igstk::SerialCommunicationForPosix    CommunicationType;
#endif

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
  igstkLoadedEventTransductionMacro( AxialSliceBoundsEvent,
                                     AxialBoundsInput, AxialBounds );

private:

  UltrasoundGuidedRFAImplementation();
  ~UltrasoundGuidedRFAImplementation();

  LoggerType::Pointer        m_Logger;
  LogOutputType::Pointer     m_LogOutput;
  TrackerType::Pointer       m_Tracker;
  CommunicationType::Pointer m_Communication;
  LiverReaderType::Pointer   m_MeshReader;
  MeshObjectRepresentation::Pointer m_LiverRepresentation;
  ContourMeshObjectRepresentation::Pointer m_ContourLiverRepresentation;
  ContourVascularNetworkObjectRepresentation::Pointer 
                                       m_ContourVascularNetworkRepresentation;
  VascularNetworkReaderType::Pointer m_VascularNetworkReader;
  VascularNetworkRepresentationType::Pointer  m_VascularNetworkRepresentation;
   
  MRImageReaderType::Pointer   m_MRImageReader;
  MRImageType::Pointer         m_LiverMR;
  MRImageRepresentationType::Pointer m_LiverMRRepresentation;
  //MRObliqueImageRepresentationType::Pointer m_ObliqueLiverMRRepresentation;
  MRImageRepresentationType::Pointer m_ObliqueLiverMRRepresentation;

  USImageReaderType::Pointer         m_USImageReader;
  USImageType::Pointer               m_LiverUS;
  USImageRepresentationType::Pointer m_LiverUSRepresentation;

  bool                      m_Tracking;
  std::ofstream             m_LogFile;
  bool                      m_HasQuitted;

  MRObliqueImageRepresentationType::PointType m_ObliquePoint;
  
  void SetAxialSliderBoundsProcessing();
  igstkDeclareInputMacro( AxialBounds );
  igstkDeclareStateMacro( Initial );
};


} // end of namespace

#endif
