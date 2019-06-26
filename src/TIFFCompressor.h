
#ifndef IIPSRV_TIFFCOMPRESSOR_H
#define IIPSRV_TIFFCOMPRESSOR_H


#include <tiffio.h>
#include <sstream>
#include "Compressor.h"

typedef struct {
  unsigned char *data;
  tsize_t mx;
  tsize_t incsiz;
  tsize_t flen;
  tsize_t previous;
  toff_t size;
  int strip;
  TIFF *tiff;
} tiff_mem;

typedef tiff_mem * tiff_mem_ptr;

class TIFFCompressor: public Compressor {

private:
  unsigned int width, height, channels, bpc;
  int tiff_compression;
  tiff_mem dest_mgr;
  tiff_mem_ptr dest;

public:

  TIFFCompressor( int compressor ) {
    dest = NULL;
    this->tiff_compression = compressor;
  }

  int setCompressor( int compressor ) {
    tiff_compression = compressor;
  }

  void InitCompression( const RawTile& rawtile, unsigned int strip_height );

  unsigned int CompressStrip( unsigned char* s, unsigned char* o, unsigned int tile_height );

  unsigned int Finish( unsigned char* output );

  const char* getMimeType() { return "image/tiff"; };

  const char* getSuffix() { return "tiff"; };
};


class RawCompressor: public TIFFCompressor {
public:
  RawCompressor() : TIFFCompressor( COMPRESSION_NONE ) {}
};


#endif //IIPSRV_TIFFCOMPRESSOR_H
