/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSceneGraph.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkSceneGraph.h"

namespace igstk
{

SceneGraph::SceneGraph()
{
  //m_SceneGraphUI = new igstk::SceneGraphUI();
  m_IsUIBeingShown = false;
  m_IsChanged = false;
}

SceneGraph::~SceneGraph()
{
  m_InstanceFlag = false;
}

bool SceneGraph::m_InstanceFlag = false;
SceneGraph* SceneGraph::m_Single = NULL;
SceneGraph* SceneGraph::getInstance()
{
    if(! m_InstanceFlag)
    {
        m_Single = new SceneGraph();
        m_InstanceFlag = true;
        return m_Single;
    }
    else
    {
        return m_Single;
    }
}


void SceneGraph
::AddCoordinateSystem(const 
            CoordinateSystemSetTransformEvent *setTransformEvent)
{
  bool parentFound = false;
  for(std::list<SceneGraphNode*>::iterator i = m_RootNodes.begin(); 
    i != m_RootNodes.end(); ++i)
  {
    if(IsParentExisting((*i), setTransformEvent->Get().
      GetSource()->GetName()))
    {
      AddCoordinateSystemToParent( (*i) ,setTransformEvent);
      parentFound = true;
    }
  }
  if(!parentFound) 
  {
    CreateNewParentAndAddCoordinateSystem(setTransformEvent);
  }
  OptimizeTree();
  //printNodeDetails();
}

void SceneGraph::OptimizeTree()
{
  bool parentFound = false;
  for(std::list<SceneGraphNode*>::iterator rootNode1 = m_RootNodes.begin(); 
    rootNode1 != m_RootNodes.end(); ++rootNode1)
  {
    for(std::list<SceneGraphNode*>::iterator rootNode2 = m_RootNodes.begin();
    rootNode2 != m_RootNodes.end(); ++rootNode2)
    {
      if(rootNode1 == rootNode2)
        continue;
      if(IsParentExisting(*rootNode1, (*rootNode2)->GetName()))
      {
        ClubBothRootNodes(*rootNode1, *rootNode2);
        parentFound = true;
        break;
      }
    }
    if(parentFound)
      break;
  }
}

bool SceneGraph
::ClubBothRootNodes(SceneGraphNode* rootNode, SceneGraphNode* childNode)
{ 
  if(strcmp(rootNode->GetName(),childNode->GetName()) == 0)
  {
    rootNode->GetParent()->GetChildren()->push_back(childNode);
    childNode->SetParent(rootNode->GetParent());
    m_RootNodes.remove(childNode);
    return true;
  }
  for(std::list<SceneGraphNode*>::iterator j = rootNode->GetChildren()->begin();
    j != rootNode->GetChildren()->end(); ++j)
  {
    if(ClubBothRootNodes((*j),childNode))
    {
      rootNode->GetChildren()->remove(*j);
      break;
    }
  }
  return false;
}

bool SceneGraph
::IsParentExisting(SceneGraphNode *rootNode, const char *parentName)
{
  bool parentFound = false;
  if(strcmp(rootNode->GetName(),parentName) == 0)
  {
    return true;
  }
  for(std::list<SceneGraphNode*>::iterator j = rootNode->GetChildren()->begin();
    j != rootNode->GetChildren()->end(); ++j)
  {
    if(strcmp((*j)->GetName(),parentName) == 0)
    {
      parentFound = true;
      return true;
    }
    else if(IsParentExisting((*j), parentName))
    {
      parentFound = true;
      return true;
    }
  }

  return parentFound;
}

void SceneGraph
::AddCoordinateSystemToParent(SceneGraphNode *rootNode, 
                const CoordinateSystemSetTransformEvent 
                *setTransformEvent)
{
  
  if(strcmp(rootNode->GetName(),setTransformEvent->Get().GetSource()->GetName())
        == 0)
  {
    for(std::list<SceneGraphNode*>::iterator i = rootNode->GetChildren()->
      begin(); i != rootNode->GetChildren()->end(); ++i)
    {
      if(strcmp((*i)->GetName(),setTransformEvent->Get().GetDestination()->
        GetName()) == 0)
      {
        return;
      }
    }
    SceneGraphNode* child = new SceneGraphNode();
    child->SetCoordinateSystem(const_cast<CoordinateSystem *> 
                               (setTransformEvent->Get().GetDestination()));

    std::string str_name = setTransformEvent->Get().GetDestination()->GetName();
    char* c_name = new char[str_name.length() + 1];
    strcpy(c_name, str_name.c_str());
    child->SetName( c_name );
    std::string str_type = setTransformEvent->Get().GetDestination()->GetType();
    char* c_type = new char[str_type.length() + 1];
    strcpy(c_type, str_type.c_str());
    child->SetType( c_type );

    child->SetParent(rootNode);
    rootNode->GetChildren()->push_back(child);
    return;
  }
  for(std::list<SceneGraphNode*>::iterator j = rootNode->GetChildren()->begin();
    j != rootNode->GetChildren()->end(); ++j)
  {
    if(strcmp((*j)->GetName(), setTransformEvent->Get().GetSource()->
      GetName()) == 0)
    {
      for(std::list<SceneGraphNode*>::iterator k = (*j)->GetChildren()->
        begin(); k != (*j)->GetChildren()->end(); ++k)
      {
        if(strcmp((*k)->GetName(),setTransformEvent->Get().
          GetDestination()->GetName()) == 0)
        {
          return;
        }
      }
      SceneGraphNode* child = new  SceneGraphNode();
      child->SetCoordinateSystem(const_cast<CoordinateSystem *> 
                                 (setTransformEvent->Get().GetDestination()));
      
      std::string str_name = setTransformEvent->Get().GetDestination()->GetName();
      char* c_name = new char[str_name.length() + 1];
      strcpy(c_name, str_name.c_str());
      child->SetName( c_name );
      std::string str_type = setTransformEvent->Get().GetDestination()->GetType();
      char* c_type = new char[str_type.length() + 1];
      strcpy(c_type, str_type.c_str());
      child->SetType( c_type );

      child->SetParent((*j));
      
      (*j)->GetChildren()->push_back(child);
    }  
    else if(IsParentExisting((*j), setTransformEvent->Get().
      GetSource()->GetName()))
    {
      AddCoordinateSystemToParent((*j), setTransformEvent);
    }
  }
}

void SceneGraph
::CreateNewParentAndAddCoordinateSystem(const 
                    CoordinateSystemSetTransformEvent 
                    *setTransformEvent)
{
  SceneGraphNode* parent = new SceneGraphNode();
  {
  std::string str_name = setTransformEvent->Get().GetSource()->GetName();
  char* c_name = new char[str_name.length() + 1];
  strcpy(c_name, str_name.c_str());
  parent->SetName( c_name );
  std::string str_type = setTransformEvent->Get().GetSource()->GetType();
  char* c_type = new char[str_type.length() + 1];
  strcpy(c_type, str_type.c_str());
  parent->SetType( c_type );
  }
  parent->SetCoordinateSystem( const_cast<CoordinateSystem *> 
                               ( setTransformEvent->Get().GetSource()) );
  parent->SetParent(NULL);

  SceneGraphNode* child = new SceneGraphNode();
  child->SetCoordinateSystem( const_cast<CoordinateSystem *> 
                              (setTransformEvent->Get().GetDestination()) );
  {
  std::string str_name = setTransformEvent->Get().GetDestination()->GetName();
  char* c_name = new char[str_name.length() + 1];
  strcpy(c_name, str_name.c_str());
  child->SetName( c_name );
  std::string str_type = setTransformEvent->Get().GetDestination()->GetType();
  char* c_type = new char[str_type.length() + 1];
  strcpy(c_type, str_type.c_str());
  child->SetType( c_type );
  }

  child->SetParent(parent);
  child->SetParentTransform(setTransformEvent->Get().GetTransform());
  parent->GetChildren()->push_back(child);
  m_RootNodes.push_back(parent);
}

void SceneGraph::printNodeDetails()
{
  std::cout << "START SceneGraph Details." << std::endl;
  std::cout << "--------------------------------" << std::endl;

  for(std::list<SceneGraphNode*>::iterator k = m_RootNodes.begin();
    k != m_RootNodes.end(); ++k)
  {
    printParentNodeDetails(*k);
  }

  std::cout << "End SceneGraph Details." << std::endl;
  std::cout << "----------------------------------------" << std::endl;
}

void SceneGraph::printParentNodeDetails(SceneGraphNode* parentNode)
{
  print(parentNode);
  for(std::list<SceneGraphNode*>::const_iterator m = parentNode->GetChildren()->begin();
    m != parentNode->GetChildren()->end(); ++m)
  {
    printParentNodeDetails(*m);
  }
}

void SceneGraph::print(igstk::SceneGraphNode *node)
{
  std::cout << "********Details********" << std::endl;
  std::cout << "Name  : " << node->GetName() << std::endl;
  if(node->GetParent() != NULL)
  {
    std::cout << "Parent: " << node->GetParent()->GetName() << std::endl;
  }
  else 
  {
    std::cout << "Parent: No parent" << std::endl;
  }
  std::cout << "Coordinate System: " << node->GetCoordinateSystem()->
      GetName() << std::endl;
  std::cout << "Trasform to parent: " << std::endl;
  std::cout << "*******End Details*******" << std::endl;
  
}

void SceneGraph
::DetachCoordinateSystem(const 
             CoordinateSystemSetTransformEvent *setTransformEvent)
{
  SceneGraphNode* parent = LocateSceneGraphNodeWithName(
    setTransformEvent->Get().GetSource()->GetName());
  SceneGraphNode* child = LocateSceneGraphNodeWithName(
    setTransformEvent->Get().GetDestination()->GetName());

  child->SetParent(NULL);
  m_RootNodes.push_back(child);
  parent->GetChildren()->remove(child);
}

void SceneGraph::ExportSceneGraphToDot(std::string filename)
{
  std::ofstream dotOutputFile;
  dotOutputFile.open(filename.c_str());
  if( dotOutputFile.fail() )
    {
    std::cerr << "Problem opening the file " << filename << std::endl;
    itk::ExceptionObject excp;
    excp.SetDescription("Problem opening file");
    throw excp;
    }
  ExportDescriptionToDot(dotOutputFile);
  dotOutputFile.close();
}

void SceneGraph::ExportDescriptionToDot(std::ostream & ostr)
{
  ostr << "digraph G {" << std::endl;
  for(std::list<SceneGraphNode *>::const_iterator rootNode = m_RootNodes.begin();
    rootNode != m_RootNodes.end(); ++rootNode)
  {
    DrawParentChildrenRelationship(ostr, *rootNode);
  }
  for(std::list<SceneGraphNode *>::const_iterator rootNode = m_RootNodes.begin();
    rootNode != m_RootNodes.end(); ++rootNode)
  {
    DrawDescription(ostr, *rootNode);
  }
  ostr << "}" << std::endl;
}

void SceneGraph
::DrawParentChildrenRelationship(std::ostream &ostr, 
                 igstk::SceneGraphNode *parentNode)
{
  for(std::list<SceneGraphNode*>::const_iterator child =
         parentNode->GetChildren()->begin();
    child != parentNode->GetChildren()->end(); ++child)
  {
    ostr << "\"" << parentNode->GetName() << "\"" <<" -> " << 
          "\""<<(*child)->GetName()<< "\""<<";" << std::endl;
    DrawParentChildrenRelationship(ostr, *child);
  } 
}

void SceneGraph
::DrawDescription(std::ostream &ostr, 
          igstk::SceneGraphNode *parentNode)
{
  if(strcmp(parentNode->GetType(), "TrackerTool") == 0)
  {
    ostr << "\"" << parentNode->GetName() << "\"" << 
      "[shape=polygon, sides=6, color=red, style=filled];" << std::endl;
  }
  else if(strcmp(parentNode->GetType(), "SpatialObject") == 0)
  {
    ostr << "\"" << parentNode->GetName() << "\"" << 
      "[shape=polygon, sides=4, color=blue, style=filled];" << std::endl;
  }
  else if(strcmp(parentNode->GetType(), "Tracker") == 0)
  {
    ostr << "\"" << parentNode->GetName() << "\"" << 
  "[shape=polygon, sides=4, distortion=-0.7, color=yellow, style=filled];"
      << std::endl;
  }
  else if(strcmp(parentNode->GetType(), "View") == 0)
  {
    ostr << "\"" << parentNode->GetName() << "\"" << 
      "[shape=polygon, sides=8, color=green, style=filled];" << 
      std::endl;
  }
  for(std::list<SceneGraphNode*>::const_iterator child =
         parentNode->GetChildren()->begin();
      child != parentNode->GetChildren()->end(); ++child)
  {
    DrawDescription(ostr, *child);
  } 
}

