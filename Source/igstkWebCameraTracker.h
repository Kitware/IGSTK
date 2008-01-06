/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkWebCameraTracker.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkWebCameraTracker_h
#define __igstkWebCameraTracker_h

#include "igstkTracker.h"

class vtkVideoSource;
class vtkImageViewer2;

namespace igstk
{

/** \class WebCameraTracker
 *   \brief Tracker based on a monocular view from a Web Camera.
 *
 *   \ingroup Trackers
 */
class WebCameraTracker : public Tracker
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( WebCameraTracker, Tracker )

  typedef Superclass::TransformType           TransformType;


protected:

  WebCameraTracker();

  virtual ~WebCameraTracker();

  typedef Tracker::ResultType                 ResultType;

  virtual ResultType InternalOpen( void );

  virtual ResultType InternalStartTracking( void );

  virtual ResultType InternalReset( void );

  virtual ResultType InternalStopTracking( void );

  virtual ResultType InternalDeactivateTools( void );

  virtual ResultType InternalClose( void );

  /** Verify tracker tool information */
  virtual ResultType VerifyTrackerToolInformation( TrackerToolType * );

  /** This is the most important method of the WebCameraTracker family.
   *  This method must be overloaded in derived classes in order to
   *  provide a particular pattern of movement.
   */
  virtual ResultType InternalUpdateStatus( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** types for internal data structures */
  typedef unsigned char               PixelType;
  typedef itk::Point< double, 3 >     PointType;
  typedef std::vector< PointType >    PointListType;
  typedef std::vector< PixelType >    PixelValueListType;


  /** Detect a collection formed by the brightest pixels of the image.
   * Results are stored in an array of positions and array of pixel values */
  void DetectBrightestPixels(void);

  /** Compute the center of mass for the collection of brightest pixels. The.
   * pixel intensity is associated to Mass. */
  void ComputeCenterOfMass(void);

private:

  WebCameraTracker(const Self&);  //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  vtkVideoSource    * m_VideoSource;
  vtkImageViewer2   * m_ImageViewer;

  double m_X; // FIXME: remove me

  PointListType               m_BrightestPixelPositions;
  PixelValueListType          m_BrightestPixelValues;
  
  PointType                   m_Position;
};

}

#endif //__igstk_WebCameraTracker_h_
