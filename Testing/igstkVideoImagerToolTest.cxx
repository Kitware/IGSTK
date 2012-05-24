/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoImagerToolTest.cxx
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
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include "igstkLandmark3DRegistration.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkObject.h"
#include "itkCommand.h"
#include "itkMacro.h"
#include "igstkEvents.h"
#include "igstkVideoImager.h"
#include "igstkVideoImagerTool.h"

namespace igstk
{

namespace VideoImagerToolTest
{
   
class VideoImager;
class VideoImagerTool;
class DummyVideoImager;
class DummyVideoImagerTool;
   
class DummyVideoImagerTool : public igstk::VideoImagerTool
{
public:
      
  igstkFriendClassMacro( DummyVideoImager );
      
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( DummyVideoImagerTool, igstk::VideoImagerTool )

  void RequestAttachToVideoImager(DummyVideoImager*  imager );

  DummyVideoImagerTool::Pointer DummyVideoImagerToolPointer;

protected:
   DummyVideoImagerTool(): m_StateMachine(this) {};
  ~DummyVideoImagerTool()
    {};
      
  /** Check if the tracker tool is configured or not. This method should
   *  be implemented in the derived classes*/
  virtual bool CheckIfVideoImagerToolIsConfigured( ) const { return true; }
};


class DummyVideoImager : public igstk::VideoImager
{
public:
      
  igstkFriendClassMacro( DummyVideoImagerTool );
      
  double GetImagerValidityTime()
    {
    return this->GetValidityTime();
    }

  /** Typedef for internal boolean return type. */
  typedef VideoImager::ResultType   ResultType;

  /** Open communication with the imaging device. */
  virtual ResultType InternalOpen( void ) {  return SUCCESS; };

  /** Close communication with the imaging device. */
  virtual ResultType InternalClose( void ){  return SUCCESS; };

  /** Put the imaging device into imaging mode. */
  virtual ResultType InternalStartImaging( void ){  return SUCCESS; };

  /** Take the imaging device out of imaging mode. */
  virtual ResultType InternalStopImaging( void ){  return SUCCESS; };

  /** Update the status and the transforms for all VideoImagerTools. */
  virtual ResultType InternalUpdateStatus( void ){  return SUCCESS; };

  /** Update the status and the frames.
      This function is called by a separate thread. */
  virtual ResultType InternalThreadedUpdateStatus( void ){  return SUCCESS; };

  /** Reset the imaging device to put it back to its original state. */
  virtual ResultType InternalReset( void ){  return SUCCESS; };

  /** Verify imager tool information */
  virtual ResultType VerifyVideoImagerToolInformation( 
                                                  const VideoImagerToolType * ){  return SUCCESS; };

  /** The "ValidateSpecifiedFrequency" method checks if the specified
   * frequency is valid for the imaging device that is being used. */
  virtual ResultType ValidateSpecifiedFrequency( double itkNotUsed(frequencyInHz) ){  return SUCCESS; };

  /** Print object information */
  virtual void PrintSelf( std::ostream& itkNotUsed(os), itk::Indent itkNotUsed(indent) ) const{  return; };

  /** Remove imager tool entry from internal containers */
  virtual ResultType RemoveVideoImagerToolFromInternalDataContainers( const
                                     VideoImagerToolType * itkNotUsed(imagerTool) ){  return SUCCESS; };

  /** Add imager tool entry to internal containers */
  virtual ResultType AddVideoImagerToolToInternalDataContainers( const
                                     VideoImagerToolType * itkNotUsed(imagerTool) ){  return SUCCESS; };

  DummyVideoImager::Pointer DummyVideoImagerPointer;
      //typedef ::itk::SmartPointer< Self >       Pointer;    

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( DummyVideoImager, igstk::VideoImager )

protected:
  DummyVideoImager( void ): m_StateMachine(this) {};
  ~DummyVideoImager( void ) {};
      
};

/** The "RequestAttachToVideoImager" method attaches
 * the imager tool to a imager. */
void DummyVideoImagerTool::RequestAttachToVideoImager(DummyVideoImager*  imager )
{
  this->VideoImagerTool::RequestAttachToVideoImager( imager );
}
   
}
}

int igstkVideoImagerToolTest( int , char * [] )
{
  typedef igstk::VideoImagerToolTest::DummyVideoImager     VideoImagerType;
  typedef igstk::VideoImagerToolTest::DummyVideoImagerTool VideoImagerToolType;
  
  VideoImagerType::Pointer videoImager= VideoImagerType::New();

  VideoImagerToolType::Pointer videoImagerTool = VideoImagerToolType::New();

  videoImagerTool->RequestConfigure();
  videoImagerTool->RequestAttachToVideoImager( videoImager );

  return EXIT_SUCCESS;
}
