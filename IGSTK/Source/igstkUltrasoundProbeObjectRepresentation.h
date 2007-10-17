/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUltrasoundProbeObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkUltrasoundProbeObjectRepresentation_h
#define __igstkUltrasoundProbeObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkUltrasoundProbeObject.h"
#include "igstkStateMachine.h"

namespace igstk
{

/** \class UltrasoundProbeObjectRepresentation
 * 
 * \brief This class represents a UltrasoundProbe object. 
 * The representation of the geometric model of the probe is done using
 * VTK implicit functions combined with the Marching Cube algorithm.
 * Boolean operators are also used on the implicit functions to obtain
 * a nice looking shape.
 *
 * \image html  igstkUltrasoundProbeObjectRepresentation.png
 *             "UltraSound Proble Object Representation State Diagram"
 *
 * \image latex igstkUltrasoundProbeObjectRepresentation.eps
 *             "UltraSound Proble Object Representation State Diagram"
 *

 *
 *
 * \ingroup ObjectRepresentation
 */

class UltrasoundProbeObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( UltrasoundProbeObjectRepresentation, 
                                 ObjectRepresentation )

public:

  /** Typedefs */
  typedef UltrasoundProbeObject  UltrasoundProbeSpatialObjectType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetUltrasoundProbeObject( const 
                   UltrasoundProbeSpatialObjectType * UltrasoundProbeObject );

protected:

  UltrasoundProbeObjectRepresentation( void );
  virtual ~UltrasoundProbeObjectRepresentation( void );
  
  UltrasoundProbeObjectRepresentation(const Self&); //purposely not implemented
  void operator=(const Self&);                      //purposely not implemented


  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Create the VTK actors */
  void CreateActors();

private:

  /** Internal itkSpatialObject */
  UltrasoundProbeSpatialObjectType::ConstPointer  
                                               m_UltrasoundProbeSpatialObject;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetUltrasoundProbeObjectProcessing(); 

  /** Method for performing a null operation during a 
   *  State Machine transition */
  void NoProcessing();

private:

 
  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidUltrasoundProbeObject );
  igstkDeclareInputMacro( NullUltrasoundProbeObject );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullUltrasoundProbeObject );
  igstkDeclareStateMacro( ValidUltrasoundProbeObject );

  UltrasoundProbeSpatialObjectType::ConstPointer m_UltrasoundProbeObjectToAdd;

};


} // end namespace igstk

#endif // __igstkUltrasoundProbeObjectRepresentation_h
