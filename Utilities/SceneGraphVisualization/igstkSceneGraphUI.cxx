/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSceneGraphUI.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkSceneGraphUI.h"

namespace igstk
{
SceneGraphUI::SceneGraphUI() 
{
  m_WindowDrawn = false;
  m_SceneGraphWindow = NULL;
}

SceneGraphUI::~SceneGraphUI()
{
}

bool SceneGraphUI::m_InstanceFlag = false;
SceneGraphUI* SceneGraphUI::m_Single = NULL;
SceneGraphUI* SceneGraphUI::getInstance()
{
    if(! m_InstanceFlag)
    {
        m_Single = new SceneGraphUI();
        m_InstanceFlag = true;
        return m_Single;
    }
    else
    {
        return m_Single;
    }
}

void SceneGraphUI
::DrawSceneGraph(SceneGraph* sceneGraph)
{

  std::list<SceneGraphNode*> rootNodes = sceneGraph->GetRootNodes();
  bool showFlag     = sceneGraph->GetIsUIBeingShown();
  std::list<Transition *> transitionsStore = sceneGraph->GetTransitionsStore();

  if(showFlag)
  {
    Fl_Window* w;
    int xDepth = FindNumberOfRootNodes(rootNodes);
    int xDepthIndividual[2][10] = { {0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0}};
    FindXDepthIndividual(rootNodes, xDepthIndividual);
    int yDepth = FindYDepth(xDepthIndividual);
    int xDepthMax = FindMaxInXDepth(xDepthIndividual);
    CalculateXYForNodes(rootNodes,xDepthIndividual);
    if(!m_WindowDrawn)
    {
      Fl_Window* o = m_SceneGraphWindow = new Fl_Window(xDepth * 
        (xDepthMax +1) * 250 + 50,yDepth * 60 + 200,"SceneGraph");
      o->resizable(o);
      w = o;
      w->show();
    }
    else 
    {
      w = m_SceneGraphWindow;
      //w->clear();
    }
    int count = 0;
    w->begin();
    for(std::list<SceneGraphNode *>::iterator rootNode = 
      rootNodes.begin();
      rootNode != rootNodes.end(); ++rootNode,count++)
    {
      int xPreset = 25 + count*250;
      Drawer* group = new Drawer(xPreset ,0, xDepthMax*250 +50 ,
        yDepth *60,"Window" + count ,*rootNode);
      group->color(FL_RED);
      CreateBoxesForNodes(group, *rootNode, xPreset);
    }
    Fl_Multiline_Output *outText = new Fl_Multiline_Output(0,yDepth *60,
      xDepthMax*250 +50,200,"Transitions" );
    
    if(transitionsStore.size() > 0)
    {
      char *cat = (char*)malloc( transitionsStore.size() * 200);
      strcpy(cat,""); 
      for(std::list<Transition *>::iterator it = 
        transitionsStore.begin();
        it != transitionsStore.end(); ++it)
      {

        strncat(cat, (*it)->source, strlen((*it)->source));
        strncat(cat, "->", strlen("->"));
        strncat(cat, (*it)->cAncestor, strlen((*it)->cAncestor));
        strncat(cat, "->", strlen("->"));
        strncat(cat, (*it)->destination, strlen((*it)->destination));
        strncat(cat,"\n", strlen("\n"));
      }
      outText->value(cat);
      free(cat);
    }
    outText->redraw();
    w->add(outText);
    w->end();
    m_WindowDrawn = true;
    w->redraw();
  }
  else 
  {
    if(m_SceneGraphWindow != NULL)
      m_SceneGraphWindow->hide();
    m_WindowDrawn = false;
  }
}

void SceneGraphUI
::CreateBoxesForNodes(Drawer* group, 
            SceneGraphNode* parentNode, 
            int xPreset)
{
  Fl_Light_Button * box = new Fl_Light_Button(
     xPreset + parentNode->GetXC1(),parentNode->GetYC1(),200,40,parentNode->GetName());
  box->box(FL_UP_BOX);
  box->color(FL_WHITE);
  group->add(box);
  box->callback((Fl_Callback*)cb_NodeBt);
  if(parentNode->GetIsCurrentTransform())
  {
    box->color(FL_GREEN);
  }
  if(parentNode->GetIsCurrentInverseTransform())
  {
    box->color(FL_RED);
  }
  if(parentNode->GetIsSelected())
  {
    box->value(1);
  }

  for(std::list<SceneGraphNode *>::const_iterator childNode = 
         parentNode->GetChildren()->begin();
    childNode != parentNode->GetChildren()->end(); ++childNode)
  {
    CreateBoxesForNodes(group,*childNode, xPreset);
  }
}

void SceneGraphUI
::CalculateXYForNodes(std::list<SceneGraphNode*> rootNodes, 
            int xDepthIndividual[2][10])
{
  int count = 0;
  for(std::list<SceneGraphNode *>::const_iterator rootNode = rootNodes.begin();
    rootNode != rootNodes.end(); ++rootNode)
  {
    int boxNumberInRow[10] = {0,0,0,0,0,0,0,0,0,0};
    CalculateXYForNode(*rootNode, 
      xDepthIndividual[count],0,boxNumberInRow);
    count++;
  }
}

void SceneGraphUI
::CalculateXYForNode(SceneGraphNode* parentNode, 
           int xDepth[10], int depth, int boxNumberInRow[10])
{
  int xValue = 0;
  int maxValue = FindMaxInArray(xDepth);
  if(maxValue != 1)
  {
    if( (boxNumberInRow[depth] %2) == 0)
    {
      xValue = (((250 * maxValue)/2) - 
        ((boxNumberInRow[depth]/2 +1) * 250));
    }
    else 
    {
      xValue = (((250 * maxValue)/2) + 
        ((boxNumberInRow[depth]/2 +1) * 250));
    }
  }
  
  int yValue = depth * 60;
  if(depth ==0) 
  {
    xValue = 250*maxValue/2;
  }

  parentNode->SetXC1(xValue);
  parentNode->SetYC1(yValue);
  boxNumberInRow[depth]++;
  for(std::list<SceneGraphNode *>::const_iterator childNode = 
    parentNode->GetChildren()->begin();
    childNode != parentNode->GetChildren()->end(); ++childNode)
  {
    CalculateXYForNode(*childNode, xDepth, depth +1, boxNumberInRow);
  }
}

int SceneGraphUI::FindMaxInArray(int array1[10])
{
  int max = array1[0];
  for(unsigned int i=0; i< 10; i++)
  {
    if(array1[i] > max)
    {
      max = array1[i];
    }
  }

  return max;
}

int SceneGraphUI::FindYDepth(int xDepthIndividual[2][10])
{
  int maxYDepth = 0;
  for(unsigned int i=0; i<2;i++)
  {
    for(int j=0; j<10 ; j++)
    {
      if(xDepthIndividual[i][j] == 0)
      {
        if(maxYDepth < j)
        {
          maxYDepth = j;
        }
        break;
      }
    }
  }

  return maxYDepth;
}

int SceneGraphUI::FindMaxInXDepth(int yDepth[2][10])
{
  int maxValue = yDepth[0][0];
  for(unsigned int i=0;i<2;i++)
  {
    for(int j=0;j<10;j++)
    {
      if(yDepth[i][j] > maxValue)
      {
        maxValue = yDepth[i][j];
      }
    }
  }
  return maxValue;
}

void SceneGraphUI
::FindXDepthIndividual(std::list<SceneGraphNode*> rootNodes, 
             int finalDepth[2][10])
{
  int count = 0;
  for(std::list<SceneGraphNode *>::const_iterator rootNode = rootNodes.begin();
    rootNode != rootNodes.end(); ++rootNode)
  {
    FindXD(*rootNode, finalDepth[count],0);
    count++;
  }
}

void SceneGraphUI
::FindXD(igstk::SceneGraphNode* parentNode, 
     int result[10], int depth)
{
  result[depth]++;
  for(std::list<SceneGraphNode *>::const_iterator childNode = 
    parentNode->GetChildren()->begin();
    childNode != parentNode->GetChildren()->end(); ++childNode)
  {
    FindXD(*childNode, result, depth +1);
  }
}

int SceneGraphUI
::FindNumberOfRootNodes(std::list<SceneGraphNode*> rootNodes)
{
  return rootNodes.size();
}

void SceneGraphUI
::cb_NodeBt(Fl_Light_Button* o, void* v) {
  SceneGraph::getInstance()->SetNodeDetails(o->label(),o->value());
  SceneGraphUI::getInstance()->DrawSceneGraph(igstk::SceneGraph::getInstance());
  (void) v; //Get rid of unused parameter warning
}
}
