/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    mainwindow.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

  Made by SINTEF Health Research - Medical technology:
  http://www.sintef.no/medtech

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>

#include "igstkView3D.h"
#include "igstkPolarisTracker.h"
#include "igstkUltrasoundProbeObject.h"
#include "igstkUltrasoundProbeObjectRepresentation.h"
#include "igstkVideoSpatialObject.h"
#include "igstkVideoObjectRepresentation.h"
#include "igstkAxesObject.h"
#include "igstkAxesObjectRepresentation.h"


class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();
  virtual ~MainWindow();

private slots:
  void idleProcessing();

  void addUsPlane();
  void startTracking();

private:
  igstk::VideoSpatialObject::Pointer mVideoObject;
  igstk::VideoObjectRepresentation::Pointer mVideoObjectRepresentation;
 
  igstk::View3D::Pointer mTheView;
  igstk::PolarisTracker::Pointer mTracker;
  igstk::PolarisTrackerTool::Pointer mTrackerTool;

  igstk::UltrasoundProbeObject::Pointer mProbe;
  igstk::UltrasoundProbeObjectRepresentation::Pointer mProbeRep;

  igstk::AxesObjectRepresentation::Pointer mAxesRepresentation;
  igstk::AxesObject::Pointer mAxes;
  bool mIsTracking;
};
#endif
