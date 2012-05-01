/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSceneGraphNode.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkSceneGraphNode.h"
namespace igstk
{

SceneGraphNode::SceneGraphNode(void)
{  
  m_XC1 = 0;
  m_YC1 = 0;
  m_IsCurrentTransform = false;
  m_IsSelected = false;
  m_IsCurrentInverseTransform = false;
  m_Children = new std::list<SceneGraphNode*>;
  m_Name = NULL; 
  m_Type = NULL;
  m_Parent = NULL;
  m_CoordinateSystem = NULL;
}

SceneGraphNode::~SceneGraphNode(void)
{
  delete m_Children;
}
   
}
