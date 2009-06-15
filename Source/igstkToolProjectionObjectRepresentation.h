/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolProjectionObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkToolProjectionObjectRepresentation_h
#define __igstkToolProjectionObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkToolProjectionSpatialObject.h"
#include "igstkReslicerPlaneSpatialObject.h"
#include "igstkStateMachine.h"

class vtkLineSource;
class vtkProperty;

namespace igstk
{

/** \class ToolProjectionObjectRepresentation
 * 
 * \brief This class provides a visual representation of a ToolProjectionObject.
 *
 * This class displays the projection of a tool spatial object on top of a 
 * reslicing plane.
 *
 * \sa ToolProjectionObject
 *
 * \ingroup ObjectRepresentation
 */

class ToolProjectionObjectRepresentation : public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( ToolProjectionObjectRepresentation, 
                                 ObjectRepresentation )

public:

  /** Typedefs */ 

  typedef ReslicerPlaneSpatialObject                   ReslicerPlaneType;

  typedef ReslicerPlaneType::Pointer            ReslicerPlanePointerType;  

  typedef ReslicerPlaneType::VectorType                       VectorType;

  typedef ToolProjectionSpatialObject    ToolProjectionSpatialObjectType;

  /** Provides the reslicing plane onto which the tool is projected */
  void RequestSetReslicePlaneSpatialObject( const ReslicerPlaneType *
                                                            planeSpatialObject);

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetToolProjectionObject( const ToolProjectionSpatialObjectType * 
                                                         toolProjectionObject );

  virtual void SetVisibility ( bool visible);

  /** Set/Get line width */
  void SetLineWidth(double LineWidth);
  igstkGetMacro( LineWidth, double );

protected:

  ToolProjectionObjectRepresentation( void );
  virtual ~ToolProjectionObjectRepresentation( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Create the VTK actors */
  void CreateActors();

  /** Verify time stamp. Use the reslicing tool transform to verify 
  * the time stamp */
  virtual bool VerifyTimeStamp() const;

private:

  ToolProjectionObjectRepresentation(const Self&); //purposely not implemented
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

  /** Declare the observer that will receive the reslice plane normal from the
  * ReslicerPlaneSpatialObject */
  igstkObserverMacro( ReslicerPlaneNormal, 
                                    ReslicerPlaneType::ReslicerPlaneNormalEvent,
                                                 ReslicerPlaneType::VectorType);

  ReslicerPlaneNormalObserver::Pointer  m_ReslicerPlaneNormalObserver;

private:

  /** Variables for managing reslice plane spatial object */
  ReslicerPlanePointerType  m_ReslicePlaneSpatialObjectToBeSet;
  ReslicerPlanePointerType  m_ReslicePlaneSpatialObject;

  vtkLineSource* m_LineSource;  
  vtkProperty*   m_LineProperty;

  double m_LineWidth;

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

#endif // __igstkToolProjectionObjectRepresentation_h
