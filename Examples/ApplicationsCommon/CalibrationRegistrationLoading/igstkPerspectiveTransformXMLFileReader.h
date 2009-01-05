/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPerspectiveTransformXMLFileReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkPerspectiveTransformXMLFileReader_h
#define __igstkPerspectiveTransformXMLFileReader_h

#include "igstkTransformXMLFileReaderBase.h"


namespace igstk
{

/** \class PerspectiveTransformXMLFileReader
 * 
 *  \brief This class reads an xml file containing a perspective transformation.
 *
 *  This class is used for reading an xml file containing a precomputed  
 *  perspective transformation. The transformation is described by two matrices
 *  a 3x4 rigid transformation (camera's extrinsic parameters) and a 3x3 
 *  perspective projection matrix (camera's intrinsic parameters), given up to
 *  a scale factor.
 *  The expected file format is:
     @verbatim
    <precomputed_transform>
   
    <description>
       C-arm calibration
    </description>
   
    <computation_date>
       Thursday July 4 12:00:00 1776
    </computation_date>
   
    <transformation estimation_error="0.5">
       alphaX     s    u0
          0    alphaY  v0
          0       0     1
        r11  r12  r13 t1
        r21  r22  r23 t2
        r31  r32  r33 t3
    </transformation>
     
   </precomputed_transform>
   @endverbatim
*/

class PerspectiveTransformXMLFileReader : public TransformXMLFileReaderBase 
{
public:

    //standard typedefs
  typedef PerspectiveTransformXMLFileReader   Self;
  typedef TransformXMLFileReaderBase          Superclass;
  typedef itk::SmartPointer<Self>             Pointer;

           //run-time type information (and related methods)
  itkTypeMacro( PerspectiveTransformXMLFileReader, TransformXMLFileReaderBase );

         //method for creation through the object factory
  itkNewMacro( Self );

protected:
          //this is the constructor that is called by the factory to 
         //create a new object
  PerspectiveTransformXMLFileReader() : TransformXMLFileReaderBase() {}
  
  /**
   * This method extracts the transformation parameters from the string data
   * which is maintained internally (m_CurrentTagData).
   */
  virtual void ProcessTransformation() throw (FileFormatException );
  virtual ~PerspectiveTransformXMLFileReader() {}

private:
  PerspectiveTransformXMLFileReader( 
    const PerspectiveTransformXMLFileReader & other );


  const PerspectiveTransformXMLFileReader & operator=( 
    const PerspectiveTransformXMLFileReader & right );
};


}
#endif //__igstkPerspectiveTransformXMLFileReader_h
