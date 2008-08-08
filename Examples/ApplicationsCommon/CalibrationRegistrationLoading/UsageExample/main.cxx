#include "igstkPrecomputedTransformData.h"
#include "igstkTransformFileReader.h"
#include "igstkAffineTransformXMLFileReader.h"
#include "igstkRigidTransformXMLFileReader.h"

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
    if( ( rfe = dynamic_cast<const igstk::TransformFileReader::ReadFailureEvent *>( &event ) ) ) 
      std::cerr<<rfe->Get()<<"\n";
    else if( dynamic_cast<const igstk::TransformFileReader::ReadSuccessEvent *>( &event ) )
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


igstkObserverMacro( TransformData, 
                    igstk::TransformFileReader::TransformDataEvent, 
                    igstk::PrecomputedTransformData::Pointer )

igstkObserverMacro( TransformationDescription, 
                    igstk::StringEvent, 
                    std::string )

igstkObserverMacro( TransformationDate, 
                    igstk::PrecomputedTransformData::TransformDateTypeEvent, 
                    std::string )

igstkObserverMacro( Transform, 
                    igstk::PrecomputedTransformData::TransformTypeEvent, 
                    igstk::PrecomputedTransformData::TransformType::Pointer )


igstkObserverMacro( TransformError, 
                    igstk::PrecomputedTransformData::TransformErrorTypeEvent, 
                    igstk::PrecomputedTransformData::ErrorType )

int main(int argc, char *argv[])
{
  if( argc!= 2 )
  {
    std::cerr<<"Wrong number of arguments.\n";
    std::cerr<<"Usage: "<<argv[0]<<"transformation_data_file_name\n";
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
  transformFileReader->AddObserver( igstk::TransformFileReader::ReadFailureEvent(),
                                    readObserver );
  transformFileReader->AddObserver( igstk::TransformFileReader::ReadSuccessEvent(),
                                    readObserver );
  transformFileReader->AddObserver( igstk::TransformFileReader::TransformDataEvent(),
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
    transformData->AddObserver( igstk::PrecomputedTransformData::TransformDateTypeEvent(), 
                                dateObserver );
      
    TransformObserver::Pointer transformObserver = 
      TransformObserver::New();
    transformData->AddObserver( igstk::PrecomputedTransformData::TransformTypeEvent(), 
                                transformObserver );

    TransformErrorObserver::Pointer transformErrorObserver = 
      TransformErrorObserver::New();
    transformData->AddObserver( igstk::PrecomputedTransformData::TransformErrorTypeEvent(), 
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
      std::cout<<"Estimation error: "<<transformErrorObserver->GetTransformError()<<"\n";
      std::cout<<"Use this data [n,y]: ";
      std::cin>>response;
            //set the tool's calibration transform
      if(response == 'y') {
        transformData->RequestTransform();
        if( transformObserver->GotTransform() )
        {
          std::cout<<"Set calibration:\n"<<transformObserver->GetTransform()<<"\n";
        }
      }
    }
  }
  return EXIT_SUCCESS;
}
