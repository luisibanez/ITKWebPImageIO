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
#include "itkByteSwapper.h"
#include "itksys/SystemTools.hxx"

extern "C" {
#define VPX_CODEC_DISABLE_COMPAT 1
#include <vpx/vpx_decoder.h>
#include <vpx/vp8dx.h>
#define interface (&vpx_codec_vp8_dx_algo)
}

namespace itk
{

class WebPImageIOInternal
{
public:
  vpx_codec_ctx_t   m_Codec;
  char              m_Frame[256*1024];
  unsigned int      m_FrameCounter;
};

WebPImageIO::WebPImageIO()
{
  this->SetNumberOfDimensions(2); // WebP is 2D.
  this->SetNumberOfComponents(1); // WebP has three components (Isn't it ?), but here we just read the first one so far.

  this->m_Internal = new WebPImageIOInternal;
  this->m_Internal->m_FrameCounter = 0;

  this->AddSupportedReadExtension(".webp");

  this->AddSupportedWriteExtension(".webp");
}

WebPImageIO::~WebPImageIO()
{
  delete this->m_Internal;
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
  // Assume that WebP only reads 8-bits unsigned char images.
  // FIXME: We know that it also reads RGB and RGBA...
  //
  this->SetPixelType( SCALAR );
  this->SetComponentType( UCHAR );

  this->SetNumberOfDimensions(2);

  this->SetSpacing(0, 1.0);    // FIXME : Get the real pixel resolution.
  this->SetSpacing(1, 1.0);    // FIXME : Get the real pixel resolution.

  this->SetOrigin(0, 0.0);     // FIXME : Get the real coordinates of the origin.
  this->SetOrigin(1, 0.0);     // FIXME : Get the real coordinates of the origin.


  const unsigned int RIFF_TAG_SIZE  = 4;
  const unsigned int WEBP_TAG_SIZE  = 4;
  const unsigned int VP8_HDR_SZ  = 4;


  char    riff_tag[RIFF_TAG_SIZE];
  char    webp_tag[WEBP_TAG_SIZE];
  char    vp8_hdr[VP8_HDR_SZ];

  this->m_InputStream.open( this->m_FileName.c_str() );

  if( this->m_InputStream.fail() )
    {
    itkExceptionMacro("Failed to open " << this->m_FileName.c_str() << " for reading");
    }

  this->m_InputStream.read( riff_tag, RIFF_TAG_SIZE );

  if( !( riff_tag[0]=='R' &&
         riff_tag[1]=='I' &&
         riff_tag[2]=='F' &&
         riff_tag[3]=='F')   )
    {
    itkExceptionMacro(" " << this->m_FileName.c_str() << "is not a RIFF file.");
    }

  uint32_t riff_block_sz;

  this->ReadInteger32( &(this->m_InputStream), riff_block_sz );

  std::cout << "RIFF block size = " << riff_block_sz << std::endl;

  this->m_ExpectedRemainingNumberOfBytesToRead = riff_block_sz;

  this->m_InputStream.read( webp_tag, WEBP_TAG_SIZE );

  if( !( webp_tag[0]=='W' &&
         webp_tag[1]=='E' &&
         webp_tag[2]=='B' &&
         webp_tag[3]=='P')   )
    {
    itkExceptionMacro(" " << this->m_FileName.c_str() << "does not have an WEBP tag.");
    }

  this->m_ExpectedRemainingNumberOfBytesToRead -= 4;

  this->m_InputStream.read( vp8_hdr, VP8_HDR_SZ );

  if( !( vp8_hdr[0]=='V' &&
         vp8_hdr[1]=='P' &&
         vp8_hdr[2]=='8' &&
         vp8_hdr[3]==' ')   )
    {
    itkExceptionMacro(" " << this->m_FileName.c_str() << "does not have an VP8 tag.");
    }

  this->m_ExpectedRemainingNumberOfBytesToRead -= 4;

  int              flags = 0;

  if ( vpx_codec_dec_init(&(this->m_Internal->m_Codec), interface, NULL, flags ) )
    {
    const char *detail = vpx_codec_error_detail(&(this->m_Internal->m_Codec));
    itkExceptionMacro("Failed to initialize decoder \n" << detail);
    }

  std::cout << "Using " << vpx_codec_iface_name(interface) << std::endl;

  uint32_t frame_sz;

  this->ReadInteger32( &(this->m_InputStream), frame_sz );

  this->m_ExpectedRemainingNumberOfBytesToRead -= 4;

  std::cout << "Frame size = " << frame_sz << std::endl;

  // FIXME: I had to subtract one from frame_sz to fit the remaining of the file size... why ?
  frame_sz--;

  this->m_Internal->m_FrameCounter++;

  if ( frame_sz > sizeof(this->m_Internal->m_Frame) )
    {
    itkExceptionMacro("Frame " << frame_sz << " data too big for example code buffer");
    }

  //
  // Unfortunately, so far, we haven't found a way of reading the image data
  // without having to read the entire frame. Hopefully there is a more efficient
  // way of getting the values of image width and height. This must be revisited.
  //
  this->m_InputStream.read( this->m_Internal->m_Frame, frame_sz );

  std::cout << "Trying to read " << frame_sz << " bytes" << std::endl;
  std::cout << "Read " << this->m_InputStream.gcount() << " bytes " << std::endl;

  if( this->m_InputStream.gcount() != frame_sz )
    {
    itkExceptionMacro("Frame " << this->m_Internal->m_FrameCounter << "  failed to read complete frame");
    }

  this->m_ExpectedRemainingNumberOfBytesToRead -= frame_sz;

  std::cout << "this->m_ExpectedRemainingNumberOfBytesToRead = " << this->m_ExpectedRemainingNumberOfBytesToRead << std::endl;

  //
  // FIXME: The compiler fiercly opposed the cast from char * to  uint8_t *... why ?
  // The last resort solution was to use reinterpret_cast<>,
  // there is something suspicious about it...
  //
  const uint8_t * frame_p = reinterpret_cast< uint8_t *>( & (this->m_Internal->m_Frame[0]) );

  //
  // Again, here we decode the entire frame, despite the fact that we are just
  // looking for the values of image width and height. This must be revisited.
  //
  if( vpx_codec_decode(&(this->m_Internal->m_Codec), frame_p, frame_sz, NULL, 0))
    {
    const char *detail = vpx_codec_error_detail(&(this->m_Internal->m_Codec));
    itkExceptionMacro("Failed to decode frame\n" << detail);
    }

  vpx_codec_iter_t  iter = NULL;
  vpx_image_t      *img;

  if( ( img = vpx_codec_get_frame( &(this->m_Internal->m_Codec), &iter ) ) )
    {
    std::cout << "Image Size = " << std::endl;
    std::cout << "X = " << img->d_w << std::endl;
    std::cout << "Y = " << img->d_h << std::endl;
    this->SetDimensions(0,  img->d_w );
    this->SetDimensions(1,  img->d_h );
    }
  else
    {
    itkExceptionMacro("Problem found while getting one frame");
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

  std::cout << "Starting to read frames" << std::endl;
  std::cout << "Return of rdstate = " << this->m_InputStream.rdstate() << std::endl;

  unsigned char * destinationCharBuffer = (unsigned char *)buffer;

  //
  // Continue with the reading that we started in ReadImageInformation()
  //
  vpx_codec_iter_t  iter = NULL;
  vpx_image_t      *img;

  while( ( img = vpx_codec_get_frame( &(this->m_Internal->m_Codec), &iter ) ) )
    {
    std::cout << "Image Size = " << std::endl;
    std::cout << "X = " << img->d_w << std::endl;
    std::cout << "Y = " << img->d_h << std::endl;

    for ( unsigned int plane = 0; plane < 3; plane++ )
      {
      unsigned char * sourceCharBuffer = img->planes[plane];

      unsigned int plane_width  = img->d_w >> (plane?1:0);
      unsigned int plane_height = img->d_h >> (plane?1:0);

      std::cout << "plane = " << plane << std::endl;
      std::cout << "plane_width  = " << plane_width  << std::endl;
      std::cout << "plane_height = " << plane_height << std::endl;


      for ( unsigned int y = 0; y < plane_height; y++)
        {
        memcpy( destinationCharBuffer, sourceCharBuffer, plane_width );
        sourceCharBuffer += img->stride[plane];
        destinationCharBuffer += plane_width;
        }
      }
    }


  //
  //   Now, see if there are more frames
  //
  //   ...Should we make a 3D image with them ?
  //

  while( this->m_ExpectedRemainingNumberOfBytesToRead > 1 )  // FIXME, it should be 0, this is due to the mismatch of the frame_sz by 1.
    {
    uint32_t frame_sz;

    this->ReadInteger32( &(this->m_InputStream), frame_sz );

    this->m_ExpectedRemainingNumberOfBytesToRead -= 4;

    std::cout << "Frame size = " << frame_sz << std::endl;

    // FIXME: I had to subtract one from frame_sz to fit the remaining of the file size... why ?
    frame_sz--;


    this->m_Internal->m_FrameCounter++;

    if ( frame_sz > sizeof(this->m_Internal->m_Frame) )
      {
      itkExceptionMacro("Frame " << frame_sz << " data too big for example code buffer");
      }

    this->m_InputStream.read( this->m_Internal->m_Frame, frame_sz );

    std::cout << "Trying to read " << frame_sz << " bytes" << std::endl;
    std::cout << "Read " << this->m_InputStream.gcount() << " bytes " << std::endl;

    if( this->m_InputStream.gcount() != frame_sz )
      {
      itkExceptionMacro("Frame " << this->m_Internal->m_FrameCounter << "  failed to read complete frame");
      }

    this->m_ExpectedRemainingNumberOfBytesToRead -= frame_sz;

    std::cout << "this->m_ExpectedRemainingNumberOfBytesToRead = " << this->m_ExpectedRemainingNumberOfBytesToRead << std::endl;

    //
    // FIXME: The compiler fiercly opposed the cast from char * to  uint8_t *... why ?
    // The last resort solution was to use reinterpret_cast<>,
    // there is something suspicious about it...
    //
    const uint8_t * frame_p = reinterpret_cast< uint8_t *>( & (this->m_Internal->m_Frame[0]) );

    if( vpx_codec_decode(&(this->m_Internal->m_Codec), frame_p, frame_sz, NULL, 0))
      {
      const char *detail = vpx_codec_error_detail(&(this->m_Internal->m_Codec));
      itkExceptionMacro("Failed to decode frame\n" << detail);
      }

    while( ( img = vpx_codec_get_frame( &(this->m_Internal->m_Codec), &iter ) ) )
      {
      std::cout << "Image Size = " << std::endl;
      std::cout << "X = " << img->d_w << std::endl;
      std::cout << "Y = " << img->d_h << std::endl;

      for ( unsigned int plane = 0; plane < 3; plane++ )
        {
        unsigned char *buf = img->planes[plane];

        unsigned int plane_width  = img->d_w >> (plane?1:0);
        unsigned int plane_height = img->d_h >> (plane?1:0);

        std::cout << "plane = " << plane << std::endl;
        std::cout << "plane_width  = " << plane_width  << std::endl;
        std::cout << "plane_height = " << plane_height << std::endl;

        for ( unsigned int y = 0; y < plane_height; y++)
          {
//           fwrite(buf, 1, img->d_w >> (plane?1:0), outfile);
          buf += img->stride[plane];
          }
       }
     }
   }  // next frame

  if ( vpx_codec_destroy(&(this->m_Internal->m_Codec)) )
    {
    const char *detail = vpx_codec_error_detail(&(this->m_Internal->m_Codec));
    itkExceptionMacro("Failed to destroy codec\n" << detail );
    }

  this->m_InputStream.close();

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


void
WebPImageIO
::ReadInteger32( std::ifstream * inputStream, uint32_t & valueToRead )
{
  if( inputStream )
    {
    inputStream->read( (char *)(&valueToRead), sizeof(valueToRead) );
    ByteSwapper<uint32_t>::SwapFromSystemToLittleEndian( &valueToRead );
    }
}


} // end namespace itk
