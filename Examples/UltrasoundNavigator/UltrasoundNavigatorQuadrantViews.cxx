/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    UltrasoundNavigatorQuadrantViews.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "UltrasoundNavigatorQuadrantViews.h"

namespace igstk
{

UltrasoundNavigatorQuadrantViews::UltrasoundNavigatorQuadrantViews(int X, int Y, int W, int H, const char *L)
  : Fl_Group(X, Y, W, H, L)
{

  m_Reporter = ::itk::Object::New();
 
  const int N = 15;    // Height of the slider bar
  const int C = 5;    // Width of the vertical separator

  Fl_Group * parentGroup = this->parent();

  m_X = 150; // width of the control panel
  m_Y = 0;

  m_Width = parentGroup->w() - m_X;
  m_Height = parentGroup->h() - 8; 

  m_WW = (int) (m_Width-2*C)/2;
  m_HH = (int) (m_Height- (N*2) )/2;

  // Create widgets
  typedef igstk::FLTKWidget   WidgetType;

  // top left view
  m_CTWidget1    = new WidgetType(X, Y, m_WW, m_HH, "Display 0");
  // top right
  m_CTWidget2    = new WidgetType(X+m_WW+C, Y, m_WW, m_HH, "Display 1");
  // bottom left
  m_VideoWidget  = new WidgetType(X, Y+m_HH+N, m_WW, m_HH, "Display 2");
  // botom right
  m_3DWidget     = new WidgetType(X+m_WW+C, Y+m_HH+N, m_WW, m_HH+N, "Display 3");       

  // Create views
  m_CTView1    = ViewType2D::New();
  m_CTView2 = ViewType2D::New();
  m_VideoView  = ViewType2D::New();
  m_3DView = ViewType3D::New();

  m_CTWidget1->RequestSetView( m_CTView1 );
  m_CTWidget2->RequestSetView( m_CTView2 );
  m_VideoWidget->RequestSetView( m_VideoView );
  m_3DWidget->RequestSetView( m_3DView );

  //m_AxialViewAnnotation = igstk::Annotation2D::New();
  //m_SagittalViewAnnotation = igstk::Annotation2D::New();
  //m_CoronalViewAnnotation = igstk::Annotation2D::New();

  // Create slider bars

  {
  Fl_Value_Slider* o = m_SuperiorRightSlider =
    new Fl_Value_Slider(X+m_WW+C, Y+m_HH, m_WW, N);
  o->type(5);
  o->box(FL_DOWN_FRAME);
  o->maximum(3);
  o->step(1);
  o->callback((Fl_Callback*)cb_SuperiorRightSlider);
  o->deactivate();
  }

  m_wl.current_x = 0;
  m_wl.current_y = 0;
  m_wl.prev_x = 0;
  m_wl.prev_y = 0;

  end();
}

UltrasoundNavigatorQuadrantViews::~UltrasoundNavigatorQuadrantViews()
{
  std::cout << " UltrasoundNavigatorQuadrantViews destructor " << std::endl;
  delete m_CTWidget1;
  delete m_CTWidget2;
  delete m_VideoWidget;
  delete m_3DWidget;
}

void UltrasoundNavigatorQuadrantViews::RequestResliceImage(void)
{
  // callback function for slider bars, to invoke the ReslicingEvent
  IndexType index;

//  index[2]  = ( int ) m_SuperiorLeftSlider->value();
//  index[0]  = ( int ) m_SuperiorRightSlider->value();
//  index[1]  = ( int ) m_InferiorLeftSlider->value();

  // todo: fix it so that when the user sets the uppper right view to e.g.
  // sagittal, the slider updates the correct index
  index[0] = 0;
  index[1] = 0;
  index[2] = ( int ) m_SuperiorRightSlider->value();

  ManualReslicingEvent reslicingEvent;
  reslicingEvent.Set( index );
  m_Reporter->InvokeEvent( reslicingEvent );

}

void UltrasoundNavigatorQuadrantViews::cb_SuperiorRightSlider_i(Fl_Value_Slider*, void*)
{
  this->redraw();
  this->RequestResliceImage();
}

void UltrasoundNavigatorQuadrantViews::cb_SuperiorRightSlider(Fl_Value_Slider* o, void* v)
{
  ((UltrasoundNavigatorQuadrantViews*)(o->parent()))->cb_SuperiorRightSlider_i(o,v);
}

unsigned long UltrasoundNavigatorQuadrantViews::AddObserver(
  const ::itk::EventObject & event, ::itk::Command * observer )
{
  return m_Reporter->AddObserver( event, observer );
}

void UltrasoundNavigatorQuadrantViews::RemoveObserver( unsigned long tag )
{
  m_Reporter->RemoveObserver( tag );
}

void UltrasoundNavigatorQuadrantViews::RemoveAllObservers()
{
  m_Reporter->RemoveAllObservers();
}

int UltrasoundNavigatorQuadrantViews::handle(int event)
{
  switch (event) 
  {      
        case FL_KEYUP:
          KeyboardCommandType keyCommand;
          keyCommand.key = Fl::event_key();
          keyCommand.state = Fl::event_state();
          return handle_key(event,keyCommand);

        case FL_PUSH:  
          if ( (Fl::event_button() == FL_RIGHT_MOUSE) && (Fl::event_state() == FL_SHIFT + FL_BUTTON3) ) 
          {
            m_wl.current_x = Fl::event_x();
            m_wl.current_y = Fl::event_y();
            return 1;
          }
          return Fl_Group::handle(event);

        case FL_DRAG:
          if (Fl::event_state() == FL_SHIFT + FL_BUTTON3) 
          {            
            MouseCommandType mouseCommand;
            mouseCommand.button = Fl::event_button();
            mouseCommand.state = Fl::event_state();            
            m_wl.prev_x = m_wl.current_x;
            m_wl.prev_y = m_wl.current_y;
            m_wl.current_x = Fl::event_x();
            m_wl.current_y = Fl::event_y();
            mouseCommand.x = Fl::event_x();
            mouseCommand.y = Fl::event_y();

            mouseCommand.dx = m_wl.current_x - m_wl.prev_x;
            mouseCommand.dy = m_wl.current_y - m_wl.prev_y;

            return handle_mouse(event, mouseCommand);
          }
          else
            return Fl_Group::handle(event);

        default: 
          return Fl_Group::handle(event);
  };
}

int UltrasoundNavigatorQuadrantViews::handle_key(int event, KeyboardCommandType &keyCommand) 
{
    KeyPressedEvent kpEvent;
    kpEvent.Set( keyCommand );
    m_Reporter->InvokeEvent( kpEvent );
    damage(1); 
    return 1;
}

int UltrasoundNavigatorQuadrantViews::handle_mouse(int event, MouseCommandType &mouseCommand) 
{
    MousePressedEvent mpEvent;
    mpEvent.Set( mouseCommand );
    m_Reporter->InvokeEvent( mpEvent );
    damage(1); 
    return 1;
}

} // end namespace igstk
