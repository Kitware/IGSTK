/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkStateMachineExportTest.cxx
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
// Warning about: identifier was truncated to '255' characters in the debug
// information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

/**
 *  This file generates the State Machine diagram of all the calsses by
 *  invoking their respective Export method.
 */

#include <fstream>

#include "igstkSystemInformation.h"
#include "igstkConfigure.h"
#include "igstkObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkUltrasoundProbeObjectRepresentation.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkPulseGenerator.h"
#include "igstkTracker.h"
#include "igstkTrackerTool.h"
#include "igstkPolarisTracker.h"
#include "igstkPolarisTrackerTool.h"
#include "igstkAuroraTracker.h"
#include "igstkAuroraTrackerTool.h"
#include "igstkSpatialObject.h"
#include "igstkSpatialObjectReader.h"
#include "igstkCTImageReader.h"
#include "igstkMRImageReader.h"
#include "igstkMeshReader.h"
#include "igstkTubeReader.h"
#include "igstkLandmark3DRegistration.h"
#include "igstkLandmark3DRegistrationErrorEstimator.h"
#include "igstkImageSpatialObject.h"
#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkImageReader.h"
#include "igstkDICOMImageReader.h"
#include "igstkCTImageSpatialObject.h"
#include "igstkCTImageSpatialObjectRepresentation.h"
#include "igstkSerialCommunication.h"
#include "igstkMRImageSpatialObject.h"
#include "igstkMRImageSpatialObjectRepresentation.h"
#include "igstkAxesObjectRepresentation.h"
#include "igstkBoxObjectRepresentation.h"
#include "igstkConeObjectRepresentation.h"
#include "igstkTubeObjectRepresentation.h"
#include "igstkMeshObjectRepresentation.h"
#include "igstkRealTimeClock.h"
#include "igstkMR3DImageToUS3DImageRegistration.h"
#include "igstkView.h"
#include "igstkView2D.h"
#include "igstkView3D.h"
#include "igstkCoordinateSystemDelegator.h"

#include "igstkPETImageReader.h"
#include "igstkPETImageSpatialObject.h"
#include "igstkPETImageSpatialObjectRepresentation.h"

#include "igstkReslicerPlaneSpatialObject.h"
#include "igstkImageResliceObjectRepresentation.h"
#include "igstkMeshResliceObjectRepresentation.h"

#if defined(IGSTK_USE_MicronTracker)
#include "igstkMicronTracker.h"
#include "igstkMicronTrackerTool.h"
#endif

#if defined(IGSTK_USE_Ascension3DGTracker)
#include "igstkAscension3DGTracker.h"
#include "igstkAscension3DGTrackerTool.h"
#endif

#if defined(IGSTK_USE_NDICertusTracker)
#include "igstkNDICertusTracker.h"
#include "igstkNDICertusTrackerTool.h"
#endif



#if defined(IGSTK_USE_FLTK)
#include "igstkFLTKWidget.h"
#endif

#if defined(WIN32) || defined(_WIN32)
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif

#include "igstkMouseTracker.h"
#include "igstkAuroraTracker.h"
#include "igstkPolarisTracker.h"
#include "igstkAuroraTrackerTool.h"
#include "igstkPolarisTrackerTool.h"

namespace igstk
{
class DummyTrackerTool : public igstk::TrackerTool
{
public:
  /** Macro with standard traits declarations. */
  igstkStandardClassBasicTraitsMacro( DummyTrackerTool, TrackerTool ) 
  igstkNewMacro( DummyTrackerTool )

private:
  typedef ::igstk::StateMachine< Self > StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer   ActionType; 
  typedef StateMachineType::StateType                StateType; 
  typedef StateMachineType::InputType                InputType;
  typedef StateMachineType::OutputStreamType OutputStreamType;
  igstkFriendClassMacro( ::igstk::StateMachine< Self > );
  StateMachineType     m_StateMachine; 
  typedef ::itk::ReceptorMemberCommand< Self >   ReceptorObserverType;
  typedef ReceptorObserverType::Pointer          ReceptorObserverPointer;

public:
  void ExportStateMachineDescription( OutputStreamType & ostr, bool skipLoops ) const
    {
    this->TrackerTool::ExportStateMachineDescription( ostr, skipLoops );
    }

