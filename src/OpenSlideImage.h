/* 
 * File:   OpenSlideImage.h
 * Author: stevben
 *
 * Created on 18 avril 2011, 13:13
 */


#ifndef OPENSLIDEIMAGE_H
#define OPENSLIDEIMAGE_H

#include "IIPImage.h"
#include <sys/time.h>
#include <iostream>
#include <fstream>


extern "C" {
#include "openslide.h"
#include "openslide-features.h"
}

#define TILESIZE 256

/// Image class for OpenSlide supported Images: Inherits from IIPImage. Uses the OpenSlide library.
class OpenSlideImage : public IIPImage {
private:
  /// OpenSlide reader
  openslide_t *osr;

  void read( int zoom, long w, long h, long x, long y, void *data );
  void downsample_region( openslide_t *osr, unsigned int *buf, long int x, long int y, int z, long int w, long int h );

public:

  /// Constructor
  OpenSlideImage() : IIPImage() {
    tile_width = TILESIZE;
    tile_height = TILESIZE;
    osr = NULL;
    numResolutions = 0;
  };

  /// Constructor
  /** \param path image path
   */
  OpenSlideImage( const std::string &path ) : IIPImage( path ) {
    tile_width = TILESIZE;
    tile_height = TILESIZE;
    osr = NULL;
    numResolutions = 0;
  };

  /// Copy Constructor
  /** \param image IIPImage object
   */
  OpenSlideImage( const IIPImage &image ) : IIPImage( image ) {
    tile_width = TILESIZE;
    tile_height = TILESIZE;
    osr = NULL;
    numResolutions = 0;
  };

  /// Destructor
  ~OpenSlideImage() {
    closeImage();
  };

  /// Overloaded function for opening a TIFF image
  void openImage() throw( file_error );


  /// Overloaded function for loading TIFF image information
  /** \param x horizontal sequence angle
      \param y vertical sequence angle
   */
  void loadImageInfo( int x, int y ) throw( file_error );

  /// Overloaded function for closing a TIFF image
  void closeImage();

  /// Overloaded function for getting a particular tile
  /** \param x horizontal sequence angle
      \param y vertical sequence angle
      \param r resolution
      \param l number of quality layers to decode
      \param t tile number
   */
  RawTile getTile( int x, int y, unsigned int r, int l, unsigned int t ) throw( file_error );
};

#endif  /* OPENSLIDEIMAGE_H */

