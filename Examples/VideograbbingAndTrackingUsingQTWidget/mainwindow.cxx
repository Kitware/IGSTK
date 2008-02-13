/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    mainwindow.cxx
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

#include "mainwindow.h"

#include "igstkQTWidget.h"
#include "igstkSerialCommunicationForPosix.h"

#include <string>

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QTimer>
#include <QStatusBar>

static const double PI = 3.1415926536;

MainWindow::MainWindow() :
  mVideoObject(igstk::VideoSpatialObject::New()),
  mVideoObjectRepresentation(igstk::VideoObjectRepresentation::New()),
  mTheView(igstk::View3D::New()),
  mIsTracking(false),
  mProbe(igstk::UltrasoundProbeObject::New()),
  mProbeRep(igstk::UltrasoundProbeObjectRepresentation::New())
{
  igstk::RealTimeClock::Initialize();
  
  // Set up widget
  igstk::QTWidget * theWidget = new igstk::QTWidget(this);
  this->setCentralWidget(theWidget);
  this->showMaximized();
  theWidget->RequestSetView(mTheView);
  theWidget->RequestEnableInteractions();
  mTheView->SetRendererBackgroundColor(0, 0, 0);
  theWidget->setMinimumSize(100, 100);
  mTheView->SetRefreshRate(30);
  mTheView->RequestResetCamera();

  // Create actions and add to menus
  QAction *addUsPlaneAction = new QAction(tr("Add USPlane"), this);
  connect(addUsPlaneAction, SIGNAL(triggered()), this, SLOT(addUsPlane()));
  QAction *startTrackingAction = new QAction(tr("Start Tracking"), this);
  connect(startTrackingAction, SIGNAL(triggered()), 
          this, SLOT(startTracking()));
  QMenu *viewMenu = menuBar()->addMenu(tr("View"));
  viewMenu->addAction(addUsPlaneAction);
  QMenu *trackerMenu = menuBar()->addMenu(tr("Tracking"));
  trackerMenu->addAction(startTrackingAction);

  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(idleProcessing()));
  timer->start(0);
 
  // Set up initial scenegraph with view, axes and probe
  igstk::Transform identity;
  identity.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());
  mVideoObjectRepresentation->RequestSetVideoSpatialObject(mVideoObject);
  mVideoObject->RequestSetTransformAndParent(identity, mTheView.GetPointer());
 
  mAxes = igstk::AxesObject::New();
  mAxes->SetSize(40, 40, 40);

  mAxesRepresentation = igstk::AxesObjectRepresentation::New();
  mAxesRepresentation->RequestSetAxesObject(mAxes);
  mAxesRepresentation->SetColor(1.0, 0.0, 0.0);
  mAxes->RequestSetTransformAndParent(identity, mTheView.GetPointer());
  mTheView->RequestAddObject(mAxesRepresentation);

  mProbe->RequestSetTransformAndParent(identity, mTheView.GetPointer());
  mProbeRep->RequestSetUltrasoundProbeObject(mProbe);
  mTheView->RequestAddObject(mProbeRep);

  mTheView->RequestResetCamera();
  mTheView->RequestStart();
}

MainWindow::~MainWindow()
{}

void MainWindow::idleProcessing()
{
  igstk::PulseGenerator::CheckTimeouts();
  if(mIsTracking)
  {
    mTracker->RequestUpdateStatus();
  }
}

void MainWindow::addUsPlane()
{
  mTheView->RequestAddObject(mVideoObjectRepresentation);
  mVideoObjectRepresentation->StartGrabbing();
}

void MainWindow::startTracking()
{
  typedef igstk::SerialCommunicationForPosix CommunicationType;

  CommunicationType::Pointer comm = CommunicationType::New();
  comm->SetPortNumber(igstk::SerialCommunication::PortNumber0);
  comm->SetParity(igstk::SerialCommunication::NoParity);
  comm->SetBaudRate(igstk::SerialCommunication::BaudRate57600);
  comm->SetDataBits(igstk::SerialCommunication::DataBits8);
  comm->SetStopBits(igstk::SerialCommunication::StopBits1);
  comm->SetHardwareHandshake(igstk::SerialCommunication::HandshakeOff);
  
  mTracker = igstk::PolarisTracker::New();
  mTracker->SetCommunication(comm);
  
  comm->OpenCommunication();
  mTracker->RequestOpen();
  
  mTrackerTool = igstk::PolarisTrackerTool::New();
  mTrackerTool->RequestSelectWirelessTrackerTool();
  
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open ROM file."));
  QByteArray ba(fileName.toLatin1());
  mTrackerTool->RequestSetSROMFileName(ba.data());
  mTrackerTool->RequestConfigure();
  mTrackerTool->RequestAttachToTracker(mTracker);
  
  // Create transform between probe and trackertool
  igstk::Transform probeTransform;
  probeTransform.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());
  itk::Versor<double> rotationZ;
  rotationZ.SetRotationAroundZ(-PI/2);
  igstk::Transform transformZ;
  transformZ.SetRotation(rotationZ, 1.0, 
                         igstk::TimeStamp::GetLongestPossibleTime());
  itk::Versor<double> rotationX;
  rotationX.SetRotationAroundX(PI);
  igstk::Transform transformX;
  transformX.SetRotation(rotationX, 1.0, 
                         igstk::TimeStamp::GetLongestPossibleTime());
  probeTransform = igstk::Transform::TransformCompose(transformZ, transformX);

  // Create transform between videoplane and probe
  itk::Vector<double, 3> videoTranslation;
  videoTranslation.SetElement(0, -100.0);
  videoTranslation.SetElement(1, 0.0);
  videoTranslation.SetElement(2, 0.0);  
  itk::Versor<double> videoRotationX;
  videoRotationX.SetRotationAroundX(PI/2);
  igstk::Transform videoTransform;
  videoTransform.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());
  videoTransform.SetTranslationAndRotation(
    videoTranslation, videoRotationX, 1.0, 
    igstk::TimeStamp::GetLongestPossibleTime() );

  // Connect the scenegraph
  //              mTracker
  // (identity)   /   \ (identity)
  //     mTheView  mTrackerTool
  //                    \  (probetransform)
  //                   mProbe
  //                      \ (videotransform)
  //                     mVideoObject

  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
  mTheView->RequestSetTransformAndParent(identity, mTracker.GetPointer());
  mTrackerTool->RequestSetTransformAndParent(identity, mTracker.GetPointer());
  mProbe->RequestSetTransformAndParent(probeTransform, 
                                       mTrackerTool.GetPointer());
  mVideoObject->RequestSetTransformAndParent(videoTransform, 
                                             mProbe.GetPointer());

  mTracker->RequestStartTracking();
  mIsTracking = true;
  statusBar()->showMessage(QString(tr("Tracking...")));  
}

