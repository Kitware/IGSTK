#ifndef __ISIS_IGMTImageSliceViewer_h__
#define __ISIS_IGMTImageSliceViewer_h__

#include "ImageSliceViewer.h"
#include "Sphere.h"
#include "TubeWrappedLine.h"
#include "Annotation.h"
#include "vtkTextMapper.h"
#include "vtkActor2D.h"
#include "vtkLineSource.h"
#include "vtkPolyDataMapper2D.h"

namespace ISIS
{
	
	class IGMTImageSliceViewer : public ImageSliceViewer
	{
	public:
		void MakeToolPositionMarkerVisible();

    void MakeEntryToTargetPathVisible();
		
		IGMTImageSliceViewer();
		
		~IGMTImageSliceViewer();

		void SetInteractor( vtkRenderWindowInteractor * interactor );
		
		virtual void SetOrientation( OrientationType orientation );

		virtual void SelectPoint( int x, int y );
		
		virtual void SelectPoint( float x, float y, float z );

		virtual void ActivateSelectedPosition( void );

		virtual void DeActivateSelectedPosition( void );

		virtual void ActivateTargetPosition( void );

		virtual void DeActivateTargetPosition( void );

// 		virtual void ActivateTipPosition( void );

//		virtual void DeActivateTipPosition( void );

		virtual void ActivateEntryPosition( void );

		virtual void DeActivateEntryPosition( void );

		virtual void ActivateEntryToTargetPathMarker( void );

		virtual void DeActivateEntryToTargetPathMarker( void );

//		virtual void ActivateToolPosition( void );

//		virtual void DeActivateToolPosition( void );

    virtual void ActivateProbe( int dev );

		virtual void DeActivateProbe( int dev );

	public:
		bool m_RightClickedPointValid;
		void GetRightClickedPoint(double point[3]);
		void SelectRightPoint(int x, int y);
		void GetImagePosition(int x, int y, double* pos);
//		void UpdateTipRadius();
		void ShowAnnotation(int s);
		void SetProbeTipAndDirection(int i, double tip[3], double end[3] );
		void SetupCamera();
		
		Sphere		m_SelectedPositionMarker;

		Sphere		m_TargetPositionMarker;

		Sphere		m_EntryPositionMarker;

//    Sphere    m_TipPositionMarker;

//		TubeWrappedLine		m_ToolPositionMarker;

    TubeWrappedLine		m_ProbeMarker[4];

		TubeWrappedLine		m_EntryToTargetPathMarker;

		vtkTextMapper*	m_LabelMapper;
		
		vtkActor2D*		m_LabelActor;

    IGSTK::Annotation *m_pAnnotation;

    float m_RightClickedPoint[3];
 
	};
	
	
}  // end namespace ISIS

#endif


