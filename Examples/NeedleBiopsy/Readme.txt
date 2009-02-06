Introduction
  This example application shows how to program a simple IGT application using IGSTK.

How to build
   1. This application builds automatically when IGSTK_USE_FLTK and IGSTK_BUILD_EXAMPLES is turned ON. 
  If you want to use Micron tracker with this application, make sure you have configured the with IGSTK_USE_MicronTracker ON.
  If you follow this process and successfully build the IGSTK, the final binary file should be in your ¡°IGSTK-Bin/bin/¡± or sub directory

Requirements to run the application
   1. You should have a Polaris, Aurora, or Micron Tracker connected to your system, with necessary drivers installed and additional files for setting up the tracker (such as SROMs, markers, and camera calibration file)
   2. Have an image volume with fiducial markers in it. Image should be DICOM format with CT or MR as modality, and should be stored in a directory with only one series. You should also have the object scanned available with the fiducial stay intact. You need to use it for landmark registration
   3. Some pointer tool that can be tracked and has known coordinate system definition and tooltip offset. In IGSTK, the cylinder object assumes the principle axis (the longest axis) to be Z. To have the needle displayed correctly in the image overlay. You might have to calibrate the axis. For instance, for Aurora tool, depending on the wiring of the sensor, some times, you might have to flip the Z axis. In the example, for Micron tracker, we assume the needle axis is the X axis in the marker coordinate system (The longer xline), so we need to switch X and Z to have the needle displayed correctly.
   4. This application also assume you might use some kind of reference tool, but that¡¯s optional. 

How to run the application
   1. Run the executable
   2. Load the image series from disk
   3. The application will automatically look for the corresponding treatment plan for this image series (DirName_TreatmentPlan.igstk). If the plan file doesn¡¯t exist, it will create a default plan of 5 points (entry, target, and 3 fiducials). You can change the values for these points by selecting them from the drop-down choice box and then click on the image. Annotation should show the updated value in red. Meanwhile the values will also be save to the disk, next time you load the same image, it will read these values back
   4. You can then choose to connect to tracker, after setting all the parameters, you can click ¡°confirm¡±, if success, the registration window will show up.
   5. You need to use the tracker tool to touch the fiducial on the object, in the same sequence as in the planning part, and then click on set tracker fiducal button. It will also jump to the next fiducial for user to set.
   6. After setting all the fiducials, you can click on ¡°Registration¡± this will do the landmark registration between image space and tracker space. If it returns successfully, a new tracker and tracker tool will be added to the list box. The view windows should also start tracking the tool location.
   7. If you have multiple tracker, or tracker tool connected. You can change the active tool from the choice box, which will switch the tool that the View window is following. 