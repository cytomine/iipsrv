/*
    IIP Environment Variable Class

    Copyright (C) 2006-2018 Ruven Pillay.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef _ENVIRONMENT_H
#define _ENVIRONMENT_H

#define NO_FILTER_DEFINED -999;

/* Define some default values
 */
#define VERBOSITY 1
#define LOGFILE "/tmp/iipsrv.log"
#define MAX_IMAGE_CACHE_SIZE 10.0
#define FILENAME_PATTERN "_pyr_"
#define JPEG_QUALITY 75
#define MAX_CVT 5000
#define MAX_LAYERS 0
#define FILESYSTEM_PREFIX ""
#define WATERMARK ""
#define WATERMARK_PROBABILITY 1.0
#define WATERMARK_OPACITY 1.0
#define LIBMEMCACHED_SERVERS "localhost"
#define LIBMEMCACHED_TIMEOUT 86400  // 24 hours
#define INTERPOLATION 1  // 1: Bilinear
#define CORS "";
#define BASE_URL "";
#define CACHE_CONTROL "max-age=86400"; // 24 hours
#define ALLOW_UPSCALING true
#define URI_MAP ""
#define EMBED_ICC true
#define KAKADU_READMODE 0


#include <string>


/// Class to obtain environment variables
class Environment {


 public:

  static int getVerbosity(){
    int loglevel = VERBOSITY;
    char *envpara = getenv( "VERBOSITY" );
    if( envpara ){
      loglevel = atoi( envpara );
      // If not a realistic level, set to zero
      if( loglevel < 0 ) loglevel = 0;
    }
    return loglevel;
  }


  static std::string getLogFile(){
    char* envpara = getenv( "LOGFILE" );
    if( envpara ) return std::string( envpara );
    else return LOGFILE;
  }


  static float getMaxImageCacheSize(){
    float max_image_cache_size = MAX_IMAGE_CACHE_SIZE;
    char* envpara = getenv( "MAX_IMAGE_CACHE_SIZE" );
    if( envpara ){
      max_image_cache_size = atof( envpara );
    }
    return max_image_cache_size;
  }


  static std::string getFileNamePattern(){
    char* envpara = getenv( "FILENAME_PATTERN" );
    std::string filename_pattern;
    if( envpara ){
      filename_pattern = std::string( envpara );
    }
    else filename_pattern = FILENAME_PATTERN;

    return filename_pattern;
  }


  static int getJPEGQuality(){
    char* envpara = getenv( "JPEG_QUALITY" );
    int jpeg_quality;
    if( envpara ){
      jpeg_quality = atoi( envpara );
      if( jpeg_quality > 100 ) jpeg_quality = 100;
      if( jpeg_quality < 1 ) jpeg_quality = 1;
    }
    else jpeg_quality = JPEG_QUALITY;

    return jpeg_quality;
  }


  static int getMaxCVT(){
    char* envpara = getenv( "MAX_CVT" );
    int max_CVT;
    if( envpara ){
      max_CVT = atoi( envpara );
      if( max_CVT < 64 ) max_CVT = 64;
    }
    else max_CVT = MAX_CVT;

    return max_CVT;
  }


  static int getMaxLayers(){
    char* envpara = getenv( "MAX_LAYERS" );
    int layers;
    if( envpara ) layers = atoi( envpara );
    else layers = MAX_LAYERS;

    return layers;
  }


  static std::string getFileSystemPrefix(){
    char* envpara = getenv( "FILESYSTEM_PREFIX" );
    std::string filesystem_prefix;
    if( envpara ){
      filesystem_prefix = std::string( envpara );
    }
    else filesystem_prefix = FILESYSTEM_PREFIX;

    return filesystem_prefix;
  }


  static std::string getWatermark(){
    char* envpara = getenv( "WATERMARK" );
    std::string watermark;
    if( envpara ){
      watermark = std::string( envpara );
    }
    else watermark = WATERMARK;

    return watermark;
  }


  static float getWatermarkProbability(){
    float watermark_probability = WATERMARK_PROBABILITY;
    char* envpara = getenv( "WATERMARK_PROBABILITY" );

    if( envpara ){
      watermark_probability = atof( envpara );
      if( watermark_probability > 1.0 ) watermark_probability = 1.0;
      if( watermark_probability < 0 ) watermark_probability = 0.0;
    }

    return watermark_probability;
  }


  static float getWatermarkOpacity(){
    float watermark_opacity = WATERMARK_OPACITY;
    char* envpara = getenv( "WATERMARK_OPACITY" );

    if( envpara ){
      watermark_opacity = atof( envpara );
      if( watermark_opacity > 1.0 ) watermark_opacity = 1.0;
      if( watermark_opacity < 0 ) watermark_opacity = 0.0;
    }

    return watermark_opacity;
  }


