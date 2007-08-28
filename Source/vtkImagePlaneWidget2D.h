#ifndef __vtkImagePlaneEidget2D_h
#define __vtkImagePlaneEidget2D_h

#include <vtkImagePlaneWidget.h>

/**
 * A simple subclass of vtkImagePlaneWidget for keeping the crosshairs visible
 * even when the mouse button is released.
 * In addition, a method SetCursorPosition method is added to set the cursor
 * position in world coordinates.
 *
 * @Date 2007.08.23
 * @Author Torleif Sandnes, Sintef Health Research
 */
class vtkImagePlaneWidget2D : public vtkImagePlaneWidget
{
public:
    static vtkImagePlaneWidget2D *New();
    //vtkTypeRevisionMacro(vtkImagePlaneWidget2D, vtkImagePlaneWidget);

    /**
     * Set the cursor position, (crosshairs), of the vtkImagePlaneWidget.
     * @Param x The x coordinate in world coordinates.
     * @Param y The y coordinate in world coordinates.
     * @Param z The z coordinate in world coordinates.
     */
    void SetCursorPosition(double x, double y, double z);
protected:
    vtkImagePlaneWidget2D();
    ~vtkImagePlaneWidget2D();

    /**
     * Overridden to not set cursor invisible.
     */
    void StopCursor();
};

#endif

