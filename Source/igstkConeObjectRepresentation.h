/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkConeObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkConeObjectRepresentation_h
#define __igstkConeObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkConeObject.h"
#include "vtkConeSource.h"
#include "igstkStateMachine.h"

namespace igstk
{

/** \class ConeObjectRepresentation
 * 
 * \brief This class represents a cone object. 
 *
 * The parameters of the object are the height of the object, and the radius.
 * Default representation axis is X.  The cylindrical object is rendered in a
 * VTK scene using the vtkConeSource object.
 * 
 * 
 *  \image html  igstkConeObjectRepresentation.png "State Machine Diagram" 
 *
 *  \image latex igstkConeObjectRepresentation.eps 
 *
 *  \ingroup ObjectRepresentation
 */

class ConeObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( ConeObjectRepresentation, 
                                 ObjectRepresentation )

public:

  /** Typedefs */
  typedef ConeObject                 ConeSpatialObjectType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetConeObject( const ConeSpatialObjectType * coneObject );

protected:

  ConeObjectRepresentation( void );
  virtual ~ConeObjectRepresentation( void );

  ConeObjectRepresentation(const Self&); //purposely not implemented
  void operator=(const Self&);           //purposely not implemented

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Create the VTK actors */
  void CreateActors();

private:

  /** Internal itkSpatialObject */
  ConeSpatialObjectType::ConstPointer   m_ConeSpatialObject;

  /** VTK class that generates the geometrical representation of the cone */
  vtkConeSource * m_ConeSource;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetConeObjectProcessing(); 

  /** Method for performing a null operation during a 
   *  State Machine transition */
  void NoProcessing();

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidConeObject );
  igstkDeclareInputMacro( NullConeObject );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullConeObject );
  igstkDeclareStateMacro( ValidConeObject );

  ConeSpatialObjectType::ConstPointer m_ConeObjectToAdd;

};


} // end namespace igstk

#endif // __igstkConeObjectRepresentation_h
