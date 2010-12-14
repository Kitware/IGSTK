#if defined(_MSC_VER)
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <math.h>
#include <iostream>
#include "igstkAffineTransform.h"
#include "igstkPulseGenerator.h"

int igstkAffineTransformTest( int , char * [] )
{

  igstk::RealTimeClock::Initialize();


  try
    {
      //test the default constructor
      igstk::AffineTransform I;
      //transformation should never be valid, default timestamp has
      //the same start and end times (zero)
      if( I.IsValidNow() )
      {
        std::cout<<"Error in default constructor.\n";
        return EXIT_FAILURE;
      }
    
      //test the transform setting and exporting
      igstk::AffineTransform transform;
      igstk::AffineTransform::MatrixType m;
      igstk::AffineTransform::OffsetType t;

      m(0,0) = 1.4570;    m(0,1) = -0.0183;     m(0,2)= -5.2449;  
      m(1,0) = 0.0591;    m(1,1) = 4.9989;      m(1,2) = 0.0405;  
      m(2,0) = 2.6218;    m(2,1) = -0.1024;     m(2,2) = 2.9137;  
       
      t[0] = 345;     t[1] = 200;     t[2] = 76;
      
      igstk::TimeStamp::TimePeriodType millisecondsToExpiration = 100;
      igstk::TransformBase::ErrorType estimationError = 0.5;
      transform.SetMatrixAndOffset( m, t, 
                                    estimationError,
                                    millisecondsToExpiration );
      
      igstk::AffineTransform coverageDummyTransform(transform);
      igstk::AffineTransform coverageDummyTransform2;
      coverageDummyTransform2 = transform;
      coverageDummyTransform2.Print(std::cout, 0); 
    
      vtkMatrix4x4* vtkM1 = vtkMatrix4x4::New();
      transform.ExportTransform( *vtkM1 );
      if( ( m(0,0)-vtkM1->GetElement(0,0) + m(0,1)-vtkM1->GetElement(0,1) + 
            m(0,2)-vtkM1->GetElement(0,2) + t[0]-vtkM1->GetElement(0,3) +
            m(1,0)-vtkM1->GetElement(1,0) + m(1,1)-vtkM1->GetElement(1,1) + 
            m(1,2)-vtkM1->GetElement(1,2) + t[1]-vtkM1->GetElement(1,3) +
            m(2,0)-vtkM1->GetElement(2,0) + m(2,1)-vtkM1->GetElement(2,1) + 
            m(2,2)-vtkM1->GetElement(2,2) + t[2]-vtkM1->GetElement(2,3)) 
            != 0.0 )
      {
        std::cout<<"Error in setting and getting transformation values.\n";
        vtkM1->Delete();
        return EXIT_FAILURE;
      }
      
      //test transformation composition
      igstk::AffineTransform composedTransform;
      composedTransform = 
        igstk::AffineTransform::TransformCompose( I, transform );
      //the composition should not be valid as the transform, I, has the
      //same start and end time (zero).
      if( composedTransform.IsValidNow() )
      {
        std::cout<<"Error in composition.\n";
        return EXIT_FAILURE;
      }

      //check the composed transform data, should be equal to the
      //original as 
      vtkMatrix4x4* vtkM2 = vtkMatrix4x4::New();
      composedTransform.ExportTransform( *vtkM2 );

      double error = 
      fabs( vtkM1->GetElement(0,0)-vtkM2->GetElement(0,0) + 
        vtkM1->GetElement(0,1)-vtkM2->GetElement(0,1) + 
        vtkM1->GetElement(0,2)-vtkM2->GetElement(0,2) + 
        vtkM1->GetElement(0,3)-vtkM2->GetElement(0,3) +
        vtkM1->GetElement(1,0)-vtkM2->GetElement(1,0) + 
        vtkM1->GetElement(1,1)-vtkM2->GetElement(1,1) + 
        vtkM1->GetElement(1,2)-vtkM2->GetElement(1,2) + 
        vtkM1->GetElement(1,3)-vtkM2->GetElement(1,3) +
        vtkM1->GetElement(2,0)-vtkM2->GetElement(2,0) + 
        vtkM1->GetElement(2,1)-vtkM2->GetElement(2,1) + 
        vtkM1->GetElement(2,2)-vtkM2->GetElement(2,2) + 
        vtkM1->GetElement(2,3)-vtkM2->GetElement(2,3) );

      if(  error  > 0.00000001 )
        {
        std::cout<<"Error in setting and getting transformation values 2.\n";
        vtkM2->Delete();
        return EXIT_FAILURE;
        }
      vtkM1->Delete();
      vtkM2->Delete();
  }
  catch(...)
    {
    std::cout << "Exception caught !!!" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test PASSED ! " << std::endl;

  return EXIT_SUCCESS;
}
