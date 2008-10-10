#include "igstkPrecomputedTransformData.h"
#include "igstkTransformFileReader.h"
#include "igstkPerspectiveTransformXMLFileReader.h"
#include "igstkAffineTransformXMLFileReader.h"
#include "igstkRigidTransformXMLFileReader.h"

/**
 * This class observes the events that are the end result of reading an xml
 * file containing a precomputed transformation: 
 * igstk::TransformFileReader::ReadFailureEvent
 * igstk::TransformFileReader::ReadSuccessEvent
 *
 * Upon failure it gets the error message which is the failure event's payload
 * and writes it to cerr.
 * Upon success it invokes the RequestGetData event on the object that notified
 * us of the "read success".
 *
 */
class ReadObserver : public itk::Command
{
public:
  typedef ReadObserver                    Self;
  typedef ::itk::Command                   Superclass;
  typedef ::itk::SmartPointer<Self>        Pointer;
  typedef ::itk::SmartPointer<const Self>  ConstPointer;

  igstkNewMacro( Self )
  igstkTypeMacro( ReadObserver, itk::Command )

  virtual void Execute( itk::Object *caller, const itk::EventObject & event )
  {
    const igstk::TransformFileReader::ReadFailureEvent *rfe;
    if( ( rfe = 
      dynamic_cast<const igstk::TransformFileReader::ReadFailureEvent *>
      ( &event ) ) ) 
      std::cerr<<rfe->Get()<<"\n";
    else if( dynamic_cast<const igstk::TransformFileReader::ReadSuccessEvent *>
      ( &event ) )
      ( static_cast<igstk::TransformFileReader *>( caller ) )->RequestGetData();
  }

  virtual void Execute( const itk::Object *caller, 
                        const itk::EventObject & event )
  {
    const itk::Object * constCaller = caller;
    this->Execute( constCaller, event );
  }
protected:

  ReadObserver(){}
  virtual ~ReadObserver(){}
private:
  //purposely not implemented
  ReadObserver(const Self&);
  void operator=(const Self&); 
};

/**
 * This object observes the event generated when a RequestGetData() method of
 * the reader is invoked, gets the precomputed transform data object. 
 */
igstkObserverMacro( TransformData, 
                    igstk::TransformFileReader::TransformDataEvent, 
                    igstk::PrecomputedTransformData::Pointer )

igstkObserverMacro( TransformationDescription, 
                    igstk::StringEvent, 
                    std::string )

igstkObserverMacro( TransformationDate, 
                    igstk::PrecomputedTransformData::TransformDateTypeEvent, 
                    std::string )

class TransformRequestObserver : public ::itk::Command 
{ 
public: 
  typedef  TransformRequestObserver             Self; 
  typedef  ::itk::Command             Superclass;
  typedef  ::itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  TransformRequestObserver() : m_GotObject( false ) {}
  ~TransformRequestObserver() {}
public:
  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    const itk::Object * constCaller = caller;
    this->Execute( constCaller, event );
  }
  void Execute(const itk::Object *caller, const itk::EventObject & event)
  {
    m_GotObject = false;
    if( igstk::PrecomputedTransformData::TransformTypeEvent().CheckEvent( &event ) )
    {
      const igstk::PrecomputedTransformData::TransformTypeEvent * objectEvent = 
        dynamic_cast< const igstk::PrecomputedTransformData::TransformTypeEvent *>( &event );
      if( objectEvent )
      {
        m_Object = objectEvent->Get();
        m_GotObject = true;
      }
    }
  }
  bool GotTransformRequest() const
  {
    return m_GotObject;
  }
  void Reset() 
  {
    m_GotObject = false; 
  }
  const igstk::PrecomputedTransformData::TransformType * GetTransformRequest() 
    const
  {
    return m_Object;
  }
private:
  const igstk::PrecomputedTransformData::TransformType *   m_Object;
  bool m_GotObject;
};



igstkObserverMacro( TransformError, 
                    igstk::PrecomputedTransformData::TransformErrorTypeEvent, 
                    igstk::PrecomputedTransformData::ErrorType )

