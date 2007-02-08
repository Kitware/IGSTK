/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    main.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "UltrasoundGuidedRFAImplementation.h"
#include "igstkUltrasoundProbeObjectRepresentation.h"
#include "igstkAxesObjectRepresentation.h"

int main(int , char** )
{ 
  igstk::RealTimeClock::Initialize();

  igstk::UltrasoundGuidedRFAImplementation::Pointer application 
                              = igstk::UltrasoundGuidedRFAImplementation::New();
  application->Show();

  // Create the probe
  igstk::UltrasoundProbeObject::Pointer UltrasoundProbe 
                                          = igstk::UltrasoundProbeObject::New();

  double validityTimeInMilliseconds = 1e20; // in seconds
  igstk::Transform transform;
  igstk::Transform::VectorType translation;
  translation[0] = 0.0;
  translation[1] = 0.0;
  translation[2] = 0.0;
  igstk::Transform::VersorType rotation;
  rotation.Set( 0.0, 0.0, 0.0, 1.0 );
  igstk::Transform::ErrorType errorValue = 0.01; // 10 microns
 
  transform.SetTranslationAndRotation(
           translation, rotation, errorValue, validityTimeInMilliseconds );
 
  UltrasoundProbe->RequestSetTransform( transform );

  // Create the UltrasoundProbe representation
  typedef igstk::UltrasoundProbeObjectRepresentation USProbeRepresentationType;
  USProbeRepresentationType::Pointer UltrasoundProbeRepresentation;
  UltrasoundProbeRepresentation = USProbeRepresentationType::New();
  UltrasoundProbeRepresentation->RequestSetUltrasoundProbeObject( 
                                                              UltrasoundProbe );
  UltrasoundProbeRepresentation->SetColor(1.0,0.0,0.0);
  UltrasoundProbeRepresentation->SetOpacity(1.0);

  // Add the probe representations to the views
  application->AddProbe(  UltrasoundProbeRepresentation  );

  // Create the axes representation
  igstk::AxesObject::Pointer axes = igstk::AxesObject::New();
  axes->SetSize(50,50,50);
  igstk::AxesObjectRepresentation::Pointer axesRepresentation = 
                                        igstk::AxesObjectRepresentation::New();
  axesRepresentation->RequestSetAxesObject( axes );

  application->AddAxes(axesRepresentation);

  // Create a second axes representation for the probe
  igstk::AxesObject::Pointer axes2 = igstk::AxesObject::New();
  axes2->SetSize(50,50,50);
  igstk::AxesObjectRepresentation::Pointer axesRepresentation2 =
                                        igstk::AxesObjectRepresentation::New();
  axesRepresentation2->RequestSetAxesObject( axes2 );
  application->AddAxes(axesRepresentation2);

  // Associate the Spatial Object to the tracker
  //application.AttachObjectToTrack( axes2 );
  application->AttachObjectToTrack(  UltrasoundProbe  );


  while( !application->HasQuitted() )
    {
    Fl::wait(0.001);
    application->Randomize();
    igstk::PulseGenerator::CheckTimeouts();
    }

  return EXIT_SUCCESS;
}
