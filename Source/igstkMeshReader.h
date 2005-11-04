/*=========================================================================

  Program:   SpatialObject Guided Surgery Software Toolkit
  Module:    igstkMeshReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkMeshReader_h
#define __igstkMeshReader_h

#include "igstkMacros.h"
#include "igstkSpatialObjectReader.h"

#include "itkSpatialObjectReader.h"
#include "itkObject.h"
#include "itkEventObject.h"

#include "igstkEvents.h"
#include "igstkMeshObject.h"

namespace igstk
{

/** \class MeshReader
 * 
 * \brief This class reads 3D Mesh in the metaIO format.
 * 
 * The file describes a list of points corresponding to the nodes in the Mesh,
 * and a list of links between the nodes. The output of this reader is of type
 * MeshSpatialObject.
 *
 * \sa MeshObject
 *
 * \ingroup Readers
 */
class MeshReader : public SpatialObjectReader<3,float>
{

public:

  /** Typedefs */
  typedef MeshReader                         Self;
  typedef SpatialObjectReader<3,float>       Superclass;
  typedef itk::SmartPointer< Self >          Pointer;
  typedef itk::SmartPointer< const Self >    ConstPointer;
  typedef Superclass::SpatialObjectType      SpatialObjectType;
  typedef Superclass::GroupSpatialObjectType GroupSpatialObjectType;
  typedef SpatialObjectType::ConstPointer    SpatialObjectTypeConstPointer;
  typedef igstk::MeshObject                  MeshObjectType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( MeshReader, SpatialObjectReader );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );
  
  /** Return the output as a group */
  const MeshObjectType * GetOutput() const;

protected:

  MeshReader( void );
  ~MeshReader( void );

  itkEventMacro( MeshReaderEvent,IGSTKEvent );
  
  //SpatialObject reading error
  itkEventMacro( MeshReadingErrorEvent, MeshReaderEvent );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** This method tries to read the Object. This method is invoked by the State
   * Machine of the superclass. */
  void AttemptReadObject();

private:

  MeshReader(const Self&);         //purposely not implemented
  void operator=(const Self&);     //purposely not implemented

  MeshObjectType::Pointer   m_Mesh;

};

} // end namespace igstk


#endif // __igstkMeshReader_h
