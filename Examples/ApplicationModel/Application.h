#ifndef __ISIS_Application__h__
#define __ISIS_Application__h__


#include "ApplicationGUI.h"
#include "ImageSliceViewer.h"

class vtkImageShiftScale;


class Application : public ApplicationGUI
{
public:
   Application();
   virtual ~Application();
  
   virtual void Show();
   virtual void Hide();
   virtual void Quit();
   virtual void Load();
   virtual void LoadDICOM();
   virtual void LoadPostProcessing();
   virtual void SelectAxialSlice( int );
   virtual void SelectCoronalSlice( int );
   virtual void SelectSagittalSlice( int );

   virtual void ProcessAxialViewInteraction( void );
   virtual void ProcessCoronalViewInteraction( void );
   virtual void ProcessSagittalViewInteraction( void );
   virtual void SyncAllViews(void);

   virtual void ProcessDicomReaderInteraction( void );

private:

  ISIS::ImageSliceViewer  m_AxialViewer;
  ISIS::ImageSliceViewer  m_CoronalViewer;
  ISIS::ImageSliceViewer  m_SagittalViewer;

  vtkImageShiftScale    * m_ShiftScaleImageFilter;

  itk::SimpleMemberCommand<Application>::Pointer      m_AxialViewerCommand;
  itk::SimpleMemberCommand<Application>::Pointer      m_CoronalViewerCommand;
  itk::SimpleMemberCommand<Application>::Pointer      m_SagittalViewerCommand;

  itk::SimpleMemberCommand<Application>::Pointer      m_DicomReaderCommand;

};


#endif