  static std::string getMemcachedServers(){
    char* envpara = getenv( "MEMCACHED_SERVERS" );
    std::string memcached_servers;
    if( envpara ){
      memcached_servers = std::string( envpara );
    }
    else memcached_servers = LIBMEMCACHED_SERVERS;

    return memcached_servers;
  }


  static unsigned int getMemcachedTimeout(){
    char* envpara = getenv( "MEMCACHED_TIMEOUT" );
    unsigned int memcached_timeout;
    if( envpara ) memcached_timeout = atoi( envpara );
    else memcached_timeout = LIBMEMCACHED_TIMEOUT;

    return memcached_timeout;
  }


  static unsigned int getInterpolation(){
    char* envpara = getenv( "INTERPOLATION" );
    unsigned int interpolation;
    if( envpara ) interpolation = atoi( envpara );
    else interpolation = INTERPOLATION;

    return interpolation;
  }


  static std::string getCORS(){
    char* envpara = getenv( "CORS" );
    std::string cors;
    if( envpara ) cors = std::string( envpara );
    else cors = CORS;
    return cors;
  }


  static std::string getBaseURL(){
    char* envpara = getenv( "BASE_URL" );
    std::string base_url;
    if( envpara ) base_url = std::string( envpara );
    else base_url = BASE_URL;
    return base_url;
  }


  static std::string getCacheControl(){
    char* envpara = getenv( "CACHE_CONTROL" );
    std::string cache_control;
    if( envpara ) cache_control = std::string( envpara );
    else cache_control = CACHE_CONTROL;
    return cache_control;
  }


  static bool getAllowUpscaling(){
    char* envpara = getenv( "ALLOW_UPSCALING" );
    bool allow_upscaling;
    if( envpara ) allow_upscaling =  atoi( envpara ); // Implicit cast to boolean, all values other than '0' treated as true
    else allow_upscaling = ALLOW_UPSCALING;
    return allow_upscaling;
  }


  static std::string getURIMap(){
    char* envpara = getenv( "URI_MAP" );
    std::string uri_map;
    if( envpara ) uri_map = std::string( envpara );
    else uri_map = URI_MAP;
    return uri_map;
  }


  static unsigned int getEmbedICC(){
    char* envpara = getenv( "EMBED_ICC" );
    bool embed;
    if( envpara ) embed = atoi( envpara );
    else embed = EMBED_ICC;
    return embed;
  }


  static unsigned int getKduReadMode(){
    unsigned int readmode;
    char* envpara = getenv( "KAKADU_READMODE" );
    if( envpara ){
      readmode = atoi( envpara );
      if( readmode > 2 ) readmode = 2;
    }
    else readmode = KAKADU_READMODE;
    return readmode;
  }

#ifdef HAVE_PNG

  /****************************************
   from zlib.h
   #define Z_NO_COMPRESSION         0
   #define Z_BEST_SPEED             1
   #define Z_BEST_COMPRESSION       9
   #define Z_DEFAULT_COMPRESSION  (-1)
  ****************************************/
  static int getPNGCompressionLevel(){
    int png_compression_level = Z_NO_COMPRESSION;
    char *envval = getenv( "PNG_COMPRESSION_LEVEL" );
    if ( envval != NULL ) {
      string envpara = string(envval);
      if ( envpara.compare("Z_BEST_SPEED") )
        png_compression_level = Z_BEST_SPEED;
      else if ( envpara.compare("Z_BEST_COMPRESSION") )
        png_compression_level = Z_BEST_COMPRESSION;
      else if ( envpara.compare("Z_DEFAULT_COMPRESSION") )
        png_compression_level = Z_DEFAULT_COMPRESSION;
    }
    return png_compression_level;
  }

  /****************************************
   from png.h
   #define PNG_NO_FILTERS     0x00
   #define PNG_FILTER_NONE    0x08
   #define PNG_FILTER_SUB     0x10
   #define PNG_FILTER_UP      0x20
   #define PNG_FILTER_AVG     0x40
   #define PNG_FILTER_PAETH   0x80
   #define PNG_ALL_FILTERS (PNG_FILTER_NONE | PNG_FILTER_SUB | PNG_FILTER_UP | \
                            PNG_FILTER_AVG | PNG_FILTER_PAETH)
  ****************************************/

