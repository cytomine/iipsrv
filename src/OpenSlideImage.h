/* 
 * File:   OpenSlideImage.h
 * Author: stevben
 *
 * Created on 18 avril 2011, 13:13
 */


#ifndef OPENSLIDEIMAGE_H
#define	OPENSLIDEIMAGE_H

#include "IIPImage.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <inttypes.h>
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
    openslide_t* osr; //the openslide reader
    /// Tile data buffer pointer
 
    uint32_t *osr_buf;
    // tdata_t tile_buf;    
 
    void downsample_region(openslide_t *osr, uint32_t *buf, int64_t x, int64_t y, int32_t z, int64_t w, int64_t h);
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
    OpenSlideImage(const std::string& path) : IIPImage(path) {
        tile_width = TILESIZE;
        tile_height = TILESIZE;
        osr = NULL;
        numResolutions = 0;
    };

    /// Copy Constructor

    /** \param image IIPImage object
     */
    OpenSlideImage(const IIPImage& image) : IIPImage(image) {
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
    void openImage() throw (std::string);


    /// Overloaded function for loading TIFF image information
    /** \param x horizontal sequence angle
        \param y vertical sequence angle
     */
    void loadImageInfo(int x, int y) throw (std::string);

    /// Overloaded function for closing a TIFF image
    void closeImage();

    /// Overloaded function for getting a particular tile
    /** \param x horizontal sequence angle
        \param y vertical sequence angle
        \param r resolution
        \param l number of quality layers to decode
        \param t tile number
     */
	RawTile getTile(int x, int y, unsigned int r, int l, unsigned int t) throw (std::string);

    /// Overloaded function for returning a region for a given angle and resolution
    /** Return a RawTile object: Overloaded by child class.
        \param ha horizontal angle
        \param va vertical angle
        \param r resolution
        \param l number of quality layers to decode
        \param x x coordinate
        \param y y coordinate
        \param w width of region
        \param h height of region
        \param b buffer to fill
     */
    //void getRegion(int ha, int va, unsigned int r, int l, int x, int y, unsigned int w, unsigned int h, unsigned char* b) throw (std::string);
    void read(double zoom, long w, long h, long x, long y, char* data);	

    void readProperties(openslide_t* osr);
};

#endif	/* OPENSLIDEIMAGE_H */

