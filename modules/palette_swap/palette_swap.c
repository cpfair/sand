#include <pebble.h>

#ifdef enabled
#ifdef PBL_COLOR
#define ROW_SIZE 144

#define PLAIN(colour) colour.argb

// For now there's only UI support for two colours
// Eventually there might be arbitrary remapping, in which case a lookup table would be more economical than if/else here
// (since, even in assembly, it'd be >=3 instructions per colour -> >= 6 bytes, vs. 1 byte ea. for LUT that's also O(1) to use)

static inline void %hook(modify_framebuffer) (const GRect layer_rect, const GRect drawable_rect, GBitmap* framebuffer_bmp, uint8_t* framebuffer) {
  const unsigned char x_start = drawable_rect.origin.x;
  const unsigned char x_extent = (drawable_rect.origin.x + drawable_rect.size.w);
  const unsigned short yoff_start = drawable_rect.origin.y * ROW_SIZE;
  const unsigned short yoff_extent = (drawable_rect.origin.y + drawable_rect.size.h) * ROW_SIZE;
  for (unsigned short row_offset = yoff_start; row_offset < yoff_extent; row_offset += ROW_SIZE) {
    const unsigned short xoff_extent = x_extent + row_offset;
    for (int xoff = x_start + row_offset; xoff < xoff_extent; xoff++) {
      if (PLAIN(remap_black) != PLAIN(GColorClear) && framebuffer[xoff] == PLAIN(GColorBlack)) {
        framebuffer[xoff] = PLAIN(remap_black);
      } else if (PLAIN(remap_white) != PLAIN(GColorClear) && framebuffer[xoff] == PLAIN(GColorWhite)) {
        framebuffer[xoff] = PLAIN(remap_white);
      }
    }
  }
}
#else // Aplite: fake it, considering there are only 2^2 choices.
#define ROW_SIZE 20

static inline void %hook(modify_framebuffer) (const GRect layer_rect, const GRect drawable_rect, GBitmap* framebuffer_bmp, uint8_t* framebuffer) {
  GRect framebuffer_bounds = gbitmap_get_bounds(framebuffer_bmp);
  if (remap_white == GColorBlack && remap_black != GColorWhite) {
    memset(framebuffer, 0, framebuffer_bounds.size.h * ROW_SIZE);
  } else if (remap_black == GColorWhite && remap_white != GColorBlack) {
    memset(framebuffer, ~0, framebuffer_bounds.size.h * ROW_SIZE);
  } else if (remap_black == GColorWhite && remap_white == GColorBlack) {
    // We ignore the drawable_rect because it doesn't really matter on Aplite
    for (int i = 0; i < framebuffer_bounds.size.h * ROW_SIZE; ++i) {
      framebuffer[i] = ~framebuffer[i];
    }
  }
}

#endif
#endif