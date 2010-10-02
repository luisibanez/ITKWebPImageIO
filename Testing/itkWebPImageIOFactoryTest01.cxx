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


#include "itkWebPImageIOFactory.h"


int main( int argc, char ** argv )
{
  //  Register the factory
  itk::WebPImageIOFactory::RegisterOneFactory();

  itk::WebPImageIOFactory::Pointer factory =
    itk::WebPImageIOFactory::New();

  std::cout << "ITK Version = " << factory->GetITKSourceVersion() << std::endl;
  std::cout << "Description = " << factory->GetDescription() << std::endl;

  std::cout << "ClassName = " << factory->GetNameOfClass() << std::endl;

  itk::WebPImageIOFactory::Pointer factory2 = itk::WebPImageIOFactory::FactoryNew();

  return EXIT_SUCCESS;
}
