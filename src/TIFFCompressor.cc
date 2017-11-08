#include <tiffio.h>
#include <iostream>

#include "TIFFCompressor.h"


extern "C" {
  static tiff_mem *memTiffOpen(tsize_t incsiz = 10240, tsize_t initsiz = 10240) {
    tiff_mem *memtif;
    memtif = (tiff_mem*) malloc(sizeof(tiff_mem));
    if (!memtif)
      exit(-1);

    memtif->incsiz = incsiz;
    if (initsiz == 0)
      initsiz = incsiz;

    memtif->data = (unsigned char*) malloc(initsiz * sizeof(unsigned char));
    if (!memtif->data)
      exit(-1);

    memtif->size = initsiz;
    memtif->flen = 0;
    memtif->fptr = 0;
    return memtif;
  }

  static tsize_t memTiffReadProc(thandle_t handle, tdata_t buf, tsize_t size) {
    tiff_mem *memtif = (tiff_mem *) handle;
    tsize_t n;
    if (((tsize_t) memtif->fptr + size) <= memtif->flen) {
      n = size;
    }
    else {
      n = memtif->flen - memtif->fptr;
    }
    memcpy(buf, memtif->data + memtif->fptr, n);
    memtif->fptr += n;

    return n;
  }

  static tsize_t memTiffWriteProc(thandle_t handle, tdata_t buf, tsize_t size) {
    tiff_mem *memtif = (tiff_mem *) handle;
    if (((tsize_t) memtif->fptr + size) > memtif->size) {
      memtif->data = (unsigned char *) realloc(memtif->data, memtif->fptr + memtif->incsiz + size);
      memtif->size = memtif->fptr + memtif->incsiz + size;
    }
    memcpy (memtif->data + memtif->fptr, buf, size);
    memtif->fptr += size;
    if (memtif->fptr > memtif->flen)
      memtif->flen = memtif->fptr;

    return size;
  }

  static toff_t memTiffSeekProc(thandle_t handle, toff_t off, int whence) {
    tiff_mem *memtif = (tiff_mem *) handle;
    switch (whence) {
      case SEEK_SET: {
        if ((tsize_t) off > memtif->size) {
          memtif->data = (unsigned char *) realloc(memtif->data, memtif->size + memtif->incsiz + off);
          memtif->size = memtif->size + memtif->incsiz + off;
        }
        memtif->fptr = off;
        break;
      }
      case SEEK_CUR: {
        if ((tsize_t)(memtif->fptr + off) > memtif->size) {
          memtif->data = (unsigned char *) realloc(memtif->data, memtif->fptr + memtif->incsiz + off);
          memtif->size = memtif->fptr + memtif->incsiz + off;
        }
        memtif->fptr += off;
        break;
      }
      case SEEK_END: {
        if ((tsize_t) (memtif->size + off) > memtif->size) {
          memtif->data = (unsigned char *) realloc(memtif->data, memtif->size + memtif->incsiz + off);
          memtif->size = memtif->size + memtif->incsiz + off;
        }
        memtif->fptr = memtif->size + off;
        break;
      }
    }
    if (memtif->fptr > memtif->flen) memtif->flen = memtif->fptr;
    return memtif->fptr;
  }

  static int memTiffCloseProc(thandle_t handle) {
    tiff_mem *memtif = (tiff_mem *) handle;
    memtif->fptr = 0;
    return 0;
  }

  static toff_t memTiffSizeProc(thandle_t handle) {
    tiff_mem *memtif = (tiff_mem *) handle;
    return memtif->flen;
  }

  static int memTiffMapProc(thandle_t handle, tdata_t* base, toff_t* psize) {
    tiff_mem *memtif = (tiff_mem *) handle;
    *base = memtif->data;
    *psize = memtif->flen;
    return (1);
  }

  static void memTiffUnmapProc(thandle_t handle, tdata_t base, toff_t size) {
    return;
  }

  static void memTiffFree(tiff_mem *memtif) {
    if (memtif->data) free (memtif->data);
    memtif->data = NULL;

    free (memtif);
    return;
  }
}

void TIFFCompressor::InitCompression( const RawTile &rawtile, unsigned int strip_height ) throw (std::string) {
  dest = &dest_mgr;

  width = rawtile.width;
  height = rawtile.height;
  channels = rawtile.channels;
  bpc =rawtile.bpc;

  dest = memTiffOpen();
  dest->tiff = TIFFClientOpen("_", "wb", (thandle_t) dest,
                              memTiffReadProc,
                              memTiffWriteProc,
                              memTiffSeekProc,
                              memTiffCloseProc,
                              memTiffSizeProc,
                              memTiffMapProc,
                              memTiffUnmapProc);
  dest->strip = 0;

  TIFFSetField(dest->tiff, TIFFTAG_IMAGEWIDTH, width);
  TIFFSetField(dest->tiff, TIFFTAG_IMAGELENGTH, height);
  TIFFSetField(dest->tiff, TIFFTAG_SAMPLESPERPIXEL, channels);
  TIFFSetField(dest->tiff, TIFFTAG_BITSPERSAMPLE, bpc);
  TIFFSetField(dest->tiff, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(dest->tiff, strip_height));
  TIFFSetField(dest->tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(dest->tiff, TIFFTAG_COMPRESSION, tiff_compression);

  if (channels == 1)
    TIFFSetField(dest->tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
  else if (channels == 3)
    TIFFSetField(dest->tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

  TIFFSetDirectory(dest->tiff, 0);
}

unsigned int TIFFCompressor::CompressStrip( unsigned char *s, unsigned char *o,
                                            unsigned int tile_height ) throw (std::string) {
  int row_stride = width * channels * bpc / 8;
  TIFFWriteEncodedStrip(dest->tiff, dest->strip++, s, tile_height * row_stride);

  return 0;
}

unsigned int TIFFCompressor::Finish( unsigned char *output ) throw (std::string) {
  TIFFClose(dest->tiff);
  int datacount = dest->flen;
  memcpy(output, dest->data, datacount);
  memTiffFree(dest);
  return datacount;
}

