#pragma once
#include "main.h"

#define SPRITE_LIST \
SPRITE(hero            , 0  , 40) \
SPRITE(hero2           , 0  , 40) \
SPRITE(hero3           , 0  , 40) \
SPRITE(hero4           , 0  , 40) \
SPRITE(shadow          , 0  , 20) \
SPRITE(crystal         , 0  , 80) \
SPRITE(border_A        , 0  , 0) \
SPRITE(border_B        , 0  , 0) \
SPRITE(border_A2       , 0  , 0) \
SPRITE(border_B2       , 0  , 0) \
SPRITE(tile            , 0  , 0) \
SPRITE(tile_A          , 0  , 0) \
SPRITE(tile_B          , 0  , 0) \
SPRITE(wall            , 0  , 0) \
SPRITE(wall_switch_A   , 0  , 0) \
SPRITE(wall_switch_A_2 , 0  , 0) \
SPRITE(wall_switch_B   , 0  , 0) \
SPRITE(wall_switch_B_2 , 0  , 0) \
SPRITE(skeleton        , 0  , 40) \
SPRITE(heart           , 0  , 0) \
SPRITE(heart_empty     , 0  , 0) \
SPRITE(gate            , 85 , 0) \
SPRITE(gate_closed     , 85 , 0) \
SPRITE(dialog          , 0  , 0) \
SPRITE(ankh            , 0  , 30) \
SPRITE(hourglass       , 0  , 0) \

typedef struct 
{
    float scale;
    float offset_x;
    float offset_y;
}Camera;

enum
{
#define SPRITE(name,a,b) SPRITE_##name,
    SPRITE_LIST
#undef SPRITE
    NUM_SPRITES
};

void load_sprites(void);
void reload_sprites(void);
void draw_sprite(u32 s, float x, float y, Camera camera);
void draw_sprite_rect(u32 s, float x1, float y1, float x2, float y2);
void draw_sprite_clipped(u32 s, float x, float y, Camera camera);
void draw_sprite_at_tile(u32 s, float x, float y, Camera camera);
void draw_sprite_at_tile_with_alpha(u32 s, float x, float y, Camera camera,
        float alpha);
void draw_dialog(float x, float y, float w, float h, float scale);

extern u32 sprite_size[];
