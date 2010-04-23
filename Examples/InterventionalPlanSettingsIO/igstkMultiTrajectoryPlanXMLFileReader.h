#ifndef __igstkMultiTrajectoryPlanXMLFileReader_h
#define __igstkMultiTrajectoryPlanXMLFileReader_h

#include "igstkInterventionalPlanXMLFileReaderBase.h"
#include "igstkTrajectory.h"

namespace igstk
{
/** \class MultiTrajectoryPlanXMLFileReader
 * 
 *  \brief This class is a reader for xml files containing a 
 *         multi trajectory intreventional plan. 
 *
 *  The xml file format is as follows:
    @verbatim
    <InterventionalPlan PatientName="Liver24" 
                        StudyInstanceUID="1.2.840.113696.563627.530.570770.20071104103124" 
                        SeriesInstanceUID="1.3.12.2.1107.5.1.4.24515.4.0.12150429059722404">
    <trajectory_list>
         <!-- a trajectory consists of a direction, outside of patient towards inside -->
      <trajectory>
        <direction>
            0.7247   -0.0569   -0.6867
        </direction>
        <entry_point>
           -165.672 -151.672 -101
        </entry_point>
        <target>
            -124.9060 -154.8710 -139.6260
        </target>
        <target>
            -122.1880 -155.0840 -142.2010
        </target>
      </trajectory>

      <trajectory>
        <direction>
           0.7514    0.1600   -0.6401 
        </direction>
        <entry_point>
        -165.672 -151.672 -101
        </entry_point>
        <target>
            -127.9130 -143.6300 -133.1650 
        </target>
        <target>
           -114.3870 -140.7490 -144.6870  
        </target>
      </trajectory>
    </trajectory_list>
    </InterventionalPlan>
   @endverbatim
 *
 * 
 */
class MultiTrajectoryPlanXMLFileReader : public InterventionalPlanXMLFileReaderBase 
{
public:

  //standard typedefs
  typedef MultiTrajectoryPlanXMLFileReader       Self;
  typedef InterventionalPlanXMLFileReaderBase    Superclass;
  typedef itk::SmartPointer<Self>                Pointer;

           //run-time type information (and related methods)
  itkTypeMacro( MultiTrajectoryPlanXMLFileReader, InterventionalPlanXMLFileReaderBase );

           //method for creation through the object factory
  itkNewMacro( Self );

  /**
   * Method called when a new xml tag start is encountered.
   */
  virtual void StartElement( const char * name, const char **atts ); 

  /**
   * Method called when an xml tag end is encountered.
   */
  virtual void EndElement( const char *name ); 

  /**
   * Return a pointer to the plan. If no plan has been read then return NULL.
   * The caller is responsible for releasing this memory via delete.
   */
  virtual igstk::InterventionalPlan * GetPlan();

protected:
          //this is the constructor that is called by the factory to 
         //create a new object
  MultiTrajectoryPlanXMLFileReader() :  InterventionalPlanXMLFileReaderBase(),
                                        m_ReadingTrajectoryList( false ), 
                                        m_ReadingTrajectory( false ),                                        
                                        m_HaveEntryPoint( false ),
                                        m_HaveDirection( false )
  {}
  virtual ~MultiTrajectoryPlanXMLFileReader() {}

  bool m_ReadingTrajectoryList;
  bool m_ReadingTrajectory;
  
  bool m_HaveEntryPoint;
  bool m_HaveDirection;
  Trajectory::PointType m_EntryPoint;
  Trajectory::DirectionType m_Direction;
  std::vector<Trajectory::PointType> m_Targets;

  std::vector<Trajectory> m_Trajectories;

private:
  MultiTrajectoryPlanXMLFileReader( 
    const MultiTrajectoryPlanXMLFileReader & other );


  const MultiTrajectoryPlanXMLFileReader & operator=( 
    const MultiTrajectoryPlanXMLFileReader & right );

  void ProcessTrajectoryList();
  void ProcessTrajectory() throw ( FileFormatException );
  void ProcessEntryPoint() throw ( FileFormatException );
  void ProcessDirection()  throw ( FileFormatException );
  void ProcessTarget() throw ( FileFormatException );

};


}
#endif //__igstkMultiTrajectoryPlanXMLFileReader_h
