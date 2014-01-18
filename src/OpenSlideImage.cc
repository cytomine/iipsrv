#include "OpenSlideImage.h"
#include "Timer.h"
#include <tiff.h>
#include <tiffio.h>
#include <cmath>
#include <sstream>

#include <sstream>
#define DEBUG 1
using namespace std;

#ifdef DEBUG
extern std::ofstream logfile;
#endif

/// Overloaded function for opening a TIFF image

void OpenSlideImage::openImage() throw (std::string) {

    string filename = getFileName(currentX, currentY);

    // Check if our image has been modified
    updateTimestamp(filename);



    /*bool canOpen = openslide_can_open(filename.c_str());
    if (!canOpen) throw string("Can't open '" + filename + "' with OpenSlide");
	*/
    Timer timer;
    timer.start();

    osr = openslide_open(filename.c_str());

    if (osr == NULL) throw string("Error opening '" + filename + "' with OpenSlide");
	if (bpp == 0) {
		loadImageInfo(currentX, currentY);
		readProperties(osr);
	}
    //readAssociatedImages("label");

#ifdef DEBUG
    logfile << "OpenSlide :: openImage() :: " << timer.getTime() << " microseconds" << endl;
#endif
	
	
	
}

void OpenSlideImage::loadImageInfo(int x, int y) throw (std::string) {
	printf("OpenSlideImage :: loadImageInfo()\n");
	fflush(stdout);	
#ifdef DEBUG
    logfile << "OpenSlideImage :: loadImageInfo()" << endl; 
#endif  

    int64_t w, h;
    currentX = x;
    currentY = y;

    tile_width = 256;
    tile_height = 256;        

    openslide_get_level0_dimensions(osr, &w, &h);

    channels = 3; // how to get it from openslide?
    bpp = 8;

    // const char* comment = openslide_get_comment(osr);

#ifdef DEBUG
    logfile << "dimensions :" << w << " x " << h << endl;
    //logfile << "comment : " << comment << endl;
#endif
    //int32_t layers = openslide_get_layer_count(osr);



#ifdef DEBUG
    //logfile << "num layers : " << layers;
#endif
    image_widths.clear();
    image_heights.clear();
    /*for (int32_t i = 0; i < layers; i++) {
	int64_t ww, hh;
        openslide_get_level_dimensions(osr, i, &ww, &hh);
    
        image_widths.push_back(ww);
        image_heights.push_back(hh);
    
    #ifdef DEBUG
	logfile << " layer " << i << "dimensions : " << ww << " x " << hh << endl;
    #endif
    }*/


    unsigned int w_tmp = w;
    unsigned int h_tmp = h;
    image_widths.push_back(w_tmp);
    image_heights.push_back(h_tmp);
    while ((w_tmp > tile_width) || (h_tmp > tile_height)) {
        w_tmp = (int) floor(w_tmp / 2);
        h_tmp = (int) floor(h_tmp / 2);

        image_widths.push_back(w_tmp);
        image_heights.push_back(h_tmp);
#ifdef DEBUG
        logfile << "Create virtual layer : " << w_tmp << "x" << h_tmp << std::endl;
#endif
    }

#ifdef DEBUG
    std::cout << std::endl;
    for (int t = 0; t < image_widths.size(); t++) {
        logfile  << "image_widths[" << t << "]" << image_widths[t] << std::endl;
        logfile  << "image_heights[" << t << "]" << image_heights[t] << std::endl;
    }
#endif

    numResolutions = image_widths.size();

	double sminvalue[4], smaxvalue[4];
    min.clear();
    max.clear();
    for( int i=0; i<channels; i++ ){
      if( (float)smaxvalue[i] == 0.0 ){
        sminvalue[i] = 0.0;
        // Set default values if values not included in header
        if( bpp == 8 ) smaxvalue[i] = 255.0;
        else if( bpp == 16 ) smaxvalue[i] = 65535.0;
        else if( bpp == 32 && sampleType == FIXEDPOINT ) smaxvalue[i] = 4294967295.0;
      }
      min.push_back( (float)sminvalue[i] );
      max.push_back( (float)smaxvalue[i] );
    }
    //    const char * const *property_names = openslide_get_property_names(osr);
    //    while (*property_names) {
    //        const char *name = *property_names;
    //        const char *value = openslide_get_property_value(osr, name);
    //        metadata[name] = value;
    //#ifdef DEBUG
    //        logfile << "property : " << name << " -> " << value << endl;
    //#endif
    //        property_names++;
    //    }


    // read associated images
    //    const char * const *associated_image_names = openslide_get_associated_image_names(osr);
    //    while (*associated_image_names) {
    //        int64_t w;
    //        int64_t h;
    //        const char *name = *associated_image_names;
    //        openslide_get_associated_image_dimensions(osr, name, &w, &h);
    //
    //#ifdef DEBUG
    //        logfile << "associated image: " << name << "(" << w << " x " << h << ")" << endl;
    //#endif
    //
    //        associated_image_names++;
    //    }

    colourspace = sRGB;	
}

