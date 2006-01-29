/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObjectTest.cxx
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

#include "igstkSpatialObject.h"
#include "igstkTracker.h"

namespace igstk
{
  
namespace SpatialObjectTest
{

class DummySpatialObject : public SpatialObject
{
public:
  igstkStandardClassTraitsMacro( DummySpatialObject, SpatialObject )

  bool TestMethods()
  {
    this->RequestAddObject( NULL );        // Test with null pointer
    this->RequestSetSpatialObject( NULL ); // Test with null pointer

    Self::Pointer sibling = Self::New();   // Test with valid pointer
    this->RequestAddObject( sibling );
      
    typedef SpatialObject::SpatialObjectType SpatialObjectType;
    SpatialObjectType::Pointer so = SpatialObjectType::New();
    this->RequestSetSpatialObject( so );   // Test with valid pointer

    sibling->RequestSetSpatialObject( so );
    this->RequestAddObject( sibling );
      
    const Self * me1 = dynamic_cast< const Self *>( this->GetObject(  0  ) );
    if( !me1 )
      {
      std::cerr << "Error in GetObject() with valid Id" << std::endl;
      return false;
      }
    
    const Self * me2 = dynamic_cast< const Self *>( this->GetObject( 100 ) );
    if( me2 )
      {
      std::cerr << "Error in GetObject() with invalid Id" << std::endl;
      return false;
      }

    return true;
  }

protected:
  DummySpatialObject( void ):m_StateMachine(this) {};
  ~DummySpatialObject( void ) {};
  
};

class MyTracker : public Tracker
{
public:

    /** Macro with standard traits declarations. */
    igstkStandardClassTraitsMacro( MyTracker, Tracker )

    typedef Superclass::TransformType           TransformType;

    void Initialize() { this->Tracker::Initialize(); }
    void GetTransform(TransformType & transform) 
          { this->GetToolTransform(0, 0, transform); }

protected:
    MyTracker():m_StateMachine(this) {};
    virtual ~MyTracker() {};

    typedef Tracker::ResultType                 ResultType;

    virtual ResultType InternalOpen( void ) { return SUCCESS; }
    virtual ResultType InternalActivateTools( void ) { return SUCCESS; }
    virtual ResultType InternalStartTracking( void ) { return SUCCESS; }
    virtual ResultType InternalUpdateStatus( void ) { return SUCCESS; }
    virtual ResultType InternalReset( void ) { return SUCCESS; }
    virtual ResultType InternalStopTracking( void ) { return SUCCESS; }
    virtual ResultType InternalDeactivateTools( void ) 
      { 
      m_ValidityTime = 100.0; // 100.0 milliseconds
      m_Port = TrackerPortType::New();
      m_Tool = TrackerToolType::New();
      m_Port->AddTool( m_Tool );
      this->AddPort( m_Port );
      return SUCCESS; 
      }
    virtual ResultType InternalClose( void ) { return SUCCESS; }

private:

    MyTracker(const Self&);  //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    typedef TrackerTool                 TrackerToolType;
    typedef TrackerPort                 TrackerPortType;
    typedef Transform::TimePeriodType   TimePeriodType;
    TimePeriodType                      m_ValidityTime;
    TrackerPortType::Pointer            m_Port;
    TrackerToolType::Pointer            m_Tool;
};



} // end SpatialObjectTest namespace

} // end igstk namespace


int igstkSpatialObjectTest( int, char * [] )
{

  typedef igstk::SpatialObjectTest::DummySpatialObject ObjectType;
  ObjectType::Pointer dummyObject = ObjectType::New();

  dummyObject->TestMethods();

  typedef igstk::SpatialObjectTest::MyTracker   TrackerType;
  TrackerType::Pointer  tracker = TrackerType::New();

  const unsigned int toolPort = 0;
  const unsigned int toolNumber = 0;


  tracker->Open();
  tracker->Initialize();

  tracker->AttachObjectToTrackerTool( toolPort, toolNumber, dummyObject );

  dummyObject->Print( std::cout );

  tracker->StartTracking();

  for(unsigned int i=0; i<50; i++)
    {
    igstk::PulseGenerator::CheckTimeouts();
    }

  tracker->StopTracking();
  tracker->Close();

  std::cout << "[PASSED]" << std::endl;

  std::cout << "Test [DONE]" << std::endl;

  return EXIT_SUCCESS;
}
