/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoFrameRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkVideoFrameRepresentation_h
#define __igstkVideoFrameRepresentation_h

#include "igstkVideoFrameSpatialObject.h"
#include "igstkObjectRepresentation.h"

class vtkImageData;
class vtkImageActor;
class vtkLookupTable;
class vtkImageMapToColors;

namespace igstk
{

template < class TVideoFrameSpatialObject >
class VideoFrameRepresentation : public ObjectRepresentation
{

public:
  igstkStandardTemplatedClassTraitsMacro( VideoFrameRepresentation,
                      ObjectRepresentation )

  /** Observer type for loaded event,
   *  the callback can be set to a member function. */
  typedef itk::ReceptorMemberCommand < Self > LoadedObserverType;

  typedef TVideoFrameSpatialObject                 VideoFrameSpatialObjectType;
  typedef typename VideoFrameSpatialObjectType::ConstPointer
                                           VideoFrameSpatialObjectConstPointer;

  /** Connect this representation class to the spatial object */
  void RequestSetVideoFrameSpatialObject(const VideoFrameSpatialObjectType*
                                                                spatialObject);

  /** Observer macro that will received a event with an image as payload and
   * will store it internally. This will be the receptor of the event sent by
   * the VideoFrameSpatialObject when an image is requested. */
  igstkObserverMacro( VTKImage, VTKImageModifiedEvent,
                      EventHelperType::VTKImagePointerType );

  /** Set the Window Level for the representation */
  void SetWindowLevel( double window, double level );

  void SaveScreenShot( const std::string & filename );

  /** Return a copy of the current object representation */
  Pointer Copy() const;

protected:
  VideoFrameRepresentation(void);

  ~VideoFrameRepresentation(void);

  /** Overloaded function to delete actors */
  void DeleteActors();

  bool VerifyTimeStamp( ) const;

  /** Create the VTK actors for displaying geometry */
  void CreateActors();

private:

  /** Internal VideoFrameSpatialObject */
  VideoFrameSpatialObjectConstPointer         m_VideoFrameSpatialObject;
  VideoFrameSpatialObjectConstPointer         m_VideoFrameSpatialObjectToAdd;

  /** VTK classes that support display of an image */
  vtkImageData              *m_ImageData;
  vtkImageActor             *m_ImageActor;
  vtkLookupTable            *m_LookupTable;
  vtkImageMapToColors       *m_MapColors;

  double                    m_Level;
  double                    m_Window;
  double                    m_Shift;
  double                    m_Scale;

  std::string               m_ScreenShotFileName;

  /** Update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Null operation for State Machine transition */
  void NoProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetVideoFrameSpatialObjectProcessing();

  /** Sets the vtkImageData from the spatial object. This method MUST be
   * private in order to prevent unsafe access from the VTK image layer. */
  void SetImage( const vtkImageData * image );

  /** Connect VTK pipeline */
  void ConnectVTKPipelineProcessing();

  /** Observer to the VTK image events */
  typename VTKImageObserver::Pointer   m_VTKImageObserver;

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidVideoFrameSpatialObject );
  igstkDeclareInputMacro( NullVideoFrameSpatialObject );
  igstkDeclareInputMacro( EmptyVideoFrameSpatialObject );
  igstkDeclareInputMacro( ConnectVTKPipeline );

  /** States for the State Machine */
  igstkDeclareStateMacro( NullVideoFrameSpatialObject );
  igstkDeclareStateMacro( ValidVideoFrameSpatialObject );
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkVideoFrameRepresentation.txx"
#endif

#endif // __igstkVideoFrameRepresentation_h
