/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSceneGraphUI.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkSceneGraphUI_h
#define __igstkSceneGraphUI_h

#include <string.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.h>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Light_Button.H>
#include "FL/Fl_Group.H"
#include <FL/Fl_Multiline_Output.H>

#include "igstkSceneGraphNode.h"
#include "igstkSceneGraph.h"
#include "igstkCoordinateSystemTransformToResult.h"

namespace igstk
{

/** \class Drawer
 * 
 * \brief class to draw the shapes like lines in FLTK
 * window.
 *
 */

class Drawer : public Fl_Group
{
public:

  /** Constructor which takes x,y position, width, height name and rootNode
  * of a scene graph.
  */
  Drawer(int x, int y, int w, int h,const char* name=0,
    igstk::SceneGraphNode* rootNode=0) : Fl_Group(x,y,w,h,name)
  {
    xPreset = x;
    rNode = rootNode;
  }

protected:
  /**  Reference to the rootnode of the scene graph to draw.
  */
  igstk::SceneGraphNode* rNode;

  /**  xPreset for this group to draw in the FLTK window.
  */
  int xPreset;

  /**  Ovveridden method from Fl_Group.
  */
  void draw()
  {
    Fl_Group::draw();
    fl_line_style(FL_SOLID,2);
    for(std::list<igstk::SceneGraphNode*>::iterator iter = rNode->children.begin();
      iter != rNode->children.end(); ++iter)
    {
      fl_line( xPreset + ((*iter)->xC1) + 200/2,(*iter)->yC1,xPreset + (rNode->xC1) + 200/2,rNode->yC1 +40);
      fl_line_style(FL_SOLID,4);
      fl_line(xPreset + (rNode->xC1) + 200/2 -3,rNode->yC1 +40 +8,xPreset + (rNode->xC1) + 200/2,rNode->yC1 +40,xPreset + (rNode->xC1) + 200/2 + 3,rNode->yC1 +40 +8);
      fl_line_style(FL_SOLID,2);
      for(std::list<igstk::SceneGraphNode*>::iterator iter1 = (*iter)->children.begin();
        iter1 != (*iter)->children.end(); ++iter1)
      {
        drawOthers((*iter1),(*iter));
      }
    }
  }

  
  /** Function to recursively draw the lines between the nodes.
  */
  void drawOthers(igstk::SceneGraphNode* childNode, 
    igstk::SceneGraphNode* parentNode)
  {
    fl_line(xPreset + (childNode)->xC1 + 200/2,childNode->yC1
      ,xPreset + (parentNode->xC1) + 200/2,parentNode->yC1 +40);
    fl_line_style(FL_SOLID,4);

    //Arrow.
    fl_line(xPreset + (parentNode->xC1) + 200/2 -3,
      parentNode->yC1 +40 +8,xPreset + (parentNode->xC1) + 200/2,
      parentNode->yC1 +40,xPreset + (parentNode->xC1) + 200/2 + 3,
      parentNode->yC1 +40 +8);
    fl_line_style(FL_SOLID,2);
    for(std::list<igstk::SceneGraphNode*>::iterator iter1 = 
      childNode->children.begin();
        iter1 != childNode->children.end(); ++iter1)
    {
      drawOthers((*iter1),(childNode));
    }
  }

};

/** \class SceneGraphUI
 * 
 * \brief This class draws the scene graph built for an application
 * in a new FLTK window.
 *
 * The SceneGraphUI class takes the scene graph data structure
 * to create an FLTK window. 
 *
 */

class SceneGraphUI
{
public:

  /**  Function to get a new instance of the scene graph. Singleton pattern.
  */
  static SceneGraphUI* getInstance();

  /**  Function called from Scene Graph to Draw the scene graph in a 
  * new window.
  */
  void DrawSceneGraph(SceneGraph* sceneGraph);

private:

  /**  Constructor.
  */
  SceneGraphUI();

  /**  Destructor.
  */
  ~SceneGraphUI();

  /**  Flag to check if the singleton class is created.
   */
  static bool instanceFlag;

  /**  The variable to hold singleton class.
   */
    static SceneGraphUI *single;

  /**  Reference to the FLTK window object.
  */
  Fl_Window *SceneGraphWindow;

  /**  Flag to check if the UI is drawn or not.
  */
  bool windowDrawn;

  /**  Function to find the number of nodes in 
  * each row in each root node.
  */
  void FindXDepthIndividual(std::list<SceneGraphNode*> rootNodes, 
    int finalDepth[2][10]);

  /**  Function to find the number of nodes in each row
  * and is used by FindXDepthIndividual().
  */
  void FindXD(igstk::SceneGraphNode* parentNode, 
    int result[10],int depth);

  /**  Function to return the number of root nodes.
  */
  int FindNumberOfRootNodes(std::list<SceneGraphNode*> rootNodes);

  /**  Function to find the mac number of rows to calculate the height
  * of the window.
  */
  int FindYDepth(int xDepthIndividual[2][10]);

  /**  Function to find maximum number of nodes among all the rows.
  */
  int FindMaxInXDepth(int yDepth[2][10]);

  /**  Calculate the x and y coordinates for each node.
  */
  void CalculateXYForNodes(std::list<SceneGraphNode*> rootNodes, 
    int xDepthIndividual[2][10]);

  /** Function to help CalculateXYForNodes() to calculate x and y 
  * coordinates for each node.
  */
  void CalculateXYForNode(SceneGraphNode* parentNode, 
    int xDepth[10], int depth, int boxNumberInRow[10]);

  /**  Function to find the maximum number in the given array.
  * A helper function.
  */
  int FindMaxInArray(int array1[10]);

  /**  Function to create the boxes required for each node.
  */
  void CreateBoxesForNodes(Drawer* group, 
    SceneGraphNode* parentNode, int xPreset);

  /**  Callback function for button click action on each 
  * of the nodes drawn by the user.
  */
    static void cb_NodeBt(Fl_Light_Button* box, void* data); 
};
}
#endif