/// Overloaded function for closing a TIFF image

void OpenSlideImage::closeImage() {
#ifdef DEBUG
    Timer timer;
    timer.start();
#endif

    //openslide_close(osr);

    osr = NULL;
#ifdef DEBUG
    logfile << "OpenSlide :: closeImage() :: " << timer.getTime() << " microseconds" << endl;
#endif
}

/// Overloaded function for getting a particular tile

/** \param x horizontal sequence angle
    \param y vertical sequence angle
    \param r resolution
    \param l number of quality layers to decode
    \param t tile number
 */
RawTile OpenSlideImage::getTile(int seq, int ang, unsigned int res, int layers, unsigned int tile) throw (string) {

	printf("OpenSlideImage :: getTile");
	fflush(stdout);	

    Timer timer;
    timer.start();

    if (res > (numResolutions-1)) {
        ostringstream tile_no;
        tile_no << "OpenSlide :: Asked for non-existant resolution: " << res;
        throw tile_no.str();
    }
	
	printf("OpenSlideImage :: getTile");
	
	std::cout << std::endl << "numResolutions" << numResolutions <<  std::endl; 
	std::cout << "res" <<  std::endl; 
	std::cout << "numResolutions - 1 - res " << (numResolutions - 1 - res) <<  std::endl; 		
	std::cout << "image_widths.size() " << image_widths.size() <<  std::endl; 
	std::cout << "image_heights.size() " << image_heights.size() <<  std::endl; 
	std::cout << "image_widths[numResolutions - 1 - res]" << image_widths[numResolutions - 1 - res] << std::endl;
	std::cout << "image_heights[numResolutions - 1 - res]" << image_heights[numResolutions - 1 - res] << std::endl;	
	fflush(stdout);	

    int64_t layer_width = image_widths[numResolutions - 1 - res];
    int64_t layer_height = image_heights[numResolutions - 1 - res];
    
    //openslide_get_layer_dimensions(osr, layers, &layer_width, &layer_height);


    unsigned int tw = tile_width;
    unsigned int th = tile_height;

    // Get the width and height for last row and column tiles
    unsigned int rem_x = layer_width % tw;
    unsigned int rem_y = layer_height % th;

    // Calculate the number of tiles in each direction
    unsigned int ntlx = (layer_width / tw) + (rem_x == 0 ? 0 : 1);
    unsigned int ntly = (layer_height / th) + (rem_y == 0 ? 0 : 1);


    if (tile >= ntlx * ntly) {
        ostringstream tile_no;
        tile_no << "OpenSlideImage :: Asked for non-existant tile: " << tile;
        throw tile_no.str();
    }


   

    double openslide_zoom = this->numResolutions - 1 - res;

    int pos_factor = pow(2, openslide_zoom); 


   

     // Alter the tile size if it's in the last column
    if ((tile % ntlx == ntlx - 1) && (rem_x != 0)) {
        tw = rem_x;
    }

    // Alter the tile size if it's in the bottom row
    if ((tile / ntlx == ntly - 1) && rem_y != 0) {
        th = rem_y;
    }
    
    // Calculate the pixel offsets for this tile
    int xoffset = (tile % ntlx) * tile_width;
    int yoffset = (unsigned int) floor(tile / ntlx) * tile_height;

    RawTile rawtile(tile, res, seq, ang, tw, th, channels, bpp);

    // Create our raw tile buffer and initialize some values
    rawtile.data = NULL;
    rawtile.dataLength = tw * th* channels;
    rawtile.filename = getImagePath();
    rawtile.timestamp = timestamp;
    /*rawtile.memoryManaged = 0;
    rawtile.padded = true;*/

    char* dest =  (char*) malloc(tw * th * channels * sizeof(char));
    if (!dest) throw string("FATAL : getTile >> allocation memory ERROR");
    read(openslide_zoom, tw, th, (long) xoffset * pos_factor, (long) yoffset * pos_factor, dest);
    rawtile.data = dest;


#ifdef DEBUG
    logfile << "OpenSlide :: getTile() :: " << timer.getTime() << " microseconds" << endl << flush;
#endif

#ifdef DEBUG
    logfile << "TILE RENDERED" << std::endl;
#endif

    return ( rawtile);


}

