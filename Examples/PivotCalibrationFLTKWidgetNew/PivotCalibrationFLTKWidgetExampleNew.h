/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    PivotCalibrationFLTKWidgetExampleNew.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __PivotCalibrationFLTKWidgetExampleNew_h
#define __PivotCalibrationFLTKWidgetExampleNew_h

#include "itkCommand.h"

#include "PivotCalibrationFLTKWidgetExampleNewUI.h"

#include "igstkTrackerConfigurationGUIBase.h"
#include "igstkTrackerInitializerNew.h"

class PivotCalibrationFLTKWidgetExample :
  public PivotCalibrationFLTKWidgetExampleNewUI
{
public:

  /** Typedefs */
  igstkStandardClassBasicTraitsMacro( PivotCalibrationFLTKWidgetExample, PivotCalibrationFLTKWidgetExampleNewUI );

  igstkLoggerMacro();

  PivotCalibrationFLTKWidgetExample();
  virtual ~PivotCalibrationFLTKWidgetExample();

  virtual void InitializeTrackingAndCalibration();

  /** Public request methods from the GUI */
  void RequestConnectToTracker();

  void RequestInitializeTracker( const itk::EventObject & event );

private:

  PivotCalibrationFLTKWidgetExample(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

  /** Observer type for loaded event, 
  *  the callback can be set to a member function. */
  typedef itk::ReceptorMemberCommand < Self > LoadedObserverType;
  LoadedObserverType::Pointer               m_TrackerConfigurationObserver;
  LoadedObserverType::Pointer               m_TrackerToolUpdateObserver;

  /** Pointers for TrackerInitializer and related objects */
  igstk::Tracker::Pointer                         m_Tracker;
  igstk::TrackerConfigurationGUIBase            * m_TrackerConfigurationGUI;
  igstk::TrackerInitializerNew                  * m_TrackerInitializer;
  igstk::TrackerTool::Pointer                     m_TrackerTool;

  bool m_initialized;

};

#endif //_PIVOT_CALIBRATION_FLTK_WIDGET_EXAMPLE_H_
