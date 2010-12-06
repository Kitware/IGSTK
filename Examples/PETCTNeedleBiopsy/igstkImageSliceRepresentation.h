/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSliceRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageSliceRepresentation_h
#define __igstkImageSliceRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkImageSpatialObject.h"
#include "igstkStateMachine.h"

#include "vtkImageMapToWindowLevelColors.h"
#include "vtkImageReslice.h"
#include "vtkImageActor.h"

#include "vtkMatrix4x4.h"
#include "vtkCamera.h"

namespace igstk
{
 

/** \class ImageSliceRepresentation
 * 
 * \brief This class represents an oblique image object. 
 * 
 * \ingroup ObjectRepresentation
 */
template < class TImageSpatialObject >
class ImageSliceRepresentation : public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro( 
                                ImageSliceRepresentation,ObjectRepresentation )

public:

  /** Available slice orientation. */
  enum SliceOrientationType { 
    Axial = 0,
    Sagittal = 1,
    Coronal = 2,
    OffAxial = 3,
    OffSagittal = 4,
    Perpendicular = 5,
    HybridPerpendicular = 5,
   };

  /** Available surgeon positions. */
  enum SurgeonPositionType { 
    Left = 0,
    Right = 1,
    };

  typedef TImageSpatialObject                 ImageSpatialObjectType;

  typedef Transform                           TransformType;

  typedef typename ImageSpatialObjectType::ConstPointer 
                                              ImageSpatialObjectConstPointer;

  typedef typename ImageSpatialObjectType::PointType  PointType;


  /** Connect this representation class to the spatial object */
  void RequestSetImageSpatialObject( const ImageSpatialObjectType * 
    ImageSpatialObject );

  void RequestSetOpacity( double opacity )
    {
    m_ActorOpacity = opacity;
    }

  /** Request to set slice orientation */
  void RequestSetSliceOrientation( SliceOrientationType orientation);

  /** Request to set slice orientation */
  void RequestSetSurgeonPosition( SurgeonPositionType position)
    {
    m_SurgeonPosition = position;
    }

  /** Request to set the probe transform used to reslice through the volume */
  void RequestSetProbeTransform( TransformType trasform);
  void RequestSetProbeVectorAndPosition( double * probeVector, 
                                         double * probePosition );
/*
  void RequestSetProbeHubAndTip( TransformType::VectorType hub, 
                                 TransformType::VectorType tip,
                                 int focus = 0 );*/

 
  /** Request reslice a 3D image */
  vtkCamera * RequestReslice();

  /** Set the Window Level for the representation */
  void RequestSetWindowLevel( double window, double level )
    {
    this->SetWindowLevel(window, level);
    }

  /** Request set virtual tip extension */
  void RequestSetVirtualTip( double vTip)
    {
    m_VirtualTip = vTip;
    }

  /** Request get camera */
  vtkCamera * RequestGetCamera()
  {
    return m_Camera;
  }

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

protected:

  /** Constructor */
  ImageSliceRepresentation();
  
  /** Destructor */
  ~ImageSliceRepresentation();

  /** Overloaded function to delete actors */
  void DeleteActors();

  /** Create the VTK actors for displaying geometry */
  void CreateActors();

  /** Reslice and texture management */
  void SetWindowLevel( double window, double level );
  void SetImageSpatialObjectProcessing();

private:

  ImageSliceRepresentation(const Self&);
  void operator=(const Self&);   //purposely not implemented

  /** Internal itkSpatialObject */
  ImageSpatialObjectConstPointer          m_ImageSpatialObject;

  SliceOrientationType                    m_SliceOrientation;

  SurgeonPositionType                     m_SurgeonPosition;

  TransformType                           m_ProbeTransform;
  double                                  m_ProbePosition[3];
  double                                  m_ProbeVector[3];

    
  /** VTK classes that support display of an image */

  vtkImageData                           * m_ImageData;
  vtkImageReslice                        * m_ImageReslice;
  vtkMatrix4x4                           * m_ResliceAxes;
  vtkImageMapToWindowLevelColors         * m_MapColors;
  vtkImageActor                          * m_Actor;
  vtkCamera                              * m_Camera;

  
  double                                 m_AxisX[3];
  double                                 m_AxisY[3];
  double                                 m_AxisZ[3];

  double                                 m_SliceSize;
  double                                 m_CameraDistance;
  double                                 m_ActorOpacity;

  int                                    m_Ext[6];
  int                                    m_Dim[3];
  vtkFloatingPointType                   m_Origin[3];
  vtkFloatingPointType                   m_Spacing[3];
  vtkFloatingPointType                   m_FocalPoint[3];

  /** Variables that store window and level values for 2D image display */
  double                                 m_Level;
  double                                 m_Window;
  double                                 m_VirtualTip;

  /** Update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Sets the vtkImageData from the spatial object. This method MUST be
   * private in order to prevent unsafe access from the VTK image layer. */
  void SetImage( const vtkImageData * image );
  
  /** Connect VTK pipeline */
  void ConnectVTKPipelineProcessing();
    
  /** Declare the observer that will receive a VTK image from the
   * ImageSpatialObject */
  igstkObserverMacro( VTKImage, VTKImageModifiedEvent,
                      EventHelperType::VTKImagePointerType);

  typename VTKImageObserver::Pointer  m_VTKImageObserver;

private:

  /** Inputs to the State Machine */
  
  /** States for the State Machine */
  
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkImageSliceRepresentation.txx"
#endif


#endif // __igstkImageSliceRepresentation_h
