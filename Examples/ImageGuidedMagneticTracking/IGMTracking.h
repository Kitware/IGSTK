#ifndef __ISIS_IGMTracking__h__
#define __ISIS_IGMTracking__h__


#include "IGMTrackingGUI.h"
#include "IGMTImageSliceViewer.h"
//#include "VolumeViewer.h"
#include "AuroraTracker.h"
#include "MotionViewer.h"
#include <process.h>

class vtkImageShiftScale;


class IGMTracking : public IGMTrackingGUI
{
public:
	bool m_ForceRecord;
	void RecordReference();
	
	IGMTracking( void );
	
	virtual ~IGMTracking( void );
	
	virtual void Show( void );
	
	virtual void Hide( void );
	
	virtual void Quit( void );
	
	virtual void Load( void );
	
	virtual void LoadDICOM( void );
	
	virtual void LoadPostProcessing( void );
	
	virtual void SelectAxialSlice( int );
	
	virtual void SelectCoronalSlice( int );
	
	virtual void SelectSaggitalSlice( int );
	
	virtual void ProcessAxialViewInteraction( void );
	
	virtual void ProcessCoronalViewInteraction( void );
	
	virtual void ProcessSaggitalViewInteraction( void );
	
	virtual void SyncAllViews( const float aboutPoint[3] );

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

	virtual void OnMotionTracking( void );

	virtual float GetImageScale( void );
	
	virtual void SetImageScale( float val );
	
	virtual float GetImageShift( void );
	
	virtual void SetImageShift( float val );
	
	virtual void SetNeedleLength( const float val );
	
	virtual void SetNeedleRadius( const float val );
	
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

private:

	ISIS::IGMTImageSliceViewer  m_AxialViewer;
	
	ISIS::IGMTImageSliceViewer  m_CoronalViewer;
	
	ISIS::IGMTImageSliceViewer  m_SaggitalViewer;
	
//	ISIS::VolumeViewer			m_VolumeViewer;

	ISIS::MotionViewer			m_MotionViewer;
	
	vtkImageShiftScale    * m_ShiftScaleImageFilter;
	
	itk::SimpleMemberCommand<IGMTracking>::Pointer      m_AxialViewerCommand;
	
	itk::SimpleMemberCommand<IGMTracking>::Pointer      m_CoronalViewerCommand;
	
	itk::SimpleMemberCommand<IGMTracking>::Pointer      m_SaggitalViewerCommand;
	
	itk::SimpleMemberCommand<IGMTracking>::Pointer      m_DicomReaderCommand;

	AuroraTracker	m_AuroraTracker;

	int				m_ToolHandle;

	int				m_ReferenceToolHandle;

	float			m_ClickedPoint[3]; 

	bool			m_ShowVolume;

	bool			m_EntryPointSelected, m_TargetPointSelected;
};


#endif
