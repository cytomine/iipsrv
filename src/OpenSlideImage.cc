#include "OpenSlideImage.h"
#include "Timer.h"
#include <cmath>
#include <sstream>

#define DEBUG true
using namespace std;

#ifdef DEBUG
extern std::ofstream logfile;
#endif


void OpenSlideImage::openImage() throw( file_error ) {

  string filename = getFileName( currentX, currentY );

  // Check if our image has been modified
  updateTimestamp( filename );

//  bool canOpen = openslide_can_open(filename.c_str());
//  if (!canOpen) throw string("Can't open '" + filename + "' with OpenSlide");

  Timer timer;
  timer.start();

  osr = openslide_open( filename.c_str());
  if ( osr == NULL )
    throw file_error( "Error opening '" + filename + "' with OpenSlide" );

#ifdef DEBUG
  const char *test = openslide_get_error( osr );
  if ( test != NULL ) {
    logfile << "OpenSlideImage :: osr errors " << test << " " << endl;
  }
#endif

  if ( bpc == 0 ) {
    loadImageInfo( currentX, currentY );
  }

  //readAssociatedImages("label");

#ifdef DEBUG
  logfile << "OpenSlide :: openImage() :: " << timer.getTime()
          << " microseconds" << endl;
#endif

  isSet = true;

}

void OpenSlideImage::loadImageInfo( int x, int y ) throw( file_error ) {

#ifdef DEBUG
  logfile << "OpenSlideImage :: loadImageInfo()" << endl;
#endif

  long int w, h;
  const char *vendor;
  currentX = x;
  currentY = y;

  openslide_get_level0_dimensions( osr, &w, &h );

  channels = 3; // how to get it from openslide?
  bpc = 8;
  colourspace = sRGB;
  vendor = openslide_get_property_value( osr, OPENSLIDE_PROPERTY_NAME_VENDOR );

#ifdef DEBUG
  logfile << "dimensions :" << w << " x " << h << endl;
  logfile << "vendor : " << vendor << endl;
#endif

  image_widths.clear();
  image_heights.clear();

  unsigned int w_tmp = w;
  unsigned int h_tmp = h;
  image_widths.push_back( w_tmp );
  image_heights.push_back( h_tmp );
  while ((w_tmp > tile_width) || (h_tmp > tile_height)) {
    w_tmp = (int) floor( w_tmp / 2 );
    h_tmp = (int) floor( h_tmp / 2 );

    image_widths.push_back( w_tmp );
    image_heights.push_back( h_tmp );

#ifdef DEBUG
    logfile << "Create virtual layer : " << w_tmp << "x" << h_tmp << std::endl;
#endif
  }

#ifdef DEBUG
  for ( int t = 0; t < image_widths.size(); t++ ) {
    logfile << "image_widths[" << t << "]" << image_widths[t] << std::endl;
    logfile << "image_heights[" << t << "]" << image_heights[t] << std::endl;
  }
#endif

  numResolutions = image_widths.size();
  min.clear();
  max.clear();

  float smaxvalue[4];
  for ( int i = 0; i < channels; i++ ) {
    // Set default values if values not included in header
    if ( bpc == 8 ) smaxvalue[i] = 255.0;
    else if ( bpc == 16 ) smaxvalue[i] = 65535.0;
    else if ( bpc == 32 ) smaxvalue[i] = 4294967295.0;

    min.push_back( 0.0 );
    max.push_back( smaxvalue[i] );
  }
}


void OpenSlideImage::closeImage() {
#ifdef DEBUG
  Timer timer;
  timer.start();
#endif

  if ( osr != NULL ) {
    openslide_close( osr );
    osr = NULL;
  }

#ifdef DEBUG
  logfile << "OpenSlide :: closeImage() :: " << timer.getTime()
          << " microseconds" << endl;
#endif
}


