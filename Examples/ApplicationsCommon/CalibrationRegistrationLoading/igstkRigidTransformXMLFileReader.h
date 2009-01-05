/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkRigidTransformXMLFileReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkRigidTransformXMLFileReader_h
#define __igstkRigidTransformXMLFileReader_h

#include "igstkTransformXMLFileReaderBase.h"


namespace igstk
{
/** \class RigidTransformXMLFileReader
 * 
 *  \brief This class reads an xml file containing a 3D rigid transformation.
 *
 *  This class is used for reading an xml file containing a precomputed 3D 
 *  rigid transformation. The transformation is described by a unit quaternion
 *  and a translation vector. The expected file format is:
     @verbatim
    <precomputed_transform>
   
    <description>
      Bayonet probe calibration
    </description>
   
    <computation_date>
       Thursday July 4 12:00:00 1776
    </computation_date>
   
    <transformation estimation_error="0.5">
       qx qy qz qw tx ty tz 
    </transformation>
   
   </precomputed_transform>
   @endverbatim
*/

class RigidTransformXMLFileReader : public TransformXMLFileReaderBase 
{
public:

    //standard typedefs
  typedef RigidTransformXMLFileReader    Self;
  typedef TransformXMLFileReaderBase     Superclass;
  typedef itk::SmartPointer<Self>        Pointer;

           //run-time type information (and related methods)
  itkTypeMacro( RigidTransformXMLFileReader, TransformXMLFileReaderBase );

         //method for creation through the object factory
  itkNewMacro( Self );

protected:
          //this is the constructor that is called by the factory to 
         //create a new object
  RigidTransformXMLFileReader() : TransformXMLFileReaderBase() {}

  /**
   * This method extracts the transformation parameters from the string data
   * which is maintained internally (m_CurrentTagData).
   */   
  virtual void ProcessTransformation() throw (FileFormatException );

  virtual ~RigidTransformXMLFileReader() {}

private:
  RigidTransformXMLFileReader( 
    const RigidTransformXMLFileReader & other );


  const RigidTransformXMLFileReader & operator=( 
    const RigidTransformXMLFileReader & right );
};


}
#endif //__igstkRigidTransformXMLFileReader_h
