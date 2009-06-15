/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPerspectiveTransform.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkPerspectiveTransform_h
#define __igstkPerspectiveTransform_h

#include "igstkTransformBase.h"

#include "vtkMatrix4x4.h"
#include "vtkPerspectiveTransform.h"
#include "itkMatrix.h"

#include "igstkMacros.h"

namespace igstk 
{
 
/** \class PerspectiveTransform 
 *  \brief A class representing a 3D rigid transformation followed by a 
 *         perspective projection.
 * 
 * This class represents a perspective transform of points from 3D to 2D. 
 * 
 * All the set methods require an argument that defines the number of
 * milliseconds for which the stored information is considered to be valid.
 * The validity period will be counted from the moment the Set method was
 * invoked.
 *
 * \sa TransformBase
 *
 * */
  class PerspectiveTransform : public TransformBase
{
public:
  /** Extrinsic perspective camera parameters [R,t]*/ 
  typedef ::itk::Matrix<double, 3, 4> ExtrinsicMatrixType;
  /** Intrinsic perspective camera parameters, upper triangular matrix*/
  typedef ::itk::Matrix<double, 3, 3> IntrinsicMatrixType;

public:

  /** Constructor and destructor */
  PerspectiveTransform();
  PerspectiveTransform( const PerspectiveTransform & t );
  virtual ~PerspectiveTransform();

  /** Assign the values of one transform to another */
  const PerspectiveTransform & operator=( 
    const PerspectiveTransform & inputTransform );


  /** Set perspective transformation. This transformation will override
   * any previously set values. The information will be
   * considered valid from the time of invocation of the method until that time
   * plus the millisecondsToExpiration value. */
  void SetTransform( const ExtrinsicMatrixType &extrinsic,
                     const IntrinsicMatrixType &intrinsic,
                     ErrorType errorValue,
                     TimePeriodType millisecondsToExpiration );

 
  /** Export the content of the transformation into a vtkPerspective transform
   * which represents the rigid transform followed by the perspective 
   * projection.
   * Users must allocate the matrix first and then pass it by reference to this
   * method.  The current method will simply fill in the transform.
   */
  void ExportTransform( vtkPerspectiveTransform &outMatrix ) const;


  /** Export the extrinsic camera parameters. This is the rigid transformation
   *  between the camera and the reference frame relative to which it was 
   *  calibrated.
   */
  void ExportExtrinsicParameters( vtkMatrix4x4 & matrix ) const;


  /** Export the intrinsic camera parameters. This is the perspective 
   *  transformation associated with the internal camera parameters.
   */
  void ExportIntrinsicParameters( vtkPerspectiveTransform &outMatrix ) const;


  /** Method for printing the member variables of this class to an ostream */
  void Print( std::ostream& os, itk::Indent indent ) const;


protected:

  void PrintHeader( std::ostream& os, itk::Indent indent ) const;

  void PrintTrailer( std::ostream& itkNotUsed(os), 
                     itk::Indent itkNotUsed(indent) ) const;

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:
  ExtrinsicMatrixType m_ExtrinsicTransform;
  IntrinsicMatrixType m_IntrinsicTransform;
};

std::ostream& operator<<( std::ostream& os, 
                          const igstk::PerspectiveTransform& o );
}

#endif
