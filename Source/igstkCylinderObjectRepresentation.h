/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCylinderObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

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
 * are the height of the object, and the radius. Default representation axis is X.
 * 
 * \ingroup Object
 */

class CylinderObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Typedefs */
  typedef CylinderObjectRepresentation                 Self;
  typedef ObjectRepresentation           Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer; 
  typedef CylinderObject                 CylinderSpatialObjectType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( Self, ObjectRepresentation );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( CylinderObjectRepresentation );

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetCylinderObject( const CylinderSpatialObjectType * cylinderObject );

protected:

  typedef StateMachine< Self > StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer   ActionType;
  typedef StateMachineType::StateType                StateType;
  typedef StateMachineType::InputType                InputType;

  igstkFriendClassMacro( StateMachineType );

  CylinderObjectRepresentation( void );
  ~CylinderObjectRepresentation( void );

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Create the VTK actors */
  void CreateActors();

private:

  /** Internal itkSpatialObject */
  CylinderSpatialObjectType::ConstPointer   m_CylinderSpatialObject;

  /** VTK class that generates the geometrical representation of the cylinder */
  vtkCylinderSource * m_CylinderSource;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationFromGeometry();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetCylinderObject(); 

private:

  StateMachineType     m_StateMachine;
  
  /** Inputs to the State Machine */
  InputType            m_ValidCylinderObjectInput;
  InputType            m_NullCylinderObjectInput;
  
  /** States for the State Machine */
  StateType            m_NullCylinderObjectState;
  StateType            m_ValidCylinderObjectState;

  CylinderSpatialObjectType::ConstPointer m_CylinderObjectToAdd;

};

} // end namespace igstk

#endif // __igstkCylinderObjectRepresentation_h

