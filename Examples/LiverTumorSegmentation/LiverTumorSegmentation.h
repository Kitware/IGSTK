#ifndef __ISIS_LiverTumorSegmentation__h__
#define __ISIS_LiverTumorSegmentation__h__


#include "LiverTumorSegmentationGUI.h"
#include "ImageSliceViewer01.h"
#include "VolumeViewer.h"

class vtkImageShiftScale;

class vtkImageBlend;

//static char *Volumes[] = { "Input Image", "Segmented Image", "Threshold Image" }; 

//typedef enum { INPUT_VOLUME=1, SEGMENTED_VOLUME, THRESHOLDED_VOLUME } DisplayVolumeType;

class LiverTumorSegmentation : public LiverTumorSegmentationGUI
{
public:

   LiverTumorSegmentation();

   virtual ~LiverTumorSegmentation();
  
   virtual void Show();
   virtual void Hide();
   virtual void Quit();
   virtual void Load();
   virtual void LoadDICOM();
   virtual void LoadPostProcessing();
   virtual void SelectAxialSlice( int );
   virtual void SelectCoronalSlice( int );
   virtual void SelectSaggitalSlice( int );

   virtual void ProcessAxialViewInteraction( void );
   virtual void ProcessCoronalViewInteraction( void );
   virtual void ProcessSagittalViewInteraction( void );
   virtual void SyncAllViews(void);

   virtual void ProcessDicomReaderInteraction( void );

//   virtual void OnImageControl( void );
//   virtual void OnImageControlOk( void );

   virtual void OnOpacityControl( double opacity );

   virtual void OnSegmentation( void );

//   virtual void OnThreshold( const double lower, const double upper );

   virtual void OnSegmentationParameters( void );

   virtual void OnSegmentationModuleSelection( int module );

   virtual void OnSegmentationParametersOk( int module );

   virtual void SetSeedPoint( double x, double y, double z );

   virtual double GetImageScale( void );
   
   virtual void SetImageScale( double val );
	
   virtual double GetImageShift( void );
	
   virtual void SetImageShift( double val );

   virtual double GetSegmentedVolumeOpacity( void );

   virtual bool SetSegmentedVolumeOpacity( const double value );

//   virtual DisplayVolumeType  GetBackgroundVolumeType( void );

//   virtual DisplayVolumeType  GetOverlayVolumeType( void );

//   virtual void  SetBackgroundVolumeType( DisplayVolumeType type );

//   virtual void  SetOverlayVolumeType( DisplayVolumeType type );

   virtual void LoadSegmentedVolume( void );

   virtual void DoDebug( void );

private:

  ISIS::ImageSliceViewer01  m_AxialViewer;
  ISIS::ImageSliceViewer01  m_CoronalViewer;
  ISIS::ImageSliceViewer01  m_SagittalViewer;

  vtkImageShiftScale    * m_ShiftScaleImageFilter;

  itk::SimpleMemberCommand<LiverTumorSegmentation>::Pointer      m_AxialViewerCommand;
  itk::SimpleMemberCommand<LiverTumorSegmentation>::Pointer      m_CoronalViewerCommand;
  itk::SimpleMemberCommand<LiverTumorSegmentation>::Pointer      m_SagittalViewerCommand;

  itk::SimpleMemberCommand<LiverTumorSegmentation>::Pointer      m_DicomReaderCommand;


  vtkImageBlend        * m_vtkImageBlender;

  // stores the array index in Volumes[] of the displayed background_volume and overlay_volume.
//  DisplayVolumeType		m_BackgroundVolume, m_OverlayVolume;

  double				        m_SegmentedVolumeOpacity;

  bool					      m_ShowVolumeView;

  ISIS::VolumeViewer	m_VolumeViewer;

  SegmentationModuleType    m_ModuleType;

};


#endif
