#ifndef __ISIS_VolumeViewer_h__
#define __ISIS_VolumeViewer_h__

#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkFlRenderWindowInteractor.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeRayCastIsosurfaceFunction.h"
#include "vtkVolumeRayCastMIPFunction.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkVolumeTextureMapper2D.h"
//#include "vtkVolume.h"
#include "vtkColorTransferFunction.h"
#include "vtkImageData.h"

namespace ISIS
{
	
	class VolumeViewer
	{
	public:
		
		VolumeViewer();
		
		virtual ~VolumeViewer();
		
		void SetInput( vtkImageData * image );
		
		void SetInteractor( vtkRenderWindowInteractor * interactor );
		
		void Render();
		
	private:
		
		vtkRenderer     *m_Renderer;
		
		vtkRenderWindow *m_RenderWindow;
		
		vtkPiecewiseFunction *opacityTransferFunction;
		
//		vtkPiecewiseFunction *colorTransferFunction;
		
		vtkColorTransferFunction *colorTransferFunction;
		
		vtkVolumeProperty *volumeProperty;
		
//		vtkVolumeRayCastIsosurfaceFunction *isoFunction;
		
		vtkVolumeRayCastCompositeFunction *compositeFunction;
		
//		vtkVolumeTextureMapper2D *volumeMapper;
		
		vtkVolumeRayCastMapper *volumeMapper;
		
		vtkVolumeRayCastMIPFunction *mipFunction; 
		
		vtkVolume *volume;
	};
	
	
}  // end namespace ISIS

#endif



