/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMeshResliceObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkMeshResliceObjectRepresentation_h
#define __igstkMeshResliceObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkTransform.h"
#include "igstkMeshObject.h"
#include "igstkStateMachine.h"
#include "igstkObjectRepresentation.h"
#include "igstkReslicerPlaneSpatialObject.h"

class vtkPlane;
class vtkCutter;
class vtkProperty;

namespace igstk
{

/** \class MeshResliceObjectRepresentation
 * 
 * \brief This class represents a Mesh object.
 * 
 *
 * \ingroup ObjectRepresentation
 */

class MeshResliceObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( MeshResliceObjectRepresentation, 
                                 ObjectRepresentation )

public:
    
  /** Typedefs */

  typedef MeshObject                               MeshObjectType;

  typedef MeshObjectType::PointType                PointType;

  typedef ReslicerPlaneSpatialObject               ReslicerPlaneType;
  typedef ReslicerPlaneType::Pointer               ReslicerPlanePointerType;  

  typedef ReslicerPlaneType::VectorType            VectorType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetMeshObject( const MeshObjectType * MeshObject );

  /** Sets visibility */
  void SetVisibility(bool visibility);

  /** Sets the reslicer plane */
  void RequestSetReslicePlaneSpatialObject( const ReslicerPlaneType *
                                                            planeSpatialObject);
  /** Set/Get line width */
  void SetLineWidth(double LineWidth);
  igstkGetMacro( LineWidth, double );

  // void RequestSetResliceAxes( vtkMatrix4x4 *matrix );

protected:

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 
  
  /** Constructor */
  MeshResliceObjectRepresentation( void );

  /** Destructor */
  ~MeshResliceObjectRepresentation( void );

  /** Create the VTK actors */
  void CreateActors();

  /** Verify time stamp. Use the reslicing tool transform to verify 
  * the time stamp */
  virtual bool VerifyTimeStamp() const;

private:
  
  /** Declare the observers that will receive the reslicing plane parameters 
   * from the ReslicerPlaneSpatialObject */

  igstkObserverMacro( ReslicerPlaneCenter, 
                                    ReslicerPlaneType::ReslicerPlaneCenterEvent,
                                                 ReslicerPlaneType::VectorType);

  ReslicerPlaneCenterObserver::Pointer  m_ReslicerPlaneCenterObserver;

  igstkObserverMacro( ReslicerPlaneNormal, 
                                    ReslicerPlaneType::ReslicerPlaneNormalEvent,
                                                 ReslicerPlaneType::VectorType);

  ReslicerPlaneNormalObserver::Pointer  m_ReslicerPlaneNormalObserver;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetMeshObjectProcessing(); 

  /** Null operation for a State Machine transition */
  void NoProcessing();

  /** Set reslice plane spatial object processing */
  void SetReslicePlaneSpatialObjectProcessing();

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidMeshObject );
  igstkDeclareInputMacro( NullMeshObject );
  igstkDeclareInputMacro( ValidReslicePlaneSpatialObject );
  igstkDeclareInputMacro( InValidReslicePlaneSpatialObject );

  igstkDeclareInputMacro( EmptyImageSpatialObject ); 
 // igstkDeclareInputMacro( Reslice );

  /** States for the State Machine */
  igstkDeclareStateMacro( NullMeshObject );
  igstkDeclareStateMacro( ValidMeshObject );
  igstkDeclareStateMacro( ValidReslicePlaneSpatialObject );

  /** Variables for managing the mesh spatial object */
  MeshObjectType::ConstPointer   m_MeshObject;
  MeshObjectType::ConstPointer   m_MeshObjectToBeSet;

  /** Variables for managing reslice plane spatial object */
  ReslicerPlanePointerType  m_ReslicePlaneSpatialObjectToBeSet;
  ReslicerPlanePointerType  m_ReslicePlaneSpatialObject;

  /** Plane defining the contour */
  vtkPlane*    m_Plane;
  vtkCutter*   m_Cutter;
  vtkProperty* m_ContourProperty;

  //vtkTubeFilter* m_Tuber;
  double       m_LineWidth;
};

} // end namespace igstk

#endif // __igstkMeshResliceObjectRepresentation_h
