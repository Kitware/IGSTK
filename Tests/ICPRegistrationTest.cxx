
#ifdef _WIN32
#pragma warning ( disable : 4786 )
#endif

#include "ICPRegistration.h"
#include <iostream>
#include <fstream>

int ICPRegistrationTest(int argc, char * argv[] )
{

  if( argc < 3 )
  {
    std::cerr << "Arguments Missing. " << std::endl;
    std::cerr << "Usage:  ICPRegistration   fixedPointsFile  movingPointsFile " << std::endl;
    return 1;
  }

  typedef IGSTK::ICPRegistration::PointSetType  PointSetType;
  typedef PointSetType::PointType  PointType;
  typedef PointSetType::PointsContainer  PointsContainer;

  PointSetType::Pointer fixedPointSet  = PointSetType::New();
  PointSetType::Pointer movingPointSet = PointSetType::New();

  PointsContainer::Pointer fixedPointContainer  = PointsContainer::New();
  PointsContainer::Pointer movingPointContainer = PointsContainer::New();

  PointType fixedPoint;
  PointType movingPoint;

  std::ifstream   fixedFile;
  std::ifstream   movingFile;
  unsigned int pointId = 0;
  IGSTK::ICPRegistration registration;

  fixedFile.open( argv[1] );
  if( fixedFile.fail() )
  {
    std::cerr << "Error opening points file with name : " << std::endl;
    std::cerr << argv[1] << std::endl;
    return 2;
  }

  while( !fixedFile.eof() )
  {
    fixedFile >> fixedPoint;
    fixedPointContainer->InsertElement( pointId, fixedPoint );    
    pointId++;
  }

  fixedPointSet->SetPoints( fixedPointContainer );
  std::cout << "Number of fixed Points = " << fixedPointSet->GetNumberOfPoints() << std::endl;
  fixedFile.close();
  
  movingFile.open( argv[2] );
  pointId = 0;
  while( !movingFile.eof() )
  {
    movingFile >> movingPoint;
    movingPointContainer->InsertElement( pointId, movingPoint );    
    pointId++;
  }

  movingPointSet->SetPoints( movingPointContainer );
  std::cout << "Number of moving Points = " << movingPointSet->GetNumberOfPoints() << std::endl;
  movingFile.close();

  registration.SetFixedPointSet( fixedPointSet );
  registration.SetMovingPointSet( movingPointSet );
  registration.SetNumberOfIterations( 20000 );
  registration.SetValueTolerance( 1e-4 );
  registration.SetGradientTolerance( 1e-4 );
  registration.SetEpsilonFunction( 1e-5 );

  try 
  {
    registration.StartRegistration();
  }
  catch( itk::ExceptionObject & e )
  {
    std::cout << e << std::endl;
    return EXIT_FAILURE;
  }

  double starterr = registration.m_LMOptimizer->GetOptimizer()->get_start_error();
  double err = registration.m_LMOptimizer->GetOptimizer()->get_end_error();

  std::cout << "Start RMS error: " << starterr << std::endl;
  std::cout << "End RMS error:" << err << std::endl;
  std::cout << "Solution = " << registration.GetTransform()->GetParameters() << std::endl;

  return EXIT_SUCCESS;
 
}