RawTile OpenSlideImage::getTile( int seq, int ang, unsigned int res,
                                 int layers, unsigned int tile ) throw( file_error ) {

  Timer timer;
  timer.start();

  if ( res > (numResolutions - 1)) {
    ostringstream tile_no;
    tile_no << "OpenSlide :: Asked for non-existant resolution: " << res;
    throw file_error( tile_no.str());
  }

  unsigned int openslide_zoom = numResolutions - 1 - res;
  long int layer_width = image_widths[openslide_zoom];
  long int layer_height = image_heights[openslide_zoom];
  //openslide_get_level_dimensions(osr, level, &layer_width, &layer_height);

  unsigned int tw = tile_width;
  unsigned int th = tile_height;

  // Get the width and height for last row and column tiles
  unsigned int rem_x = layer_width % tw;
  unsigned int rem_y = layer_height % th;

  // Calculate the number of tiles in each direction
  unsigned int ntlx = (layer_width / tw) + (rem_x == 0 ? 0 : 1);
  unsigned int ntly = (layer_height / th) + (rem_y == 0 ? 0 : 1);


  if ( tile >= ntlx * ntly ) {
    ostringstream tile_no;
    tile_no << "OpenSlideImage :: Asked for non-existant tile: " << tile;
    throw file_error( tile_no.str());
  }

  // Alter the tile size if it's in the last column
  if ((tile % ntlx == ntlx - 1) && (rem_x != 0)) {
    tw = rem_x;
  }

  // Alter the tile size if it's in the bottom row
  if ((tile / ntlx == ntly - 1) && rem_y != 0 ) {
    th = rem_y;
  }

  // Calculate the pixel offsets for this tile
  int xoffset = (tile % ntlx) * tile_width;
  int yoffset = (unsigned int) floor( tile / ntlx ) * tile_height;

  // Create our raw tile buffer and initialize some values
  RawTile rawtile( tile, res, seq, ang, tw, th, channels, bpc );
  rawtile.dataLength = tw * th * channels * bpc / 8;
  rawtile.filename = getImagePath();
  rawtile.timestamp = timestamp;
  rawtile.data = new unsigned char[tw * th * channels];
  //rawtile.memoryManaged = 0;
  //rawtile.padded = false;

#ifdef DEBUG
  logfile << "Allocating tw * th * channels * sizeof(char) : "
          << tw << " * " << th << " * " << channels
          << " * sizeof(char) " << endl << flush;
#endif

  int pos_factor = pow( 2, openslide_zoom );
  read( openslide_zoom, tw, th, (long) xoffset * pos_factor,
        (long) yoffset * pos_factor, rawtile.data );

#ifdef DEBUG
  logfile << "OpenSlide :: getTile() :: " << timer.getTime()
          << " microseconds" << endl << flush;
#endif

#ifdef DEBUG
  logfile << "TILE RENDERED" << std::endl;
#endif

  return (rawtile);
}


void removeAlphaAndSwapRB( unsigned char *destPixel, unsigned char *sourcePixel ) {
  unsigned char a = sourcePixel[3];
  if ( a == 255 ) {
    // Common case.  Compiles to a shift and a BSWAP.
    memcpy( destPixel + 0, sourcePixel + 2, 1 );
    memcpy( destPixel + 1, sourcePixel + 1, 1 );
    memcpy( destPixel + 2, sourcePixel + 0, 1 );
  } else if ( a == 0 ) {
    destPixel[0] = 0xFF;
    destPixel[1] = 0xFF;
    destPixel[2] = 0xFF;
  } else {
    // Unusual case.
    destPixel[0] = 255 * sourcePixel[2] / a;
    destPixel[1] = 255 * sourcePixel[2] / a;
    destPixel[2] = 255 * sourcePixel[0] / a;
  }
}

