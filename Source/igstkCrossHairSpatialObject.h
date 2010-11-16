/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCrossHairSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCrossHairSpatialObject_h
#define __igstkCrossHairSpatialObject_h

#include "igstkMacros.h"
#include "igstkTransform.h"
#include "igstkSpatialObject.h"
#include "itkGroupSpatialObject.h"
#include "igstkStateMachine.h"

namespace igstk
{

/** \class CrossHairSpatialObject
 * 
 * \brief This class represents a cross hair spatial object. 
 *
 * This class is used to indicate a position in an input image. The position  
 * could be manually set using RequestSetCursorPostion or specified using a
 *  reslicing tool transform RequestSetToolSpatialObject
 * 
 * \ingroup Object
 */


class CrossHairSpatialObject : public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CrossHairSpatialObject, SpatialObject )

  /** Typedefs */
  typedef igstk::Transform::PointType          PointType;
  typedef igstk::Transform::VectorType         VectorType;

  typedef SpatialObject                        SpatialObjectType;

  typedef SpatialObjectType::BoundingBoxType   BoundingBoxType;
  typedef SpatialObjectType::Pointer           SpatialObjectPointerType;

public:  

  /** Inquiry if a tool spatial object is set */
  bool IsToolSpatialObjectSet();

  /** Inquiry if tool position is inside bounds */
  bool IsInsideBounds();

  /** Request get cross hair position */
  void RequestGetCrossHairPosition();

  /** Request set tool spatial object */
  void RequestSetToolSpatialObject( const SpatialObjectType * spatialObject );

  /** Request set bounding box provider spatial object */
  void RequestSetBoundingBoxProviderSpatialObject( 
                                      const SpatialObjectType * spatialObject );

  /** Request set cursor position */
  void RequestSetCursorPosition( PointType point);

  /** Get tool transform */
  igstk::Transform GetToolTransform() const;

  /** Get boundings by index */
  double GetBoundingBoxDimensionByIndex(unsigned int index) const;
 
protected:

  CrossHairSpatialObject( void );
  ~CrossHairSpatialObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  igstkObserverMacro( ImageTransform, CoordinateSystemTransformToEvent, 
     CoordinateSystemTransformToResult );

private:

  /** Typedefs */
  typedef itk::GroupSpatialObject<3>     CrossHairSpatialObjectType;

  CrossHairSpatialObject(const Self&);         //purposely not implemented
  void operator=(const Self&);     //purposely not implemented  
   
  /** Set Cursor position processing */
  void SetCursorPositionProcessing( void );

  /** Attempt set Cursor position processing */
  void AttemptSetCursorPositionProcessing( void );

  /** Attempt set bounding box provider spatial object */
  void AttemptSetBoundingBoxProviderSpatialObjectProcessing( void );

  /** Set tool spatial object processing */
  void SetToolSpatialObjectProcessing( void );

  /** Set reference spatial object processing */
  void SetBoundingBoxProviderSpatialObjectProcessing( void );

  /** Get cross hair position processing */
  void GetCrossHairPositionProcessing( void );

  /** Report invalid tool spatial object type */
  void ReportInvalidToolSpatialObjectProcessing( void );

  /** Report invalid reference spatial object */
  void ReportInvalidBoundingBoxProviderSpatialObjectProcessing( void );

  /** Report invalid Cursor position */
  void ReportInvalidCursorPositionProcessing( void );

  /** Report invalid request */
  void ReportInvalidRequestProcessing( void );

  /** Request get tool transform with respect to image coordinate system */ 
  void RequestGetToolTransformWRTImageCoordinateSystemProcessing( void );

  /** Request update tool transform WRT image coordinate system */
  void RequestUpdateToolTransformWRTImageCoordinateSystem();

  /** Receive tool transform with respect to image coordinate system */ 
  void ReceiveToolTransformWRTImageCoordinateSystemProcessing( void );

  CrossHairSpatialObjectType::Pointer     m_CrossHairSpatialObject;

  PointType                               m_Position;

  /** Cursor position member variables */
  double               m_CursorPositionToBeSet[3];
  double               m_CursorPosition[3];
  bool                 m_CursorPositionSetFlag;

  /** tool spatial object member variables */
  bool                         m_ToolSpatialObjectSet;
  bool                         m_InsideBounds;
  SpatialObjectPointerType     m_ToolSpatialObjectToBeSet;
  SpatialObjectPointerType     m_ToolSpatialObject;

  /** reference spatial object member variables */
  SpatialObjectPointerType     m_BoundingBoxProviderSpatialObjectToBeSet;
  SpatialObjectPointerType     m_BoundingBoxProviderSpatialObject;

  /** bounding box member variables */
  BoundingBoxType::ConstPointer           m_BoundingBox;

  /** tool transform with respect to the image coordinate system */
  igstk::Transform m_ToolTransformWRTImageCoordinateSystem;

  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( BoundingBoxProviderSpatialObjectSet );
  igstkDeclareStateMacro( AttemptingToSetBoundingBoxProviderSpatialObject );
  igstkDeclareStateMacro( AttemptingToSetCursorPosition );
  igstkDeclareStateMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem);

  /** Inputs to the State Machine */  
  igstkDeclareInputMacro( SetBoundingBoxProviderSpatialObject );
  igstkDeclareInputMacro( ValidBoundingBoxProviderSpatialObject );
  igstkDeclareInputMacro( InValidBoundingBoxProviderSpatialObject );
  igstkDeclareInputMacro( ValidToolSpatialObject );
  igstkDeclareInputMacro( InValidToolSpatialObject );
  igstkDeclareInputMacro( SetCursorPosition );
  igstkDeclareInputMacro( ValidCursorPosition );
  igstkDeclareInputMacro( InValidCursorPosition );
  igstkDeclareInputMacro( GetToolTransformWRTImageCoordinateSystem );
  igstkDeclareInputMacro( ToolTransformWRTImageCoordinateSystem );
  igstkDeclareInputMacro( GetCrossHairPosition );

  // Event macro setup to receive the tool spatial object transform
  // with respect to the image coordinate system
  igstkLoadedEventTransductionMacro( CoordinateSystemTransformTo, 
                                        ToolTransformWRTImageCoordinateSystem );

  igstkObserverConstObjectMacro( BoundingBox, 
                                            SpatialObjectType::BoundingBoxEvent,
                                            SpatialObjectType::BoundingBoxType);

  inline 
  PointType 
  TransformToPoint( igstk::Transform transform )
    {
    PointType point;
    for (unsigned int i=0; i<3; i++)
      {
      point[i] = transform.GetTranslation()[i];
      }
    return point;
    }

};

} // end namespace igstk

#endif // __igstkCrossHairSpatialObject_h
