/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkBoxObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkBoxObjectRepresentation_h
#define __igstkBoxObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkBoxObject.h"
#include "vtkCubeSource.h"
#include "igstkStateMachine.h"

namespace igstk
{

/** \class BoxObjectRepresentation
 * 
 * \brief This class represents a box object. 
 *
 * The parameters of the object are the height of the object, and the radius.
 * Default representation axis is X.  The cylindrical object is rendered in a
 * VTK scene using the vtkCubeSource object.
 * 
 * 
 * \image html  igstkBoxObjectRepresentation.png  "State Machine Diagram"
 * \image latex igstkBoxObjectRepresentation.eps  "State Machine Diagram" 
 * 
 *
 * \ingroup ObjectRepresentation
 */

class BoxObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( BoxObjectRepresentation, ObjectRepresentation )

public:

  /** Typedefs */
  typedef BoxObject                 BoxSpatialObjectType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetBoxObject( const BoxSpatialObjectType * boxObject );

protected:

  BoxObjectRepresentation( void );
  virtual ~BoxObjectRepresentation( void );

  BoxObjectRepresentation(const Self&);  //purposely not implemented
  void operator=(const Self&);           //purposely not implemented

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Create the VTK actors */
  void CreateActors();

private:

  /** Internal itkSpatialObject */
  BoxSpatialObjectType::ConstPointer   m_BoxSpatialObject;

  /** VTK class that generates the geometrical representation of the box */
  vtkCubeSource * m_BoxSource;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetBoxObjectProcessing(); 

  /** Method for performing a null operation during a State Machine 
   *  transition */
  void NoProcessing();

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidBoxObject );
  igstkDeclareInputMacro( NullBoxObject );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullBoxObject );
  igstkDeclareStateMacro( ValidBoxObject );

  BoxSpatialObjectType::ConstPointer m_BoxObjectToAdd;

};


} // end namespace igstk

#endif // __igstkBoxObjectRepresentation_h
