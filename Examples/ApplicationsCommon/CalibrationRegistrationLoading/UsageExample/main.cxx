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
      std::cerr<<"Failed reading: "<<rfe->Get()<<"\n";
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

typedef igstk::PrecomputedTransformData::TransformType *  TransformTypePointer;

igstkObserverMacro( TransformRequest, 
                    igstk::PrecomputedTransformData::TransformTypeEvent, 
                    TransformTypePointer )

igstkObserverMacro( TransformError, 
                    igstk::PrecomputedTransformData::TransformErrorTypeEvent, 
                    igstk::PrecomputedTransformData::ErrorType )

/**
 * This program recieves an xml file containing a precomputed
 * transformation, reads it, presents the user with the general information 
 * about the transformation (when computed and estimation error). The user is
 * then asked if to use the transformation or not. If the answer is yes, the
 * program prints the transformation and exits, otherwise it just exits.
 *
 */
int main(int argc, char *argv[])
{
  const unsigned int NUM_TRANSFORM_TYPES = 3;
  std::string transformTypes[NUM_TRANSFORM_TYPES]; 
  enum { RIGID_TRANSFORM, AFFINE_TRANSFORM, PERSPECTIVE_TRANSFORM };
  transformTypes[RIGID_TRANSFORM] = "rigid";
  transformTypes[AFFINE_TRANSFORM] = "affine";
  transformTypes[PERSPECTIVE_TRANSFORM] = "perspective";

  if( argc!= 3 )
  {
    std::cerr<<"Wrong number of arguments.\n";
    std::cerr<<"Usage: "<<argv[0]<<" transformation_type ";
    std::cerr<<"transformation_data_file_name\n";
    std::cerr<<"Transformation types:\n";
    for( unsigned int i=0; i<NUM_TRANSFORM_TYPES; i++ )
      std::cerr<<"\t"<<i<<" == "<<transformTypes[i]<<"\n";
    return EXIT_FAILURE;
  }

  igstk::PrecomputedTransformData::Pointer transformData;
                                  //our reader
  igstk::TransformFileReader::Pointer transformFileReader = 
    igstk::TransformFileReader::New();
                  //set our reader to read a rigid transformation from the 
                  //given file
  igstk::TransformXMLFileReaderBase::Pointer xmlFileReader;
       //assumes the second argument is an integer, not checking for junk input
  switch( atoi( argv[1] ) )
  {
  case RIGID_TRANSFORM:
    xmlFileReader = igstk::RigidTransformXMLFileReader::New();
    break;
  case AFFINE_TRANSFORM:
    xmlFileReader = igstk::AffineTransformXMLFileReader::New();
    break;
  case PERSPECTIVE_TRANSFORM:
    xmlFileReader = igstk::PerspectiveTransformXMLFileReader::New();
    break;
  default:
    std::cerr<<"Given transfromation type ("<<argv[1]<<") is invalid.\n";
    std::cerr<<"Transformation types:\n";
    for( unsigned int i=0; i<NUM_TRANSFORM_TYPES; i++ )
      std::cerr<<"\t"<<i<<" == "<<transformTypes[i]<<"\n";
    return EXIT_FAILURE;
  }

  transformFileReader->RequestSetReader( xmlFileReader );
  transformFileReader->RequestSetFileName( argv[2] );

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
