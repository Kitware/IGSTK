/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSliceRepresentationPlus.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageSliceRepresentationPlus_h
#define __igstkImageSliceRepresentationPlus_h

#include "igstkMacros.h"
#include "igstkTransformObserver.h"
#include "igstkObjectRepresentation.h"
#include "igstkImageSpatialObject.h"
#include "igstkStateMachine.h"

#include "vtkImageMapToWindowLevelColors.h"
#include "vtkLookupTable.h"
#include "vtkImageReslice.h"
#include "vtkImageActor.h"
#include "vtkImageBlend.h"

#include "vtkMatrix4x4.h"
#include "vtkCamera.h"

namespace igstk
{


/** \class ImageSliceRepresentationPlus
 *
 * \brief This class represents an oblique image object.
 *
 * \ingroup ObjectRepresentation
 */
template < class TBGImageSO,  class TFGImageSO  >
class ImageSliceRepresentationPlus : public ObjectRepresentation
{
public:
  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro(
                                ImageSliceRepresentationPlus, ObjectRepresentation )
public:
  typedef TBGImageSO                                      BGImageSOType;
  typedef TFGImageSO                                      FGImageSOType;
  typedef typename BGImageSOType::PointType               PointType;

  /** Available slice orientation. */
  enum SliceOrientationType {
    Axial = 0,
    Sagittal = 1,
    Coronal = 2,
    OffAxial = 3,
    OffSagittal = 4,
    Perpendicular = 5,        //6DOF tool
    OffPerpendicular = 6,     //5DOF tool, need surgeon position information
    HybridPerpendicular = 7,  //Targeting view, need setup path
   };

  /** Available surgeon positions. */
  enum SurgeonPositionType {
    PatientLeft = 0,
    PatientRight = 1,
    };

  /** Connect this representation class to the spatial object */
  void RequestSetBGImageSO( BGImageSOType * imageSO);
  void RequestSetFGImageSO( FGImageSOType * imageSO);
  void GetExtent(int & min, int & max);

  /** Set/Get background opacity */
  void SetBGOpacity( double o );

  /** Request to set slice orientation */
  igstkSetMacro( Orientation, SliceOrientationType );
  igstkGetMacro( Orientation, SliceOrientationType );

  /** Request to set the probe transform used to reslice through the volume */
  // Reslice position and axis, in BGImage space
  void SetResliceTransform( igstk::Transform t );
  void SetCalibrationTransform( igstk::Transform t);
 
  /** These settings are only relavant under off-orthogonal reslicing mode */

  /** Request to set surgeon's position relative to patient */
  igstkSetMacro( SurgeonPosition, SurgeonPositionType );
  igstkGetMacro( SurgeonPosition, SurgeonPositionType );

  /** Set the needle's principal axes and virtual tip length */
  // When using needle tools for reslicing, this is the pricipal axes
  // of the needle tool in its local coordinate system
  void SetPrincipalAxes( igstk::Transform::VectorType pAxes );

  igstkSetMacro( VirtualTip, double );
  igstkGetMacro( VirtualTip, double );

  /** Set entry and target point from path planning for targeting view */
  igstkSetMacro( PathVector, igstk::Transform::VectorType );          // In the BGImage space
  igstkGetMacro( PathVector, igstk::Transform::VectorType );
 
  /** Request reslice a 3D image */
  vtkCamera * RequestReslice();
  vtkCamera * GetCamera()
    {
    return m_Camera;
    }

  /** Visualization settings */
  void SetSliceSize(double size);

  void SetBGImageMapper(vtkImageMapToColors * mapper); 
  void SetBGImageLUT(vtkLookupTable * lut); 
  void SetFGImageMapper(vtkImageMapToColors * mapper); 
  void SetFGImageLUT(vtkLookupTable * lut); 
  void SetBGWindowLevel(double window, double level)
    {
    m_BGImageLUT->SetRange(level-(window/2), level+(window/2));
    }
  void SetFGWindowLevel(double window, double level)
    {
    m_FGImageLUT->SetRange(level-(window/2), level+(window/2));
    }

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

protected:

  /** Constructor */
  ImageSliceRepresentationPlus();

  /** Destructor */
  ~ImageSliceRepresentationPlus();

  /** Overloaded function to delete actors */
  void DeleteActors();

  /** Create the VTK actors for displaying geometry */
  void CreateActors();

  /** Reslice and texture management */
  //void SetImageSpatialObjectProcessing();

private:

  ImageSliceRepresentationPlus(const Self&);
  void operator=(const Self&);   //purposely not implemented

  /** Internal itkSpatialObject */
  typename BGImageSOType::Pointer         m_BGImageSO;
  typename FGImageSOType::Pointer         m_FGImageSO;

  SliceOrientationType                    m_Orientation;
  SurgeonPositionType                     m_SurgeonPosition;

  igstk::Transform                        m_Transform;
  igstk::Transform                        m_ResliceTransform;
  igstk::Transform                        m_CalibrationTransform;
  igstk::TransformObserver::Pointer       m_TransformObserver;
  igstk::Transform::VectorType            m_PrincipalAxes;
  igstk::Transform::VectorType            m_NeedleVector;
  igstk::Transform::VectorType            m_PathVector;
  igstk::Transform::VectorType            m_ResliceCenter;
  igstk::Transform::VectorType            m_ImageCenter;
  
  double                                  m_BGOpacity;
  double                                  m_VirtualTip;
  double                                  m_SliceSize; //Sqaure
  double                                  m_SizeX;     //non-sqaure
  double                                  m_SizeY;
  double                                  m_CameraDistance;
  
  /** VTK classes that support display of an image */
  vtkImageData                          * m_BGImageData;
  vtkImageData                          * m_FGImageData;

  vtkImageReslice                       * m_BGImageReslice;
  vtkImageReslice                       * m_FGImageReslice;

  vtkImageMapToColors                   * m_BGImageMapper;
  vtkImageMapToColors                   * m_FGImageMapper;
  vtkLookupTable                        * m_BGImageLUT;
  vtkLookupTable                        * m_FGImageLUT;

  vtkImageActor                         * m_Actor;

  vtkCamera                             * m_Camera;
  vtkMatrix4x4                          * m_ResliceAxes;
  vtkImageBlend                         * m_Blender;

  int                                     m_Extent[6];
  int                                     m_Dim[3];
  vtkFloatingPointType                    m_Origin[3];
  vtkFloatingPointType                    m_Spacing[3];
  double                                  m_Size[3];
  
  /** Update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect VTK pipeline */
 // void ConnectVTKPipelineProcessing();

 void ComputeOrthogonalResliceAxes();
 void ComputeOffOrthogonalResliceAxes();

  /** Declare the observer that will receive a VTK image from the
   * ImageSpatialObject */
  igstkObserverMacro( VTKImage, VTKImageModifiedEvent,
                      EventHelperType::VTKImagePointerType);

  typename VTKImageObserver::Pointer  m_VTKImageObserver;

private:

  virtual bool VerifyTimeStamp() const; 
  /** Inputs to the State Machine */

  /** States for the State Machine */

};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkImageSliceRepresentationPlus.txx"
#endif


#endif // __igstkImageSliceRepresentationPlus_h
