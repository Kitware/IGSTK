/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageReslicePlaneSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkImageReslicePlaneSpatialObject_h
#define __igstkImageReslicePlaneSpatialObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include "igstkImageSpatialObject.h"
#include "igstkStateMachine.h"

class vtkPlane;
class vtkMatrix4x4;
class vtkImageData;

namespace igstk
{

/** \class ImageReslicePlaneSpatialObject
 * 
 * \brief This class generates reslicing plane equation.  
 *
 * The computed plane is used by igstkImageResliceSpatialObjectRepresentation
 * class to generate a resliced image representation.
 *
 * The class provides three modes of reslicing i.e Orthogonal, OffOrthogonal and Oblique. 
 *
 * Orthogonal mode ....
 *
 * OffOrthogonal mode ....
 *
 * Oblique mode .....
 *
 * In orthogonal mode, three types of orientation are defined: Axial, Coronal and Sagittal. 
 * For this mode, the tool tip spatial object provides the reslicing plane postion and orientation
 * information is extraced from the input image spatial object.
 *
 * For OffOrthogonal mode, the three types of orientation are Perpendicular, OffAxial and OffSagittal.
 * The orientation of the reslicing plane is computed using the tracker tool spatial object and
 * the input image spatial object.  The postion is determined from the tracker tool tip postion.
 *
 * For oblique mode, all the orientation and postion information of the reslicing plane are obtained
 * from the tracker tool.
 *
 *
 * \ingroup SpatialObject
 */

template < class TImageSpatialObject > 
class ImageReslicePlaneSpatialObject : public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro( ImageReslicePlaneSpatialObject, SpatialObject )

public:

  /** Typedefs */
  typedef TImageSpatialObject                    ImageSpatialObjectType;

  typedef typename ImageSpatialObjectType::ConstPointer
                                                 ImageSpatialObjectConstPointer;

  typedef SpatialObject                          ToolSpatialObjectType;
  typedef ToolSpatialObjectType::Pointer         ToolSpatialObjectPointer;

  typedef unsigned int                           SliceNumberType;

  /** Reslicing modes */
  enum ReslicingMode
    { 
    Orthogonal, 
    OffOrthogonal, 
    Oblique 
    }; 

  /** Orthogonal orientation types */
  enum OrientationType
    { 
    Axial, 
    Sagittal, 
    Coronal, 
    Perpendicular,
    OffAxial,
    OffSagittal,
    PlaneOrientationWithZAxesNormal,
    PlaneOrientationWithXAxesNormal,
    PlaneOrientationWithYAxesNormal
    };

  /** Request the state machine to attempt to set the reslicing mode*/
  void RequestSetReslicingMode( ReslicingMode reslicingMode ); 

  /** Request the state machine to attempt to set orientation type */
  void RequestSetOrientationType( OrientationType orientationType ); 

  /** Request set image spatial object*/ 
  void RequestSetImageSpatialObject( const ImageSpatialObjectType * imageSpatialObject ); 

  /** Request set tool spatial object */
  void RequestSetToolSpatialObject( const ToolSpatialObjectType * toolSpatialObject );

  /** Request Set Slice number */
  void RequestSetSliceNumber( SliceNumberType sliceNumber );

  /** Request set reslicing mouse position */
  void RequestSetMousePosition( double  mousePostion[3] );

  /** Request get image slice number bounds */
  void RequestGetSliceNumberBounds();

  /** Request compute reslicing plane */
  void RequestComputeReslicingPlane( ); 

  /** Request Get reslicing plane equation */
  vtkPlane * GetReslicingPlane();

  /** Request Get reslicing matrix */
  vtkMatrix4x4 * RequestGetReslicingMatrix();

  /** Request update tool transform WRT image coordinate system */
  void RequestUpdateToolTransform();

  /** Get tool transform */
  Transform GetToolTransform() const;

  /** Inquiry if a tool spatial object is set for reslicing */
  bool  IsToolSpatialObjectSet();


protected:

  /** Constructor */
  ImageReslicePlaneSpatialObject( void );

  /** Destructor */
  ~ImageReslicePlaneSpatialObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

