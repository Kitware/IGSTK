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
#include "igstkStateMachine.h"


namespace igstk
{

/** \class ReslicerPlaneSpatialObject
 * 
 * \brief This class generates reslicing plane equation.  
 *
 * The computed plane is used by spatial object representation classes such as 
 * igstkImageResliceSpatialObjectRepresentation and 
 * igstkMeshResliceSpatialObjectRepresentation.
 *
 * The class provides three modes of reslicing: Orthogonal, OffOrthogonal and 
 * Oblique. 
 *
 * In orthogonal mode, three types of orientation are defined: Axial, Coronal 
 * and Sagittal. 
 * For this mode, the tool tip spatial object provides the reslicing plane 
 * position and orientation
 * information is extracted from the bounding box provider spatial object.
 *
 * For OffOrthogonal mode, the three types of orientation are OffAxial, 
 * OffSagittal and OffCoronal.
 * The orientation of the reslicing plane is computed from both, the tracker 
 * tool spatial object and
 * the bounding box provider spatial object. The position of the reslice plane 
 * is determined from 
 * the tracker tool tip position.
 *
 * For the oblique mode, both orientation and position information of the 
 * reslicing plane are obtained
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
  typedef SpatialObject            BoundingBoxProviderSpatialObjectType;
  typedef BoundingBoxProviderSpatialObjectType::ConstPointer
                                   BoundingBoxProviderSpatialObjectConstPointer;

  typedef SpatialObject                               ToolSpatialObjectType;
  typedef ToolSpatialObjectType::Pointer              ToolSpatialObjectPointer;

  typedef igstk::Transform::VectorType                  VectorType;
  typedef igstk::Transform::VersorType                  VersorType;

  igstkLoadedEventMacro( ToolTipPositionEvent, IGSTKEvent, VectorType );
  igstkLoadedEventMacro( ReslicerPlaneNormalEvent, IGSTKEvent, VectorType );
  igstkLoadedEventMacro( ReslicerPlaneCenterEvent, IGSTKEvent, VectorType );

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

  /** Actually sets boundingbox provider spatial object*/ 
  void RequestSetBoundingBoxProviderSpatialObject( const
     BoundingBoxProviderSpatialObjectType * boundingBoxProviderSpatialObject ); 

  /** Request the state machine to attempt to set the tool spatial object. A 
   * tool spatial object is actually optional; althought required for the 
   * OffOrthogonal and Oblique reslicing modes.
   **/
  void RequestSetToolSpatialObject( const ToolSpatialObjectType * 
                                                            toolSpatialObject );

  /** Request the state machine to attempt to set reslicing cursor position. */
  void RequestSetCursorPosition( const double *point);

  /** Request get tool position */
  void RequestGetToolPosition();

  /** Request get reslicing plane parameters */
  void RequestGetReslicingPlaneParameters();

  /** Request compute reslicing plane */
  void RequestComputeReslicingPlane(); 

  /** Retrieve current orientation mode*/
  OrientationType GetOrientationType() const;

  /** Retrieve current reslicing mode */
  ReslicingMode GetReslicingMode() const;

  /** Request update tool transform WRT reference spatial object coordinate 
   * system */
  void RequestUpdateToolTransformWRTImageCoordinateSystem();

  /** Get tool transform */
  igstk::Transform GetToolTransform() const;

  /** fixme: put this into events */
  VectorType GetToolPosition() const;

  /** Inquiry if a tool spatial object is set for reslicing */
  bool  IsToolSpatialObjectSet();

