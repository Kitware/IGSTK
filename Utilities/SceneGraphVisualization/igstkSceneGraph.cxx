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
  isUIBeingShown = false;
  isChanged = false;
}

SceneGraph::~SceneGraph()
{
  instanceFlag = false;
}

bool SceneGraph::instanceFlag = false;
SceneGraph* SceneGraph::single = NULL;
SceneGraph* SceneGraph::getInstance()
{
    if(! instanceFlag)
    {
        single = new SceneGraph();
        instanceFlag = true;
        return single;
    }
    else
    {
        return single;
    }
}


void SceneGraph
::AddCoordinateSystem(const 
            CoordinateSystemSetTransformEvent *setTransformEvent)
{
  bool parentFound = false;
  for(std::list<SceneGraphNode*>::iterator i = rootNodes.begin(); 
    i != rootNodes.end(); ++i)
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
  for(std::list<SceneGraphNode*>::iterator rootNode1 = rootNodes.begin(); 
    rootNode1 != rootNodes.end(); ++rootNode1)
  {
    for(std::list<SceneGraphNode*>::iterator rootNode2 = rootNodes.begin();
    rootNode2 != rootNodes.end(); ++rootNode2)
    {
      if(rootNode1 == rootNode2)
        continue;
      if(IsParentExisting(*rootNode1, (*rootNode2)->name))
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
  if(strcmp(rootNode->name,childNode->name) == 0)
  {
    rootNode->parent->children.push_back(childNode);
    childNode->parent = rootNode->parent;
    rootNodes.remove(childNode);
    return true;
  }
  for(std::list<SceneGraphNode*>::iterator j = rootNode->children.begin();
    j != rootNode->children.end(); ++j)
  {
    if(ClubBothRootNodes((*j),childNode))
    {
      rootNode->children.remove(*j);
      break;
    }
  }
  return false;
}

bool SceneGraph
::IsParentExisting(SceneGraphNode *rootNode, const char *parentName)
{
  bool parentFound = false;
  if(strcmp(rootNode->name,parentName) == 0)
  {
    return true;
  }
  for(std::list<SceneGraphNode*>::iterator j = rootNode->children.begin();
    j != rootNode->children.end(); ++j)
  {
    if(strcmp((*j)->name,parentName) == 0)
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
  
  if(strcmp(rootNode->name,setTransformEvent->Get().GetSource()->GetName())
        == 0)
  {
    for(std::list<SceneGraphNode*>::iterator i = rootNode->children.
      begin(); i != rootNode->children.end(); ++i)
    {
      if(strcmp((*i)->name,setTransformEvent->Get().GetDestination()->
        GetName()) == 0)
      {
        return;
      }
    }
    SceneGraphNode* child = new SceneGraphNode();
    child->coordinateSystem = const_cast<CoordinateSystem *> 
            (setTransformEvent->Get().GetDestination());
    child->type = const_cast<char*> 
            (setTransformEvent->Get().GetDestination()->GetType());
    child->name = const_cast<char*> 
            (setTransformEvent->Get().GetDestination()->GetName());
    child->parent = rootNode;

    rootNode->children.push_back(child);
    return;
  }
  for(std::list<SceneGraphNode*>::iterator j = rootNode->children.begin();
    j != rootNode->children.end(); ++j)
  {
    if(strcmp((*j)->name, setTransformEvent->Get().GetSource()->
      GetName()) == 0)
    {
      for(std::list<SceneGraphNode*>::iterator k = (*j)->children.
        begin(); k != (*j)->children.end(); ++k)
      {
        if(strcmp((*k)->name,setTransformEvent->Get().
          GetDestination()->GetName()) == 0)
        {
          return;
        }
      }
      SceneGraphNode* child = new  SceneGraphNode();
      child->coordinateSystem = const_cast<CoordinateSystem *> 
            (setTransformEvent->Get().GetDestination());
      child->type = const_cast<char*> 
            (setTransformEvent->Get().GetDestination()->GetType());
      child->name = const_cast<char*> 
            (setTransformEvent->Get().GetDestination()->GetName());
      child->parent = (*j);
      
      (*j)->children.push_back(child);
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
  parent->type = const_cast<char *> 
          (setTransformEvent->Get().GetSource()->GetType());
  parent->name = const_cast<char *> 
          (setTransformEvent->Get().GetSource()->GetName());
  parent->coordinateSystem = const_cast<CoordinateSystem *> 
          ( setTransformEvent->Get().GetSource());
  parent->parent = NULL;

  SceneGraphNode* child = new SceneGraphNode();
  child->coordinateSystem = const_cast<CoordinateSystem *> 
          (setTransformEvent->Get().GetDestination());
  child->type = const_cast<char *> 
          (setTransformEvent->Get().GetDestination()->GetType());
  child->name = const_cast<char *> 
          (setTransformEvent->Get().GetDestination()->GetName());
  child->parent = parent;
  child->parentTransform = setTransformEvent->Get().GetTransform();
  parent->children.push_back(child);
  rootNodes.push_back(parent);
}

void SceneGraph::printNodeDetails()
{
  std::cout << "START SceneGraph Details." << std::endl;
  std::cout << "--------------------------------" << std::endl;

  for(std::list<SceneGraphNode*>::iterator k = rootNodes.begin();
    k != rootNodes.end(); ++k)
  {
    printParentNodeDetails(*k);
  }

  std::cout << "End SceneGraph Details." << std::endl;
  std::cout << "----------------------------------------" << std::endl;
}

void SceneGraph::printParentNodeDetails(SceneGraphNode* parentNode)
{
  print(parentNode);
  for(std::list<SceneGraphNode*>::iterator m = parentNode->children.begin();
    m != parentNode->children.end(); ++m)
  {
    printParentNodeDetails(*m);
  }
}

void SceneGraph::print(igstk::SceneGraphNode *node)
{
  std::cout << "********Details********" << std::endl;
  std::cout << "Name  : " << node->name << std::endl;
  if(node->parent != NULL)
  {
    std::cout << "Parent: " << node->parent->name << std::endl;
  }
  else 
  {
    std::cout << "Parent: No parent" << std::endl;
  }
  std::cout << "Coordinate System: " << node->coordinateSystem->
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

  child->parent = NULL;
  rootNodes.push_back(child);
  parent->children.remove(child);
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
  for(std::list<SceneGraphNode *>::iterator rootNode = rootNodes.begin();
    rootNode != rootNodes.end(); ++rootNode)
  {
    DrawParentChildrenRelationship(ostr, *rootNode);
  }
  for(std::list<SceneGraphNode *>::iterator rootNode = rootNodes.begin();
    rootNode != rootNodes.end(); ++rootNode)
  {
    DrawDescription(ostr, *rootNode);
  }
  ostr << "}" << std::endl;
}

void SceneGraph
::DrawParentChildrenRelationship(std::ostream &ostr, 
                 igstk::SceneGraphNode *parentNode)
{
  for(std::list<SceneGraphNode*>::iterator child = parentNode->
      children.begin();
    child != parentNode->children.end(); ++child)
  {
    ostr << "\"" << parentNode->name << "\"" <<" -> " << 
          "\""<<(*child)->name<< "\""<<";" << std::endl;
    DrawParentChildrenRelationship(ostr, *child);
  } 
}

void SceneGraph
::DrawDescription(std::ostream &ostr, 
          igstk::SceneGraphNode *parentNode)
{
  if(strcmp(parentNode->type, "TrackerTool") == 0)
  {
    ostr << "\"" << parentNode->name << "\"" << 
      "[shape=polygon, sides=6, color=red, style=filled];" << std::endl;
  }
  else if(strcmp(parentNode->type, "SpatialObject") == 0)
  {
    ostr << "\"" << parentNode->name << "\"" << 
      "[shape=polygon, sides=4, color=blue, style=filled];" << std::endl;
  }
  else if(strcmp(parentNode->type, "Tracker") == 0)
  {
    ostr << "\"" << parentNode->name << "\"" << 
  "[shape=polygon, sides=4, distortion=-0.7, color=yellow, style=filled];"
      << std::endl;
  }
  else if(strcmp(parentNode->type, "View") == 0)
  {
    ostr << "\"" << parentNode->name << "\"" << 
      "[shape=polygon, sides=8, color=green, style=filled];" << 
      std::endl;
  }
  for(std::list<SceneGraphNode*>::iterator child = parentNode->
      children.begin();
    child != parentNode->children.end(); ++child)
  {
    DrawDescription(ostr, *child);
  } 
}

 void SceneGraph::ShowSceneGraph(bool showFlag)
{
  isUIBeingShown = showFlag;
  //m_SceneGraphUI->DrawSceneGraph(rootNodes,showFlag,transitionsStore);
} 

void SceneGraph::ResetCurrentTransformFlag(SceneGraphNode* parentNode)
{
  parentNode->isCurrentTransform = false;
  parentNode->isCurrentInverseTransform = false;
  for(std::list<SceneGraphNode*>::iterator it = parentNode->
    children.begin();
    it != parentNode->children.end(); ++it)
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
    for(std::list<SceneGraphNode*>::iterator i = 
      rootNodes.begin(); i != rootNodes.end(); ++i)
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
    /* m_SceneGraphUI->DrawSceneGraph(rootNodes,
      isUIBeingShown, transitionsStore); */
    isChanged = true;
  }
}

bool SceneGraph
::IsNewTranformEvent(const 
           CoordinateSystemTransformToEvent *setTransformEvent)
{
  bool isNew = true;
  
  for(std::list<Transition *>::iterator it = transitionsStore.begin();
      it != transitionsStore.end(); ++it)
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
    transitionsStore.push_back(newTransition);
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
  if(strcmp(ancestorNode->name,childName) == 0)
  {
    if(!isInverse)
    {
      ancestorNode->isCurrentTransform = true;
    }
    else 
    {
      ancestorNode->isCurrentInverseTransform = true;
    }
    return true;
  }
  for(std::list<SceneGraphNode*>::iterator i = ancestorNode->
    children.begin(); i != ancestorNode->children.end(); ++i)
  {
    if(strcmp((*i)->name, childName) == 0)
    {
      if(!isInverse)
      {
        (*i)->isCurrentTransform = true;
        ancestorNode->isCurrentTransform = true;
      }
      else 
      {
        (*i)->isCurrentInverseTransform = true;
        ancestorNode->isCurrentInverseTransform = true;
      }
      return true;
    }
    if(CheckAndSetTransformFlag((*i),childName, isInverse))
    {
      if(!isInverse)
      {
        ancestorNode->isCurrentTransform = true;
      }
      else 
      {
        ancestorNode->isCurrentInverseTransform = true;
      }
      break;
    }
  }
  return false;
}

SceneGraphNode* SceneGraph
::LocateSceneGraphNodeWithName(const char * name)
{
  bool nodeFound = false;
  SceneGraphNode* node = NULL;
  for(std::list<SceneGraphNode*>::iterator i = 
    rootNodes.begin(); i != rootNodes.end(); ++i)
  {
    node = LocateSceneGraphNodeInRootNode((*i), name);
    if(node != NULL)
    {
      nodeFound = true;
      break;
    }
  }
  return node;
}

SceneGraphNode* SceneGraph
::LocateSceneGraphNodeInRootNode(SceneGraphNode* parentNode,
                 const char * name)
{
  if(strcmp(parentNode->name,name) == 0)
  {
    return parentNode;
  }
  for(std::list<SceneGraphNode*>::iterator i = 
    parentNode->children.begin(); i != parentNode->children.end(); ++i)
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
     SceneGraphNode* sceneGraphNode = 
    LocateSceneGraphNodeWithName(nodeName);
  sceneGraphNode->isSelected = includeFlag;
  for(std::list<SceneGraphNode*>::iterator i = 
    rootNodes.begin(); i != rootNodes.end(); ++i)
  {
    ResetCurrentTransformFlag((*i));
  }
  /*m_SceneGraphUI->DrawSceneGraph(rootNodes,
    isUIBeingShown, transitionsStore); */
}

bool SceneGraph
::IsRequestedItemThere(const CoordinateSystemTransformToEvent 
             *setTransformEvent)
{
  for(std::list<SceneGraphNode*>::iterator i = rootNodes.begin(); 
    i != rootNodes.end(); ++i)
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
  for(std::list<SceneGraphNode*>::iterator j = node->children.begin() 
    ; j != node->children.end() ; ++j)
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
  if(node->isSelected)
  {
    if( (strcmp(node->name,setTransformEvent->Get().
        GetSource()->GetName()) == 0) || 
      (strcmp(node->name,setTransformEvent->Get().
        GetDestination()->GetName()) == 0) )
    {
      return true;
    }
  }
  return false;
}

std::list<SceneGraphNode*> SceneGraph::GetRootNodes()
{
  return rootNodes;
}

} // end namespace igstk
