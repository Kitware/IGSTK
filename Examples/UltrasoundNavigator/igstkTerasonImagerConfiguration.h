/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkTerasonImagerConfiguration.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTerasonImagerConfiguration_h
#define __igstkTerasonImagerConfiguration_h

#include "igstkImagerConfiguration.h"

namespace igstk
{


/**
 * A terason imager tool.
 */
class TerasonToolConfiguration : public ImagerToolConfiguration
{
public:
  TerasonToolConfiguration();
  TerasonToolConfiguration(const TerasonToolConfiguration &other);
  virtual ~TerasonToolConfiguration();

  virtual std::string GetToolTypeAsString();

protected:

};


/**
 * Configuration for the Terason imager.
 */
class TerasonImagerConfiguration : public SocketCommunicatingImagerConfiguration
{
public:
  TerasonImagerConfiguration();
  virtual ~TerasonImagerConfiguration();

  igstkSetMacro( BufferSize, unsigned int );
  igstkGetMacro( BufferSize, unsigned int );

  /**
   * Get the manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate();

protected:
  virtual void InternalAddTool( const ImagerToolConfiguration *tool );

private:

                  //manufacturer specified maximal refresh rate [Hz]
  static const double MAXIMAL_REFERESH_RATE;

  unsigned int m_BufferSize;


};


} // end of name space
#endif