  static int PNGFilterTypeToInt( const char *filterType ) {
    int png_ftype = NO_FILTER_DEFINED;
    if ( filterType != NULL ) {
      string filter = string(filterType);
      if ( filter.compare("PNG_FILTER_NONE") )
        png_ftype = PNG_FILTER_NONE;
      else if ( filter.compare("PNG_FILTER_SUB") )
        png_ftype = PNG_FILTER_SUB;
      else if ( filter.compare("PNG_FILTER_UP") )
        png_ftype = PNG_FILTER_UP;
      else if ( filter.compare("PNG_FILTER_AVG") )
        png_ftype = PNG_FILTER_AVG;
      else if ( filter.compare("PNG_FILTER_PAETH") )
        png_ftype = PNG_FILTER_PAETH;
      else if ( filter.compare("PNG_ALL_FILTERS") )
        png_ftype = PNG_ALL_FILTERS;
    }
    return png_ftype;
  }

  static int getPNGFilterType() {
    int filterType = PNGFilterTypeToInt( getenv( "PNG_FILTER_TYPE" ) );
    int checkType = NO_FILTER_DEFINED;
    if ( filterType == checkType ) {
      filterType = PNG_NO_FILTERS;
    }
    return 0;
  }

#endif // HAVE_PNG




  /**
   * From tiff.h
   */
//  #define	    COMPRESSION_NONE		1	/* dump mode */
//  #define	    COMPRESSION_CCITTRLE	2	/* CCITT modified Huffman RLE */
//  #define	    COMPRESSION_CCITTFAX3	3	/* CCITT Group 3 fax encoding */
//  #define     COMPRESSION_CCITT_T4        3       /* CCITT T.4 (TIFF 6 name) */
//  #define	    COMPRESSION_CCITTFAX4	4	/* CCITT Group 4 fax encoding */
//  #define     COMPRESSION_CCITT_T6        4       /* CCITT T.6 (TIFF 6 name) */
//  #define	    COMPRESSION_LZW		5       /* Lempel-Ziv  & Welch */
//  #define	    COMPRESSION_OJPEG		6	/* !6.0 JPEG */
//  #define	    COMPRESSION_JPEG		7	/* %JPEG DCT compression */
//  #define     COMPRESSION_T85			9	/* !TIFF/FX T.85 JBIG compression */
//  #define     COMPRESSION_T43			10	/* !TIFF/FX T.43 colour by layered JBIG compression */
//  #define	    COMPRESSION_NEXT		32766	/* NeXT 2-bit RLE */
//  #define	    COMPRESSION_CCITTRLEW	32771	/* #1 w/ word alignment */
//  #define	    COMPRESSION_PACKBITS	32773	/* Macintosh RLE */
//  #define	    COMPRESSION_THUNDERSCAN	32809	/* ThunderScan RLE */
//  /* codes 32895-32898 are reserved for ANSI IT8 TIFF/IT <dkelly@apago.com) */
//  #define	    COMPRESSION_IT8CTPAD	32895   /* IT8 CT w/padding */
//  #define	    COMPRESSION_IT8LW		32896   /* IT8 Linework RLE */
//  #define	    COMPRESSION_IT8MP		32897   /* IT8 Monochrome picture */
//  #define	    COMPRESSION_IT8BL		32898   /* IT8 Binary line art */
//  /* compression codes 32908-32911 are reserved for Pixar */
//  #define     COMPRESSION_PIXARFILM	32908   /* Pixar companded 10bit LZW */
//  #define	    COMPRESSION_PIXARLOG	32909   /* Pixar companded 11bit ZIP */
//  #define	    COMPRESSION_DEFLATE		32946	/* Deflate compression */
//  #define     COMPRESSION_ADOBE_DEFLATE   8       /* Deflate compression,
//                 as recognized by Adobe */
//  /* compression code 32947 is reserved for Oceana Matrix <dev@oceana.com> */
//  #define     COMPRESSION_DCS             32947   /* Kodak DCS encoding */
//  #define	    COMPRESSION_JBIG		34661	/* ISO JBIG */
//  #define     COMPRESSION_SGILOG		34676	/* SGI Log Luminance RLE */
//  #define     COMPRESSION_SGILOG24	34677	/* SGI Log 24-bit packed */
//  #define     COMPRESSION_JP2000          34712   /* Leadtools JPEG2000 */
//  #define	    COMPRESSION_LZMA		34925	/* LZMA2 */
  static int TIFFCompressionTypeToInt(const char *compressionType) {
    int type = COMPRESSION_NONE;
    if (compressionType != NULL) {
      std::string compressor = std::string(compressionType);
      if (compressor.compare("COMPRESSION_LZW"))
        type = COMPRESSION_LZW;
      else if (compressor.compare("COMPRESSION_DEFLATE"))
        type = COMPRESSION_DEFLATE;
    }
    return type;
  }

  static int getTIFFCompressionType() {
    return TIFFCompressionTypeToInt( getenv( "TIFF_COMPRESSION_TYPE" ));
  }

};


#endif
