/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTubeObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTubeObjectRepresentation_h
#define __igstkTubeObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkTubeObject.h"
#include "igstkStateMachine.h"

namespace igstk
{

/** \class TubeObjectRepresentation
 * 
 * \brief This class implements the representation a Tube object.
 * Basically the tube is represented as a VTK actor using a
 * vtkTubeFilter.
 *
 * \image html  igstkTubeObjectRepresentation.png "State Machine Diagram"
 * \image latex igstkTubeObjectRepresentation.eps "State Machine Diagram"
 *
 * \ingroup ObjectRepresentation
 */

class TubeObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( TubeObjectRepresentation, 
                                 ObjectRepresentation )

public:

  typedef TubeObject                     TubeObjectType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetTubeObject( const TubeObjectType * TubeObject );

protected:
  
  /** Constructor */
  TubeObjectRepresentation( void );

  /** Destructor */
  ~TubeObjectRepresentation( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Create the VTK actors */
  void CreateActors();

private:

  /** Internal itkSpatialObject */
  TubeObjectType::ConstPointer   m_TubeSpatialObject;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetTubeObjectProcessing(); 

  /** Null operation for a State Machine transition */
  void NoProcessing();

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidTubeObject );
  igstkDeclareInputMacro( NullTubeObject );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullTubeObject );
  igstkDeclareStateMacro( ValidTubeObject );

  TubeObjectType::ConstPointer m_TubeObjectToAdd;

};


} // end namespace igstk

#endif // __igstkTubeObjectRepresentation_h
