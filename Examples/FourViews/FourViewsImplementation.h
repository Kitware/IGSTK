/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    FourViewsImplementation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __FourViewsImplementation_h
#define __FourViewsImplementation_h

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in the 
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif


#include "igstkSandboxConfigure.h"
#include "FourViews.h"
#include "igstkVascularNetworkObjectRepresentation.h"
#include "igstkMeshObjectRepresentation.h"
#include "igstkLogger.h"
#include "igstkVascularNetworkReader.h"
#include "igstkMeshReader.h"
#include <itkStdStreamLogOutput.h>
#include "FL/Fl_File_Chooser.H"
#include "igstkView2D.h"
#include "igstkView3D.h"

class FourViewsImplementation : public FourViews
{

public:

  typedef igstk::View2D ViewType2D;
  typedef igstk::View3D ViewType3D;

  igstkObserverObjectMacro(VascularNetwork,
                     igstk::VascularNetworkObjectModifiedEvent,
                     igstk::VascularNetworkObject)

  igstkObserverObjectMacro(Vessel,
                     igstk::VesselObjectModifiedEvent,
                     igstk::VesselObject)
  
  FourViewsImplementation()
    {
    this->Display3D = ViewType3D::New();
    this->DisplayAxial = ViewType2D::New();
    this->DisplayCoronal = ViewType2D::New();
    this->DisplaySagittal = ViewType2D::New();

    this->Display3DWidget->RequestSetView( this->Display3D );
    this->DisplayAxialWidget->RequestSetView( this->DisplayAxial );
    this->DisplayCoronalWidget->RequestSetView( this->DisplayCoronal );
    this->DisplaySagittalWidget->RequestSetView( this->DisplaySagittal );

    // Set up the four quadrant views
    this->Display3DWidget->RequestEnableInteractions();
    this->Display3D->SetRefreshRate( 60 ); // 60 Hz
    this->Display3D->RequestStart();

    this->DisplayAxialWidget->RequestEnableInteractions();
    this->DisplayAxial->SetRefreshRate( 60 ); // 60 Hz
    this->DisplayAxial->RequestStart();
    this->DisplayAxial->RequestSetOrientation( ViewType2D::Axial );

    this->DisplayCoronalWidget->RequestEnableInteractions();
    this->DisplayCoronal->SetRefreshRate( 60 ); // 60 Hz
    this->DisplayCoronal->RequestStart();
    this->DisplayCoronal->RequestSetOrientation( ViewType2D::Coronal );

    this->DisplaySagittalWidget->RequestEnableInteractions();
    this->DisplaySagittal->SetRefreshRate( 60 ); // 60 Hz
    this->DisplaySagittal->RequestStart();
    this->DisplaySagittal->RequestSetOrientation( ViewType2D::Sagittal );
      
    m_TubeGroup = NULL;
    m_Logger = NULL;
    }

  ~FourViewsImplementation()
    {
    this->Display3D->RequestStop();
    this->DisplayAxial->RequestStop();
    this->DisplaySagittal->RequestStop();
    this->DisplayCoronal->RequestStop();
    }
    
  void SetLogger(igstk::Object::LoggerType * logger)
    {
    m_Logger = logger;
    }


  void ResetCameras()
    {
    this->Display3D->RequestResetCamera();
    // this->Display3D->Update();
    this->DisplayAxial->RequestResetCamera();
    // this->DisplayAxial->Update();
    this->DisplayCoronal->RequestResetCamera();
    // this->DisplayCoronal->Update();
    this->DisplaySagittal->RequestResetCamera();
    // this->DisplaySagittal->Update();
    }
 
  void AddTube( igstk::VascularNetworkObjectRepresentation 
                                                  * tubeRepresentation )
    {
    igstk::VascularNetworkObjectRepresentation::Pointer object = 
                                            tubeRepresentation->Copy();
    m_VesselRepresentationList.push_back(object);
    this->Display3D->RequestAddObject(  object     );
    object = tubeRepresentation->Copy();
    m_VesselRepresentationList.push_back(object);
    this->DisplayAxial->RequestAddObject( object);

    object = tubeRepresentation->Copy();
    m_VesselRepresentationList.push_back(object);
    this->DisplayCoronal->RequestAddObject( object );
    object = tubeRepresentation->Copy();
    m_VesselRepresentationList.push_back(object);
    this->DisplaySagittal->RequestAddObject( object );
    }

  void AddMesh( igstk::MeshObjectRepresentation * meshRepresentation )
    {
    this->Display3D->RequestAddObject(       meshRepresentation         );
    this->DisplayAxial->RequestAddObject(    meshRepresentation->Copy() );
    this->DisplayCoronal->RequestAddObject(  meshRepresentation->Copy() );
    this->DisplaySagittal->RequestAddObject( meshRepresentation->Copy() );
    }

     
  void LoadVessels()
    {
    igstk::VascularNetworkReader::Pointer tubeReader 
                                    =  igstk::VascularNetworkReader::New();
    tubeReader->SetLogger( m_Logger );
       
    const char * filename = 
               fl_file_chooser("Select a vessel file","*.tre","*.tre");

    if(filename)
      {
      tubeReader->RequestSetFileName(filename);
      tubeReader->RequestReadObject();


      VascularNetworkObserver::Pointer vascularNetworkObserver 
                                            = VascularNetworkObserver::New();
      tubeReader->AddObserver(
           igstk::VascularNetworkReader::VascularNetworkObjectModifiedEvent(),
           vascularNetworkObserver);

      tubeReader->RequestGetVascularNetwork();

      m_TubeGroup = vascularNetworkObserver->GetVascularNetwork();

      igstk::VascularNetworkObjectRepresentation::Pointer tubeRepresentation =
                            igstk::VascularNetworkObjectRepresentation::New();
      tubeRepresentation->RequestSetVascularNetworkObject( m_TubeGroup );
      tubeRepresentation->SetColor(0.0,1.0,0.0);
      tubeRepresentation->SetOpacity(1.0);
      this->AddTube( tubeRepresentation );
      }

    this->ResetCameras();

    }

