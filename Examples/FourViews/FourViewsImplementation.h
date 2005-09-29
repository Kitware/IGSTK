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

#ifndef _FourViewsImplementation_h
#define _FourViewsImplementation_h

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif


#include "igstkSandboxConfigure.h"
#include "FourViews.h"
#include "igstkTubeObjectRepresentation.h"
#include "igstkMeshObjectRepresentation.h"

namespace igstk
{


class FourViewsImplementation : public FourViews
{

public:

    FourViewsImplementation()
      {
      // Set up the four quadrant views
      this->Display3D->RequestEnableInteractions();
      this->Display3D->RequestSetRefreshRate( 60 ); // 60 Hz
      this->Display3D->RequestStart();

      this->DisplayAxial->RequestEnableInteractions();
      this->DisplayAxial->RequestSetRefreshRate( 60 ); // 60 Hz
      this->DisplayAxial->RequestStart();

      this->DisplayCoronal->RequestEnableInteractions();
      this->DisplayCoronal->RequestSetRefreshRate( 60 ); // 60 Hz
      this->DisplayCoronal->RequestStart();

      this->DisplaySagittal->RequestEnableInteractions();
      this->DisplaySagittal->RequestSetRefreshRate( 60 ); // 60 Hz
      this->DisplaySagittal->RequestStart();      
      }

    ~FourViewsImplementation()
      {
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
 
    void AddTube( igstk::TubeObjectRepresentation * tubeRepresentation )
      {
      this->Display3D->RequestAddObject(       tubeRepresentation->Copy() );
      this->DisplayAxial->RequestAddObject(    tubeRepresentation->Copy() );
      this->DisplayCoronal->RequestAddObject(  tubeRepresentation->Copy() );
      this->DisplaySagittal->RequestAddObject( tubeRepresentation->Copy() );
      }

     void AddMesh( igstk::MeshObjectRepresentation * meshRepresentation )
      {
      this->Display3D->RequestAddObject(       meshRepresentation->Copy() );
      this->DisplayAxial->RequestAddObject(    meshRepresentation->Copy() );
      this->DisplayCoronal->RequestAddObject(  meshRepresentation->Copy() );
      this->DisplaySagittal->RequestAddObject( meshRepresentation->Copy() );
      }
private:

};


} // end of namespace

#endif

