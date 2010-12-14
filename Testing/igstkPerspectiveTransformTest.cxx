#if defined(_MSC_VER)
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <math.h>
#include <iostream>
#include "igstkPerspectiveTransform.h"
#include "igstkPulseGenerator.h"


int igstkPerspectiveTransformTest( int , char * [] )
{
  
  igstk::RealTimeClock::Initialize();


  try
    {
                 //test the default constructor
      igstk::PerspectiveTransform P;
              //transformation should never be valid, default timestamp has
              //the same start and end times (zero)
      if( P.IsValidNow() )
      {
        std::cerr<<"Error in default constructor.\n";
        return EXIT_FAILURE;
      }
                     //test the transform setting and exporting
      igstk::PerspectiveTransform::ExtrinsicMatrixType Rt;
      igstk::PerspectiveTransform::IntrinsicMatrixType K;
        
      Rt(0,0)= -0.77;  Rt(0,1)= -0.64;  Rt(0,2)= 0.00;  Rt(0,3)= -0.92;
      Rt(1,0)= 0.00;   Rt(1,1)= -0.00;  Rt(1,2)= -1.00; Rt(1,3)= -5.83;
      Rt(2,0)= -0.64;  Rt(2,1)= 0.77;   Rt(2,2)= -0.00; Rt(2,3)= -745.91;

      K(0,0)=3886.68;  K(0,1)= 0.00;      K(0,2)= 630.57;
      K(1,0)= 0;       K(1,1) = 3886.68;  K(1,2)= 511.11;
      K(2,0)= 0;       K(2,1) = 0;        K(2,2) = 1.00;

      igstk::PerspectiveTransform coverageDummyTransform(P);
      igstk::PerspectiveTransform coverageDummyTransform2;
      coverageDummyTransform2 = P;
      coverageDummyTransform2.Print(std::cout, 0); 
      
      igstk::TimeStamp::TimePeriodType millisecondsToExpiration = 100;
      igstk::TransformBase::ErrorType estimationError = 0.5;
      P.SetTransform( Rt, K, estimationError, millisecondsToExpiration ); 

                    //export the combined transform K[R,t]
      vtkPerspectiveTransform *vtkP = vtkPerspectiveTransform::New();
      vtkMatrix4x4 *vtkPMatrix = vtkMatrix4x4::New();
      vtkMatrix4x4 *vtkRtMatrix = vtkMatrix4x4::New();

      P.ExportTransform( *vtkP );       
      vtkP->GetMatrix( vtkPMatrix );
  
      igstk::PerspectiveTransform::ExtrinsicMatrixType PMatrix;

      PMatrix = K.GetVnlMatrix()*Rt.GetVnlMatrix();

      if( ( PMatrix(0,0)-vtkPMatrix->GetElement(0,0) + 
            PMatrix(0,1)-vtkPMatrix->GetElement(0,1) + 
            PMatrix(0,2)-vtkPMatrix->GetElement(0,2) + 
            PMatrix(0,3)-vtkPMatrix->GetElement(0,3) +
            PMatrix(1,0)-vtkPMatrix->GetElement(1,0) + 
            PMatrix(1,1)-vtkPMatrix->GetElement(1,1) + 
            PMatrix(1,2)-vtkPMatrix->GetElement(1,2) + 
            PMatrix(1,3)-vtkPMatrix->GetElement(1,3) +
            PMatrix(2,0)-vtkPMatrix->GetElement(2,0) + 
            PMatrix(2,1)-vtkPMatrix->GetElement(2,1) + 
            PMatrix(2,2)-vtkPMatrix->GetElement(2,2) + 
            PMatrix(2,3)-vtkPMatrix->GetElement(2,3) )
           != 0.0 )
      {
        std::cerr<<"Error in export transformation.\n";
        vtkP->Delete();
        vtkPMatrix->Delete();
        vtkRtMatrix->Delete();
        return EXIT_FAILURE;
      }
          //export the intrinsic calibration matrix K
      P.ExportIntrinsicParameters( *vtkP );       
      vtkP->GetMatrix( vtkPMatrix );
            
      if( ( K(0,0)-vtkPMatrix->GetElement(0,0) + 
            K(0,1)-vtkPMatrix->GetElement(0,1) + 
            K(0,2)-vtkPMatrix->GetElement(0,2) + 
            K(1,0)-vtkPMatrix->GetElement(1,0) +
            K(1,1)-vtkPMatrix->GetElement(1,1) + 
            K(1,2)-vtkPMatrix->GetElement(1,2) + 
            K(2,0)-vtkPMatrix->GetElement(2,0) + 
            K(2,1)-vtkPMatrix->GetElement(2,1) +
            K(2,2)-vtkPMatrix->GetElement(2,2) ) 
           != 0.0 )
      {
        std::cerr<<"Error in export intrinsic transformation.\n";
        vtkP->Delete();
        vtkPMatrix->Delete();
        vtkRtMatrix->Delete();
        return EXIT_FAILURE;
      }
       
      P.ExportExtrinsicParameters( *vtkRtMatrix );       
                  
      if( ( Rt(0,0)-vtkRtMatrix->GetElement(0,0) + 
            Rt(0,1)-vtkRtMatrix->GetElement(0,1) + 
            Rt(0,2)-vtkRtMatrix->GetElement(0,2) + 
            Rt(0,3)-vtkRtMatrix->GetElement(0,3) +
            Rt(1,0)-vtkRtMatrix->GetElement(1,0) + 
            Rt(1,1)-vtkRtMatrix->GetElement(1,1) + 
            Rt(1,2)-vtkRtMatrix->GetElement(1,2) + 
            Rt(1,3)-vtkRtMatrix->GetElement(1,3) +
            Rt(2,0)-vtkRtMatrix->GetElement(2,0) + 
            Rt(2,1)-vtkRtMatrix->GetElement(2,1) + 
            Rt(2,2)-vtkRtMatrix->GetElement(2,2) + 
            Rt(2,3)-vtkRtMatrix->GetElement(2,3) )
           != 0.0 )
      {
        std::cerr<<"Error in export intrinsic transformation.\n";
        vtkP->Delete();
        vtkPMatrix->Delete();
        vtkRtMatrix->Delete();
        return EXIT_FAILURE;
      }      
      vtkP->Delete();
      vtkPMatrix->Delete();
      vtkRtMatrix->Delete();  

  }
  catch(...)
    {
    std::cerr << "Exception caught !!!" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test PASSED ! " << std::endl;

  return EXIT_SUCCESS;
}
