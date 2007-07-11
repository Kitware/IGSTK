/*
 * No license text yet. 
 */
#include "igstkCrosshairs2D.h"

#include <vtkAxisActor2D.h>
#include <vtkCoordinate.h>
#include <vtkViewport.h>
#include <vtkProperty2D.h>
namespace igstk
{

Crosshairs2D::Crosshairs2D() :
    m_StateMachine(this),
    m_Viewport(NULL)
{
    m_HorizontalAxisActor = vtkAxisActor2D::New();
    m_VerticalAxisActor = vtkAxisActor2D::New();

    m_HorizontalAxisActor->TickVisibilityOff();
    m_VerticalAxisActor->TickVisibilityOff();
    m_HorizontalAxisActor->LabelVisibilityOff();
    m_VerticalAxisActor->LabelVisibilityOff();
    igstkAddInputMacro(ValidViewport);
    igstkAddInputMacro(InvalidViewport);
    igstkAddInputMacro(ValidPosition);
    igstkAddInputMacro(InvalidPosition);
    
    igstkAddStateMacro(Idle);
    igstkAddStateMacro(ViewportSet);
    igstkAddStateMacro(PositionSet);

    igstkAddTransitionMacro(Idle, ValidViewport, ViewportSet, SetViewport);
    igstkAddTransitionMacro(Idle, InvalidViewport, Idle, No);

    igstkAddTransitionMacro(ViewportSet, ValidPosition, PositionSet, SetPosition);
    igstkAddTransitionMacro(ViewportSet, InvalidPosition, ViewportSet, No);
    
    igstkAddTransitionMacro(PositionSet, ValidPosition, PositionSet, SetPosition);
    igstkAddTransitionMacro(PositionSet, InvalidPosition, PositionSet, No);

    igstkSetInitialStateMacro(Idle);

    m_StateMachine.SetReadyToRun();
}

Crosshairs2D::~Crosshairs2D()
{
    m_HorizontalAxisActor->Delete();
    m_VerticalAxisActor->Delete();
}


void Crosshairs2D::RequestSetViewport(vtkViewport *viewport)
{
    ///@TODO: Add complete checks for valid viewport?
    if(viewport != NULL)
    {
        m_ViewportToBeSet = viewport;
        igstkPushInputMacro(ValidViewport);
    }
    else
    {
        igstkPushInputMacro(InvalidViewport);
    }
    m_StateMachine.ProcessInputs();
}

void Crosshairs2D::SetViewportProcessing()
{
    m_Viewport = m_ViewportToBeSet;
}

void Crosshairs2D::RequestSetPosition(double x, double y, double z)
{
    ///@TODO: Add checks for valid Points?
    m_XPosition = x;
    m_YPosition = y;
    m_ZPosition = z;
    igstkPushInputMacro(ValidPosition);
    m_StateMachine.ProcessInputs();
}

void Crosshairs2D::SetPositionProcessing()
{
    // Convert from world coordinates to display coordinates
    vtkCoordinate *positionCoordinate = vtkCoordinate::New();
    positionCoordinate->SetCoordinateSystemToWorld();
    positionCoordinate->SetValue(m_XPosition, m_YPosition, m_ZPosition);

    int *displayCoordinates = positionCoordinate->GetComputedDisplayValue(m_Viewport);
    int displayXPosition = displayCoordinates[0];
    int displayYPosition = displayCoordinates[1];
    positionCoordinate->Delete();
    
    // Get viewport size
    int *portSize = m_Viewport->GetSize();
    int viewportXSizeInPixels = portSize[0];
    int viewportYSizeInPixels = portSize[1];

    // Horizontal axis
    m_HorizontalAxisActor->GetPoint1Coordinate()->SetCoordinateSystemToDisplay();
    m_HorizontalAxisActor->GetPoint2Coordinate()->SetCoordinateSystemToDisplay();
    m_HorizontalAxisActor->SetPoint1(0, displayYPosition);
    m_HorizontalAxisActor->SetPoint2(viewportXSizeInPixels, displayYPosition);
    m_HorizontalAxisActor->GetProperty()->SetColor(0.0, 1.0, 0.0);

    // Vertical axis
    m_VerticalAxisActor->GetPoint1Coordinate()->SetCoordinateSystemToDisplay();
    m_VerticalAxisActor->GetPoint2Coordinate()->SetCoordinateSystemToDisplay();
    m_VerticalAxisActor->SetPoint1(displayXPosition, 0);
    m_VerticalAxisActor->SetPoint2(displayXPosition, viewportYSizeInPixels);
    m_VerticalAxisActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
}

void Crosshairs2D::RequestSetXPosition(double x)
{
    RequestSetPosition(x, m_YPosition, m_ZPosition);
}

void Crosshairs2D::RequestSetYPosition(double y)
{
    RequestSetPosition(m_XPosition, y, m_ZPosition);
}

void Crosshairs2D::RequestSetZPosition(double z)
{
    RequestSetPosition(m_XPosition, m_YPosition, z);
}

void Crosshairs2D::NoProcessing()
{}
}
