/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkCylinderObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkCylinderObject_h
#define __igstkCylinderObject_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include <itkTubeSpatialObject.h>

namespace igstk
{

/** \class CylinderObject
 * 
 * \brief This class represents a cylinder object. The parameters of the object
 * are the height of the object, and the radius. Default representation axis is X.
 * 
 * \ingroup Object
 */

class CylinderObject 
: public ObjectRepresentation
{

public:

  /** Typedefs */
  typedef CylinderObject                 Self;
  typedef ObjectRepresentation           Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer; 
  typedef itk::TubeSpatialObject<3>      TubeSOType;

  /**  Run-time type information (and related methods). */
  itkTypeMacro( Self, Superclass );

  /** Method for creation of a reference counted object. */
  NewMacro( CylinderObject );

  /** Set all radii to the same radius value.  Each radius is
   *  half the length of one axis of the ellipse.  */
  GetMacro( Radius, double );
  SetMacro( Radius, double );

  /** Set the Height of the cylinder 
   */
  GetMacro(Height, double);
  SetMacro(Height, double);

  /** Create the VTK actors */
  void CreateActors();

protected:

  CylinderObject( void );
  ~CylinderObject( void );

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  double             m_Radius;
  double             m_Height;

  /** Internal itkSpatialObject. A cylinder is represented as a tube spatial object with no point
   *  This is just for the hierarchy.
   */
  TubeSOType::Pointer   m_TubeSpatialObject;

};

} // end namespace igstk

#endif // __igstkCylinderObject_h

