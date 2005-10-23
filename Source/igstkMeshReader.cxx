/*=========================================================================

  Program:   SpatialObject Guided Surgery Software Toolkit
  Module:    igstkMeshReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkMeshReader.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
MeshReader::MeshReader()
{ 
  m_Mesh = MeshObjectType::New();
} 

/** Destructor */
MeshReader::~MeshReader()  
{
}

/** Read the spatialobject file */
void MeshReader::AttemptReadObject()
{
  igstkLogMacro( DEBUG, "igstk::MeshReader::AttemptReadObject called...\n");
  Superclass::AttemptReadObject();

  // Do the conversion
  GroupSpatialObjectType::Pointer m_GroupSpatialObject = m_SpatialObjectReader->GetGroup();
  GroupSpatialObjectType::ChildrenListType * children = m_GroupSpatialObject->GetChildren(99999);
  GroupSpatialObjectType::ChildrenListType::const_iterator it = children->begin();

  while(it != children->end())
    {
    if(!strcmp((*it)->GetTypeName(),"MeshSpatialObject"))
      {
      MeshObjectType::MeshSpatialObjectType* meshSO = dynamic_cast<MeshObjectType::MeshSpatialObjectType*>((*it).GetPointer());
      if( meshSO )
        {
        typedef MeshObjectType::MeshType MeshType;
        MeshObjectType::MeshType::Pointer mesh = meshSO->GetMesh();
  
        MeshType::PointsContainerPointer points = mesh->GetPoints();
        MeshType::PointsContainer::const_iterator it = points->begin();

        for(;it != points->end();it++)
          {
          m_Mesh->AddPoint((*it).first,
                           (*it).second[0],
                           (*it).second[1],
                           (*it).second[2]);
          }

        typedef MeshType::CellsContainer::ConstIterator  CellIterator;
        CellIterator cellItr = mesh->GetCells()->Begin();
        CellIterator cellEnd = mesh->GetCells()->End();

        while( cellItr != cellEnd )
          {
          if((*cellItr).Value()->GetNumberOfPoints() == 3)
            {
            typedef MeshType::CellTraits::PointIdConstIterator PointIterator;
            PointIterator  itptids = (*cellItr)->Value()->GetPointIds();
            unsigned int i =0;
            MeshType::CellTraits::PointIdentifier ids[3];
            while(itptids != (*cellItr)->Value()->PointIdsEnd())
              {
              ids[i] = *itptids;
              itptids++;
              i++;
              }
            m_Mesh->AddTriangleCell((*cellItr).Index(),ids[0],ids[1],ids[2]);
            }
          cellItr++;
          }

        delete children;
        return;
        }
      }
    it++;
    }
  delete children;
}

/** Return the output as a group */
const MeshReader::MeshObjectType *
MeshReader::GetOutput() const
{
  return m_Mesh;
}

/** Print Self function */
void MeshReader::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << "Mesh = " << m_Mesh.GetPointer() << std::endl;
}

} // end namespace igstk


