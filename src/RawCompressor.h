#ifndef IIPSRV_RAWCOMPRESSOR_H
#define IIPSRV_RAWCOMPRESSOR_H

#include "Compressor.h"


class RawCompressor: public Compressor {

private:

  /// the width, height and number of channels per sample for the image
  unsigned int width, height, channels;

  /// Buffer for the image data
  unsigned char *data;


public:

  /// Constructor
  RawCompressor() { };


  /// Return the mime type
  inline const char* getMimeType(){ return "application/octet-stream"; }

  /// Return the image filename suffix
  inline const char* getSuffix(){ return "raw"; }

};

#endif //IIPSRV_RAWCOMPRESSOR_H
