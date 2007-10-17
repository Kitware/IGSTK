/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCylinderObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCylinderObjectRepresentation_h
#define __igstkCylinderObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkCylinderObject.h"
#include "vtkCylinderSource.h"
#include "igstkStateMachine.h"

namespace igstk
{

/** \class CylinderObjectRepresentation
 * 
 * \brief This class represents a cylinder object. The parameters of the object
 * are the height of the object, and the radius. Default representation axis is
 * X.  The cylindrical object is rendered in a VTK scene using the
 * vtkCylinderSource object.
 * 
 * \image html  igstkCylinderObjectRepresentation.png 
 *              "CylinderObjectRepresentation State Machine Diagram"
 * \image latex igstkCylinderObjectRepresentation.eps 
 *              "CylinderObjectRepresentation State Machine Diagram"
 *
 * \ingroup ObjectRepresentation
 */

class CylinderObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CylinderObjectRepresentation, 
                                 ObjectRepresentation )

public:

  /** Typedefs */
  typedef CylinderObject                 CylinderSpatialObjectType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetCylinderObject( const CylinderSpatialObjectType * 
                                                             cylinderObject );

protected:

  CylinderObjectRepresentation( void );
  virtual ~CylinderObjectRepresentation( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Create the VTK actors */
  void CreateActors();

private:

  /** Internal itkSpatialObject */
  CylinderSpatialObjectType::ConstPointer   m_CylinderSpatialObject;

  /** VTK class that generates the geometrical representation of the cylinder */
  vtkCylinderSource * m_CylinderSource;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetCylinderObjectProcessing(); 

  /** Method for performing a null operation during a 
   *  State Machine transition */
  void NoProcessing();

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidCylinderObject );
  igstkDeclareInputMacro( NullCylinderObject );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullCylinderObject );
  igstkDeclareStateMacro( ValidCylinderObject );

  CylinderSpatialObjectType::ConstPointer m_CylinderObjectToAdd;

};


} // end namespace igstk

#endif // __igstkCylinderObjectRepresentation_h
