#ifndef __ISIS_IGMTImageSliceViewer_h__
#define __ISIS_IGMTImageSliceViewer_h__

#include "ImageSliceViewer.h"
#include "Sphere.h"
#include "TubeWrappedLine.h"
//#include "Line.h"

namespace ISIS
{
	
	class IGMTImageSliceViewer : public ImageSliceViewer
	{
	public:
		void MakeToolPositionMarkerVisible();
		
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

		virtual void ActivateEntryPosition( void );

		virtual void DeActivateEntryPosition( void );

		virtual void ActivateEntryToTargetPathMarker( void );

		virtual void DeActivateEntryToTargetPathMarker( void );

		virtual void ActivateToolPosition( void );

		virtual void DeActivateToolPosition( void );

	public:
		
		Sphere		m_SelectedPositionMarker;

		Sphere		m_TargetPositionMarker;

		Sphere		m_EntryPositionMarker;

		TubeWrappedLine		m_ToolPositionMarker;

		TubeWrappedLine		m_EntryToTargetPathMarker;

		//Line		m_ToolPositionMarker;

		//Line		m_EntryToTargetPathMarker;
	};
	
	
}  // end namespace ISIS

#endif


