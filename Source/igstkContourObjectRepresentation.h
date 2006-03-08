/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkContourObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkContourObjectRepresentation_h
#define __igstkContourObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkMeshObject.h"
#include "igstkStateMachine.h"

#include <vtkPlane.h>

namespace igstk
{

/** \class ContourObjectRepresentation
 * 
 * \brief This class represents a Mesh object.
 * \ingroup ObjectRepresentation
 */

class ContourObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardAbstractClassTraitsMacro( ContourObjectRepresentation, 
                                         ObjectRepresentation )

public:

  /** Orientation Type: Publically declared
   * orientation types supported for slice viewing. */
  typedef enum
    { 
    Sagittal, 
    Coronal, 
    Axial
    } 
  OrientationType;

  /** Request the state machine to attempt to select a slice position */
  void RequestSetSlicePosition( float slicePosition );

  /** Request the state machine to attempt to select a slice orientation */
  void RequestSetOrientation( OrientationType orientation );

protected:

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 
  
  /** Constructor */
  ContourObjectRepresentation( void );

  /** Destructor */
  ~ContourObjectRepresentation( void );

  /** Create the VTK actors */
  void CreateActors() = 0;

  /** Plane defining the contour */
  vtkPlane* m_Plane;

private:

 /** Attempt to set the Slice Number. */
  void AttemptSetSlicePositionProcessing();

  /** Actually set the Slice Number. */
  void SetSlicePositionProcessing();

  /** Actually set the Slice Orientation. */
  void SetOrientationProcessing();

  /** Null operation for a State Machine transition */
  void NoProcessing();

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( SetSlicePosition );
  igstkDeclareInputMacro( ValidSlicePosition );
  igstkDeclareInputMacro( InvalidSlicePosition );
  igstkDeclareInputMacro( ValidOrientation );

  /** States for the State Machine */
  igstkDeclareStateMacro( ValidOrientation );
  igstkDeclareStateMacro( ValidSlicePosition );
  igstkDeclareStateMacro( AttemptingToSetSlicePosition );
  igstkDeclareStateMacro( Initial );
  
  /** Variables for managing the Slice number through the StateMachine */
  float      m_SlicePositionToBeSet;
  float      m_SlicePosition;

  /** Variables for managing the Orientation of the slices */
  OrientationType      m_OrientationToBeSet;
  OrientationType      m_Orientation;

};


} // end namespace igstk

#endif // __igstkContourObjectRepresentation_h
