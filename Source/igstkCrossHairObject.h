/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCrossHairObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCrossHairObject_h
#define __igstkCrossHairObject_h

#include "igstkMacros.h"
#include "igstkTransform.h"
#include "igstkSpatialObject.h"
#include "itkGroupSpatialObject.h"
#include "igstkStateMachine.h"

namespace igstk
{

/** \class CrossHairObject
 * 
 * \brief This class represents a Axes object. 
 *
 * This class ...
 * 
 * \ingroup Object
 */


class CrossHairObject : public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CrossHairObject, SpatialObject )

  /** Typedefs */

  typedef igstk::Transform::PointType        PointType;
  typedef igstk::Transform::VectorType       VectorType;

  typedef SpatialObject::BoundingBoxType     BoundingBoxType;

  typedef SpatialObject                      ToolSpatialObjectType;
  typedef ToolSpatialObjectType::Pointer     ToolSpatialObjectPointer;

public:  


  /** Inquiry if a tool spatial object is set */
  bool  IsToolSpatialObjectSet();

  /** Request get cross hair position */
  void RequestGetCrossHairPosition();

  /** Request set tool spatial object */
  void RequestSetToolSpatialObject( const ToolSpatialObjectType * toolSpatialObject );

  /** Request set bounding box */
  void RequestSetBoundingBox(const BoundingBoxType* bounds);

  /** Request set mouse position */
  void RequestSetMousePosition( PointType point);

//  igstkGetMacro(Position, PointType);

  double GetBoundingBoxDimensionByIndex(unsigned int index);
 
protected:

  CrossHairObject( void );
  ~CrossHairObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  igstkObserverMacro( ImageTransform, CoordinateSystemTransformToEvent, 
     CoordinateSystemTransformToResult );

private:

  /** Typedefs */
  typedef itk::GroupSpatialObject<3>     CrossHairSpatialObjectType;

  CrossHairObject(const Self&);         //purposely not implemented
  void operator=(const Self&);     //purposely not implemented  

  /** Attempt set bounding box attempt */
  void AttemptSetBoundingBoxProcessing( void );
  
  /** Set bounding box processing */
  void SetBoundingBoxProcessing( void );
 
  /** Set mouse position processing */
  void SetMousePositionProcessing( void );

  /** Attempt set mouse position processing */
  void AttemptSetMousePositionProcessing( void );

  /** Set tool spatial object processing */
  void SetToolSpatialObjectProcessing( void );

  /** Get cross hair position processing */
  void GetCrossHairPositionProcessing( void );

  /** Report invalid tool spatial object type */
  void ReportInvalidToolSpatialObjectProcessing( void );

  /** Report invalid mouse position */
  void ReportInvalidMousePositionProcessing( void );

  /** Report invalid bounding box */
  void ReportInvalidBoundingBoxProcessing( void );

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

  /** mouse position member variables */
  double               m_MousePositionToBeSet[3];
  double               m_MousePosition[3];
  bool                 m_MousePositionSetFlag;

  /** tool spatial object member variables */
  bool                         m_ToolSpatialObjectSet;
  ToolSpatialObjectPointer     m_ToolSpatialObjectToBeSet;
  ToolSpatialObjectPointer     m_ToolSpatialObject;

  /** bounding box member variables */
  BoundingBoxType::ConstPointer           m_BoundingBox;
  BoundingBoxType::ConstPointer           m_BoundingBoxToBeSet;

  /** tool transform with respect to the image coordinate system */
  igstk::Transform m_ToolTransformWRTImageCoordinateSystem;

  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( ToolSpatialObjectSet );
  igstkDeclareStateMacro( ValidBoundingBoxSet );
  igstkDeclareStateMacro( AttemptingToSetBoundingBox );
  igstkDeclareStateMacro( AttemptingToSetMousePosition );
  igstkDeclareStateMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem );

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( SetBoundingBox );
  igstkDeclareInputMacro( ValidBoundingBox );
  igstkDeclareInputMacro( InValidBoundingBox );
  igstkDeclareInputMacro( ValidToolSpatialObject );
  igstkDeclareInputMacro( InValidToolSpatialObject );
  igstkDeclareInputMacro( SetMousePosition );
  igstkDeclareInputMacro( ValidMousePosition );
  igstkDeclareInputMacro( InValidMousePosition );
  igstkDeclareInputMacro( GetToolTransformWRTImageCoordinateSystem );
  igstkDeclareInputMacro( ToolTransformWRTImageCoordinateSystem );
  igstkDeclareInputMacro( GetCrossHairPosition );

  // Event macro setup to receive the tool spatial object transform
  // with respect to the image coordinate system
  igstkLoadedEventTransductionMacro( CoordinateSystemTransformTo, ToolTransformWRTImageCoordinateSystem );

};

} // end namespace igstk

#endif // __igstkCrossHairObject_h
