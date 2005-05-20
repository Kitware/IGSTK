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
  ~AuroraTool();


private:
  char *ToolType;
  char *ToolRevision;
  char *ToolSerialNumber;
  char *ToolPartNumber;
  char *ToolManufacturer;
};  


} // namespace igstk


#endif  // __igstk_AuroraTool_h_

