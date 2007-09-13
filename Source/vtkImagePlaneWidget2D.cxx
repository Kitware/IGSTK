#include "vtkImagePlaneWidget2D.h"

#include <vtkCallbackCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkObjectFactory.h>
#include <vtkCoordinate.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>

#include <assert.h>

#include "vtkImagePlaneWidget2DInteractionListener.h"

vtkStandardNewMacro(vtkImagePlaneWidget2D);

vtkImagePlaneWidget2D::vtkImagePlaneWidget2D() : vtkImagePlaneWidget(),
                                                 mIsPanning(false)
{
  this->ActivateMargins(0);
  this->EventCallbackCommand->SetCallback(vtkImagePlaneWidget2D::ProcessEvents);
}

vtkImagePlaneWidget2D::~vtkImagePlaneWidget2D()
{}

// used to update this view according to other views
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
  mLastXPosition = displayPosition[0];
  mLastYPosition = displayPosition[1];
}

// Makes cursor permanently visible.
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


void vtkImagePlaneWidget2D::ProcessEvents(vtkObject *object,
                                          unsigned long event,
                                          void *clientData,
                                          void *callData)
{
  vtkImagePlaneWidget2D * self = reinterpret_cast<vtkImagePlaneWidget2D *>(clientData);

  self->LastButtonPressed = vtkImagePlaneWidget::NO_BUTTON;

  switch ( event )
  {
  case vtkCommand::LeftButtonPressEvent:
    self->LastButtonPressed = vtkImagePlaneWidget::LEFT_BUTTON;
    self->OnLeftButtonDown();
    break;
  case vtkCommand::LeftButtonReleaseEvent:
    self->LastButtonPressed = vtkImagePlaneWidget::LEFT_BUTTON;
    self->OnLeftButtonUp();
    self->mLastXPosition = self->Interactor->GetEventPosition()[0];
    self->mLastYPosition = self->Interactor->GetEventPosition()[1];
    break;
  case vtkCommand::MiddleButtonPressEvent:
    self->LastButtonPressed = vtkImagePlaneWidget::MIDDLE_BUTTON;
    self->OnMiddleButtonDown();
    break;
  case vtkCommand::MiddleButtonReleaseEvent:
    self->LastButtonPressed = vtkImagePlaneWidget::MIDDLE_BUTTON;
    self->OnMiddleButtonUp();
    break;
  case vtkCommand::RightButtonPressEvent:
    self->LastButtonPressed = vtkImagePlaneWidget::RIGHT_BUTTON;
    self->OnRightButtonDown();
    break;
  case vtkCommand::RightButtonReleaseEvent:
    self->LastButtonPressed = vtkImagePlaneWidget::RIGHT_BUTTON;
    self->OnRightButtonUp();
    break;
  case vtkCommand::MouseMoveEvent:
    self->OnMouseMove();
    break;
//   case vtkCommand::MouseWheelForwardEvent:
//     self->OnMouseWheelForward();
//     break;
//   case vtkCommand::MouseWheelBackwardEvent:
//     break;
  }
}

void vtkImagePlaneWidget2D::OnMouseMove()
{
  if ( this->State == vtkImagePlaneWidget::Outside ||
       this->State == vtkImagePlaneWidget::Start )
  {
    return;
  }

  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];
  
  if(this->State == vtkImagePlaneWidget::WindowLevelling)
  {
    this->WindowLevel(X, Y);
    this->ManageTextDisplay();
  }
  else if(this->State == vtkImagePlaneWidget::Cursoring)
  {
    this->UpdateCursor(X, Y);
    this->ManageTextDisplay();
  }
  else if(mIsPanning)
  {
    PerformPanning();
  }

  this->EventCallbackCommand->SetAbortFlag(1);
  this->InvokeEvent(vtkCommand::InteractionEvent, 0);
}

void vtkImagePlaneWidget2D::OnLeftButtonDown()
{
  this->StartCursor();
}

void vtkImagePlaneWidget2D::OnLeftButtonUp()
{
  this->StopCursor();
}
void vtkImagePlaneWidget2D::OnMiddleButtonDown()
{
  mIsPanning = true;
}

void vtkImagePlaneWidget2D::OnMiddleButtonUp()
{
  mIsPanning = false;
}

void vtkImagePlaneWidget2D::OnRightButtonDown()
{
  this->StartWindowLevel();
}

void vtkImagePlaneWidget2D::OnRightButtonUp()
{
  this->StopWindowLevel();
}
void vtkImagePlaneWidget2D::PerformPanning()
{
  // Convert last leftclick point to world coordinates
  if(!this->CurrentRenderer)
    std::cout << "No renderer in imageplane" << std::endl;
  assert(this->CurrentRenderer);
  
  // Get zoom point world coordinates
  this->CurrentRenderer->SetDisplayPoint(mLastXPosition, mLastYPosition, 0.0);
  this->CurrentRenderer->DisplayToWorld();
  double zoomPointWorld[4];
  this->CurrentRenderer->GetWorldPoint(zoomPointWorld);

  double cameraXPos, cameraYPos, cameraZPos;

  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();

  double oldCameraPos[3];
  camera->GetPosition(oldCameraPos);

  double projectionVector[3];
  camera->GetDirectionOfProjection(projectionVector);
  projectionVector[0] = abs(projectionVector[0]);
  projectionVector[1] = abs(projectionVector[1]);
  projectionVector[2] = abs(projectionVector[2]);

  // According to camera orientation, move camera in the appropriate plane
  if( (projectionVector[0] > projectionVector[1]) && 
      (projectionVector[0] > projectionVector[2]) )
  {
    // Projection along x-axis --> move camera in y-z plane
    cameraXPos = oldCameraPos[0];
    cameraYPos = zoomPointWorld[1];
    cameraZPos = zoomPointWorld[2];
  }

  else if( (projectionVector[1] > projectionVector[0]) &&
           (projectionVector[1] > projectionVector[2]) )
  {
    // projection along y-axis --> move camera in x-z plane
    cameraXPos = zoomPointWorld[0];
    cameraYPos = oldCameraPos[1];
    cameraZPos = zoomPointWorld[2];
  }
  else if( (projectionVector[2] > projectionVector[0]) &&
           (projectionVector[2] > projectionVector[1]) )
  {
    // Projection along z-axis -> move camera in x-y plane
    cameraXPos = zoomPointWorld[0];
    cameraYPos = zoomPointWorld[1];
    cameraZPos = oldCameraPos[2];
  }
  camera->SetPosition(cameraXPos, cameraYPos, cameraZPos);
  camera->SetFocalPoint(zoomPointWorld);

  this->Interactor->Render();
}

int vtkImagePlaneWidget2D::GetState() const
{
  switch(this->State)
  {
  case Cursoring:
    return CURSORING;
  case WindowLevelling:
    return WINDOWLEVELLING;
  case Pushing:
    return PUSHING;
  default:
    return NONE;
  }
}

void vtkImagePlaneWidget2D::
ConnectWindowLevel(vtkImagePlaneWidget2D *imagePlaneWidget)
{
  vtkImagePlaneWidget2DInteractionListener *observer = 
    vtkImagePlaneWidget2DInteractionListener::New();

  observer->SubscribeToWindowLevel(imagePlaneWidget);
  this->AddObserver(vtkCommand::InteractionEvent, observer);
  observer->Delete();
}
