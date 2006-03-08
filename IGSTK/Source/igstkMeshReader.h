/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
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

namespace Friends 
{

/** \class MeshReaderToMeshSpatialObject
 * \brief This class is intended to make the connection between the MeshReader
 * and its output, the MeshSpatialObject. 
 *
 * With this class it is possible to enforce encapsulation of the Reader and
 * the MeshSpatialObject, and make their GetMesh() and SetMesh() methods
 * private, so that developers cannot gain access to the ITK or VTK layers of
 * these two classes.
 */
class MeshReaderToMeshSpatialObject
{
public:

  template < class TReader, class TMeshSpatialObject >
  static void 
  ConnectMesh(  TReader * reader, 
                TMeshSpatialObject * imageSpatialObject )
    {
    imageSpatialObject->SetMesh( reader->GetITKMesh() );  
    }

}; // end of MeshReaderToMeshSpatialObject class

} // end of Friend namespace


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

  /** Typedef for the superclass is needed because the StandardClassTraitsMacro
   * would get confused with the commas in the template. */
  typedef SpatialObjectReader<3,float>       SuperclassType;

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( MeshReader, SuperclassType )

public:

  /** Typedefs */
  typedef Superclass::SpatialObjectType      SpatialObjectType;
  typedef Superclass::GroupSpatialObjectType GroupSpatialObjectType;
  typedef SpatialObjectType::ConstPointer    SpatialObjectTypeConstPointer;
  typedef igstk::MeshObject                  MeshObjectType;
  typedef MeshObjectType::MeshType           MeshType;

  /** Return the output as a group */
  const MeshObjectType * GetOutput() const;

  /** Declare the MeshReaderToMeshSpatialObject class to be a friend 
   *  in order to give it access to the private method GetITKMesh(). */
  igstkFriendClassMacro( igstk::Friends::MeshReaderToMeshSpatialObject );

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
  void AttemptReadObjectProcessing();

  /** Connect the ITK mesh to the output MeshSpatialObject */
  void ConnectMesh();

private:

  MeshReader(const Self&);         //purposely not implemented
  void operator=(const Self&);     //purposely not implemented

  // FIXME : This must be replaced with StateMachine logic
  virtual MeshType * GetITKMesh() const;

  MeshObjectType::Pointer   m_MeshObject;
  MeshType::Pointer         m_Mesh;

};

} // end namespace igstk


#endif // __igstkMeshReader_h
