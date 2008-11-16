/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolProjectionRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkToolProjectionRepresentation_h
#define __igstkToolProjectionRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkToolProjectionObject.h"
#include "igstkReslicerPlaneSpatialObject.h"
#include "igstkStateMachine.h"

class vtkLineSource;
class vtkTubeFilter;
class vtkPolyDataMapper;

namespace igstk
{

/** \class ToolProjectionRepresentation
 * 
 * \brief This class provides a visual representation of a ToolProjectionObject. 
 *
 *
 * \sa ToolProjectionRepresentation
 *
 * \ingroup ObjectRepresentation
 */

class ToolProjectionRepresentation : public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( ToolProjectionRepresentation, 
                                 ObjectRepresentation )

public:

  /** Typedefs */ 

  typedef ReslicerPlaneSpatialObject                  ReslicerPlaneType;

  typedef ReslicerPlaneType::Pointer            ReslicerPlanePointerType;  

  typedef ReslicerPlaneType::VectorType                       VectorType;

  typedef ToolProjectionObject           ToolProjectionSpatialObjectType;

  /** Provides the reslicing plane onto which the tool is projected */
  void RequestSetReslicePlaneSpatialObject( const ReslicerPlaneType *
                                                             planeSpatialObject);

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetToolProjectionObject( const ToolProjectionSpatialObjectType * 
                                                             toolProjectionObject );

  virtual void SetVisibility ( bool visible);

protected:

  ToolProjectionRepresentation( void );
  virtual ~ToolProjectionRepresentation( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Create the VTK actors */
  void CreateActors();

  /** Verify time stamp. Use the reslicing tool transform to verify 
  * the time stamp */
  virtual bool VerifyTimeStamp() const;

private:

  ToolProjectionRepresentation(const Self&); //purposely not implemented
  void operator=(const Self&);     //purposely not implemented

  
  /** Internal itkSpatialObject */
  ToolProjectionSpatialObjectType::ConstPointer   m_ToolProjectionSpatialObject;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetToolProjectionObjectProcessing();

  /** Method for performing a null operation during a State Machine 
   *  transition */
  void NoProcessing();

  /** Set the reslice plane spatial object */
  void SetReslicePlaneSpatialObjectProcessing();

   /** Define observers for event communication */

  igstkObserverMacro( ImageBounds, igstk::ImageBoundsEvent, 
                                  igstk::EventHelperType::ImageBoundsType );

  /** Declare the observer that will receive the reslicer plane normal from the
  * ReslicerPlaneSpatialObject */
  igstkObserverMacro( ReslicerPlaneNormal, ReslicerPlaneType::ReslicerPlaneNormalEvent,
                      ReslicerPlaneType::VectorType);

  ReslicerPlaneNormalObserver::Pointer  m_ReslicerPlaneNormalObserver;

private:

  /** Variables for maanging reslice plane spatial object */
  ReslicerPlanePointerType  m_ReslicePlaneSpatialObjectToBeSet;
  ReslicerPlanePointerType  m_ReslicePlaneSpatialObject;

  vtkLineSource* m_LineSource;
  vtkTubeFilter *m_Tuber;
  vtkPolyDataMapper* m_LineMapper;

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidToolProjectionObject );
  igstkDeclareInputMacro( NullToolProjectionObject );
  igstkDeclareInputMacro( ValidReslicePlaneSpatialObject );
  igstkDeclareInputMacro( InValidReslicePlaneSpatialObject );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullToolProjectionObject );
  igstkDeclareStateMacro( ValidToolProjectionObject );
  igstkDeclareStateMacro( ValidReslicePlaneSpatialObject );

  ToolProjectionSpatialObjectType::ConstPointer m_ToolProjectionObjectToAdd;

};

} // end namespace igstk

#endif // __igstkToolProjectionRepresentation_h