  void LoadLiver()
    {
    // Try to read a mesh
    igstk::MeshReader::Pointer meshReader =  igstk::MeshReader::New();
    meshReader->SetLogger( m_Logger );
    const char * filename = 
                     fl_file_chooser("Select a liver file","*.msh","*.msh");

    if(filename)
      {
      meshReader->RequestSetFileName(filename);
      meshReader->RequestReadObject();

      // Attach an observer
      igstkObserverObjectMacro( MeshObject, 
        igstk::MeshReader::MeshModifiedEvent,
        igstk::MeshObject);

      MeshObjectObserver::Pointer observer = MeshObjectObserver::New();

      meshReader->AddObserver( igstk::MeshReader::MeshModifiedEvent(),
        observer);

      meshReader->RequestGetOutput();

      igstk::MeshObject::ConstPointer mesh;

      if(observer->GotMeshObject())
      {
         mesh = observer->GetMeshObject();
      }

      igstk::Transform identity;
      identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

      this->SetDisplayTransformsAndParents( identity, mesh );

      // Create the object representations for the mesh
      igstk::MeshObjectRepresentation::Pointer meshRepresentation =
                                     igstk::MeshObjectRepresentation::New();
      meshRepresentation->RequestSetMeshObject( mesh );
      meshRepresentation->SetColor(1.0,0.0,0.0);
      meshRepresentation->SetOpacity(1.0);
      this->AddMesh( meshRepresentation );
      
      this->ResetCameras();
      }
    }


  /** We dynamically move the vessels */
  void MoveVessels()
    {
    if(!m_TubeGroup)
      {
      std::cout << "Please load a vessel file first!" << std::endl;
      return;
      }

    for(double k=0;k<10;k+=0.1)
      {
      // Remove all the tubes
      std::list<igstk::VascularNetworkObjectRepresentation::Pointer>::iterator
                                      it = m_VesselRepresentationList.begin();
      while(it != m_VesselRepresentationList.end())
        {
        this->Display3D->RequestRemoveObject( *it );
        this->DisplayAxial->RequestRemoveObject( *it );
        this->DisplayCoronal->RequestRemoveObject( *it );
        this->DisplaySagittal->RequestRemoveObject( *it );
        it++;
        }
      m_VesselRepresentationList.clear();
       
      // Create the object representations for the tube
      typedef igstk::VascularNetworkObject   VascularNetworkObjectType;
      typedef VascularNetworkObjectType::VesselObjectType VesselObjectType;
      VascularNetworkObjectType::Pointer newNetwork 
                                        = VascularNetworkObjectType::New();


      VesselObserver::Pointer vesselObserver = VesselObserver::New();
 
      m_TubeGroup->AddObserver(
            igstk::VesselObjectModifiedEvent(),
            vesselObserver);

      for(unsigned int i=0;i<m_TubeGroup->GetNumberOfVessels();i++)
        {
        const_cast<VascularNetworkObjectType*>(m_TubeGroup.GetPointer()
                                                       )->RequestGetVessel(i);
        igstk::VesselObject::Pointer tube = vesselObserver->GetVessel();
        igstk::VesselObject::Pointer newVessel = igstk::VesselObject::New();
        for(unsigned int j=0;j<tube->GetNumberOfPoints();j++)
          {
          const igstk::VesselObject::PointType  * pt = tube->GetPoint(j);
          igstk::VesselObject::PointType ptnew;
          ptnew.SetPosition(pt->GetPosition()[0] + k*sin(j*0.01*k),
                            pt->GetPosition()[1],
                            pt->GetPosition()[2]);
          ptnew.SetRadius(pt->GetRadius());
          newVessel->AddPoint(ptnew);
          }
        }

      igstk::VascularNetworkObjectRepresentation::Pointer tubeRepresentation 
                          = igstk::VascularNetworkObjectRepresentation::New();
      tubeRepresentation->RequestSetVascularNetworkObject( newNetwork );
      tubeRepresentation->SetColor(0.0,1.0,0.0);
      tubeRepresentation->SetOpacity(1.0);
      this->AddTube( tubeRepresentation );
     
      this->Display3DWidget->redraw();
      this->DisplayAxialWidget->redraw();
      this->DisplayCoronalWidget->redraw();
      this->DisplaySagittalWidget->redraw();

      Fl::check();
      }
    }

private:
  template <class ParentPointerType>
  void SetDisplayTransformsAndParents( igstk::Transform transform,
                                       ParentPointerType parent)
    {
    this->Display3D->RequestSetTransformAndParent( transform, parent );
    this->DisplayAxial->RequestSetTransformAndParent( transform, parent );
    this->DisplayCoronal->RequestSetTransformAndParent( transform, parent );
    this->DisplaySagittal->RequestSetTransformAndParent( transform, parent );
    }

  igstk::VascularNetworkObject::ConstPointer  m_TubeGroup;
  igstk::Object::LoggerType::Pointer                        m_Logger;
  std::list<igstk::VascularNetworkObjectRepresentation::Pointer> 
                                              m_VesselRepresentationList;

  ViewType2D::Pointer DisplayAxial;
  ViewType2D::Pointer DisplayCoronal;
  ViewType2D::Pointer DisplaySagittal;
  ViewType3D::Pointer Display3D;

};


#endif
