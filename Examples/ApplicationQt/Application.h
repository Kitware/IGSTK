
#ifndef _Application_H_
#define _Application_H_

#include "applicationgui.h"
#include "applicationbase.h"

#include "ImageSliceViewer.h"
#include "IGMTVolumeViewer.h"

class Application : public ApplicationGUI, public ApplicationBase  
{
public:

  void OnAxialSlider(int i);

  void OnCoronalSlider(int i);

  void OnSagittalSlider(int i);
	
  void OnRender();

	void OnLoad();

	Application();
	
  virtual ~Application();

public:

  ISIS::ImageSliceViewer*  m_pAxialViewer;

  ISIS::ImageSliceViewer*  m_pCoronalViewer;

  ISIS::ImageSliceViewer*  m_pSagittalViewer;

  IGSTK::IGMTVolumeViewer*  m_pVolumeViewer;

};

#endif