 /** Observer macro that will received a event with an image as payload and
   * will store it internally. This will be the receptor of the event sent by
   * the ImageSpatialObject when an image is requested. */
   igstkObserverMacro( VTKImage, VTKImageModifiedEvent, 
                       EventHelperType::VTKImagePointerType );

private:
  ImageReslicePlaneSpatialObject(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidReslicingMode );
  igstkDeclareInputMacro( InValidReslicingMode );
  igstkDeclareInputMacro( ValidOrientationType );
  igstkDeclareInputMacro( InValidOrientationType );
  igstkDeclareInputMacro( ValidImageSpatialObject );
  igstkDeclareInputMacro( InValidImageSpatialObject );
  igstkDeclareInputMacro( ValidToolSpatialObject );
  igstkDeclareInputMacro( InValidToolSpatialObject );
  igstkDeclareInputMacro( SetSliceNumber );
  igstkDeclareInputMacro( ValidSliceNumber );
  igstkDeclareInputMacro( InValidSliceNumber );
  igstkDeclareInputMacro( SetMousePosition );
  igstkDeclareInputMacro( ValidMousePosition );
  igstkDeclareInputMacro( InValidMousePosition );
  igstkDeclareInputMacro( GetSliceNumberBounds );
  igstkDeclareInputMacro( GetToolTransformWRTImageCoordinateSystem );
  igstkDeclareInputMacro( ToolTransformWRTImageCoordinateSystem );
  igstkDeclareInputMacro( ComputeReslicePlane );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( ReslicingModeSet );
  igstkDeclareStateMacro( OrientationTypeSet );
  igstkDeclareStateMacro( ImageSpatialObjectSet );
  igstkDeclareStateMacro( ToolSpatialObjectSet );
  igstkDeclareStateMacro( ValidSliceNumberSet );
  igstkDeclareStateMacro( AttemptingToSetSliceNumber );
  igstkDeclareStateMacro( ValidMousePositionSet );
  igstkDeclareStateMacro( AttemptingToSetMousePosition );
  igstkDeclareStateMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem );

  /** Internal itkSpatialObject */

  /** Set the orientation type */
  void SetOrientationTypeProcessing( void );

  /** Set the reslcing mode */
  void SetReslicingModeProcessing( void );

  /** Set slice number */
  void AttemptSetSliceNumberProcessing( void );
  void SetSliceNumberProcessing( void );

  /** Set mouse position */
  void AttemptSetMousePositionProcessing( void );
  void SetMousePositionProcessing( void );
 
  /** Reslice plance compute methold */
  void ComputeReslicePlaneProcessing();
 
  /** Set the image spatial object */
  void SetImageSpatialObjectProcessing( void );

  /** Set the tool spatial object */
  void SetToolSpatialObjectProcessing( void );

  /** Report invalid reslicing mode */
  void ReportInvalidReslicingModeProcessing( void );

  /** Report invalid orientation type */
  void ReportInvalidOrientationTypeProcessing( void );

  /** Report invalid image spatial object type */
  void ReportInvalidImageSpatialObjectProcessing( void );

  /** Report invalid tool spatial object type */
  void ReportInvalidToolSpatialObjectProcessing( void );

  /** Report invalid slice number*/
  void ReportInvalidSliceNumberProcessing( void );

  /** Report invalid mouse position */
  void ReportInvalidMousePositionProcessing( void );

  /** Report invalid request */
  void ReportInvalidRequestProcessing( void );

  /** Request get tool transform with respect to image coordinate system */ 
  void RequestGetToolTransformWRTImageCoordinateSystemProcessing( void );

  /** Receive tool transform with respect to image coordinate system */ 
  void ReceiveToolTransformWRTImageCoordinateSystemProcessing( void );

  /** Report image slice number bounds */
  void ReportSliceNumberBoundsProcessing( void );

  /** Methods to compute reslcing plane for the different modes*/
  void ComputeOrthgonalReslicingPlane();
  void ComputeObliqueReslicingPlane();
  void ComputeOffOrthgonalReslicingPlane();

  /** Variables for managing reslicing mode */
  ReslicingMode     m_ReslicingModeToBeSet;
  ReslicingMode     m_ReslicingMode;

  /** Variables for managing orientation type */
  OrientationType     m_OrientationTypeToBeSet;
  OrientationType     m_OrientationType;

  /** Variables for managing image spatial object type */
  ImageSpatialObjectConstPointer     m_ImageSpatialObjectToBeSet;
  ImageSpatialObjectConstPointer     m_ImageSpatialObject;

  /** Variables for managing tool spatial object type */
  ToolSpatialObjectPointer     m_ToolSpatialObjectToBeSet;
  ToolSpatialObjectPointer     m_ToolSpatialObject;
  
  /** Image reslice plane */
  vtkPlane *                        m_ImageReslicePlane;

  /** Image reslicing matrix */
  vtkMatrix4x4 *                    m_ResliceAxes; 

  /** vtk image data */
  vtkImageData *                      m_ImageData;

  // Event macro setup to receive the tool spatial object transform
  // with respect to the image coordinate system
  igstkLoadedEventTransductionMacro( CoordinateSystemTransformTo, ToolTransformWRTImageCoordinateSystem );

  // Tool transform with respect to the image coordinate system
  Transform m_ToolTransformWRTImageCoordinateSystem;

  /** Observer to the VTK image events */
  typename VTKImageObserver::Pointer         m_VTKImageObserver;

  /** Slice number member variables */
  SliceNumberType      m_SliceNumberToBeSet;
  SliceNumberType      m_SliceNumber;
  bool                 m_SliceNumberSetFlag;

  /** mouse position member variables */
  double                 m_MousePositionToBeSet[3];
  double                 m_MousePosition[3];
  bool                   m_MousePostionSetFlag;

  /** flag indicating tool spatial object used for reslicing */
  bool                   m_ToolSpatialObjectSet;
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkImageReslicePlaneSpatialObject.txx"
#endif

#endif // __igstkImageReslicePlaneSpatialObject_h
