// generated by Fast Light User Interface Designer (fluid) version 1.0107

#include "igstkQuadrantViews.h"

namespace igstk
{

QuadrantViews::QuadrantViews(int X, int Y, int W, int H, const char *L)
  : Fl_Group(X, Y, W, H, L) 
{

  m_Reporter = ::itk::Object::New();
  
  const int N = 15;    // Height of the slider bar
  const int WW = (int) (W-N)/2;
  const int HH = (int) (H- (N*2) )/2;

  // Create widgets
  Displays.clear();
  DisplayAxialWidget    = new igstk::FLTKWidget(X, Y, WW, HH, "Display 0");
  Displays.push_back( DisplayAxialWidget);    // Displays[0]
  DisplaySagittalWidget = new igstk::FLTKWidget(X+WW+N, Y, WW, HH, "Display 1");
  Displays.push_back( DisplaySagittalWidget); // Displays[1]
  DisplayCoronalWidget  = new igstk::FLTKWidget(X, Y+HH+N, WW, HH, "Display 2");
  Displays.push_back( DisplayCoronalWidget);  // Displays[2]
  Display3DWidget       = new igstk::FLTKWidget(X+WW+N, Y+HH+N, WW, HH, "Display 3");
  Displays.push_back( Display3DWidget);       // Displays[3]

  // Create views
  Views.clear();
  ViewType::Pointer v;
  v = DisplayAxial    = ViewType2D::New();
  DisplayAxial->RequestSetOrientation( View2D::Axial );
  Views.push_back( v );    // Views[0];
  v = DisplaySagittal = ViewType2D::New();
  DisplaySagittal->RequestSetOrientation( View2D::Sagittal);
  Views.push_back( v );    // Views[1];
  v = DisplayCoronal  = ViewType2D::New();
  DisplayCoronal->RequestSetOrientation( View2D::Coronal );
  Views.push_back( v );    // Views[2];
  v = Display3D       = ViewType3D::New();
  Views.push_back( v );    // Views[3];

  // Connect views to widgets
  for ( int i=0; i<4; i++)
  {
    Displays[i]->RequestSetView( Views[i] );
  }
  

  // Create slider bars
  Sliders.clear();
{ Fl_Value_Slider* o = AxialSlider = new Fl_Value_Slider(X, Y+HH, WW, N);
  o->type(5);
  o->box(FL_DOWN_FRAME);
  o->maximum(3);
  o->step(1);
  o->callback((Fl_Callback*)cb_AxialSlider);
  o->deactivate();
}
  Sliders.push_back( AxialSlider );
{ Fl_Value_Slider* o = SagittalSlider = new Fl_Value_Slider(X+WW+N, Y+HH, WW, N);
  o->type(5);
  o->box(FL_DOWN_FRAME);
  o->maximum(3);
  o->step(1);
  o->callback((Fl_Callback*)cb_SagittalSlider);
  o->deactivate();
}
  Sliders.push_back( SagittalSlider );
{ Fl_Value_Slider* o = CoronalSlider = new Fl_Value_Slider(X, HH*2+N+Y, WW, N);
  o->type(5);
  o->box(FL_DOWN_FRAME);
  o->maximum(3);
  o->step(1);
  o->callback((Fl_Callback*)cb_CoronalSlider);
  o->deactivate();
}
  Sliders.push_back( CoronalSlider );

end();
}

QuadrantViews::~QuadrantViews()
{
  Displays.clear();
  Views.clear();
  Sliders.clear();

  delete DisplayAxialWidget;
  delete DisplayCoronalWidget;
  delete DisplaySagittalWidget;
  delete Display3DWidget;

}
void QuadrantViews::RequestResliceImage(void) 
{
  IndexType index;

  index[2]  = (int ) AxialSlider->value();
  index[0]  = (int ) SagittalSlider->value();
  index[1]  = (int ) CoronalSlider->value();

  ReslicingEvent reslicingEvent;
  reslicingEvent.Set( index );
  m_Reporter->InvokeEvent( reslicingEvent );

}

void QuadrantViews::cb_AxialSlider_i(Fl_Value_Slider*, void*) 
{
  this->redraw();
  this->RequestResliceImage();
}
void QuadrantViews::cb_AxialSlider(Fl_Value_Slider* o, void* v) 
{
  ((QuadrantViews*)(o->parent()))->cb_AxialSlider_i(o,v);
}

void QuadrantViews::cb_SagittalSlider_i(Fl_Value_Slider*, void*) 
{
  this->redraw();
  this->RequestResliceImage();
}
void QuadrantViews::cb_SagittalSlider(Fl_Value_Slider* o, void* v) 
{
  ((QuadrantViews*)(o->parent()))->cb_SagittalSlider_i(o,v);
}

void QuadrantViews::cb_CoronalSlider_i(Fl_Value_Slider*, void*) 
{
  this->redraw();
  this->RequestResliceImage();
}
void QuadrantViews::cb_CoronalSlider(Fl_Value_Slider* o, void* v) 
{
  ((QuadrantViews*)(o->parent()))->cb_CoronalSlider_i(o,v);
}

unsigned long QuadrantViews::AddObserver( const ::itk::EventObject & event, ::itk::Command * observer )
{
  return m_Reporter->AddObserver( event, observer );
}

void QuadrantViews::RemoveObserver( unsigned long tag )
{
  m_Reporter->RemoveObserver( tag );
}

} // end namespace igstk
