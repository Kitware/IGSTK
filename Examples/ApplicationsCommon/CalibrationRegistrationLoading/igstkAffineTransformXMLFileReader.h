/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAffineTransformXMLFileReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkAffineTransformXMLFileReader_h
#define __igstkAffineTransformXMLFileReader_h

#include "igstkTransformXMLFileReaderBase.h"


namespace igstk
{

/** \class AffineTransformXMLFileReader
 * 
 *  \brief This class reads an xml file containing a 3D affine transformation.
 *
 *  This class is used for reading an xml file containing a precomputed 3D 
 *  affine transformation. The transformation is described by a 4x4 homogenous
 *  transformation matrix. The expected file format is:
     @verbatim
    <precomputed_transform>
   
    <description>
       US calibration
    </description>
   
    <computation_date>
       Thursday July 4 12:00:00 1776
    </computation_date>
   
    <transformation estimation_error="0.5">
       a b c d
       e f g h
       i j k l
    </transformation>
  
   </precomputed_transform>
   @endverbatim
*/

class AffineTransformXMLFileReader : public TransformXMLFileReaderBase 
{
public:

    //standard typedefs
  typedef AffineTransformXMLFileReader   Self;
  typedef TransformXMLFileReaderBase     Superclass;
  typedef itk::SmartPointer<Self>        Pointer;

           //run-time type information (and related methods)
  itkTypeMacro( AffineTransformXMLFileReader, TransformXMLFileReaderBase );

         //method for creation through the object factory
  itkNewMacro( Self );

protected:
          //this is the constructor that is called by the factory to 
         //create a new object
  AffineTransformXMLFileReader() : TransformXMLFileReaderBase() {}
  
  /**
   * This method extracts the transformation parameters from the string data
   * which is maintained internally (m_CurrentTagData).
   */
  virtual void ProcessTransformation() throw ( FileFormatException );

  virtual ~AffineTransformXMLFileReader() {}
private:
  AffineTransformXMLFileReader( 
    const AffineTransformXMLFileReader & other );


  const AffineTransformXMLFileReader & operator=( 
    const AffineTransformXMLFileReader & right );
};


}
#endif //__igstkAffineTransformXMLFileReader_h
