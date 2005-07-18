/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTubeObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

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
 * \brief This class represents a Tube object. The parameters of the object
 * are the height of the object, and the radius. Default representation axis is
 * X.  The cylindrical object is rendered in a VTK scene using the
 * vtkTubeSource object.
 * 
 * \ingroup ObjectRepresentation
 */

class TubeObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Typedefs */
  typedef TubeObjectRepresentation       Self;
  typedef ObjectRepresentation           Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer; 
  typedef TubeObject                     TubeObjectType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( TubeObjectRepresentation, ObjectRepresentation );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( TubeObjectRepresentation );

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetTubeObject( const TubeObjectType * TubeObject );

  TubeObjectRepresentation( void );
  ~TubeObjectRepresentation( void );

  /** Create the VTK actors */
  void CreateActors();

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

protected:

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Internal itkSpatialObject */
  TubeObjectType::ConstPointer   m_TubeSpatialObject;

  /** VTK class that generates the geometrical representation of the Tube */
//  vtkTubeSource * m_TubeSource;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentation();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetTubeObject(); 

private:

  /** Inputs to the State Machine */
  InputType            m_ValidTubeObjectInput;
  InputType            m_NullTubeObjectInput;
  
  /** States for the State Machine */
  StateType            m_NullTubeObjectState;
  StateType            m_ValidTubeObjectState;

  TubeObjectType::ConstPointer m_TubeObjectToAdd;

};


} // end namespace igstk

#endif // __igstkTubeObjectRepresentation_h

