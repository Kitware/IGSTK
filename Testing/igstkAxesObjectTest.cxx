/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAxesObjectTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>

#include "igstkAxesObject.h"
#include "igstkAxesObjectRepresentation.h"
#include "igstkView2D.h"
#include "igstkRealTimeClock.h"

namespace igstk
{
namespace AxesObjectTest
{
  class TransformObserver : public ::itk::Command 
  {
  public:
    typedef  TransformObserver   Self;
    typedef  ::itk::Command    Superclass;
    typedef  ::itk::SmartPointer<Self>  Pointer;
    itkNewMacro( Self );
  protected:
    TransformObserver() 
      {
      m_GotTransform = false;
      }
    ~TransformObserver() {}
  public:
    
      typedef ::igstk::TransformModifiedEvent  EventType;
        
      void Execute(itk::Object *caller, const itk::EventObject & event)
        {
        const itk::Object * constCaller = caller;
        this->Execute( constCaller, event );
        }

      void Execute(const itk::Object *caller, const itk::EventObject & event)
        {
        m_GotTransform = false;
        if( EventType().CheckEvent( &event ) )
          {
          const EventType * transformEvent = 
                    dynamic_cast< const EventType *>( &event );
          if( transformEvent )
            {
            m_Transform = transformEvent->Get();
            m_GotTransform = true;
            }
          }
        }

      bool GotTransform() const
        {
        return m_GotTransform;
        }

      const ::igstk::Transform & GetTransform() const
        {
        return m_Transform;
        }
        
  private:

    ::igstk::Transform  m_Transform;

    bool m_GotTransform;

  };

} // end namespace AxesObjectTest
} // end namespace igstk



