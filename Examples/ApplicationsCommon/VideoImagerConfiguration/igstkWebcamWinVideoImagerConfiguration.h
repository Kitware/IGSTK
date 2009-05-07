/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkWebcamWinVideoImagerConfiguration.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkWebcamWinVideoImagerConfiguration_h
#define __igstkWebcamWinVideoImagerConfiguration_h

#include "igstkVideoImagerConfiguration.h"

namespace igstk
{


/**
 * \class WebcamWinToolConfiguration
 * A WebcamWin VideoImager tool.
 */
class WebcamWinToolConfiguration : public VideoImagerToolConfiguration
{
public:
  WebcamWinToolConfiguration();
  WebcamWinToolConfiguration(const WebcamWinToolConfiguration &other);
  virtual ~WebcamWinToolConfiguration();

  virtual std::string GetToolTypeAsString();

protected:

};


/**
 * \class WebcamWinVideoImagerConfiguration
 * Configuration for the WebcamWin VideoImager.
 */
class WebcamWinVideoImagerConfiguration : public VideoImagerConfiguration
{
public:
  WebcamWinVideoImagerConfiguration();
  virtual ~WebcamWinVideoImagerConfiguration();

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
