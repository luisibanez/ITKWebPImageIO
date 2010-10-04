/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkWebPImageIO.h,v $
  Language:  C++
  Date:      $Date: 2007/03/22 14:28:51 $
  Version:   $Revision: 1.31 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkWebPImageIO_h
#define __itkWebPImageIO_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <fstream>
#include "itkImageIOBase.h"
#include "itkIntTypes.h"

namespace itk
{

class WebPImageIOInternal;

/** \class WebPImageIO
 *
 *  \brief Read WebPImage file format.
 *
 *  \ingroup IOFilters
 */
class ITK_EXPORT WebPImageIO : public ImageIOBase
{
public:
  /** Standard class typedefs. */
  typedef WebPImageIO        Self;
  typedef ImageIOBase        Superclass;
  typedef SmartPointer<Self> Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(WebPImageIO, ImageIOBase);

  /*-------- This part of the interfaces deals with reading data. ----- */

  /** Determine the file type. Returns true if this ImageIO can read the
   * file specified. */
  virtual bool CanReadFile(const char*);

  /** Set the spacing and dimension information for the set filename. */
  virtual void ReadImageInformation();

  /** Reads the data from disk into the memory buffer provided. */
  virtual void Read(void* buffer);

  /*-------- This part of the interfaces deals with writing data. ----- */

  /** Determine the file type. Returns true if this ImageIO can write the
   * file specified. */
  virtual bool CanWriteFile(const char*);

  /** Set the spacing and dimension information for the set filename. */
  virtual void WriteImageInformation();

  /** Writes the data to disk from the memory buffer provided. Make sure
   * that the IORegions has been set properly. */
  virtual void Write(const void* buffer);


protected:
  WebPImageIO();
  ~WebPImageIO();
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  WebPImageIO(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // Read a four byte integer in binary regardless of endianess.
  void ReadInteger32( std::ifstream * inputStream, uint32_t & valueToRead );

  std::ifstream     m_InputStream;
  std::ofstream     m_OutputStream;

  uint32_t          m_ExpectedRemainingNumberOfBytesToRead;

  WebPImageIOInternal *  m_Internal;
};

} // end namespace itk

#endif // __itkWebPImageIO_h