int igstkAxesObjectTest( int, char * [] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::AxesObjectRepresentation  ObjectRepresentationType;
  ObjectRepresentationType::Pointer AxesRepresentation = ObjectRepresentationType::New();

  typedef igstk::AxesObject  ObjectType;
  ObjectType::Pointer AxesObject = ObjectType::New();
    
  // Test Set/GetRadius()
  std::cout << "Testing Set/GetSize() : ";
  AxesObject->SetSize(10,20,30);

  if(AxesObject->GetSizeX() != 10.0)
    {
    std::cerr << "SizeX error : " << AxesObject->GetSizeX() << " v.s " << 10.0 << std::endl; 
    return EXIT_FAILURE;
    }
   
  if(AxesObject->GetSizeY() != 20.0)
    {
    std::cerr << "SizeX error : " << AxesObject->GetSizeX() << " v.s " << 20.0 << std::endl; 
    return EXIT_FAILURE;
    }
 
  if(AxesObject->GetSizeZ() != 30.0)
    {
    std::cerr << "SizeX error : " << AxesObject->GetSizeX() << " v.s " << 30.0 << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Test Property
  std::cout << "Testing Property : ";
  AxesRepresentation->SetColor(0.1,0.2,0.3);
  AxesRepresentation->SetOpacity(0.4);
  if(fabs(AxesRepresentation->GetRed()-0.1)>0.00001)
    {
    std::cerr << "GetRed() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(AxesRepresentation->GetGreen()-0.2)>0.00001)
    {
    std::cerr << "GetGreen()[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(AxesRepresentation->GetBlue()-0.3)>0.00001)
    {
    std::cerr << "GetBlue() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(AxesRepresentation->GetOpacity()-0.4)>0.00001)
    {
    std::cerr << "GetOpacity() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Testing PrintSelf()
  AxesRepresentation->RequestSetAxesObject(AxesObject);
  AxesRepresentation->Print(std::cout);
  AxesObject->Print(std::cout);
  AxesObject->GetNameOfClass();
  AxesRepresentation->GetNameOfClass();

  // testing actors
  std::cout << "Testing actors : ";

  typedef igstk::View2D  View2DType;
  View2DType * view2D = new View2DType(0,0,200,200,"View 2D");
  
  // this will indirectly call CreateActors() 
  view2D->RequestAddObject( AxesRepresentation );
    
  std::cout << "[PASSED]" << std::endl;

  // Testing UpdateRepresentationFromGeometry. Changing the Spatial Object
  // geometrical parameters should trigger an update in the representation
  // class.
  std::cout << "Testing UpdateRepresentationFromGeometry() : ";
  AxesObject->SetSize( 20.0, 30.0, 40.0 );
  
  // Testing Update
  AxesRepresentation->IsModified();

  // Test GetTransform()
  std::cout << "Testing Set/GetTransform(): ";

  const double tolerance = 1e-8;
  double validityTimeInMilliseconds = 20000.0; // 20 seconds
  igstk::Transform transform;
  igstk::Transform::VectorType translation;
  translation[0] = 0;
  translation[1] = 1;
  translation[2] = 2;
  igstk::Transform::VersorType rotation;
  rotation.Set( 0.707, 0.0, 0.707, 0.0 );
  igstk::Transform::ErrorType errorValue = 0.01; // 10 microns

  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds );


  typedef ::igstk::AxesObjectTest::TransformObserver  TransformObserverType;

  TransformObserverType::Pointer transformObserver = TransformObserverType::New();

  AxesObject->AddObserver( ::igstk::TransformModifiedEvent(), transformObserver );
  
  AxesObject->RequestSetTransform( transform );
  AxesObject->RequestGetTransform();
  
  if( !transformObserver->GotTransform() )
    {
    std::cerr << "The AxesObject did not returned a Transform event" << std::endl;
    return EXIT_FAILURE;
    }
      
  igstk::Transform  transform2 = transformObserver->GetTransform();

  igstk::Transform::VectorType translation2 = transform2.GetTranslation();
  for( unsigned int i=0; i<3; i++ )
    {
    if( fabs( translation2[i]  - translation[i] ) > tolerance )
      {
      std::cerr << "Translation component is out of range [FAILED]" << std::endl;
      std::cerr << "input  translation = " << translation << std::endl;
      std::cerr << "output translation = " << translation2 << std::endl;
      return EXIT_FAILURE;
      }
    }

  igstk::Transform::VersorType rotation2 = transform2.GetRotation();
  if( fabs( rotation2.GetX() - rotation.GetX() ) > tolerance ||
      fabs( rotation2.GetY() - rotation.GetY() ) > tolerance ||
      fabs( rotation2.GetZ() - rotation.GetZ() ) > tolerance ||
      fabs( rotation2.GetW() - rotation.GetW() ) > tolerance     )
    {
    std::cerr << "Rotation component is out of range [FAILED]" << std::endl;
    std::cerr << "input  rotation = " << rotation << std::endl;
    std::cerr << "output rotation = " << rotation2 << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "[PASSED]" << std::endl;


  // Testing the Copy() function
  std::cout << "Testing Copy(): ";
  ObjectRepresentationType::Pointer copy = AxesRepresentation->Copy();
  if(copy->GetOpacity() != AxesRepresentation->GetOpacity())
    {
    std::cerr << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Exercise RequestSetAxesObject() with a null pointer as argument
  std::cout << "Testing RequestSetAxesObject() with NULL argument: ";
  ObjectRepresentationType::Pointer AxesRepresentation3 = ObjectRepresentationType::New();
  AxesRepresentation3->RequestSetAxesObject( 0 );

  // Exercise RequestSetAxesObject() called twice. The second call should be ignored.
  std::cout << "Testing RequestSetAxesObject() called twice: ";
  ObjectRepresentationType::Pointer AxesRepresentation4 = ObjectRepresentationType::New();
  ObjectType::Pointer AxesObjectA = ObjectType::New();
  ObjectType::Pointer AxesObjectB = ObjectType::New();
  AxesRepresentation4->RequestSetAxesObject( AxesObjectA );
  AxesRepresentation4->RequestSetAxesObject( AxesObjectB );

  std::cout << AxesRepresentation << std::endl;
  std::cout << AxesObjectA << std::endl;

  std::cout << "Test [DONE]" << std::endl;

  delete view2D;
  
  return EXIT_SUCCESS;
}
