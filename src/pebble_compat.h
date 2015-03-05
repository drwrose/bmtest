#ifndef PEBBLE_COMPAT_H
#define PEBBLE_COMPAT_H

// Some definitions to aid in cross-compatibility between Aplite and Basalt.

// GBitmap accessors, not appearing on Aplite.
#ifdef PBL_PLATFORM_APLITE

#define gbitmap_get_bounds(bm) ((bm)->bounds)
#define gbitmap_get_bytes_per_row(bm) ((bm)->row_size_bytes)
#define gbitmap_get_data(bm) ((bm)->addr)
#define gbitmap_get_format(bm) (0)

#endif  // PBL_PLATFORM_APLITE

// Weirdly different function prototypes.
#ifdef PBL_PLATFORM_APLITE

#define gbitmap_create_blank(size, format) gbitmap_create_blank(size)

#endif  // PBL_PLATFORM_APLITE


// GBitmapFormat definition.
#ifdef PBL_PLATFORM_APLITE

typedef enum GBitmapFormat {
  GBitmapFormat1Bit = 0, //<! 1-bit black and white. 0 = black, 1 = white.
  GBitmapFormat8Bit,      //<! 6-bit color + 2 bit alpha channel. See \ref GColor8 for pixel format.
  GBitmapFormat1BitPalette,
  GBitmapFormat2BitPalette,
  GBitmapFormat4BitPalette,
} GBitmapFormat;

#endif  // PBL_PLATFORM_APLITE


// GColor static initializers, slightly different syntax needed on Basalt.
#ifdef PBL_PLATFORM_APLITE

#define GColorBlackInit GColorBlack
#define GColorWhiteInit GColorWhite
#define GColorClearInit GColorClear

#else  // PBL_PLATFORM_APLITE

#define GColorBlackInit { GColorBlackARGB8 }
#define GColorWhiteInit { GColorWhiteARGB8 }
#define GColorClearInit { GColorClearARGB8 }

#endif  // PBL_PLATFORM_APLITE


#endif
