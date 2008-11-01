/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkReslicerPlaneSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkReslicerPlaneSpatialObject_h
#define __igstkReslicerPlaneSpatialObject_h

#include "igstkMacros.h"
#include "igstkTransform.h"
#include "igstkSpatialObject.h"
//#include "igstkImageSpatialObject.h"
#include "igstkStateMachine.h"

class vtkPlaneSource;
class vtkMatrix4x4;
//class vtkImageData;
class vtkTransform;

namespace igstk
{

/** \class ReslicerPlaneSpatialObject
 * 
 * \brief This class generates a reslicing plane equation.  
 *
 * The computed plane is used by e.g. igstkImageResliceSpatialObjectRepresentation and
 * igstkMeshResliceSpatialObjectRepresentation.
 *
 * The class provides three modes of reslicing i.e Orthogonal, OffOrthogonal and Oblique. 
 *
 * In orthogonal mode, three types of orientation are defined: Axial, Coronal and Sagittal. 
 * For this mode, the tool tip spatial object provides the reslicing plane postion and orientation
 * information is extraced from the input reference spatial object.
 *
 * For OffOrthogonal mode, the three types of orientation are Perpendicular, OffSagittal and OffSagittal.
 * The orientation of the reslicing plane is computed using the tracker tool spatial object and
 * the input reference spatial object.  The postion is determined from the tracker tool tip postion.
 *
 * For oblique mode, all the orientation and postion information of the reslicing plane are obtained
 * from the tracker tool.
 *
 *
 * \ingroup SpatialObject
 */

class ReslicerPlaneSpatialObject : public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( ReslicerPlaneSpatialObject, SpatialObject )

public:

  /** Typedefs */
  typedef SpatialObject                                 ReferenceSpatialObjectType;
  typedef ReferenceSpatialObjectType::ConstPointer      ReferenceSpatialObjectConstPointer;

  typedef SpatialObject                                 ToolSpatialObjectType;
  typedef ToolSpatialObjectType::Pointer                ToolSpatialObjectPointer;

  typedef igstk::Transform::VectorType                  VectorType;

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
    OffAxial,
    OffSagittal,
    OffCoronal,
    PlaneOrientationWithZAxesNormal,
    PlaneOrientationWithXAxesNormal,
    PlaneOrientationWithYAxesNormal
    };

  /** Request the state machine to attempt to set the reslicing mode*/
  void RequestSetReslicingMode( ReslicingMode reslicingMode ); 

  /** Request the state machine to attempt to set orientation type */
  void RequestSetOrientationType( OrientationType orientationType ); 

  /** Attempts to set reference spatial object*/ 
  void AttemptSetReferenceSpatialObjectProcessing();

  /** Actually sets reference spatial object*/ 
  void RequestSetReferenceSpatialObject( const ReferenceSpatialObjectType * referenceSpatialObject ); 

  /** Request the state machine to attempt to set the tool spatial object. A tool 
  * spatial object is actually optional; althought required for the OffOrthogonal and 
  * Oblique reslicing modes.
  **/
  void RequestSetToolSpatialObject( const ToolSpatialObjectType * toolSpatialObject );

  /** Request the state machine to attempt to set reslicing cursor position. */
  void RequestSetCursorPosition( const double *point);

  /** Request get tool position */
  void RequestGetToolPosition();

  /** Request get reference spatial object bounds */
  void RequestGetBounds();

  /** Request get reslicing plane */
  void RequestGetVTKPlane();

  vtkPlaneSource * GetReslicingPlane();

  /** Request compute reslicing plane */
  void RequestComputeReslicingPlane(); 

  OrientationType GetOrientationType()
  { return m_OrientationType; }; 

  /** Request update tool transform WRT reference spatial object coordinate system */
  void RequestUpdateToolTransformWRTImageCoordinateSystem();

  /** Get tool transform */
  igstk::Transform GetToolTransform() const;

  /** Inquiry if a tool spatial object is set for reslicing */
  bool  IsToolSpatialObjectSet();


