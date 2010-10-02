/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkWebPImageIOFactory.h"
#include "itkCreateObjectFunction.h"
#include "itkWebPImageIO.h"
#include "itkVersion.h"


namespace itk
{
WebPImageIOFactory::WebPImageIOFactory()
{
  this->RegisterOverride("itkImageIOBase",
                         "itkWebPImageIO",
                         "WebP Image IO",
                         1,
                         CreateObjectFunction<WebPImageIO>::New());
}

WebPImageIOFactory::~WebPImageIOFactory()
{
}

const char*
WebPImageIOFactory::GetITKSourceVersion() const
{
  return ITK_SOURCE_VERSION;
}

const char*
WebPImageIOFactory::GetDescription() const
{
  return "WebP ImageIO Factory, allows the loading of WebP images into insight";
}

//
// Entry point function for ITK to invoke, in order to create a new instance of
// a factory.
//
extern "C"
#ifdef _WIN32
__declspec( dllexport )
#endif
itk::ObjectFactoryBase* itkLoad()
{
  std::cout << "Calling WebPImageIO itkLoad()" << std::endl;
  return itk::WebPImageIOFactory::FactoryNew();
}


} // end namespace itk
