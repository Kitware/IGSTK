/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkBinaryDataTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include "igstkBinaryData.h"


int igstkBinaryDataTest( int, char * [] )
{
  try
    {
    std::cout << "Testing igstk::BinaryData" << std::endl;

    /* creating binary data objects */
    igstk::BinaryData data;
    igstk::BinaryData data2;
    igstk::BinaryData data3;
    
    /* creating an array */
    unsigned char buffer[100];

    /* allocating memory and assigning data */
    data.SetSize(11);
    data2.SetSize(21);
    unsigned int i;
    for( i = 0; i < 10; ++i )
      {
      data[i] = (unsigned char)i + '0';
      }
    data[10] = 0;
    for( i = 0; i < 20; ++i )
      {
      data2[i] = (unsigned char)i + '0';
      }
    data2[20] = 0;

    /* copy data into an array */
    data2.CopyTo(buffer);

    /* print objects out */
    std::cout << data << std::endl;
    std::cout << data2 << std::endl;
    std::cout << "buffer: " << buffer << std::endl;

    /* copy data from an array */
    data3.CopyFrom(buffer, data2.GetSize());
    std::cout << data3 << std::endl;

    /* get container objects */
    const igstk::BinaryData::ContainerType& obj = data.GetData();
    igstk::BinaryData::ContainerType& obj2 = data.GetData();

    /* comparison operator tests */
    if( data == data2 )
      {
      std::cerr << "operator== failed !!" << std::endl;
      return EXIT_FAILURE;
      }

    if( data != data2 )
      {
      }
    else
      {
      std::cerr << "operator!= failed !!" << std::endl;
      return EXIT_FAILURE;
      }

    if( data < data2 )
      {
      }
    else
      {
      std::cerr << "operator< failed !!" << std::endl;
      return EXIT_FAILURE;
      }
    }
  catch(...)
    {
    std::cerr << "Exception catched !!" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}