  void ExportStateMachineDescriptionToLTS( OutputStreamType & ostr, bool skipLoops ) const
    {
    this->TrackerTool::ExportStateMachineDescriptionToLTS( ostr, skipLoops );
    }

  void ExportStateMachineDescriptionToSCXML( OutputStreamType & ostr, bool skipLoops ) const
    {
    this->TrackerTool::ExportStateMachineDescriptionToSCXML( ostr, skipLoops );
    }
protected:
  DummyTrackerTool():m_StateMachine(this)
    {
    }
  ~DummyTrackerTool()
    {
    }

  /** Check if the tracker tool is configured or not. This method should
   *  be implemented in the derived classes*/
  virtual bool CheckIfTrackerToolIsConfigured( ) const { return true; }
};

class DummyTracker : public Tracker
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassBasicTraitsMacro( DummyTracker, Tracker ) 
  igstkNewMacro( DummyTracker )

private:
  typedef ::igstk::StateMachine< Self > StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer   ActionType; 
  typedef StateMachineType::StateType                StateType; 
  typedef StateMachineType::InputType                InputType;
  typedef StateMachineType::OutputStreamType OutputStreamType;
  igstkFriendClassMacro( ::igstk::StateMachine< Self > );
  StateMachineType     m_StateMachine; 
  typedef ::itk::ReceptorMemberCommand< Self >   ReceptorObserverType;
  typedef ReceptorObserverType::Pointer          ReceptorObserverPointer;

public:
  void ExportStateMachineDescription( OutputStreamType & ostr, bool skipLoops ) const
    {
    this->Tracker::ExportStateMachineDescription( ostr, skipLoops );
    }

  void ExportStateMachineDescriptionToLTS( OutputStreamType & ostr, bool skipLoops ) const
    {
    this->Tracker::ExportStateMachineDescriptionToLTS( ostr, skipLoops );
    }

  void ExportStateMachineDescriptionToSCXML( OutputStreamType & ostr, bool skipLoops ) const
    {
    this->Tracker::ExportStateMachineDescriptionToSCXML( ostr, skipLoops );
    }

  typedef Superclass::TransformType           TransformType;
  typedef Superclass::ResultType              ResultType;


protected:

  DummyTracker():m_StateMachine(this)
    {
    }

  ~DummyTracker()
    {
    }

  ResultType InternalOpen( void )
    {
    return SUCCESS;
    }

  ResultType InternalStartTracking( void )
    {
    return SUCCESS;
    }

  ResultType InternalReset( void )
    {
    return SUCCESS;
    }

  ResultType InternalStopTracking( void )
    {
    return SUCCESS;
    }

  ResultType InternalClose( void )
    {
    return SUCCESS;
    }

  ResultType
  VerifyTrackerToolInformation( const TrackerToolType * itkNotUsed(trackerTool) )
    {
    return SUCCESS;
    }

  ResultType
  AddTrackerToolToInternalDataContainers( const TrackerToolType * itkNotUsed(trackerTool) )
    {
    return SUCCESS;
    }

  ResultType
  RemoveTrackerToolFromInternalDataContainers(
    const TrackerToolType * itkNotUsed(trackerTool) )
    {
    return SUCCESS;
    }

  ResultType
  InternalUpdateStatus( void )
    {
    igstkLogMacro( DEBUG, "DummyTracker::InternalUpdateStatus called ...\n");
    return SUCCESS;
    }

  ResultType
  InternalThreadedUpdateStatus( void )
    {
    igstkLogMacro( DEBUG,
      "DummyTracker::InternalThreadedUpdateStatus called ...\n");
    return SUCCESS;
    }

  /** Print Self function */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const
    {
    Superclass::PrintSelf(os, indent);
    }

};

