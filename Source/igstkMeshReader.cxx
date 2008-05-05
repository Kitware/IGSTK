/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMeshReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkMeshReader.h"

namespace igstk
{ 

/** Constructor */
MeshReader::MeshReader():m_StateMachine(this)
{ 
  m_MeshObject = MeshObjectType::New();
  m_Mesh = MeshType::New();
} 

/** Destructor */
MeshReader::~MeshReader()  
{
}

/** Read the spatialobject file */
void MeshReader::AttemptReadObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::MeshReader::AttemptReadObject called...\n");
  Superclass::AttemptReadObjectProcessing();

  // Do the conversion
  GroupSpatialObjectType::Pointer m_GroupSpatialObject 
                                          = m_SpatialObjectReader->GetGroup();
  GroupSpatialObjectType::ChildrenListType * children 
                                   = m_GroupSpatialObject->GetChildren(99999);
  GroupSpatialObjectType::ChildrenListType::const_iterator it 
                                                          = children->begin();

  while(it != children->end())
    {
    if(!strcmp((*it)->GetTypeName(),"MeshSpatialObject"))
      {
      MeshObjectType::MeshSpatialObjectType* meshSO  = dynamic_cast<
                  MeshObjectType::MeshSpatialObjectType*>((*it).GetPointer());
      if( meshSO )
        {
        m_Mesh = meshSO->GetMesh();
        break;
        }
      }
    it++;
    }
  delete children;

  this->ConnectMesh();

}

void MeshReader::ReportObjectProcessing()
{
  MeshModifiedEvent  event;
  event.Set( this->m_MeshObject );
  this->InvokeEvent( event );
}

 
// FIXME : This must be replaced with StateMachine logic
MeshReader::MeshType * 
MeshReader::GetITKMesh() const
{
  return m_Mesh;
}

void MeshReader::ConnectMesh() 
{
  typedef Friends::MeshReaderToMeshSpatialObject  HelperType;
  HelperType::ConnectMesh( this, m_MeshObject.GetPointer() );
}

/** Print Self function */
void MeshReader::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << "Mesh = " << m_MeshObject.GetPointer() << std::endl;
  os << "Mesh = " << m_Mesh.GetPointer() << std::endl;
}

} // end namespace igstk
