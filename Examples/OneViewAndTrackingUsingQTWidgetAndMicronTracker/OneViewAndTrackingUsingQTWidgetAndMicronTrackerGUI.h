/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI_h
#define __OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI_h

#include <QtGui>

#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkMicronTracker.h"
#include "igstkMicronTrackerTool.h"
#include "igstkView.h"
#include "igstkTransform.h"
#include "igstkSerialCommunication.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "ui_OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI.h"

class OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI : public QMainWindow
{
  Q_OBJECT

  typedef igstk::Object::LoggerType            LoggerType; 
  typedef itk::StdStreamLogOutput              LogOutputType;

  typedef igstk::MicronTracker                 TrackerType;
  typedef igstk::MicronTrackerTool             TrackerToolType;
  typedef TrackerToolType::TransformType       TransformType;
  typedef igstk::SerialCommunication           CommunicationType;

public:
  OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI();
  ~OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI();
  void AttachObjectToTrackerTool( int trackerToolNumber,
                                  igstk::SpatialObject * objectToTrack );
  void InitializeTracker( std::string InitializationFile,
                          std::string CameraCalibrationFileDirectory,
                          std::string markerTemplateDirectory );
  void ConfigureTrackerToolsAndAttachToTheTracker();
  void SetView( igstk::View * view );
  bool IsTrackingTurnedOn()
    {
    return m_Tracking;
    }

  void AttachTrackerToView( igstk::View * view);
 
  bool HasQuitted();

public slots:
  void OnQuitAction();
  void OnTrackingAction( int state);
  void OnInteractionAction( int state);

private:
  void CreateActions();

  Ui::MainWindow          m_GUI;

  LoggerType::Pointer     m_Logger;
  LogOutputType::Pointer  m_LogOutput;
  TrackerType::Pointer    m_Tracker;

  TrackerToolType::Pointer    m_TrackerTool;
  TrackerToolType::Pointer    m_TrackerTool2;

  CommunicationType::Pointer m_Communication;

  bool                    m_Tracking;
  
  std::ofstream           m_LogFile;

  bool                    m_GUIQuit;

  typedef itk::ReceptorMemberCommand < 
       OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI > ObserverType;
  ObserverType::Pointer                       m_ViewPickerObserver;


  /** Callback functions for picking */ 
  void ParsePickedPoint( const itk::EventObject & event );

};

#endif
