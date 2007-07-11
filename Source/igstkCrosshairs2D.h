/*
 * No license text yet. (What license should contributors to IGSTK use?)
 * Would like to include the Sintef name somewhere in the IGSTK license.
 */

#ifndef __igstkCrosshairs2D_h
#define __igstkCrosshairs2D_h

#include <igstkObject.h>
#include <vector>
#include <igstkMacros.h>
#include <igstkStateMachine.h>

class vtkViewport;
class vtkAxisActor2D;
namespace igstk
{

/**
 * @brief A class to display a 2D crosshair in the overlay plane of QView.
 * @date 2007.06.30
 * @author Torleif Sandnes
 */
class Crosshairs2D : public Object
{
public:
    igstkStandardClassTraitsMacro(Crosshairs2D, igstk::Object);
    
    Crosshairs2D();
    ~Crosshairs2D();
    
    /**
     * Set the x,y and z coordinates of the crosshairs.
     * @param x The x coordinate.
     * @param y The y coordinate.
     * @param z The z coordinate.
     */
    void RequestSetPosition(double x, double y, double z);

    /**
     * Set only the X position of the crosshairs. The other coordinates remain
     * unchanged.
     */
    void RequestSetXPosition(double x);

    /**
     * Set only the Y position of the crosshairs. The other coordinates remain
     * unchanged.
     */
    void RequestSetYPosition(double y);

    /**
     * Set only the Z position of the crosshairs. The other coordinates remain
     * unchanged.
     */
    void RequestSetZPosition(double z);

    /**
     * Set the viewport to render in. (Required for coordinate conversions.)
     * @param viewPort The vtk viewport that the crosshairs will be rendered in.  
     */
    void RequestSetViewport(vtkViewport *viewPort);
    
    /**
     * Return the horizontal axis actor.
     * @warning This method should not be accessed by clients of this class. 
     * It's only purpose is to enable view objects to add the actor to a scene.
     * @return A pointer to the horizontal vtkAxisActor2D object.
     */
    vtkAxisActor2D *GetHorizontalAxisActor() const 
    { return m_HorizontalAxisActor; }
    
    /**
     * Return the vertical axis actor.
     * @warning This method should not be accessed by clients of this class. 
     * It's only purpose is to enable view objects to add the actor to a scene.
     * @return A pointer to the vertical vtkAxisActor2D object.
     */
    vtkAxisActor2D *GetVerticalAxisActor() const 
    { return m_VerticalAxisActor; }

private:
    Crosshairs2D(const Self&);
    void operator=(const Self&);

    vtkAxisActor2D *m_HorizontalAxisActor;
    vtkAxisActor2D *m_VerticalAxisActor;
    vtkViewport    *m_Viewport;
    vtkViewport    *m_ViewportToBeSet;

    double m_XPosition;
    double m_YPosition;
    double m_ZPosition;

    void SetViewportProcessing();
    void SetPositionProcessing();
    void NoProcessing();

    igstkDeclareInputMacro(ValidViewport);
    igstkDeclareInputMacro(InvalidViewport);
    igstkDeclareInputMacro(ValidPosition);
    igstkDeclareInputMacro(InvalidPosition);

    igstkDeclareStateMacro(Idle);
    igstkDeclareStateMacro(ViewportSet);
    igstkDeclareStateMacro(PositionSet);
};    
}
#endif
