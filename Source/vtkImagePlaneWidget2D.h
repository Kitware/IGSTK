#ifndef __vtkImagePlaneEidget2D_h
#define __vtkImagePlaneEidget2D_h

#include <vtkImagePlaneWidget.h>

/**
 * A simple subclass of vtkImagePlaneWidget for keeping the crosshairs visible
 * even when the mouse button is released.
 * In addition, a method SetCursorPosition method is added to set the cursor
 * position in world coordinates.
 *
 * @date 2007.08.23
 * @author Torleif Sandnes, Sintef Health Research
 */
class vtkImagePlaneWidget2D : public vtkImagePlaneWidget
{
public:
    static vtkImagePlaneWidget2D *New();

    /**
     * Set the cursor position, (crosshairs), of the vtkImagePlaneWidget.
     * @param x The x coordinate in world coordinates.
     * @param y The y coordinate in world coordinates.
     * @param z The z coordinate in world coordinates.
     */
    void SetCursorPosition(double x, double y, double z);

    enum WidgetState
    {
        START=0,
        CURSORING,
        WINDOWLEVELLING,
        PUSHING,
        // Spinning,
        // Rotating,
        //Moving,
        //Scaling,
        //Outside
        NONE
    };
  
  /**
   * Get the current interactor state of the widget. This is a rehash of the
   * internal interactor states necessary for connecting events among
   * vtkImagePlaneWidget2Ds
   * @return The current interactor state of the widget.
   */
  int GetState() const;

  /**
   * Connect window/level for this ImagePlaneWidget2D to another
   * ImagePlaneWidget2D´s window/level
   * @param imagePlaneWidget The vtkImagePlaneWidget2D whose window/level should
   *        always be the same as this vtkImagePlaneWidget´s window/level
   */
  void ConnectWindowLevel(vtkImagePlaneWidget2D *imagePlaneWidget);


  void ConnectSliceIndex(vtkImagePlaneWidget2D *imagePlaneWidget);

protected:
    vtkImagePlaneWidget2D();
    ~vtkImagePlaneWidget2D();

    /**
     * Overridden to not set cursor invisible.
     */
    void StopCursor();
  static  void ProcessEvents(vtkObject *object,
                             unsigned long event,
                             void *clientData,
                             void *callData);

private:
 virtual void OnMouseMove();
  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  virtual void OnMiddleButtonDown();
  virtual void OnMiddleButtonUp();
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();
  void PerformPanning();
  int mLastXPosition;
  int mLastYPosition;
  bool mIsPanning;
};

#endif

