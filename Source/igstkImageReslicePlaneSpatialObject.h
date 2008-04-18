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

#include "vtkPlane.h"

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
    OffSagittal
    };

  /** Request the state machine to attempt to set the reslicing mode*/
  void RequestSetReslicingMode( ReslicingMode reslicingMode ); 

  /** Request the state machine to attempt to set orientation type */
  void RequestSetOrientationType( OrientationType orientationType ); 

  /** Request set image spatial object*/ 
  void RequestSetImageSpatialObject( const ImageSpatialObjectType * imageSpatialObject ); 

  /** Request set tool spatial object */
  void RequestSetToolSpatialObject( const ToolSpatialObjectType * toolSpatialObject );

  /** Request compute reslicing plane */
  void RequestComputeReslicingPlane( ); 

  /** Request Get reslicing plane equation */
  vtkPlane * RequestGetReslicingPlane();

  /** Request Get tool transform with respect to image coordinate system */
  void RequestGetToolTransformWRTImageCoordinateSystem();

protected:

  /** Constructor */
  ImageReslicePlaneSpatialObject( void );

  /** Destructor */
  ~ImageReslicePlaneSpatialObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

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
  igstkDeclareInputMacro( GetToolTransformWRTImageCoordinateSystem );
  igstkDeclareInputMacro( ToolTransformWRTImageCoordinateSystem );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( ReslicingModeSet );
  igstkDeclareStateMacro( OrientationTypeSet );
  igstkDeclareStateMacro( ImageSpatialObjectSet );
  igstkDeclareStateMacro( ToolSpatialObjectSet );
  igstkDeclareStateMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem );

  /** Internal itkSpatialObject */

  /** Set the orientation type */
  void SetOrientationTypeProcessing( void );

  /** Set the reslcing mode */
  void SetReslicingModeProcessing( void );

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

  /** Report invalid request */
  void ReportInvalidRequestProcessing( void );

  /** Request get tool transform with respect to image coordinate system */ 
  void RequestGetToolTransformWRTImageCoordinateSystemProcessing( void );

  /** Receive tool transform with respect to image coordinate system */ 
  void ReceiveToolTransformWRTImageCoordinateSystemProcessing( void );

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

  // Event macro setup to receive the tool spatial object transform
  // with respect to the image coordinate system
  igstkLoadedEventTransductionMacro( CoordinateSystemTransformTo, ToolTransformWRTImageCoordinateSystem );

  // Tool transform with respect to the image coordinate system
  Transform m_ToolTransformWRTImageCoordinateSystem;
  
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkImageReslicePlaneSpatialObject.txx"
#endif

#endif // __igstkImageReslicePlaneSpatialObject_h
