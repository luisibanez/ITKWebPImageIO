/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkWebPImageIO.cxx,v $
  Language:  C++
  Date:      $Date: 2007/03/29 18:39:28 $
  Version:   $Revision: 1.69 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include "itkExceptionObject.h"
#include "itkIOCommon.h"
#include "itkWebPImageIO.h"
#include "itksys/SystemTools.hxx"

namespace itk
{

WebPImageIO::WebPImageIO()
{
  this->SetNumberOfDimensions(2); // WebP is 2D.
  this->SetNumberOfComponents(1); // WebP only has one component.
}

WebPImageIO::~WebPImageIO()
{
}

void WebPImageIO::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

bool WebPImageIO::CanReadFile( const char* filename )
{
  itkDebugMacro("WebPImageIO::CanReadFile() ");
  //
  // If the file exists, and have extension .webp, then we are good to read it.
  //
  if( !itksys::SystemTools::FileExists( filename ) )
    {
    itkDebugMacro("File doesn't exist");
    return false;
    }

  if( itksys::SystemTools::GetFilenameLastExtension( filename ) != ".webp" )
    {
    itkDebugMacro("Wrong extension");
    return false;
    }

  return true;
}


void WebPImageIO::ReadImageInformation()
{
  // WebP only reads 8-bits unsigned char images.
  this->SetPixelType( SCALAR );
  this->SetComponentType( UCHAR );

  this->m_InputStream.open( this->m_FileName.c_str() );

  if( this->m_InputStream.fail() )
    {
    itkExceptionMacro("Failed to open file " << this->m_InputStream );
    }
}


void WebPImageIO::Read( void * buffer)
{
  itkDebugMacro("WebPImageIO::Read() Begin");

  const unsigned int nx = this->GetDimensions( 0 );
  const unsigned int ny = this->GetDimensions( 1 );

  ImageIORegion regionToRead = this->GetIORegion();

  ImageIORegion::SizeType  size  = regionToRead.GetSize();
  ImageIORegion::IndexType start = regionToRead.GetIndex();

  const unsigned int mx = size[0];
  const unsigned int my = size[1];

  const unsigned int sx = start[0];
  const unsigned int sy = start[1];

  std::cout << "largest    region size  = " << nx << " " << ny << " " << std::endl;
  std::cout << "streamable region size  = " << mx << " " << my << " " << std::endl;
  std::cout << "streamable region begin = " << sx << " " << sy << " " << std::endl;

  itkDebugMacro("WebPImageIO::Read() End");
}


bool WebPImageIO::CanWriteFile( const char * name )
{
  //
  // WebP is not affraid of writing either !!
  //
  return true;
}


void
WebPImageIO
::WriteImageInformation(void)
{
  // add writing here
}


/**
 *
 */
void
WebPImageIO
::Write( const void* buffer)
{
}

/** Given a requested region, determine what could be the region that we can
 * read from the file. This is called the streamable region, which will be
 * smaller than the LargestPossibleRegion and greater or equal to the
RequestedRegion */
ImageIORegion
WebPImageIO
::GenerateStreamableReadRegionFromRequestedRegion( const ImageIORegion & requested ) const
{
  std::cout << "WebPImageIO::GenerateStreamableReadRegionFromRequestedRegion()" << std::endl;
  std::cout << "Requested region = " << requested << std::endl;
  //
  // WebP is the ultimate streamer.
  //
  ImageIORegion streamableRegion = requested;

  std::cout << "StreamableRegion = " << streamableRegion << std::endl;

  return streamableRegion;
}


} // end namespace itk
