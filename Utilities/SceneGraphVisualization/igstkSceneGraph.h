/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSceneGraph.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSceneGraph_h
#define __igstkSceneGraph_h

#if defined(_MSC_VER)
// _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )
#endif

#include "igstkCoordinateSystemSetTransformResult.h"
#include "igstkSceneGraphNode.h"
#include "igstkCoordinateSystem.h"
#include "igstkCoordinateSystemTransformToResult.h"

#include <iostream>
#include <string>
namespace igstk 
{

 /** \class Transition
 * 
 * \brief Bean class for storing the transitions
 * occurred.
 *
 */

class Transition
{
public:
  const char * source;
  const char * destination;
  const char * cAncestor;
};
  
  /** \class SceneGraph
 * 
 * \brief This class represents the Scene Graph of the application.
 *
 * The Scene Graph class will capture the relationships between 
 * coordinate systems created in the application and will create
 * a data structure to hold all the relations. It also provides
 * functionalities to export the tree structure in .dot format and 
 * see the structure dynamically in an FLTK window.
 *
 * http://en.wikipedia.org/wiki/Scene_graph
 *
 */
class SceneGraph
{

public:

  /**  Function to add the coordinate system relationships 
  * to the scene graph.
  */
  void AddCoordinateSystem(const 
             CoordinateSystemSetTransformEvent* setTransformEvent);

  /**  Function to get a new instance of the scene graph. Singleton pattern.
  */
  static SceneGraph* getInstance();

  /** Function to print the node details to std::cout. 
  */
  void printNodeDetails();

  /**  Function to detach the coordinate system.
  */
  void DetachCoordinateSystem(const 
          CoordinateSystemSetTransformEvent* setTransformEvent);

  /**  Function to export the scene graph to dot format.
  */
  void ExportSceneGraphToDot(std::string fileName);

  /**  Function to show the scene graph in a new FLTK window.
  */
  void ShowSceneGraph(bool showFlag);

  /**  Function to Show the transform path for the given transform event.
  */
  void ShowTheTransformPath(const 
          CoordinateSystemTransformToEvent* transformPathEvent);

  /**  Function to set the node selected when the user selects of deselects it.
  */
  void SetNodeDetails(const char* nodeName, bool includeFlag);


  /** Get/Set for rootNodes list */
  igstkGetMacro(RootNodes, std::list<SceneGraphNode*>); 
  igstkSetMacro(RootNodes, std::list<SceneGraphNode*>); 

  /** Get/Set flag to check if the UI window is open. */
  igstkGetMacro(IsUIBeingShown, bool); 
  igstkSetMacro(IsUIBeingShown, bool); 

  /** Get/Set flag to check if the root Nodes have changed 
   *  to draw the scene graph UI.*/
  igstkGetMacro(IsChanged, bool);
  igstkSetMacro(IsChanged, bool);
      
  /** Get/Set data structure to store all the wanted transitions. */
  igstkGetMacro(TransitionsStore, std::list<Transition *>); 
  igstkSetMacro(TransitionsStore, std::list<Transition *>); 
      
private:
  /**  Constructor.
   */
  SceneGraph();

  /** Destructor. 
   */
  ~SceneGraph();

   /** Function to check if the parent is existing, when a new relationship
   *  is created.
   */
  bool IsParentExisting(SceneGraphNode* parentNode, const char *parentName);

  /**  Method to add the coordinate system if the parent exists.
   */
  void AddCoordinateSystemToParent(SceneGraphNode* parentNode, 
    const CoordinateSystemSetTransformEvent *setTransformEvent);
  
  /**  Function to create a new parent and add the corresponding 
  * nodes when the parent in not existing.
   */
  void CreateNewParentAndAddCoordinateSystem(const 
       CoordinateSystemSetTransformEvent *setTransformEvent);

  /**  Function to print the details of the given parent.
   */
  void printParentNodeDetails(SceneGraphNode* parentNode);

  /**  Function to print the details of the given node.
   */
  void print(SceneGraphNode* node);

  /**  Function to export the description to dot to the given
  * stream.
   */
  void ExportDescriptionToDot(std::ostream & ostr);

  /**  Function to draw the parent child relationship for .dot 
  * export functionality.
   */
  void DrawParentChildrenRelationship(std::ostream &ostr,  
    igstk::SceneGraphNode *parentNode);

  /**  Function to draw the description of each node to the stream given.
   */
  void DrawDescription(std::ostream &ostr,  
    igstk::SceneGraphNode *parentNode);

  /**  Function to Optimize the tree structure.
   */
  void OptimizeTree();

  /**  Function called by OptimizeTree() to club if the node
  * is duplicated.
   */
  bool ClubBothRootNodes(SceneGraphNode* rootNode, 
    SceneGraphNode* childNode);

  /**  Reset the CurrentTranform Flag for the all the nodes under
  * the parent Node given.
   */
  void ResetCurrentTransformFlag(SceneGraphNode* parentNode);

  /**  Function to set the transform path. This function calculates the path
  * from ancestor to the child and sets inverse if the tranform in inverse.
  */
  void SetTransformPath(const char * ancestorName,
    const char * childName, bool isInverse);

  /**  Function to set the tranformflag of the common ancestor found and the child.
  */
  bool CheckAndSetTransformFlag(SceneGraphNode* ancestorNode,
    const char * childName, bool isInverse);

  /**  Function to get the reference to the scene graph node with 
  * the given name.
  */
  SceneGraphNode* LocateSceneGraphNodeWithName(const char * name);

  /** Function to get the reference to the scene graph in the parent node 
  * with the given name. This function is called internally by 
  * LocateSceneGraphNodeWithName() function.
  */
  SceneGraphNode* LocateSceneGraphNodeInRootNode(SceneGraphNode* parentNode,
    const char * name);

  /**  Function to check if the user selected node is there in the 
  * transform event.
  */
  bool IsRequestedItemThere(const 
    CoordinateSystemTransformToEvent *setTransformEvent);

  /**  Function called by IsRequestedItemThere() to check if the requested 
  * item is there or not.
  */
  bool CheckRequestedItemThere(SceneGraphNode* node,
    const CoordinateSystemTransformToEvent *setTransformEvent);

  /** Function called by CheckRequestedItemThere() to check if the tranform 
  * event is there in the source or destination.
  */
  bool CompareEventSourceAndDestination(SceneGraphNode* node,
    const CoordinateSystemTransformToEvent *setTransformEvent);
  
  /**  Function to check if the tranform event is a new one or one among 
  * already captured events.
  */
  bool IsNewTranformEvent(const 
    CoordinateSystemTransformToEvent *setTransformEvent);
      
      
  /** The main data structure of the scene graph class. 
   */
  std::list<SceneGraphNode*> m_RootNodes;

  /**  Flag to check if the singleton class is created.
   */
  static bool m_InstanceFlag;

  /**  The variable to hold singleton class.
   */
  static SceneGraph *m_Single;

  /** flag to check if the UI window is open. 
   */
  bool m_IsUIBeingShown;

  /** flag to check if the root Nodes have changed 
   *  to draw the scene graph UI.
   */
  bool m_IsChanged;

  /** The data structure to store all the wanted transitions. 
   */
  std::list<Transition *> m_TransitionsStore;
    
};
} //end namespace igstk.
#endif
