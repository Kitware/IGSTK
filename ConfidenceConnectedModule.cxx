/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ConfidenceConnectedModule.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "ConfidenceConnectedModule.h"

namespace ISIS {

ConfidenceConnectedModule
::ConfidenceConnectedModule()
{

  m_SmoothingFilter             =   SmoothingFilterType::New();
  m_ConfidenceConnectedFilter   =   ConnectedFilterType::New();


  m_ConfidenceConnectedFilter->SetInput( m_SmoothingFilter->GetOutput() );


  /*  Define reasonable defaults for the parameters */
  m_SmoothingFilter->SetNumberOfIterations( 20 );
  m_SmoothingFilter->SetTimeStep( 0.0625 );

  m_ConfidenceConnectedFilter->SetMultiplier( 2.0 );
  m_ConfidenceConnectedFilter->SetNumberOfIterations( 10 );
  m_ConfidenceConnectedFilter->SetReplaceValue( 255 );
  m_ConfidenceConnectedFilter->SetInitialNeighborhoodRadius( 2 );

  // Allow filter to release data as the execution of
  // pipeline progresses from filter to filter.
  // This reduces memory consumption
  m_SmoothingFilter->ReleaseDataFlagOn();
  m_ConfidenceConnectedFilter->ReleaseDataFlagOn();

}





ConfidenceConnectedModule
::~ConfidenceConnectedModule()
{
}




void
ConfidenceConnectedModule
::SetInput( const InternalImageType * image )
{
  m_SmoothingFilter->SetInput( image );
}



const ConfidenceConnectedModule::OutputImageType *
ConfidenceConnectedModule
::GetOutput()
{
   return m_ConfidenceConnectedFilter->GetOutput();
}




void
ConfidenceConnectedModule
::SetSeedPoint( int x, int y, int z )
{
   typedef InternalImageType::IndexType   IndexType;
   IndexType SeedPoint;
   SeedPoint[0] = x;
   SeedPoint[1] = y;
   SeedPoint[2] = z;
   m_ConfidenceConnectedFilter->SetSeed( SeedPoint );
}



void
ConfidenceConnectedModule
::SetMultiplier( float multiplier )
{
   m_ConfidenceConnectedFilter->SetMultiplier( multiplier );
}



void
ConfidenceConnectedModule
::SetNumberOfIterations( unsigned int numberOfIterations )
{
   m_ConfidenceConnectedFilter->SetNumberOfIterations( 
                                      numberOfIterations  );
}



void
ConfidenceConnectedModule
::Execute()
{
  try
    {
    std::cout << "Initiating segmentation..." << std::endl;

    m_SmoothingFilter->Update();

    std::cout << "Image smoothed" << std::endl;

    m_ConfidenceConnectedFilter->Update();

    std::cout << "Image segmented" << std::endl;
    std::cout << std::endl << "Module done !"; 
    std::cout << std::endl << std::endl;
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    }
}


}  // end of namespace ISIS

