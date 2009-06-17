/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAffineTransform.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAffineTransform_h
#define __igstkAffineTransform_h

#include "igstkTransformBase.h"
#include <itkAffineTransform.h>
#include "vtkMatrix4x4.h"

#include "igstkMacros.h"

namespace igstk 
{
 
/** \class AffineTransform 
 *  \brief A class representing a 3D affine transform.
 * 
 * This class represents an affine transform of points in 3D space. 
 * 
 * All the set methods require an argument that defines the number of
 * milliseconds for which the stored information is considered to be valid.
 * The validity period will be counted from the moment the Set method was
 * invoked.
 *
 * \sa TransformBase
 *
 * */
class AffineTransform : public TransformBase
{

public:

  typedef ::itk::AffineTransform<double, 3>::MatrixType          MatrixType;
  typedef ::itk::AffineTransform<double, 3>::OutputVectorType    OffsetType;
  
public:

  /** Default constructor, results in identity transform with a time stamp that
   * is never valid (start==end==0). */
  AffineTransform();
  AffineTransform( const AffineTransform & t );
  virtual ~AffineTransform();


  /** Transform composition method */
  static AffineTransform TransformCompose( AffineTransform &leftTransform, 
                                           AffineTransform &rightTransform );


  /** Assign the values of one transform to another */
  const AffineTransform & operator=( const AffineTransform & inputTransform );


  /** Set affine transformation. This transformation will override
   * any previously set values. The information will be
   * considered valid from the time of invocation of the method until that time
   * plus the millisecondsToExpiration value. */
  void SetMatrixAndOffset( const  MatrixType &matrix,
                           const OffsetType &offset, 
                           ErrorType errorValue,
                           TimeStamp::TimePeriodType millisecondsToExpiration );

 
  /** Export the content of the transformation in the format of a vtkMatrix4x4.
   * Users must allocate the matrix first and then pass it by reference to this
   * method.  The current method will simply fill in the transform using the
   * current information of translation and rotation.
   */
  void ExportTransform( vtkMatrix4x4 & matrix ) const;


  /** Method for printing the member variables of this class to an ostream */
  void Print( std::ostream& os, itk::Indent indent ) const;


protected:

  void PrintHeader( std::ostream& os, itk::Indent indent ) const;

  void PrintTrailer( std::ostream& itkNotUsed(os), 
                     itk::Indent itkNotUsed(indent) ) const;

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  ::itk::AffineTransform<double, 3>::Pointer m_Transform;

};

std::ostream& operator<<( std::ostream& os, const igstk::AffineTransform& o );
}

#endif