 void SceneGraph::ShowSceneGraph(bool showFlag)
{
  m_IsUIBeingShown = showFlag;
  //m_SceneGraphUI->DrawSceneGraph(m_RootNodes,showFlag,transitionsStore);
} 

void SceneGraph::ResetCurrentTransformFlag(SceneGraphNode* parentNode)
{
  parentNode->SetIsCurrentTransform(false);
  parentNode->SetIsCurrentInverseTransform(false);
  for(std::list<SceneGraphNode*>::const_iterator it =
         parentNode->GetChildren()->begin();
      it != parentNode->GetChildren()->end(); ++it)
  {
    ResetCurrentTransformFlag(*it);
  }
}

void SceneGraph
::ShowTheTransformPath(const 
             CoordinateSystemTransformToEvent* transformPathEvent)
{
  if(IsRequestedItemThere(transformPathEvent) && 
    IsNewTranformEvent(transformPathEvent))
  {
    std::cout << "Got tranformPathEvent" << std::endl;
    for(std::list<SceneGraphNode*>::const_iterator i = 
      m_RootNodes.begin(); i != m_RootNodes.end(); ++i)
    {
      ResetCurrentTransformFlag((*i));
    }
    if(transformPathEvent->Get().GetCommonAncestor() != NULL)
    {
      SetTransformPath(transformPathEvent->Get().GetCommonAncestor()->
        GetName(), transformPathEvent->
        Get().GetSource()->GetName(), true);
      SetTransformPath(transformPathEvent->Get().GetCommonAncestor()->
        GetName(), transformPathEvent->
        Get().GetDestination()->GetName(), false);
    }
    /* m_SceneGraphUI->DrawSceneGraph(m_RootNodes,
      m_IsUIBeingShown, transitionsStore); */
    m_IsChanged = true;
  }
}

bool SceneGraph
::IsNewTranformEvent(const 
           CoordinateSystemTransformToEvent *setTransformEvent)
{
  bool isNew = true;
  
  for(std::list<Transition *>::const_iterator it = m_TransitionsStore.begin();
      it != m_TransitionsStore.end(); ++it)
  {
    if( (strcmp(setTransformEvent->Get().GetSource()->GetName(), 
          (*it)->source) == 0)
      && (strcmp(setTransformEvent->Get().GetDestination()->
          GetName(), (*it)->destination) == 0) 
      && (strcmp(setTransformEvent->Get().GetCommonAncestor()->
          GetName(), (*it)->cAncestor) == 0))
    {
      isNew = false;
      return false;
    }
  }

  if(isNew)
  {
    Transition* newTransition = new Transition();
    newTransition->source = setTransformEvent->Get().
        GetSource()->GetName();
    newTransition->destination = setTransformEvent->Get().
        GetDestination()->GetName();
    newTransition->cAncestor = setTransformEvent->Get().
        GetCommonAncestor()->GetName();
    m_TransitionsStore.push_back(newTransition);
  }
  return true;
}

void SceneGraph
::SetTransformPath(const char * ancestorName,
           const char * childName, 
           bool isInverse)
{
  SceneGraphNode* commonAncestor = 
    LocateSceneGraphNodeWithName(ancestorName);
  CheckAndSetTransformFlag(commonAncestor, childName, isInverse);
}

bool SceneGraph
::CheckAndSetTransformFlag(SceneGraphNode* ancestorNode,
               const char * childName, 
               bool isInverse)
{
  if(strcmp(ancestorNode->GetName(),childName) == 0)
  {
    if(!isInverse)
    {
      ancestorNode->SetIsCurrentTransform(true);
    }
    else 
    {
      ancestorNode->SetIsCurrentInverseTransform(true);
    }
    return true;
  }
  for(std::list<SceneGraphNode*>::const_iterator i = ancestorNode->
    GetChildren()->begin(); i != ancestorNode->GetChildren()->end(); ++i)
  {
    if(strcmp((*i)->GetName(), childName) == 0)
    {
      if(!isInverse)
      {
        (*i)->SetIsCurrentTransform(true);
        ancestorNode->SetIsCurrentTransform(true);
      }
      else 
      {
        (*i)->SetIsCurrentInverseTransform(true);
        ancestorNode->SetIsCurrentInverseTransform(true);
      }
      return true;
    }
    if(CheckAndSetTransformFlag((*i),childName, isInverse))
    {
      if(!isInverse)
      {
        ancestorNode->SetIsCurrentTransform(true);
      }
      else 
      {
        ancestorNode->SetIsCurrentInverseTransform(true);
      }
      break;
    }
  }
  return false;
}

SceneGraphNode* SceneGraph
::LocateSceneGraphNodeWithName(const char * name)
{
  SceneGraphNode* node = NULL;
  for(std::list<SceneGraphNode*>::const_iterator i = 
    m_RootNodes.begin(); i != m_RootNodes.end(); ++i)
  {
    node = LocateSceneGraphNodeInRootNode((*i), name);
    if(node != NULL)
    {
      break;
    }
  }
  return node;
}

SceneGraphNode* SceneGraph
::LocateSceneGraphNodeInRootNode(SceneGraphNode* parentNode,
                 const char * name)
{
  if(strcmp(parentNode->GetName(),name) == 0)
  {
    return parentNode;
  }
  for(std::list<SceneGraphNode*>::const_iterator i = 
    parentNode->GetChildren()->begin(); i != parentNode->GetChildren()->end(); ++i)
  {
    SceneGraphNode* node = LocateSceneGraphNodeInRootNode((*i),name);
    if(node != NULL)
    {
      return node;
    }
  }
  return NULL;
}

void SceneGraph::SetNodeDetails(const char* nodeName, bool includeFlag)
{
  SceneGraphNode* sceneGraphNode = LocateSceneGraphNodeWithName(nodeName);
  sceneGraphNode->SetIsSelected(includeFlag);
  for(std::list<SceneGraphNode*>::iterator i = 
    m_RootNodes.begin(); i != m_RootNodes.end(); ++i)
  {
    ResetCurrentTransformFlag((*i));
  }
  /*m_SceneGraphUI->DrawSceneGraph(m_RootNodes,
    m_IsUIBeingShown, m_TransitionsStore); */
}

bool SceneGraph
::IsRequestedItemThere(const CoordinateSystemTransformToEvent 
             *setTransformEvent)
{
  for(std::list<SceneGraphNode*>::iterator i = m_RootNodes.begin(); 
    i != m_RootNodes.end(); ++i)
  {
    if(CheckRequestedItemThere((*i),setTransformEvent))
      return true;
  }
  return false;
}

bool SceneGraph
::CheckRequestedItemThere(SceneGraphNode* node,
              const CoordinateSystemTransformToEvent 
              *setTransformEvent)
{
  if(CompareEventSourceAndDestination(node, setTransformEvent))
  {
    return true;
  }
  for(std::list<SceneGraphNode*>::const_iterator j = node->GetChildren()->begin() 
  ; j != node->GetChildren()->end() ; ++j)
  {
    if(CheckRequestedItemThere((*j),setTransformEvent))
    {
      return true;
    }
  }
  return false;
}

bool SceneGraph
::CompareEventSourceAndDestination(SceneGraphNode* node,
                   const CoordinateSystemTransformToEvent 
                   *setTransformEvent)
{
  if(node->GetIsSelected())
  {
    if( (strcmp(node->GetName(),setTransformEvent->Get().
        GetSource()->GetName()) == 0) || 
      (strcmp(node->GetName(),setTransformEvent->Get().
        GetDestination()->GetName()) == 0) )
    {
      return true;
    }
  }
  return false;
}

} // end namespace igstk
