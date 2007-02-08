/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkContourMeshObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkContourMeshObjectRepresentation_h
#define __igstkContourMeshObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkContourObjectRepresentation.h"
#include "igstkMeshObject.h"
#include "igstkStateMachine.h"

#include <vtkPlane.h>

namespace igstk
{

/** \class ContourMeshObjectRepresentation
 * 
 * \brief This class represents a Mesh object.
 * 
 *
 * \ingroup ObjectRepresentation
 */

class ContourMeshObjectRepresentation 
: public ContourObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( ContourMeshObjectRepresentation, 
                                 ContourObjectRepresentation )

public:
    
  /** Typedefs */
  typedef MeshObject                     MeshObjectType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetMeshObject( const MeshObjectType * MeshObject );

protected:

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 
  
  /** Constructor */
  ContourMeshObjectRepresentation( void );

  /** Destructor */
  ~ContourMeshObjectRepresentation( void );

  /** Create the VTK actors */
  void CreateActors();

private:

  /** Internal itkSpatialObject */
  MeshObjectType::ConstPointer   m_MeshObject;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetMeshObjectProcessing(); 

  /** Null operation for a State Machine transition */
  void NoProcessing();

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidMeshObject );
  igstkDeclareInputMacro( NullMeshObject );

  /** States for the State Machine */
  igstkDeclareStateMacro( NullMeshObject );
  igstkDeclareStateMacro( ValidMeshObject );

  MeshObjectType::ConstPointer m_MeshObjectToAdd;
};


} // end namespace igstk

#endif // __igstkContourMeshObjectRepresentation_h