/**
 * This program recieves an xml file containing a precomputed rigid 
 * transformation, reads it, presents the user with the general information 
 * about the transformation (when computed and estimation error). The user is
 * then asked if to use the transformation or not. If the answer is yes, the
 * program prints the transformation and exits, otherwise it just exits.
 *
 * To load other types of transformations change 
 * the instantiation of igstk::RigidTransformXMLFileReader to 
 * igstk::PerspectiveTransformXMLFileReader or 
 * igstk::AffineTransformXMLFileReader.
 */
int main(int argc, char *argv[])
{
  if( argc!= 2 )
  {
    std::cerr<<"Wrong number of arguments.\n";
    std::cerr<<"Usage: "<<argv[0]<<" transformation_data_file_name\n";
    return EXIT_FAILURE;
  }

  igstk::PrecomputedTransformData::Pointer transformData;
                                  //our reader
  igstk::TransformFileReader::Pointer transformFileReader = 
    igstk::TransformFileReader::New();
                  //set our reader to read a rigid transformation from the 
                  //given file
  igstk::TransformXMLFileReaderBase::Pointer xmlFileReader = 
    igstk::RigidTransformXMLFileReader::New();
  transformFileReader->RequestSetReader( xmlFileReader );
  transformFileReader->RequestSetFileName( argv[1] );

            //observer for the read success and failure events
  ReadObserver::Pointer readObserver = ReadObserver::New();
            //observer for the get data event (initiated by the readObserver)
  TransformDataObserver::Pointer getDataObserver = TransformDataObserver::New();
                       
            //add our observers     
  transformFileReader->AddObserver( 
    igstk::TransformFileReader::ReadFailureEvent(),
    readObserver );
  transformFileReader->AddObserver( 
    igstk::TransformFileReader::ReadSuccessEvent(),
    readObserver );
  transformFileReader->AddObserver( 
    igstk::TransformFileReader::TransformDataEvent(),
    getDataObserver );

                //request read. if it fails the readObserver will print the 
                //error, otherwise it will request to get the transform data                 
  transformFileReader->RequestRead();
               //if the read succeeded it invoked a request data, so check if
               //we got the data
  if( getDataObserver->GotTransformData() ) 
  {
    transformData = getDataObserver->GetTransformData();
                           //attach all observers to the transformation data
                           //object
    TransformationDescriptionObserver::Pointer descriptionObserver = 
      TransformationDescriptionObserver::New();
    transformData->AddObserver( igstk::StringEvent(), 
                                descriptionObserver );

    TransformationDateObserver::Pointer dateObserver = 
      TransformationDateObserver::New();
    transformData->AddObserver( 
      igstk::PrecomputedTransformData::TransformDateTypeEvent(), 
      dateObserver );
      
    TransformRequestObserver::Pointer transformObserver = 
      TransformRequestObserver::New();
    transformData->AddObserver( 
      igstk::PrecomputedTransformData::TransformTypeEvent(), 
      transformObserver );

    TransformErrorObserver::Pointer transformErrorObserver = 
      TransformErrorObserver::New();
    transformData->AddObserver( 
      igstk::PrecomputedTransformData::TransformErrorTypeEvent(), 
      transformErrorObserver );
                           //request all the info
    transformData->RequestTransformDescription();
    transformData->RequestComputationDateAndTime();
    transformData->RequestEstimationError();     
    if( descriptionObserver->GotTransformationDescription() &&
        dateObserver->GotTransformationDate() &&
        transformErrorObserver->GotTransformError() )
    {
      char response;
      std::cout<<descriptionObserver->GetTransformationDescription();
      std::cout<<" ("<<dateObserver->GetTransformationDate()<<").\n";
      std::cout<<"Estimation error: ";
      std::cout<<transformErrorObserver->GetTransformError()<<"\n";
      std::cout<<"Use this data [n,y]: ";
      std::cin>>response;
            //set the tool's calibration transform
      if(response == 'y') {
        transformData->RequestTransform();
        if( transformObserver->GotTransformRequest() )
        {
          std::cout<<"Set calibration:\n";
          transformObserver->GetTransformRequest()->Print( std::cout, 
                                                           itk::Indent() );
        }
      }
    }
  }
  return EXIT_SUCCESS;
}
