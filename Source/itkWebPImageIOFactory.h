/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkWebPImageIOFactory_h
#define __itkWebPImageIOFactory_h

#include "itkObjectFactoryBase.h"
#include "itkImageIOBase.h"

namespace itk
{
/** \class WebPImageIOFactory
 * \brief WebP : Reader for the WebP file format based on VP8.

   This is the library distributed by Google for supporting
   faster transmission of images on the Web.

 */
class ITK_EXPORT WebPImageIOFactory : public ObjectFactoryBase
{
public:
  /** Standard class typedefs. */
  typedef WebPImageIOFactory       Self;
  typedef ObjectFactoryBase        Superclass;
  typedef SmartPointer<Self>       Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Class methods used to interface with the registered factories. */
  virtual const char* GetITKSourceVersion() const;
  virtual const char* GetDescription() const;

  /** Method for class instantiation. */
  itkFactorylessNewMacro(Self);
  static WebPImageIOFactory* FactoryNew() { return new WebPImageIOFactory;}

  /** Run-time type information (and related methods). */
  itkTypeMacro(WebPImageIOFactory, ObjectFactoryBase);

  /** Register one factory of this type  */
  static void RegisterOneFactory()
    {
    WebPImageIOFactory::Pointer metaFactory = WebPImageIOFactory::New();
    ObjectFactoryBase::RegisterFactory(metaFactory);
    }

protected:
  WebPImageIOFactory();
  ~WebPImageIOFactory();

private:
  WebPImageIOFactory(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk

#endif
