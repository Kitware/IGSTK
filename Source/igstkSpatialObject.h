/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSpatialObject_h
#define __igstkSpatialObject_h

#include "igstkMacros.h"
#include "itkObject.h"
#include "itkSpatialObject.h"
#include "igstkTransform.h"

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
  igstkTypeMacro( Self, Object );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( SpatialObject );

  /** Set the Transform corresponding to the ObjectToWorld transformation of
 * the SpatialObject. */
  void SetTransform( const Transform & transform );

  /** Return the Transform associated to the ObjectToWorld transformation
   * of the SpatialObject */
  const Transform & GetTransform() const;


protected:

  SpatialObject( void );
  ~SpatialObject( void );

  /** Connect this representation class to the spatial object */
  void SetSpatialObject( SpatialObjectType * );

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Internal itkSpatialObject */
  SpatialObjectType::Pointer   m_SpatialObject;

  /** Internal fake vector and matrix */
  Transform m_Transform;
};

} // end namespace igstk

#endif // __igstkSpatialObject_h

