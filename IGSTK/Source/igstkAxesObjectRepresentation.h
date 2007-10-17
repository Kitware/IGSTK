/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAxesObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAxesObjectRepresentation_h
#define __igstkAxesObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkAxesObject.h"
#include "igstkStateMachine.h"

namespace igstk
{

/** \class AxesObjectRepresentation
 * 
 * \brief This class provides a visual representation of an Axes object. 
 *
 * The parameters of the object are the height of the object, and the radius.
 * Default representation axis is X.  The cylindrical object is rendered in a
 * VTK scene using the vtkAxesSource object.
 *
 *  \image html  igstkAxesObjectRepresentation.png 
 *               "Axes Object Representation State Machine Diagram"
 *  \image latex igstkAxesObjectRepresentation.eps 
 *               "Axes Object Representation State Machine Diagram"
 * \sa AxesObject
 *
 * \ingroup ObjectRepresentation
 */

class AxesObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( AxesObjectRepresentation, 
                                 ObjectRepresentation )

public:

  /** Typedefs */
  typedef AxesObject                 AxesSpatialObjectType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetAxesObject( const AxesSpatialObjectType * AxesObject );

protected:

  AxesObjectRepresentation( void );
  virtual ~AxesObjectRepresentation( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Create the VTK actors */
  void CreateActors();

private:

  AxesObjectRepresentation(const Self&); //purposely not implemented
  void operator=(const Self&);     //purposely not implemented

  /** Internal itkSpatialObject */
  AxesSpatialObjectType::ConstPointer   m_AxesSpatialObject;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetAxesObjectProcessing(); 

  /** Method for performing a null operation during a State Machine 
   *  transition */
  void NoProcessing();

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidAxesObject );
  igstkDeclareInputMacro( NullAxesObject );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullAxesObject );
  igstkDeclareStateMacro( ValidAxesObject );

  AxesSpatialObjectType::ConstPointer m_AxesObjectToAdd;

};


} // end namespace igstk

#endif // __igstkAxesObjectRepresentation_h
