/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSceneGraphNode.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSceneGraphNode_h
#define __igstkSceneGraphNode_h

#include <iostream>
#include "igstkTransform.h"
#include "igstkCoordinateSystem.h"

namespace igstk 
{

/** \class SceneGraphNode
 * 
 * \brief Class to represent each of the nodes of the 
 * scene graph.
 *
 */

class SceneGraphNode
{
public:
   
  /**  Constructor.
  */
   SceneGraphNode(void);

   /**  Destructor.
  */
  virtual ~SceneGraphNode(void);
      
  /**  Get/Set value of x coordinate of the node in FLTK window. */
  igstkGetMacro(XC1, int); 
  igstkSetMacro(XC1, int); 

  /**  Get/Set value of y coordinate of the node in FLTK window. */
  igstkGetMacro(YC1, int);
  igstkSetMacro(YC1, int);

  /**  Get/Set value of flag to set if this node is in the current transform
  * being computed.*/
  igstkGetMacro(IsCurrentTransform, bool);
  igstkSetMacro(IsCurrentTransform, bool);

  /**  Get/Set value of flag to indicate if this is selected by the user.*/
  igstkGetMacro(IsSelected, bool);
  igstkSetMacro(IsSelected, bool);

  /**  Get/Set value of flag to set if this node is in the current inverse transform
  * being computed.*/
  igstkGetMacro(IsCurrentInverseTransform, bool);
  igstkSetMacro(IsCurrentInverseTransform, bool);

  /**  Get/Set reference to the parent node.*/
  typedef SceneGraphNode* SceneGraphNodePtr;
  igstkGetMacro(Parent, SceneGraphNodePtr);
  igstkSetMacro(Parent, SceneGraphNodePtr);

  /**  Get/Set name of the node.*/
  typedef char* CharPtr; 
  igstkGetMacro(Name, CharPtr);
  igstkSetMacro(Name, CharPtr);

  /**  Get/Set type of the node (Spatial Object/View/TrackerTool/Tracker).*/
  igstkGetMacro(Type, CharPtr);      
  igstkSetMacro(Type, CharPtr);      

  /**  Get/Set reference to the coordinate system object 
  * represented by this node. */
  typedef CoordinateSystem* CoordinateSystemPtr;
  igstkGetMacro(CoordinateSystem, CoordinateSystemPtr);
  igstkSetMacro(CoordinateSystem, CoordinateSystemPtr);
      
  /**  Get/Set list of children.*/
  typedef std::list<SceneGraphNode*> SceneGraphNodeList;
  typedef SceneGraphNodeList* SceneGraphNodeListPtr;
  igstkGetMacro(Children, SceneGraphNodeListPtr);
  igstkSetMacro(Children, SceneGraphNodeListPtr);

  /**  Get/Set Reference to the transform with parent coordiante system.*/
  igstkGetMacro(ParentTransform, Transform); 
  igstkSetMacro(ParentTransform, Transform); 
      
private:
  /**  x coordinate of the node in FLTK window.
  */
  int m_XC1;

  /**  y coordinate of the node in FLTK window.
  */
  int m_YC1;

  /**  Flag to set if this node is in the current transform
  * being computed.
  */
  bool m_IsCurrentTransform;

  /**  Flag to indicate if this is selected by the user.
  */
  bool m_IsSelected;

  /**  Flag to set if this node is in the current inverse transform
  * being computed.
  */
  bool m_IsCurrentInverseTransform;

  /**  Reference to the parent node.
  */
  SceneGraphNode* m_Parent;

  /**  Name of the node.
  */
  char* m_Name;

  /**  Type of the node (Spatial Object/View/
  * TrackerTool/Tracker).
  */
  char* m_Type;

  /**  Reference to the coordinate system object 
  * represented by this node.
  */
  CoordinateSystem* m_CoordinateSystem;

  /**  List of children.
  */
  std::list<SceneGraphNode*> *m_Children;

  /**  Reference to the transform with parent coordiante system.
  */
  Transform m_ParentTransform;

};
}
#endif