void OpenSlideImage::read(double zoom, long w, long h, long x, long y, char* dest) {
	
#ifdef DEBUG
    logfile << "OpenSlide READ zoom, w, h, x, y :" << zoom  << "," << w << "," << h << "," << x << "," << y << std::endl;
#endif
    uint32_t* buffer = (uint32_t*) malloc(w * h * 4);
    if (!buffer) throw string("FATAL : OpenSlideImage READ => allocation memory ERROR");
    //openslide_read_region(osr, buffer, (int64_t) x, (int64_t) y, zoom, (int64_t) w, (int64_t) h);

    this->downsample_region(osr, buffer, (int64_t) x, (int64_t) y, zoom, (int64_t) w, (int64_t) h);
    
    //std::cout << "READ " << x << "," << y << "," << w << "," << h << std::endl;
    unsigned char *temp1 = reinterpret_cast<unsigned char*> (dest);
    unsigned char *temp2 = reinterpret_cast<unsigned char*> (buffer);
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            // memory copy
            memcpy(temp1 + 0, temp2 + 2, 1);
            memcpy(temp1 + 1, temp2 + 1, 1);
            memcpy(temp1 + 2, temp2 + 0, 1);
            // imageData jump to next line
            temp1 = temp1 + channels;
            // buffer jump to next line
            temp2 = temp2 + 4;
        }

    }
#ifdef DEBUG
    logfile << "FREE BUFFER..." << std::endl;
