#ifndef __igstkMultiTrajectoryPlanXMLFileWriter_h
#define __igstkMultiTrajectoryPlanXMLFileWriter_h

#include "igstkInterventionalPlanXMLFileWriterBase.h"


namespace igstk
{
/** \class MultiTrajectoryPlanXMLFileWriter
 * 
 *  \brief This class writes an xml file containing the given multi trajectory 
 *         intreventional plan. 
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
class MultiTrajectoryPlanXMLFileWriter : public InterventionalPlanXMLFileWriterBase 
{
public:

  //standard typedefs
  typedef MultiTrajectoryPlanXMLFileWriter       Self;
  typedef InterventionalPlanXMLFileWriterBase    Superclass;
  typedef itk::SmartPointer<Self>                Pointer;

           //run-time type information (and related methods)
  itkTypeMacro( MultiTrajectoryPlanXMLFileWriter, InterventionalPlanXMLFileWriterBase );

           //method for creation through the object factory
  itkNewMacro( Self );

protected:
          //this is the constructor that is called by the factory to 
         //create a new object
  MultiTrajectoryPlanXMLFileWriter(){}
  virtual ~MultiTrajectoryPlanXMLFileWriter() {}

  virtual int WritePlan( std::ofstream &out );

private:
  void WriteTrajectory( Trajectory &trajectory, std::ofstream &out );

  MultiTrajectoryPlanXMLFileWriter( 
    const MultiTrajectoryPlanXMLFileWriter & other );


  const MultiTrajectoryPlanXMLFileWriter & operator=( 
    const MultiTrajectoryPlanXMLFileWriter & right );


};


}
#endif //__igstkMultiTrajectoryPlanXMLFileWriter_h
