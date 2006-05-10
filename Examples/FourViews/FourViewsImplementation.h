/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    FourViewsImplementation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

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
#include "itkLogger.h"
#include "igstkVascularNetworkReader.h"
#include "igstkMeshReader.h"
#include <itkStdStreamLogOutput.h>
#include "FL/Fl_File_Chooser.H"

namespace igstk
{


class FourViewsImplementation : public FourViews
{

public:

  typedef igstk::View2D ViewType;


  FourViewsImplementation()
    {
    // Set up the four quadrant views
    this->Display3D->RequestEnableInteractions();
    this->Display3D->RequestSetRefreshRate( 60 ); // 60 Hz
    this->Display3D->RequestStart();

    this->DisplayAxial->RequestEnableInteractions();
    this->DisplayAxial->RequestSetRefreshRate( 60 ); // 60 Hz
    this->DisplayAxial->RequestStart();
    this->DisplayAxial->RequestSetOrientation( ViewType::Axial );

    this->DisplayCoronal->RequestEnableInteractions();
    this->DisplayCoronal->RequestSetRefreshRate( 60 ); // 60 Hz
    this->DisplayCoronal->RequestStart();
    this->DisplayCoronal->RequestSetOrientation( ViewType::Coronal );

    this->DisplaySagittal->RequestEnableInteractions();
    this->DisplaySagittal->RequestSetRefreshRate( 60 ); // 60 Hz
    this->DisplaySagittal->RequestStart();
    this->DisplaySagittal->RequestSetOrientation( ViewType::Sagittal );
      
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
    
  void SetLogger(itk::Logger * logger)
    {
    m_Logger = logger;
    }


  void ResetCameras()
    {
    this->Display3D->RequestResetCamera();
    this->Display3D->Update();
    this->DisplayAxial->RequestResetCamera();
    this->DisplayAxial->Update();
    this->DisplayCoronal->RequestResetCamera();
    this->DisplayCoronal->Update();
    this->DisplaySagittal->RequestResetCamera();
    this->DisplaySagittal->Update();
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
    this->Display3D->RequestAddObject(       meshRepresentation->Copy() );
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
      m_TubeGroup = tubeReader->GetOutput();

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
      igstk::MeshObject::ConstPointer mesh = meshReader->GetOutput();

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
      igstk::VascularNetworkObject::Pointer newNetwork 
                                        = igstk::VascularNetworkObject::New();

      for(unsigned int i=0;i<m_TubeGroup->GetNumberOfObjects();i++)
        {
        igstk::VesselObject::ConstPointer tube = m_TubeGroup->GetVessel(i);
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
        newNetwork->RequestAddObject(newVessel);
        }

      igstk::VascularNetworkObjectRepresentation::Pointer tubeRepresentation 
                          = igstk::VascularNetworkObjectRepresentation::New();
      tubeRepresentation->RequestSetVascularNetworkObject( newNetwork );
      tubeRepresentation->SetColor(0.0,1.0,0.0);
      tubeRepresentation->SetOpacity(1.0);
      this->AddTube( tubeRepresentation );
     
      this->Display3D->redraw();
      this->DisplayAxial->redraw();
      this->DisplayCoronal->redraw();
      this->DisplaySagittal->redraw();

      Fl::check();
      }
    }

private:

  igstk::VascularNetworkObject::ConstPointer  m_TubeGroup;
  itk::Logger::Pointer                        m_Logger;
  std::list<igstk::VascularNetworkObjectRepresentation::Pointer> 
                                              m_VesselRepresentationList;

};


} // end of namespace

#endif