#endif
    free(buffer);
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
void OpenSlideImage::downsample_region(openslide_t *osr, uint32_t *buf, int64_t x, int64_t y, int32_t z, int64_t w, int64_t h) {

    /* find the next layer to downsample to desired zoom level z*/
    int bestLayer = openslide_get_best_level_for_downsample(osr, pow(2, z));

    /*calculate downsampling factor, should be 1,2,4,8...*/
    double downSamplingFactor = (double) (pow(2, z) /
            openslide_get_level_downsample(osr, bestLayer));
    /*   printf("Down Sampling Factor = %ld\n", downSamplingFactor); */

    if (downSamplingFactor > 1.0) { /* need to downsample */
#ifdef DEBUG
        logfile << "openslide_downsampling bestLayer " << bestLayer << std::endl;
#endif
        // allocate a buffer large enough to hold the best layer
	
        uint32_t *tmpbuf = (uint32_t *) malloc(ceil(w * downSamplingFactor) * ceil(h * downSamplingFactor) * 4);
        
        openslide_read_region(osr, tmpbuf, x, y, bestLayer, ceil(w * downSamplingFactor), ceil(h * downSamplingFactor));
        if (!tmpbuf) throw string("FATAL : OpenSlideImage downsample_region => allocation memory ERROR");
        // Debugging output Before Downsampling/
        //    char tileFileName[MAX_PATH];
        //    sprintf(tileFileName, "zoom%d-row%ld.jpg", z, y); 
        //    SaveJPGFile((unsigned char*)tmpbuf, 
        //            (unsigned long)w*downSamplingFactor, 
        //            (unsigned long)h*downSamplingFactor, 
        //            (unsigned long)w*downSamplingFactor*4, 32, tileFileName, 75); 

        // down sample loop 
        int row, col;
        for (row = 0; row < h; row++) {
            uint32_t *dest = buf + (unsigned long) (w * row);
            uint32_t *src = tmpbuf + (unsigned long) (ceil(w * downSamplingFactor) * ceil(row * downSamplingFactor));
            uint32_t *cdest = src, *csrc = src;
            for (col = 1; col < w; col++) {
                *(cdest + (unsigned long) col) = *(csrc + (unsigned long) (col * downSamplingFactor));
            }
            memcpy(dest, src, (unsigned long) (w * 4));
        }
        free(tmpbuf);

    } else { /* no need to downsample, since zoom level is in the slide  */
#ifdef DEBUG
        logfile << "openslide_read_region" << std::endl;
#endif
        openslide_read_region(osr, buf, x, y, bestLayer, w, h);

    }

}

void OpenSlideImage::readProperties(openslide_t *osr) {
    // read properties

    const char * const *property_names = openslide_get_property_names(osr);
    while (*property_names) {
        const char *name = *property_names;
        const char *value = openslide_get_property_value(osr, name);
        metadata[name] = value;
#ifdef DEBUG
        /*std::cout << "property: " <<  name << " -> " << value << std::endl;*/
#endif
        property_names++;
    }

}

RawTile OpenSlideImage::readAssociatedImages(const char* image_name) {
    std::cout << "readAssociatedImages " << image_name << std::endl << std::flush;
    //const char * const *associated_image_names = openslide_get_associated_image_names(osr);
    //unsigned int i = 0;
    /* while (*associated_image_names) {*/
    int64_t w;
    int64_t h;
    //        const char *name = *associated_image_names;

    /*if (strcmp(name, image_name) == 0) {*/
    openslide_get_associated_image_dimensions(osr, image_name, &w, &h);
    std::cout << "readAssociatedImages w,h" << w << "," << h << std::endl << std::flush;
    uint32_t* data = (uint32_t*) malloc(w * h * 4);


    RawTile rawtile(0, 0, 0, 0, w, h, channels, bpp);

    // Create our raw tile buffer and initialize some values
    rawtile.data = NULL;
    rawtile.dataLength = w * h* channels;
    rawtile.filename = "youyou.jpg";
    rawtile.timestamp = timestamp;
    //    rawtile.memoryManaged = 0;
    //rawtile.padded = true;


    char* dest = (char *) malloc(w * h * channels);
    for (int i = 0; i < w * h * channels; i++) dest[i] = 0;
    /*openslide_read_associated_image(osr, image_name, data);
    unsigned char *temp1 = reinterpret_cast<unsigned char*> (dest);
    unsigned char *temp2 = reinterpret_cast<unsigned char*> (data);
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            // memory copy
            memcpy(temp1 + 0, temp2 + 2, 1);
            memcpy(temp1 + 1, temp2 + 1, 1);
            memcpy(temp1 + 2, temp2 + 0, 1);
            // imageData jump to next line
            temp1 = temp1 + channels;
            // buffer jump to next line
            temp2 = temp2 + 4;
        }

    }*/
    free(data);
    rawtile.data = dest;

    return ( rawtile);

    //}

    /*    associated_image_names++;

    }*/

}


