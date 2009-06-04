/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkRigidTransformXMLFileWriter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkRigidTransformXMLFileWriter_h
#define __igstkRigidTransformXMLFileWriter_h

#include "igstkTransformXMLFileWriterBase.h"


namespace igstk
{
/** \class RigidTransformXMLFileWriter
 * 
 *  \brief This class writes a 3D rigid transformation to an xml file.
 *
 *  This class is used for writing a precomputed 3D rigid transformation to an 
 *  xml file. The end result is a file with the following format:
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

class RigidTransformXMLFileWriter : public TransformXMLFileWriterBase 
{
public:

  //standard typedefs
  typedef RigidTransformXMLFileWriter    Self;
  typedef TransformXMLFileWriterBase     Superclass;
  typedef itk::SmartPointer<Self>        Pointer;

  //run-time type information (and related methods)
  itkTypeMacro( RigidTransformXMLFileWriter, TransformXMLFileWriterBase );

  //method for creation through the object factory
  itkNewMacro( Self );

  virtual bool IsCompatible(
     const PrecomputedTransformData::Pointer transformation );

protected:

  RigidTransformXMLFileWriter() : TransformXMLFileWriterBase() {}
  virtual ~RigidTransformXMLFileWriter() {}
  virtual void WriteTransformation( std::ofstream &out );

private:

  RigidTransformXMLFileWriter( 
    const RigidTransformXMLFileWriter & other );

  const RigidTransformXMLFileWriter & operator=( 
    const RigidTransformXMLFileWriter & right );
};


}
#endif //__igstkRigidTransformXMLFileWriter_h
