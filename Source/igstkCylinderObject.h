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
#include <itkCylinderSpatialObject.h>

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
  typedef itk::CylinderSpatialObject     CylinderSOType;

  /**  Run-time type information (and related methods). */
  itkTypeMacro( Self, Superclass );

  /** Method for creation of a reference counted object. */
  NewMacro( CylinderObject );

  /** Set/Get the radius of the cylinder */
  void SetRadius(double radius);
  double GetRadius();

  /** Set/Get the Height of the cylinder */
  void SetHeight(double height);
  double GetHeight();

  /** Create the VTK actors */
  void CreateActors();

protected:

  CylinderObject( void );
  ~CylinderObject( void );

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Internal itkSpatialObject */
  CylinderSOType::Pointer   m_CylinderSpatialObject;

};

} // end namespace igstk

#endif // __igstkCylinderObject_h

