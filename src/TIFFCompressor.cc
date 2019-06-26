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

    memtif->mx = initsiz;
    memtif->flen = 0;
    memtif->size = 0;
    return memtif;
  }

  static tsize_t memTiffReadProc(thandle_t handle, tdata_t buf, tsize_t size) {
    tiff_mem *memtif = (tiff_mem *) handle;
    tsize_t n;
    if (((tsize_t) memtif->size + size) <= memtif->flen) {
      n = size;
    }
    else {
      n = memtif->flen - memtif->size;
    }
    memcpy(buf, memtif->data + memtif->size, n);
    memtif->size += n;

    return n;
  }

  static tsize_t memTiffWriteProc(thandle_t handle, tdata_t buf, tsize_t length) {
    tiff_mem *memtif = (tiff_mem *) handle;
    if (((tsize_t) memtif->size + length) > memtif->mx) {
      memtif->mx = memtif->size + memtif->incsiz + length;
      memtif->data = (unsigned char *) realloc(memtif->data, memtif->mx);
    }
    memcpy (memtif->data + memtif->size, buf, length);
    memtif->size += length;
    if (memtif->size > memtif->flen)
      memtif->flen = memtif->size;
    return length;
  }

  static toff_t memTiffSeekProc(thandle_t handle, toff_t off, int whence) {
    tiff_mem *memtif = (tiff_mem *) handle;
    switch (whence) {
      case SEEK_SET: {
        if ((tsize_t) off > memtif->mx) {
          memtif->data = (unsigned char *) realloc(memtif->data, memtif->mx + memtif->incsiz + off);
          memtif->mx = memtif->mx + memtif->incsiz + off;
        }
        memtif->size = off;
        break;
      }
      case SEEK_CUR: {
        if ((tsize_t)(memtif->size + off) > memtif->mx) {
          memtif->data = (unsigned char *) realloc(memtif->data, memtif->size + memtif->incsiz + off);
          memtif->mx = memtif->size + memtif->incsiz + off;
        }
        memtif->size += off;
        break;
      }
      case SEEK_END: {
        if ((tsize_t) (memtif->mx + off) > memtif->mx) {
          memtif->data = (unsigned char *) realloc(memtif->data, memtif->mx + memtif->incsiz + off);
          memtif->mx = memtif->mx + memtif->incsiz + off;
        }
        memtif->size = memtif->mx + off;
        break;
      }
    }
    if (memtif->size > memtif->flen) memtif->flen = memtif->size;
    return memtif->size;
  }

  static int memTiffCloseProc(thandle_t handle) {
    tiff_mem *memtif = (tiff_mem *) handle;
    memtif->size = 0;
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

void TIFFCompressor::InitCompression( const RawTile &rawtile, unsigned int strip_height ) {
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
  dest->previous = 0;

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
                                            unsigned int tile_height ) {
  int row_stride = width * channels * bpc / 8;
  TIFFWriteEncodedStrip(dest->tiff, dest->strip++, s, tile_height * row_stride);

  unsigned int datacount = dest->size - dest->previous;
  memcpy(o, dest->data+dest->previous, datacount);
  dest->previous = dest->size;
  return datacount;
}

unsigned int TIFFCompressor::Finish( unsigned char *output ) {
  TIFFClose(dest->tiff);

  unsigned int datacount = dest->flen;
  memcpy(output, dest->data, datacount);
  memTiffFree(dest);
  return datacount;
}

