#ifndef __ISIS_IGMTracking__h__
#define __ISIS_IGMTracking__h__


#include "IGMTrackingGUI.h"
#include "AuroraTracker.h"
#include "IGMTImageSliceViewer.h"
#include "MotionViewer.h"
#include "IGMTVolumeViewer.h"
#include "IGMTTargetViewer.h"
#include "FantasticRegistration.h"
#include "SkeletonModule.h"
#include <process.h>

#include "itkCommand.h"
#include "vtkCallbackCommand.h"

#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

class vtkImageShiftScale;

#define PORTNUM 4

class IGMTracking : public IGMTrackingGUI
{

public:

  typedef itk::BinaryThresholdImageFilter<VolumeType, VolumeType> InvertFilterType;
  typedef itk::DanielssonDistanceMapImageFilter<VolumeType, VolumeType> DistanceFilterType;

  InvertFilterType::Pointer m_InvertFilter;
  DistanceFilterType::Pointer m_DistanceMapFilter;

public:
	void OnSkeleton();
	void OnUpdateRegParameters();

	void OnSaveSegmentationData();
	void OnUpdateSegParameters();
	void RenderAllWindow();
	void SetInputData(int type);
	void OnSegmentation();
	static void MCProgressFunc(void* arg);
  static void DecProgressFunc(void* arg);
  static void SmoothProgressFunc(void* arg);

	void SaveSnapshot();
	void RenderSectionWindow();
	void OnUpdateOpacity(double opa);

  typedef SkeletonModule<VolumeType, VolumeType> SkeletonModuleType;

  SkeletonModuleType m_Skeleton;

	int m_VolumeType;
  int m_Pipeline;
	void OnFusionImage();

  int m_Probe[PORTNUM], m_Register[PORTNUM], m_Ref[PORTNUM], m_UID[PORTNUM];
  int m_ProbeID, m_RefID, m_RegisterID;
  int m_FullSizeIdx;

//  double m_FusionOpacity;

  double m_Offset[PORTNUM], m_Length[PORTNUM], m_Radius[PORTNUM];

 	void OnUpdateProbeParameters();
	void OnUpdateLayout();
	void OnLayout();
	void SetVolumeThreshold(int thres);
//	void SaveAsRaw();
	bool m_ForceRecord;
	void RecordReference();
	
	IGMTracking( void );
	
	virtual ~IGMTracking( void );

	void ProcessDicomReading();

  void ProcessConnectedFilter();

  void ProcessConnectedFilterStart();

  void ProcessConnectedFilterEnd();

  void ProcessResampleImagePreFilter();

  void ProcessResampleImagePreFilterStart();

  void ProcessResampleImagePreFilterEnd();

  void ProcessResampleImagePostFilter();

  void ProcessResampleImagePostFilterStart();

  void ProcessResampleImagePostFilterEnd();

  void ProcessDistanceMapFilter();
	
	virtual void Show( void );
	
	virtual void Hide( void );
	
	virtual void Quit( void );
	
	virtual void Load( void );
	
	virtual void LoadDICOM( void );
	
	virtual void LoadPostProcessing( void );

	virtual void FusionPostProcessing( void );
	
	virtual void SelectAxialSlice( int );
	
	virtual void SelectCoronalSlice( int );
	
	virtual void SelectSagittalSlice( int );
	
	virtual void ProcessAxialViewInteraction( void );

  virtual void ProcessCoronalViewInteraction( void );
	
	virtual void ProcessSagittalViewInteraction( void );
	
	virtual void SyncAllViews( const double aboutPoint[3] );

  int TrackingPoint( const double aboutPoint[3], bool showpos = true );

	virtual void ProcessDicomReaderInteraction( void );
	
	virtual void SetNumberOfFiducials( unsigned int number );
	
	virtual void SetImageFiducial( unsigned int fiducialNumber );

	virtual void InitializeTracker( const char *fileName = NULL );

	virtual void ActivateTools( void );

	virtual void StartTracking( void );

	virtual void StopTracking( void );

	virtual void PrintToolPosition( const int toolHandle ); // basically should be in AuroraTracker
	
	virtual void SetTrackerFiducial( unsigned int fiducialNumber ); // basically should call AuroraTracker

	virtual void OnTargetPoint( void ); 
	
	virtual void OnEntryPoint( void );

	virtual bool OnRegister( void );

	virtual void OnOverlay( void );

	virtual double GetImageScale( void );
	
	virtual void SetImageScale( double val );
	
	virtual double GetImageShift( void );
	
	virtual void SetImageShift( double val );
	
	virtual void SetNeedleLength( const double val );
	
  virtual void SetProbeLength( int i, const float val );
	
