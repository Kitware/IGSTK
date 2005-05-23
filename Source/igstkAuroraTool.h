#ifndef __igstk_AuroraTool_h_
#define __igstk_AuroraTool_h_

#include "igstkTrackerTool.h"

namespace igstk
{
/** \class AuroraTool
    \brief Implementation of the Aurora Tool class.

*/

class AuroraTool : public igstk::TrackerTool
{
public:
  AuroraTool();

  ~AuroraTool() {};

  typedef AuroraTool                    Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(AuroraTool, ::itk::Object);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  


private:
  char *ToolType;
  char *ToolRevision;
  char *ToolSerialNumber;
  char *ToolPartNumber;
  char *ToolManufacturer;
};  


} // namespace igstk


#endif  // __igstk_AuroraTool_h_

