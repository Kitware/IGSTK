/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkImagingControlVideoSource.h
  Author:  Janne Beate Bakeng, SINTEF Health
  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkImagignControlVideoSource - VTK interface for video input from Imaging Control compatible hardware.
// .SECTION Description
// vtkImagingControlVideoSource is a class for providing video input interfaces between VTK and Imaging Control (IC) compatible hardware.
// The goal is to provide the ability to be able to do acquisition
// of images, buffer the image/volume series being acquired
// and stream the frames to output. 
// This class talks to Imaging Control SDK for executing the tasks.
// .SECTION Caveats
// You must call the ReleaseSystemResources() method before the application
// exits.  Otherwise the application might hang while trying to exit.
// When saving as *.bmp the images will be upside down. UYVY format not properly implemented.
// .SECTION See Also
// vtkWin32VideoSource vtkMILVideoSource vtkSonixVideoSource

//README:
//1. Copy the source files (vktImagingControlVideoSource.h and .cxx) into vtk's ../source/hybrid folder.
//2. Copy all IC header fiels in the same folder as used in (1).
//3. Copy TIS_UDSHL07_vc71d.lib, from ../IC Imaging Control 3.0/classlib/debug/, into VTKs ../bin/debug folder, found somewhere in the VTK build folder.
//4. Add the line: "vtkImagingControlVideoSource.cxx" in SET(Kit_SCRS ...here...) in the CMakeList.txt in the ../source/hybrid folder.
//5. CMake VTK
//6. Build vtkHybrid

#ifndef __vtkImagingControlVideoSource_h
#define __vtkImagingControlVideoSource_h

#include "vtkVideoSource.h"

#include "tisudshl.h"

class vtkUnstructuredGrid;

namespace DShowLib
{
  class Grabber;
}

class VTK_HYBRID_EXPORT vtkImagingControlVideoSource : public vtkVideoSource
{
public:
  //Description:
  //Available formats for writing to file
  typedef enum{
    BMP = 0,
    RAW = 1
  } SaveAs;

  vtkTypeRevisionMacro(vtkImagingControlVideoSource, vtkVideoSource);
  void PrintSelf(ostream& os, vtkIndent indent);
  // Description:
  // This is a singleton pattern New.  There will only be ONE
  // reference to a vtkImagingControlVideoSource object per process.  Clients that
  // call this must call Delete on the object so that the reference
  // counting will work.   The single instance will be unreferenced when
  // the program exits.
  static vtkImagingControlVideoSource* New();
  // Description:
  // Return the singleton instance with no reference counting.
  static vtkImagingControlVideoSource* GetInstance();
  // Description:
  // Supply a user defined output window. Call ->Delete() on the supplied
  // instance after setting it.
  static void SetInstance(vtkImagingControlVideoSource *instance);
  
  // Description:
  // Initialize the driver (this is called automatically when the
  // first grab is done).
  virtual void Initialize();

  // Description:
  // Record incoming video at the specified FrameRate.  The recording
  // continues indefinitely until Stop() is called. 
  virtual void Record();

  // Description:
  // Play through the 'tape' sequentially at the specified frame rate.
  // If you have just finished Recoding, you should call Rewind() first.
  virtual void Play();

  // Description:
  // Stop recording or playing.
  virtual void Stop();

  // Description:
  // Rewind to the frame with the earliest timestamp.  Record operations
  // will start on the following frame, therefore if you want to re-record
  // over this frame you must call Seek(-1) before calling Grab() or Record().
  virtual void Rewind();

  // Description:
  // Grab a single video frame. Sets the internal buffer to 1 and shows a preview
  // of the videostream.
  virtual void Grab();

  // Description:
  // For internal use only, writes a frame into the internal vtk buffer
  void InternalGrab(smart_ptr<DShowLib::MemBuffer> pBuffer, DWORD currFrame);

  // Description:
  // Request a particular vtk output format (default: VTK_RGB).
  virtual void SetOutputFormat(int format);
  
  // Description:
  // Set the IC licence key
  void SetImagingControlLicenceKey(char* ImagingControlLicenceKey);
  vtkGetMacro(ImagingControlLicenceKey, char*);

  // Description:
  // Writes the content of the buffer to file.
  // Writes either bmp or metaheader/raw files and a timestamps.txt fil.
  // The timestamps.txt starts with the number of frames and then the timestamps.
  // The syncFactor is a number used to compute an adjustment in the number of 
  // written frames. (-1 is default and writes all valid frames in the buffer.)
  // Caveats:
  // When writing to bmp the images will be upside-down.
  void Save(std::string filename, SaveAs saveAs, double syncFactor = -1);

protected:
  vtkImagingControlVideoSource();
  ~vtkImagingControlVideoSource();

private:
  //The pixels are stored as BGR in IC
  //Description:
  //Helping struct for converting pixel formats
  struct RGB24Pixel{
    BYTE b;
    BYTE g;
    BYTE r;
  };
  //Description:
  //Helping struct for converting pixel formats
  struct RGB32Pixel{
    BYTE b;
    BYTE g;
    BYTE r;
    BYTE a;
  };
  //Description:
  //Helping struct for converting pixel formats
  struct UYVY{
    BYTE u;
    BYTE y1;
    BYTE v;
    BYTE y2;
  };
  
protected:
  //Description:
  // Sets up the internal vtk structure to be the same as the connected device
  void DoFormatSetup();
  //Description:
  //Unpacks a rasterline and converts it to the proper outputformat if
  //the input format dosn't match the outputformat.
  void UnpackRasterLine(unsigned char *outptr, unsigned char *inptr, int start, int count);

  //Description:
  //The licence key to the IC library
  char* ImagingControlLicenceKey;
  //Description:
  //The colorformat of the IC videostream
  DShowLib::tColorformatEnum AcquisitionDataType;
  //Description:
  //The colorformat of the IC sink
  DShowLib::tColorformatEnum SinkDataType;
  //Description:
  //Pointer to the grabber
  DShowLib::Grabber* Grabber;

private:
  vtkImagingControlVideoSource(const vtkImagingControlVideoSource&);  // Not implemented.
  void operator=(const vtkImagingControlVideoSource&);  // Not implemented.

  //Description:
  //Converts from BYTE to VTK_LUMINANCE
  void BYTEToVTKLuminance(BYTE *outptr, BYTE *inptr, int start, int count);
  //Description:
  //Converts from BYTE to VTK_RGB
  void RGB24ToVTKRGB(RGB24Pixel *outptr, RGB24Pixel *inptr, int start, int count);
  //Description:
  //Converts from RGB24 to VTK_RGB
  void RGB32ToVTKRGBA(RGB32Pixel *outptr, RGB32Pixel *inptr, int start, int count);
  //Description:
  //Converts from RGB32 to VTK_LUMINANCE
  void RGB32ToVTKLuminance(BYTE *outptr, RGB32Pixel *inptr, int start, int count);
  //Description:
  //Converts from UYVY to RGB
  // Caveats: Not implemented properly.
  void UYVYToVTKRGB(RGB24Pixel *outptr, UYVY *inptr, int start, int count);
  //Description:
  //Clamps a value (0-255)
  unsigned char Clamp(unsigned char value);

  //Description:
  //The instance.
  static vtkImagingControlVideoSource* Instance;
  //Description:
  //Setting the number of IC buffers to a constant value
  const int IC_FRAMEBUFFER_SIZE;
};

#endif
