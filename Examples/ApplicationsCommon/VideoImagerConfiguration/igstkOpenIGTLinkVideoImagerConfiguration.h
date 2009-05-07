/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkOpenIGTLinkVideoImagerConfiguration.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTerasonVideoImagerConfiguration_h
#define __igstkTerasonVideoImagerConfiguration_h

#include "igstkVideoImagerConfiguration.h"

namespace igstk
{


/**
 * \class TerasonToolConfiguration
 * A terason VideoImager tool.
 */
class TerasonToolConfiguration : public VideoImagerToolConfiguration
{
public:
  TerasonToolConfiguration();
  TerasonToolConfiguration(const TerasonToolConfiguration &other);
  virtual ~TerasonToolConfiguration();

  virtual std::string GetToolTypeAsString();

protected:

};

/**
 * \class TerasonVideoImagerConfiguration
 * Configuration for the Terason VideoImager.
 */
class TerasonVideoImagerConfiguration : public SocketCommunicatingVideoImagerConfiguration
{
public:
  TerasonVideoImagerConfiguration();
  virtual ~TerasonVideoImagerConfiguration();

  igstkSetMacro( BufferSize, unsigned int );
  igstkGetMacro( BufferSize, unsigned int );

  /**
   * Get the manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate();

protected:
  virtual void InternalAddTool( const VideoImagerToolConfiguration *tool );

private:

  //manufacturer specified maximal refresh rate [Hz]
  static const double MAXIMAL_REFERESH_RATE;

  unsigned int m_BufferSize;


};


} // end of name space
#endif
