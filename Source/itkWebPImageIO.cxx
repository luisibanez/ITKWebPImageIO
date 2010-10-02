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

extern "C" {
#define VPX_CODEC_DISABLE_COMPAT 1
#include <vpx/vpx_decoder.h>
#include <vpx/vp8dx.h>
#define interface (&vpx_codec_vp8_dx_algo)
}

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

  const unsigned int RIFF_FILE_HDR_SZ  = 12;
  const unsigned int VP8_HDR_SZ  = 4;
  const unsigned int IVF_FRAME_HDR_SZ  = 12;

  char    frame_hdr[IVF_FRAME_HDR_SZ];
  char    frame[256*1024];

  std::ifstream    infile;

  char             file_hdr[RIFF_FILE_HDR_SZ];
  char             vp8_hdr[VP8_HDR_SZ];

  infile.open( this->m_FileName.c_str() );

  if( infile.fail() )
    {
    itkExceptionMacro("Failed to open " << this->m_FileName.c_str() << " for reading");
    }

  /* Read file header */
  infile.read( file_hdr, RIFF_FILE_HDR_SZ );

  if( !( file_hdr[0]=='R' &&
         file_hdr[1]=='I' &&
         file_hdr[2]=='F' &&
         file_hdr[3]=='F')   )
    {
    itkExceptionMacro(" " << this->m_FileName.c_str() << "is not a RIFF file.");
    }

  if( !( file_hdr[ 8]=='W' &&
         file_hdr[ 9]=='E' &&
         file_hdr[10]=='B' &&
         file_hdr[11]=='P')   )
    {
    itkExceptionMacro(" " << this->m_FileName.c_str() << "does not have an WEBP tag.");
    }

  vpx_codec_ctx_t  codec;
  int              flags = 0;
  int              frame_cnt = 0;

  if(vpx_codec_dec_init(&codec, interface, NULL, flags))
    {
    itkExceptionMacro("Failed to initialize decoder");
    }


  infile.read( vp8_hdr, VP8_HDR_SZ );

  if( !( vp8_hdr[0]=='V' &&
         vp8_hdr[1]=='P' &&
         vp8_hdr[2]=='8' &&
         vp8_hdr[3]==' ')   )
    {
    itkExceptionMacro(" " << this->m_FileName.c_str() << "does not have an VP8 tag.");
    }


  std::cout << "Using " << vpx_codec_iface_name(interface) << std::endl;

  // check for eofbit or failbit
  infile.read(frame_hdr, IVF_FRAME_HDR_SZ );

  while( infile.rdstate() & std::ifstream::failbit )
    {
    unsigned int  frame_sz = 1000; // FIXME mem_get_le32(frame_hdr);
    vpx_codec_iter_t  iter = NULL;
    vpx_image_t      *img;

    frame_cnt++;

    if(frame_sz > sizeof(frame))
      {
      itkExceptionMacro("Frame " << frame_sz << " data too big for example code buffer");
      }

    infile.read( frame, frame_sz );

    if( infile.rdstate() & std::ifstream::failbit )
      {
      itkExceptionMacro("Frame " << frame_cnt << "  failed to read complete frame");
      }

    void * frame_p = (void *)( frame );

    if( vpx_codec_decode(&codec, frame_p, frame_sz, NULL, 0))
      {
      itkExceptionMacro("Failed to decode frame");
      }

    while( ( img = vpx_codec_get_frame( &codec, &iter ) ) )
      {
      }

    }

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