  /** Declarations needed for the Logger */
  igstkLoggerMacro();

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
  igstkDeclareInputMacro( SetBoundingBoxProviderSpatialObject );
  igstkDeclareInputMacro( ValidReslicingMode );
  igstkDeclareInputMacro( InValidReslicingMode );
  igstkDeclareInputMacro( ValidOrientationType );
  igstkDeclareInputMacro( InValidOrientationType );
  igstkDeclareInputMacro( ValidBoundingBoxProviderSpatialObject );
  igstkDeclareInputMacro( InValidBoundingBoxProviderSpatialObject );
  igstkDeclareInputMacro( ValidToolSpatialObject );
  igstkDeclareInputMacro( InValidToolSpatialObject );
  igstkDeclareInputMacro( SetCursorPosition );
  igstkDeclareInputMacro( ValidCursorPosition );
  igstkDeclareInputMacro( InValidCursorPosition );
  igstkDeclareInputMacro( GetToolPosition );
  igstkDeclareInputMacro( GetReslicingPlaneParameters );
  igstkDeclareInputMacro( GetToolTransformWRTImageCoordinateSystem );
  igstkDeclareInputMacro( ToolTransformWRTImageCoordinateSystem );
  igstkDeclareInputMacro( ComputeReslicePlane );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( ReslicingModeSet );
  igstkDeclareStateMacro( OrientationTypeSet );
  igstkDeclareStateMacro( BoundingBoxProviderSpatialObjectSet );
  igstkDeclareStateMacro( AttemptingToSetCursorPosition );
  igstkDeclareStateMacro( AttemptingToSetBoundingBoxProviderSpatialObject );  
  igstkDeclareStateMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem);

  /** Null operation for State Machine transition */
  void NoProcessing();

  /** Set the orientation type */
  void SetOrientationTypeProcessing( void );

  /** Set the reslcing mode */
  void SetReslicingModeProcessing( void );

 /** Attempts to set bounding box provider spatial object*/ 
  void AttemptSetBoundingBoxProviderSpatialObjectProcessing();

  /** Set Cursor position */
  void AttemptSetCursorPositionProcessing( void );
  void SetCursorPositionProcessing( void );
 
  /** Reslice plance compute methold */
  void ComputeReslicePlaneProcessing();
 
  /** Set the reference spatial object */
  void SetBoundingBoxProviderSpatialObjectProcessing( void );

  /** Set the tool spatial object */
  void SetToolSpatialObjectProcessing( void );

  /** Report invalid reslicing mode */
  void ReportInvalidReslicingModeProcessing( void );

  /** Report invalid orientation type */
  void ReportInvalidOrientationTypeProcessing( void );

  /** Report invalid reference spatial object type */
  void ReportInvalidBoundingBoxProviderSpatialObjectProcessing( void );

  /** Report invalid tool spatial object type */
  void ReportInvalidToolSpatialObjectProcessing( void );

  /** Report invalid Cursor position */
  void ReportInvalidCursorPositionProcessing( void );

  /** Report invalid request */
  void ReportInvalidRequestProcessing( void );

  /** Request get tool transform with respect to reference spatial object 
   * coordinate system */ 
  void RequestGetToolTransformWRTImageCoordinateSystemProcessing( void );

  /** Receive tool transform with respect to reference spatial object 
   * coordinate system */ 
  void ReceiveToolTransformWRTImageCoordinateSystemProcessing( void );

  /** Report tool position */
  void ReportToolPositionProcessing( void );

  /** Report plane */
  void ReportReslicingPlaneParametersProcessing( void );

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

  /** Variables for managing bounding box provider spatial object */
  BoundingBoxProviderSpatialObjectConstPointer
                                      m_BoundingBoxProviderSpatialObjectToBeSet;
  BoundingBoxProviderSpatialObjectConstPointer
                                      m_BoundingBoxProviderSpatialObject;

  /** Variables for managing tool spatial object */
  ToolSpatialObjectPointer     m_ToolSpatialObjectToBeSet;
  ToolSpatialObjectPointer     m_ToolSpatialObject;  
  
  /** Plane parameters */
  VectorType                        m_PlaneNormal;
  VectorType                        m_PlaneCenter;

  // Event macro setup to receive the tool spatial object transform
  // with respect to the reference spatial object coordinate system
  igstkLoadedEventTransductionMacro( CoordinateSystemTransformTo, 
                                        ToolTransformWRTImageCoordinateSystem );
  

  // Tool transform with respect to the reference spatial object 
  // coordinate system
  igstk::Transform m_ToolTransformWRTImageCoordinateSystem;

  /** Observer for the bounding box event */
  igstkObserverObjectMacro( BoundingBox, SpatialObject::BoundingBoxEvent,
                                              SpatialObject::BoundingBoxType);

  /** Cursor position member variables */
  double                        m_CursorPositionToBeSet[3];
  double                        m_CursorPosition[3];
  bool                          m_CursorPositionSetFlag;

  VectorType                    m_ToolPosition;

  double                        m_Bounds[6];

  BoundingBoxType::Pointer      m_BoundingBox;
   
  /** flag indicating that a tool spatial object for reslicing is set */
  bool                          m_ToolSpatialObjectSet;

};

} // end namespace igstk

#endif // __igstkReslicerPlaneSpatialObject_h
