#ifndef __CX16_H__
#define __CX16_H__

#include <stdint.h>

#define __CX16__ // for llvm-mos header
#include "llvm-mos-cx16.h"

typedef struct sprite {
  union {
    struct {
      uint16_t address : 12;
      uint16_t : 3;
      uint16_t mode : 1;
#define Mode4bpp 0
#define Mode8bpp 1
    };
    uint16_t address_4bpp; // may use this as direct access when bpp == 4
  };
  uint16_t x;
  uint16_t y;
  struct {
    uint16_t horizontal_flip : 1;
    uint16_t vertical_flip : 1;
    uint16_t zdepth : 2;
#define SpriteDisabled    0
#define SpriteBelowLayer0 1
#define SpriteBelowLayer1 2
#define SpriteAboveLayer1 3
    uint16_t collision_mask : 4;
    uint16_t palette_offset : 4;
    uint16_t width : 2;
    uint16_t height : 2;
#define SpriteSize8   0
#define SpriteSize16  1
#define SpriteSize32  2
#define SpriteSize64  3
  };
} sprite_t;

inline uint16_t vera_sprite_address(__vram void *p) {
  return (uint16_t) (((uint32_t) p) >> 5);
}

// ----------------------------------------------------------------------
//
// Complete Vera video RAM (except for data part)
//
// ----------------------------------------------------------------------

struct CompleteVera {
  char psg[40];
  char palette[0x200];
  struct sprite sprite[128];
};

#define _CompleteVera (* (struct CompleteVera __vram *)0x1f9c0)

#define Sprite     _CompleteVera.sprite

#endif // __CX16_H__