void OpenSlideImage::read( int zoom, long w, long h, long x, long y, void *dest ) {
#ifdef DEBUG
  logfile << "OpenSlide READ zoom, w, h, x, y :" << zoom << "," << w << ","
          << h << "," << x << "," << y << std::endl;
#endif

  unsigned int *buffer = new unsigned int[w * h * 4];
  downsample_region( osr, buffer, x, y, zoom, w, h );

  unsigned char *temp1 = reinterpret_cast<unsigned char *> (dest);
  unsigned char *temp2 = reinterpret_cast<unsigned char *> (buffer);
  for ( int i = 0; i < h; i++ ) {
    for ( int j = 0; j < w; j++ ) {
      removeAlphaAndSwapRB( temp1, temp2 );
      // imageData jump to next line
      temp1 = temp1 + channels;
      // buffer jump to next line
      temp2 = temp2 + 4;
    }
  }

#ifdef DEBUG
  logfile << "FREE BUFFER..." << std::endl;
#endif

  delete[](buffer);

#ifdef DEBUG
  logfile << "DONE..." << std::endl;
#endif
}

/*
 * use this function in place of openslide_read_region(). This function
 * automatically downsample a region in the missing zoom level z, if needed.
 * Arguments are exactly as what would be given to openslide_read_region().
 * Note that z is not the openslide layer, but the desired zoom level, because
 * the slide may not have all the layers that correspond to all the
 * zoom levels. The number of layers is equal or less than the number of
 * zoom levels in an equivalent zoomify format.
 * This downsampling method simply skips pixel. If interpolation is desired,
 * an image processing library could be used.
 */
void OpenSlideImage::downsample_region( openslide_t *osr, unsigned int *buf, long int x,
                                        long int y, int z, long int w, long int h ) {

  /* find the next layer to downsample to desired zoom level z*/
  int bestLayer = openslide_get_best_level_for_downsample( osr, pow( 2, z ));

  /*calculate downsampling factor, should be 1,2,4,8...*/
  double downSamplingFactor = (pow( 2, z ) / openslide_get_level_downsample( osr, bestLayer ));

  if ( downSamplingFactor > 1.0 ) {
    /* need to downsample */
#ifdef DEBUG
    logfile << "openslide_downsampling bestLayer " << bestLayer << std::endl;
#endif
    // allocate a buffer large enough to hold the best layer
    unsigned int *tmpbuf = (unsigned int *) malloc( ceil( w * downSamplingFactor )
                                                    * ceil( h * downSamplingFactor ) * 4 );
    if ( !tmpbuf )
      throw string( "FATAL : OpenSlideImage downsample_region => allocation memory ERROR" );

    openslide_read_region( osr, tmpbuf, x, y, bestLayer, ceil( w * downSamplingFactor ),
                           ceil( h * downSamplingFactor ));

    // Debugging output Before Downsampling/
    //    char tileFileName[MAX_PATH];
    //    sprintf(tileFileName, "zoom%d-row%ld.jpg", z, y);
    //    SaveJPGFile((unsigned char*)tmpbuf,
    //            (unsigned long)w*downSamplingFactor,
    //            (unsigned long)h*downSamplingFactor,
    //            (unsigned long)w*downSamplingFactor*4, 32, tileFileName, 75);

    // down sample loop
    int row, col;
    for ( row = 0; row < h; row++ ) {
      unsigned int *dest = buf + (unsigned long) (w * row);
      unsigned int *src = tmpbuf + (unsigned long) (ceil( w * downSamplingFactor )
                                                    * ceil( row * downSamplingFactor ));
      unsigned int *cdest = src, *csrc = src;
      for ( col = 1; col < w; col++ ) {
        *(cdest + (unsigned long) col) = *(csrc + (unsigned long) (col * downSamplingFactor));
      }
      memcpy( dest, src, (unsigned long) (w * 4));
    }
    free( tmpbuf );

  } else {
    /* no need to downsample, since zoom level is in the slide  */
#ifdef DEBUG
    logfile << "openslide_read_region" << std::endl;
#endif

    openslide_read_region( osr, buf, x, y, bestLayer, w, h );
  }
}