template<class ClassType>
void ExportStateMachineDescription(
              const ClassType * instance,
              const std::string & outputDirectory, bool skipLoops )
{
  std::string filename = outputDirectory+"/";
  filename = filename + "igstk";
  filename = filename + instance->GetNameOfClass();


  std::string dotfilename = filename + ".dot";
  std::ofstream dotOutputFile;
  dotOutputFile.open( dotfilename.c_str() );
  if( dotOutputFile.fail() )
    {
    std::cerr << "Problem opening the file " << filename << std::endl;
    itk::ExceptionObject excp;
    excp.SetDescription("Problem opening file");
    throw excp;
    }

  instance->ExportStateMachineDescription( dotOutputFile, skipLoops );
  dotOutputFile.close();


  std::string ltsfilename = filename + ".lts";
  std::ofstream ltsOutputFile;
  ltsOutputFile.open( ltsfilename.c_str() );
  if( ltsOutputFile.fail() )
    {
    std::cerr << "Problem opening the file " << filename << std::endl;
    itk::ExceptionObject excp;
    excp.SetDescription("Problem opening file");
    throw excp;
    }
  instance->ExportStateMachineDescriptionToLTS( ltsOutputFile, skipLoops );
  ltsOutputFile.close();


  std::string scxmlfilename = filename + ".xml";
  std::ofstream scxmlOutputFile;
  scxmlOutputFile.open( scxmlfilename.c_str() );
  if( scxmlOutputFile.fail() )
    {
    std::cerr << "Problem opening the file " << scxmlfilename << std::endl;
    itk::ExceptionObject excp;
    excp.SetDescription("Problem opening file");
    throw excp;
    }
  instance->ExportStateMachineDescriptionToSCXML( scxmlOutputFile, skipLoops );
  scxmlOutputFile.close();

}


} // end namespace igstk


// This is for classes that use SmartPointers
#define igstkTestExportStateMachine1( type, outputDirectory, skipLoops ) \
{ \
  type::Pointer instance = type::New(); \
  igstk::ExportStateMachineDescription( instance.GetPointer(), outputDirectory,\
                                                                  skipLoops ); \
}

// This is for classes that do not use SmartPointers and have a default
// constructor
#define igstkTestExportStateMachine2( type, outputDirectory, skipLoops ) \
{ \
  type * instance = new type; \
  igstk::ExportStateMachineDescription( instance, outputDirectory, skipLoops );\
  delete instance; \
}

// This is for classes that use SmartPointers but lack a New operator
#define igstkDeclareSurrogateClass( surrogate, type ) \
class surrogate : public type \
{  \
public:      \
  typedef surrogate                      Self;    \
  typedef itk::Object                    Superclass;    \
  typedef itk::SmartPointer<Self>        Pointer;       \
  igstkTypeMacro( surrogate, type );   \
  igstkNewMacro( Self );      \
};    \


namespace igstk
{

typedef ImageSpatialObject<float,3>                 ImageSpatialObjectType;
typedef ImageReader< ImageSpatialObjectType >       ImageReaderType;
typedef DICOMImageReader< ImageSpatialObjectType >  DICOMImageReaderType;
typedef ImageSpatialObjectRepresentation< ImageSpatialObjectType >
                                        ImageSpatialObjectRepresentationType;

typedef SpatialObjectReader<3,float>                SpatialObjectReaderType;

typedef ImageResliceObjectRepresentation< ImageSpatialObjectType >
                                        ImageResliceRepresentationType;

class ObjectRepresentationSurrogate : public ObjectRepresentation
{
public:
  typedef ObjectRepresentationSurrogate     Self;
  typedef ObjectRepresentation              Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  igstkTypeMacro( ObjectRepresentationSurrogate, ObjectRepresentation );
  igstkNewMacro( Self );
  virtual void CreateActors() {};
  virtual void UpdateRepresentationProcessing() {};

};

igstkDeclareSurrogateClass( SpatialObjectSurrogate, SpatialObject );
igstkDeclareSurrogateClass( ImageSpatialObjectSurrogate,
                             ImageSpatialObjectType );
igstkDeclareSurrogateClass( DICOMImageReaderSurrogate, DICOMImageReaderType );
}


