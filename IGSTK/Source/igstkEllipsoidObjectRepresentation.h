/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkEllipsoidObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkEllipsoidObjectRepresentation_h
#define __igstkEllipsoidObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkEllipsoidObject.h"
#include "vtkSuperquadricSource.h"
#include "igstkStateMachine.h"

namespace igstk
{

/** \class EllipsoidObjectRepresentation
 * 
 * \brief This class represents an ellipsoid object. The parameters of the
 * object are the three radii defining the lenght of the principal axes of the
 * ellipsoid.  The axes of the ellipsoid are orthogonal. The Ellipsoidal object
 * is rendered in a VTK scene using the vtkSuperquadricSource object.
 *
 * \image html  igstkEllipsoidObjectRepresentation.png "State Machine Diagram"
 * \image latex igstkEllipsoidObjectRepresentation.eps "State Machine Diagram"
 * 
 * \ingroup ObjectRepresentation
 */

class EllipsoidObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( EllipsoidObjectRepresentation, 
                                 ObjectRepresentation )

public:

  /** Typedefs */
  typedef EllipsoidObject                EllipsoidObjectType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetEllipsoidObject( const EllipsoidObjectType * ellipsoidObject );

protected:

  EllipsoidObjectRepresentation( void );
  virtual ~EllipsoidObjectRepresentation( void );

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Create the VTK actors per view */
  void CreateActors();

private:

  /** Internal itkSpatialObject */
  EllipsoidObjectType::ConstPointer   m_EllipsoidObject;

  /** VTK class that generates the geometrical representation of the cylinder */
  vtkSuperquadricSource * m_EllipsoidSource;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetEllipsoidObjectProcessing(); 

  /** Null operation for a State Machine transition */
  void NoProcessing();

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidEllipsoidObject );
  igstkDeclareInputMacro( NullEllipsoidObject );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullEllipsoidObject );
  igstkDeclareStateMacro( ValidEllipsoidObject );

  EllipsoidObjectType::ConstPointer m_EllipsoidObjectToAdd;

};

} // end namespace igstk

#endif // __igstkEllipsoidObjectRepresentation_h
