#include <pebble.h>


#ifdef PBL_COLOR
#define ROW_SIZE 144
#define ROW_ACTUAL_SIZE 144
#define PIXELS_PER_BYTE 1
#else
#define ROW_SIZE 20
// Aplite rows are padded to word-alignment
#define ROW_ACTUAL_SIZE 18
#define PIXELS_PER_BYTE 8
#endif

// Unfortunately not all of this respects framebuffer_bounds
// Fortunately, the parts that don't are on Aplite, where I think the compositor hides this fact
// Also, this changes the look of the animations on Basalt - it doesn't run fast enough
// Hence, this is contrived so it runs reasonably fast
// (it still doesn't run fast enough)
#ifdef flip_hz
static inline void %hook(modify_framebuffer) (const GRect layer_rect, const GRect drawable_rect, GBitmap* framebuffer_bmp, uint8_t* framebuffer) {
  const char row_buffer[ROW_SIZE];
  const unsigned char x_start = drawable_rect.origin.x;
  const unsigned char x0_start = drawable_rect.origin.x * 2 + drawable_rect.size.w - 1;
  const unsigned char copy_width = drawable_rect.size.w / PIXELS_PER_BYTE;
  const int xoff = x_start/PIXELS_PER_BYTE;
  const unsigned short yoff_start_with_xoff = drawable_rect.origin.y * ROW_SIZE + xoff;
  const unsigned short yoff_half_extent = (drawable_rect.origin.y + drawable_rect.size.h/2) * ROW_SIZE;
  const unsigned short yoff_extent = (drawable_rect.origin.y + drawable_rect.size.h - 1) * ROW_SIZE + xoff;
  for (unsigned short row_offset = yoff_start_with_xoff, row_offset0 = yoff_extent; row_offset < yoff_half_extent ; row_offset += ROW_SIZE, row_offset0 -= ROW_SIZE) {
    // This might be a bit off on Aplite since we're copying entire bytes
    memcpy(row_buffer, &framebuffer[row_offset], copy_width);
    memcpy(&framebuffer[row_offset], &framebuffer[row_offset0], copy_width);
    memcpy(&framebuffer[row_offset0], row_buffer, copy_width);
  }
}
#endif

#ifdef flip_vt
static inline void %hook(modify_framebuffer) (const GRect layer_rect, const GRect drawable_rect, GBitmap* framebuffer_bmp, uint8_t* framebuffer) {
  const unsigned char x_start = drawable_rect.origin.x;
  const unsigned char x0_start = drawable_rect.origin.x * 2 + drawable_rect.size.w - 1;
  const unsigned char x_half_extent = (drawable_rect.origin.x + drawable_rect.size.w/2);
  const unsigned short yoff_start = drawable_rect.origin.y * ROW_SIZE;
  const unsigned short yoff_extent = (drawable_rect.origin.y + drawable_rect.size.h) * ROW_SIZE;
  for (unsigned short row_offset = yoff_start; row_offset < yoff_extent ; row_offset += ROW_SIZE) {
#ifdef PBL_COLOR
    const unsigned short xoff_extent = x_half_extent + row_offset;
    for (int xoff = x_start + row_offset, x0off = x0_start + row_offset; xoff < xoff_extent; xoff++, x0off--) {
      const uint8_t tmp = framebuffer[xoff];
      framebuffer[xoff] = framebuffer[x0off];
      framebuffer[x0off] = tmp;
    }
#else
    // There is an RBIT instruction that can reverse an entire word at once
    // Unfortunately, the framebuffer row does not take up an even multiple of words
    // So we do our best
    for (int x = 0; x < ROW_ACTUAL_SIZE/2; ++x) {
      uint32_t tmp_word, tmp_word_2;
      tmp_word = framebuffer[row_offset + x];
      __asm__("rbit %0, %1\n" : "=r"(tmp_word_2) : "r"(tmp_word));
      const uint8_t tmp = tmp_word_2 >> 24;
      tmp_word = framebuffer[row_offset + (ROW_ACTUAL_SIZE - x - 1)];
      __asm__("rbit %0, %1\n" : "=r"(tmp_word_2) : "r"(tmp_word));
      framebuffer[row_offset + x] = tmp_word_2 >> 24;
      framebuffer[row_offset + (ROW_ACTUAL_SIZE - x - 1)] = tmp;
    }
#endif
  }
}
#endif