int main( int argc, char * argv [] )
{
  igstk::RealTimeClock::Initialize();

  std::string outputDirectory;

  if( argc > 1 )
    {
    outputDirectory = argv[1];
    }

  bool skipLoops = true;

  if( argc > 2 )
    {
    skipLoops = atoi( argv[2] );
    }

  std::cout << "Output directory = " << outputDirectory << std::endl;
  std::cout << "Skip Loops option = " << skipLoops << std::endl;

  // This is for classes that use SmartPointers
  igstkTestExportStateMachine1( igstk::CylinderObjectRepresentation,
                                                   outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::EllipsoidObjectRepresentation,
                                                   outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::AxesObjectRepresentation,
                                                   outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::BoxObjectRepresentation,
                                                   outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::ConeObjectRepresentation,
                                                   outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::MeshObjectRepresentation,
                                                   outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::TubeObjectRepresentation,
                                                   outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::UltrasoundProbeObjectRepresentation,
                                                   outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::EllipsoidObject, outputDirectory,
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::CylinderObject, outputDirectory,
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::PulseGenerator, outputDirectory,
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::DummyTracker, outputDirectory,
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::DummyTrackerTool, outputDirectory,
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::AuroraTracker, outputDirectory,
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::AuroraTrackerTool, outputDirectory,
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::PolarisTracker, outputDirectory,
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::PolarisTrackerTool, outputDirectory,
                                                                    skipLoops );

  igstkTestExportStateMachine1( igstk::Landmark3DRegistration, outputDirectory,
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::Landmark3DRegistrationErrorEstimator,
                                                   outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::CTImageReader, outputDirectory,
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::MeshReader, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::TubeReader, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::MRImageReader, outputDirectory,
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::MRImageSpatialObject, outputDirectory,
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::MRImageSpatialObjectRepresentation,
                                                   outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::CTImageSpatialObject, outputDirectory,
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::CTImageSpatialObjectRepresentation,
                                                   outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::ImageSpatialObjectRepresentationType,
                                                   outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::SpatialObjectReaderType, outputDirectory,
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::MR3DImageToUS3DImageRegistration,
                                                   outputDirectory, skipLoops );

  igstkTestExportStateMachine1( igstk::PETImageReader, outputDirectory,
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::PETImageSpatialObject, outputDirectory,
                                                                   skipLoops );
  igstkTestExportStateMachine1( igstk::PETImageSpatialObjectRepresentation, outputDirectory,
                                                                    skipLoops );
 
  igstkTestExportStateMachine1( igstk::View, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::View2D, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::View3D, outputDirectory, skipLoops );

  igstkTestExportStateMachine1(
    igstk::CoordinateSystemDelegator, outputDirectory, skipLoops );


  igstkTestExportStateMachine1( igstk::ReslicerPlaneSpatialObject, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::ImageResliceRepresentationType, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::MeshResliceObjectRepresentation, outputDirectory, skipLoops );

#if defined(IGSTK_USE_FLTK)
  // The Widget classes don't use SmartPointer and don't have a
  // default constructor.
  igstk::FLTKWidget widget(0,0, 100, 100, "dummy view for testing");
  igstk::ExportStateMachineDescription( &widget, outputDirectory, skipLoops );
#endif

  // Exporting Abstract classes by creating derived surrogates for them.
  igstkTestExportStateMachine1( igstk::SpatialObjectSurrogate, outputDirectory,
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::ImageSpatialObjectSurrogate ,
                                                   outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::DICOMImageReaderSurrogate,
                                                   outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::ObjectRepresentationSurrogate,
                                                   outputDirectory, skipLoops );
  // Export the state diagrams for the Serial Communication classes according
  // to the current platform.
#if defined(WIN32) || defined(_WIN32)
  igstkTestExportStateMachine1( igstk::SerialCommunicationForWindows,
                                                   outputDirectory, skipLoops );
#else
  igstkTestExportStateMachine1( igstk::SerialCommunicationForPosix,
                                                  outputDirectory, skipLoops );
#endif

#if defined(IGSTK_USE_MicronTracker)
  igstkTestExportStateMachine1( igstk::MicronTracker, 
                                outputDirectory, skipLoops );

  igstkTestExportStateMachine1( igstk::MicronTrackerTool, 
                                outputDirectory, skipLoops );
#endif

#if defined(IGSTK_USE_Ascension3DGTracker)
  igstkTestExportStateMachine1( igstk::Ascension3DGTracker, 
                                outputDirectory, skipLoops );

  igstkTestExportStateMachine1( igstk::Ascension3DGTracker, 
                                outputDirectory, skipLoops );
#endif

#if defined(IGSTK_USE_NDICertusTracker)
  igstkTestExportStateMachine1( igstk::NDICertusTracker, 
                                outputDirectory, skipLoops );

  igstkTestExportStateMachine1( igstk::NDICertusTrackerTool, 
                                outputDirectory, skipLoops );
#endif

  return EXIT_SUCCESS;
}