	virtual void SetProbeRadius( int i, const float val );

  virtual void SetNeedleRadius( const double val );
	
	virtual void DisplaySelectedPosition( const bool show);

	virtual void DisplayTargetPosition( const bool show);

	virtual void DisplayEntryPosition( const bool show);

	virtual void DisplayToolPosition( const bool show);

	virtual void DisplayEntryToTargetPath( const bool show);

	virtual void PrintDebugInfo( void );

//protected:
	
	virtual void OnToolPosition( void ); 
	
	virtual void OnTrackToolPosition( void ); 
	
	bool		m_Overlay;							// remove this later with a state variable

  static void  ProcessAxialViewMouseMoveInteraction(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);

  static void  ProcessAxialViewRightClickInteraction(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);

  static void  ProcessCoronalViewMouseMoveInteraction(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);

  static void  ProcessCoronalViewRightClickInteraction(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);

  static void  ProcessSagittalViewMouseMoveInteraction(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);

  static void  ProcessSagittalViewRightClickInteraction(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);

private:

  int m_WindowType;

  int m_RenderingMethod;

  int m_RenderingCategory;

  int m_DesiredTriNum;

	ISIS::IGMTImageSliceViewer  m_AxialViewer;
	
	ISIS::IGMTImageSliceViewer  m_CoronalViewer;
	
	ISIS::IGMTImageSliceViewer  m_SagittalViewer;
	
	ISIS::MotionViewer			m_MotionViewer;

  IGSTK::IGMTVolumeViewer     m_VolumeViewer;

  IGSTK::IGMTTargetViewer     m_TargetViewer;
	
	vtkImageShiftScale    * m_ShiftScaleImageFilter;

  vtkImageShiftScale    * m_FusionShiftScaleImageFilter;

  vtkImageBlend         * m_ImageBlend;
	
	itk::SimpleMemberCommand<IGMTracking>::Pointer      m_AxialViewerCommand;

  vtkCallbackCommand*      m_AxialViewerMouseMoveCommand;

  vtkCallbackCommand*      m_AxialViewerRightClickCommand;

  itk::SimpleMemberCommand<IGMTracking>::Pointer      m_CoronalViewerCommand;

  vtkCallbackCommand*      m_CoronalViewerMouseMoveCommand;

  vtkCallbackCommand*      m_CoronalViewerRightClickCommand;
	
	itk::SimpleMemberCommand<IGMTracking>::Pointer      m_SagittalViewerCommand;

  vtkCallbackCommand*      m_SagittalViewerMouseMoveCommand;

  vtkCallbackCommand*      m_SagittalViewerRightClickCommand;
	
	itk::SimpleMemberCommand<IGMTracking>::Pointer      m_DicomReaderCommand;

	itk::SimpleMemberCommand<IGMTracking>::Pointer		  m_ProgressCommand;

  itk::SimpleMemberCommand<IGMTracking>::Pointer		  m_ConnectedFilterProgressCommand;

  itk::SimpleMemberCommand<IGMTracking>::Pointer		  m_ConnectedFilterStartCommand;

  itk::SimpleMemberCommand<IGMTracking>::Pointer		  m_ConnectedFilterEndCommand;

  itk::SimpleMemberCommand<IGMTracking>::Pointer		  m_ResampleImagePreProgressCommand;

  itk::SimpleMemberCommand<IGMTracking>::Pointer		  m_ResampleImagePreStartCommand;

  itk::SimpleMemberCommand<IGMTracking>::Pointer		  m_ResampleImagePreEndCommand;

  itk::SimpleMemberCommand<IGMTracking>::Pointer		  m_ResampleImagePostProgressCommand;

  itk::SimpleMemberCommand<IGMTracking>::Pointer		  m_ResampleImagePostStartCommand;

  itk::SimpleMemberCommand<IGMTracking>::Pointer		  m_ResampleImagePostEndCommand;

  itk::SimpleMemberCommand<IGMTracking>::Pointer		  m_DistanceMapProgressCommand;

	AuroraTracker	m_AuroraTracker;

  IGSTK::FantasticRegistration m_FantasticRegistration;

	int				m_ToolHandle, m_RegToolHandle, m_RegToolHandle1, m_RegToolHandle2;

	int				m_ReferenceToolHandle;

	double    m_ClickedPoint[3], m_RightClickedPoint[3]; 

  int       m_PointIndex[3];

	bool			m_ShowVolume;

	bool			m_EntryPointSelected, m_TargetPointSelected;

  double    m_CoilOffset1[3], m_CoilOffset2[3];

  double    m_CoilPosition[4][3];
public:

  static void ProcessSkeleton(void* calldata, double);

};


#endif
