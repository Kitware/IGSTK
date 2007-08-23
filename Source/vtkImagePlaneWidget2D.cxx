#include "vtkImagePlaneWidget2D.h"

#include <vtkCallbackCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkObjectFactory.h>
#include <vtkCoordinate.h>
#include <vtkRenderer.h>

vtkStandardNewMacro(vtkImagePlaneWidget2D);

vtkImagePlaneWidget2D::vtkImagePlaneWidget2D() : vtkImagePlaneWidget()
{}

vtkImagePlaneWidget2D::~vtkImagePlaneWidget2D()
{}

void vtkImagePlaneWidget2D::SetCursorPosition(double x, double y, double z)
{
    // 1. Convert from World to Display
    vtkCoordinate *positionCoordinate = vtkCoordinate::New();
    positionCoordinate->SetCoordinateSystemToWorld();
    positionCoordinate->SetValue(x, y, z);
    int *displayPosition = 
        positionCoordinate->GetComputedDisplayValue(this->CurrentRenderer);
    positionCoordinate->Delete();

    // 2. Update cursor
    this->UpdateCursor(displayPosition[0], displayPosition[1]);
}

void vtkImagePlaneWidget2D::StopCursor()
{
     if ( this->State == vtkImagePlaneWidget::Outside ||
          this->State == vtkImagePlaneWidget::Start )
     {
         return;
     }
    
     this->State = vtkImagePlaneWidget::Start;
     this->HighlightPlane(0);
    
     this->EventCallbackCommand->SetAbortFlag(1);
     this->EndInteraction();
     this->InvokeEvent(vtkCommand::EndInteractionEvent,0);
     this->Interactor->Render();
}


