/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "itkWebPImageIO.h"


int main( int argc, char ** argv )
{
  itk::WebPImageIO::Pointer imageIO = itk::WebPImageIO::New();

  std::cout << "ClassName = " << imageIO->GetNameOfClass() << std::endl;

  imageIO->Print( std::cout );

  return EXIT_SUCCESS;
}



