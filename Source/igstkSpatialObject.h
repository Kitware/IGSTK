/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkSpatialObject_h
#define __igstkSpatialObject_h

#include "igstkMacros.h"
#include "itkObject.h"
#include "itkSpatialObject.h"

namespace igstk
{

/** \class SpatialObject
 * 
 * \brief This class encapsulates an ITK spatial object with the goal of
 * restricting access to functionalities that are not essential for IGS
 * applications, or to functionalities thay may present risks and unnecessary
 * flexibility. 
 *
 * \ingroup Object
 */

class SpatialObject : public itk::Object
{

public:

  /** Typedefs */
  typedef SpatialObject                  Self;
  typedef itk::Object                    Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer; 
  typedef itk::SpatialObject<3>          SpatialObjectType;

  /**  Run-time type information (and related methods). */
  itkTypeMacro( Self, Object );

  /** Method for creation of a reference counted object. */
  NewMacro( SpatialObject );

  /** Set the offset in its Transformation */
  void SetOffset( double ox, double oy, double oz );

protected:

  SpatialObject( void );
  ~SpatialObject( void );

  /** Connect this representation class to the spatial object */
  SetMacro( SpatialObject, SpatialObjectType * );

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Internal itkSpatialObject */
  SpatialObjectType::ConstPointer   m_SpatialObject;

};

} // end namespace igstk

#endif // __igstkSpatialObject_h