protected:

  /** Constructor */
  ReslicerPlaneSpatialObject( void );

  /** Destructor */
  ~ReslicerPlaneSpatialObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:
  ReslicerPlaneSpatialObject(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

  /** Inputs to the State Machine */  
  igstkDeclareInputMacro( SetReferenceSpatialObject );
  igstkDeclareInputMacro( ValidReslicingMode );
  igstkDeclareInputMacro( InValidReslicingMode );
  igstkDeclareInputMacro( ValidOrientationType );
  igstkDeclareInputMacro( InValidOrientationType );
  igstkDeclareInputMacro( ValidReferenceSpatialObject );
  igstkDeclareInputMacro( InValidReferenceSpatialObject );
  igstkDeclareInputMacro( ValidToolSpatialObject );
  igstkDeclareInputMacro( InValidToolSpatialObject );
  igstkDeclareInputMacro( SetCursorPosition );
  igstkDeclareInputMacro( ValidCursorPosition );
  igstkDeclareInputMacro( InValidCursorPosition );
  igstkDeclareInputMacro( GetToolPosition );
  igstkDeclareInputMacro( GetBounds );
  igstkDeclareInputMacro( GetVTKPlane );
  igstkDeclareInputMacro( GetToolTransformWRTImageCoordinateSystem );
  igstkDeclareInputMacro( ToolTransformWRTImageCoordinateSystem );
  igstkDeclareInputMacro( ComputeReslicePlane );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( ReslicingModeSet );
  igstkDeclareStateMacro( OrientationTypeSet );
  igstkDeclareStateMacro( ReferenceSpatialObjectSet );
  igstkDeclareStateMacro( AttemptingToSetCursorPosition );
  igstkDeclareStateMacro( AttemptingToSetReferenceSpatialObject );  
  igstkDeclareStateMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem );

  /** Null operation for State Machine transition */
  void NoProcessing();

  /** Set the orientation type */
  void SetOrientationTypeProcessing( void );

  /** Set the reslcing mode */
  void SetReslicingModeProcessing( void );

  /** Set Cursor position */
  void AttemptSetCursorPositionProcessing( void );
  void SetCursorPositionProcessing( void );
 
  /** Reslice plance compute methold */
  void ComputeReslicePlaneProcessing();
 
  /** Set the reference spatial object */
  void SetReferenceSpatialObjectProcessing( void );

  /** Set the tool spatial object */
  void SetToolSpatialObjectProcessing( void );

  /** Report invalid reslicing mode */
  void ReportInvalidReslicingModeProcessing( void );

  /** Report invalid orientation type */
  void ReportInvalidOrientationTypeProcessing( void );

  /** Report invalid reference spatial object type */
  void ReportInvalidReferenceSpatialObjectProcessing( void );

  /** Report invalid tool spatial object type */
  void ReportInvalidToolSpatialObjectProcessing( void );

  /** Report invalid Cursor position */
  void ReportInvalidCursorPositionProcessing( void );

  /** Report invalid request */
  void ReportInvalidRequestProcessing( void );

  /** Request get tool transform with respect to reference spatial object coordinate system */ 
  void RequestGetToolTransformWRTImageCoordinateSystemProcessing( void );

  /** Receive tool transform with respect to reference spatial object coordinate system */ 
  void ReceiveToolTransformWRTImageCoordinateSystemProcessing( void );

  /** Report image slice number bounds */
//  void ReportSliceNumberBoundsProcessing( void );

  /** Report image bounds */
  void ReportBoundsProcessing( void );

  /** Report tool position */
  void ReportToolPositionProcessing( void );

  /** Report plane */
  void ReportVTKPlaneProcessing( void );

  /** Auxiliary function to get the distance to the bounding plane in the 
  direction of the tool*/
  double GetDistanceToPlane(VectorType p, VectorType pv, unsigned int pi);

  /** Methods to compute reslcing plane for the different modes*/
  void ComputeOrthogonalReslicingPlane();
  void ComputeObliqueReslicingPlane();
  void ComputeOffOrthogonalReslicingPlane();

  /** Variables for managing reslicing mode */
  ReslicingMode     m_ReslicingModeToBeSet;
  ReslicingMode     m_ReslicingMode;

  /** Variables for managing orientation type */
  OrientationType     m_OrientationTypeToBeSet;
  OrientationType     m_OrientationType;

  /** Variables for managing reference spatial object type */
  ReferenceSpatialObjectConstPointer     m_ReferenceSpatialObjectToBeSet;
  ReferenceSpatialObjectConstPointer     m_ReferenceSpatialObject;

  /** Variables for managing tool spatial object type */
  ToolSpatialObjectPointer     m_ToolSpatialObjectToBeSet;
  ToolSpatialObjectPointer     m_ToolSpatialObject;
  
  /** reslicing plane */
  vtkPlaneSource *                  m_PlaneSource;

  /** reslicing matrix */
  vtkMatrix4x4 *                    m_ResliceAxes; 

//  vtkTransform *                    m_ResliceTransform;

  /** vtk image data */
  //vtkImageData *                    m_ImageData;

  /** Transform containing the information about image origin
   *  and image orientation taken from the DICOM input image */
//  igstk::Transform                  m_ImageTransform;
  
  /** Plane parameters */
  VectorType                        m_PlaneNormal;
  VectorType                        m_PlaneCenter;
  VectorType                        m_PlanePoint1;
  VectorType                        m_PlanePoint2;
  VectorType                        m_PlaneOrigin;

  // Event macro setup to receive the tool spatial object transform
  // with respect to the reference spatial object coordinate system
  igstkLoadedEventTransductionMacro( CoordinateSystemTransformTo, ToolTransformWRTImageCoordinateSystem );
  

  // Tool transform with respect to the reference spatial object coordinate system
  igstk::Transform m_ToolTransformWRTImageCoordinateSystem;

  /** Observer to the VTK image events */
//  typename VTKImageObserver::Pointer         m_VTKImageObserver;

//  typename ImageTransformObserver::Pointer   m_ImageTransformObserver;

  /** Cursor position member variables */
  double               m_CursorPositionToBeSet[3];
  double               m_CursorPosition[3];
  bool                 m_CursorPositionSetFlag;

  VectorType           m_ToolPosition;

  //int                  m_ImageDimension[3];
  //double               m_ImageOrigin[3];
  //double               m_ImageSpacing[3];
  //int                  m_ImageExtent[6];
  double               m_ImageBounds[6];

  BoundingBoxType::ConstPointer       m_BoundingBox;
  

  std::vector< VectorType > m_BoundsCenters;
  std::vector< VectorType > m_BoundsNormals;
 
  /** flag indicating tool spatial object used for reslicing */
  bool                 m_ToolSpatialObjectSet;

  igstkObserverConstObjectMacro( BoundingBox, SpatialObject::BoundingBoxEvent,
                                              SpatialObject::BoundingBoxType);

};

} // end namespace igstk

#endif // __igstkReslicerPlaneSpatialObject_h
