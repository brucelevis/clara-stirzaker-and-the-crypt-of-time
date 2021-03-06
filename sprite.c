#include "sprite.h"
#include "main.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef DEBUG
//NOTE(Vidar): For file reloading
#include <sys/inotify.h>
#include <limits.h>
#include <unistd.h>
#include <poll.h>
#else
#include "incbin.h"
#endif

extern u32 tile_w;
extern u32 tile_h;

#ifdef DEBUG
    static const char* sprite_filenames[NUM_SPRITES] = 
    {
    #define SPRITE(name,a,b) #name ".png",
        SPRITE_LIST
    #undef SPRITE
    };
    static void init_sprite_filesizes(){}
#else
    #define SPRITE(name,a,b) INCBIN(name, "data/" #name ".png");
        SPRITE_LIST
    #undef SPRITE
    const u8 *sprite_data[] = {
    #define SPRITE(name,a,b) g ## name ## Data,
        SPRITE_LIST
    #undef SPRITE
    };
    u32 sprite_filesize[NUM_SPRITES];
    static void init_sprite_filesizes(){
        u32 i=0;
    #define SPRITE(name,a,b) sprite_filesize[i++] =  g ## name ## Size;
        SPRITE_LIST
    #undef SPRITE
    }
#endif

static const u32 sprite_center[NUM_SPRITES*2] = 
{
#define SPRITE(name,center_x,center_y) center_x, center_y,
    SPRITE_LIST
#undef SPRITE
};
u32 sprite_size[NUM_SPRITES*2] = {};
static SDL_Texture *sprite_texture[NUM_SPRITES] = {};

static const u32 rmask = 0x000000ff;
static const u32 gmask = 0x0000ff00;
static const u32 bmask = 0x00ff0000;
static const u32 amask = 0xff000000;

static SDL_Texture *load_texture(u32 index, s32 *w, s32 *h){
#ifdef DEBUG
    char sprite_filename_buffer[128];
    sprintf(sprite_filename_buffer,"data/%s",sprite_filenames[index]);
    FILE *f = fopen(sprite_filename_buffer,"rb");
    if(f){
        s32 c;
        u8 *sprite_pixels = stbi_load_from_file(f,w,h,&c,4);
        fclose(f);
        SDL_Surface *sprite_surf = SDL_CreateRGBSurfaceFrom(sprite_pixels,
                *w,*h,32, (*w)*4,rmask,gmask,bmask,amask);
        SDL_Texture *tex= SDL_CreateTextureFromSurface(renderer,
                sprite_surf);
        SDL_FreeSurface(sprite_surf);
        free(sprite_pixels);
        return tex;
    }
    *w = *h = 0;
    return 0;
#else
    s32 c;
    u8 *sprite_pixels = stbi_load_from_memory(sprite_data[index],
            sprite_filesize[index],w,h,&c,4);
    SDL_Surface *sprite_surf = SDL_CreateRGBSurfaceFrom(sprite_pixels,
            *w,*h,32, (*w)*4,rmask,gmask,bmask,amask);
    SDL_Texture *tex= SDL_CreateTextureFromSurface(renderer,
            sprite_surf);
    SDL_FreeSurface(sprite_surf);
    free(sprite_pixels);
    return tex;
#endif
}

void load_sprites()
{
    init_sprite_filesizes();
    for(u32 i=0;i<NUM_SPRITES;i++){
        s32 w,h;
        sprite_texture[i] = load_texture(i, &w, &h);
        sprite_size[i*2] = (u32)w;
        sprite_size[i*2+1] = (u32)h;
    }
}

#ifdef DEBUG
void reload_sprites()
{
    static s32 watch = -1;
    static s32 data_watch = -1;
    if(watch == -1){
        watch = inotify_init();
        if(watch == -1){
            printf("Error adding inotify watch!\n");
        }else{
            data_watch = inotify_add_watch(watch,"data/",IN_CLOSE_WRITE);
            if(data_watch == -1){
                printf("Error adding inotify watch!\n");
            }
        }
    }
    struct pollfd pfd = {watch,POLLIN|POLLPRI,0};
    if(poll(&pfd,1,0) > 0){
        const size_t buffer_len = sizeof(struct inotify_event) + NAME_MAX + 1;
        struct inotify_event *e = malloc(buffer_len);
        read(watch,e,buffer_len);
        if(e->mask == IN_CLOSE_WRITE){
            for(u32 i=0;i<NUM_SPRITES;i++){
                if(strcmp(sprite_filenames[i],e->name) == 0){
                    printf("Reloading sprite %s\n", e->name);
                    if(sprite_texture[i] != 0){
                        SDL_DestroyTexture(sprite_texture[i]);
                    }
                    s32 w,h;
                    sprite_texture[i] = load_texture(i, &w, &h);
                    sprite_size[i*2] = (u32)w;
                    sprite_size[i*2+1] = (u32)h;
                }
            }
        }
    }
}
#endif

void draw_sprite(u32 s, float x, float y, Camera camera)
{
    SDL_Rect dest_rect = {
        (s32)((x-sprite_center[s*2]-(sprite_size[s*2]-tile_w)*.5f)*camera.scale
                +camera.offset_x)
        ,(s32)((y-sprite_center[s*2+1]-sprite_size[s*2+1]+tile_h)*camera.scale
                +camera.offset_y)
        ,(s32)(sprite_size[s*2]*camera.scale)
        ,(s32)(sprite_size[s*2+1]*camera.scale)};
    SDL_RenderCopy(renderer,sprite_texture[s],0,&dest_rect);
}

void draw_sprite_rect(u32 s, float x1, float y1, float x2, float y2)
{
    SDL_Rect dest_rect = {(s32)x1,(s32)y1,(s32)(x2-x1),(s32)(y2-y1)};
    SDL_RenderCopy(renderer,sprite_texture[s],0,&dest_rect);
}

void draw_sprite_clipped(u32 s, float x, float y, Camera camera)
{
    SDL_Rect dest_rect = {(s32)(x*camera.scale+camera.offset_x)
        ,(s32)(y*camera.scale+camera.offset_y)
        ,(s32)(tile_w*camera.scale) ,(s32)(tile_h*camera.scale)};
    SDL_Rect src_rect = {0,0,(s32)tile_w,(s32)tile_h};
    SDL_RenderCopy(renderer,sprite_texture[s],&src_rect,&dest_rect);
}

void draw_sprite_at_tile_with_alpha(u32 s, float x, float y, Camera camera,
        float alpha)
{
    u8 a = (u8)(alpha*255);
    SDL_SetTextureAlphaMod(sprite_texture[s],a);
    draw_sprite(s,x*tile_w,y*tile_h, camera);
    SDL_SetTextureAlphaMod(sprite_texture[s],255);
}

void draw_sprite_at_tile(u32 s, float x, float y, Camera camera)
{
    draw_sprite(s,x*tile_w,y*tile_h, camera);
}

void draw_dialog(float x, float y, float w, float h, float scale)
{
    float border_x = 0.1f;
    float border_y = 0.1f;
    float sw = sprite_size[SPRITE_dialog*2];
    float sh = sprite_size[SPRITE_dialog*2+1];
    float x1 = border_x*sw;
    float y1 = border_y*sw;
        //TODO(Vidar): Make this prettier...
#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wfloat-conversion"
    {
        SDL_Rect dest_rect = {x,y ,x1*scale,y1*scale};
        SDL_Rect src_rect = {0,0,x1, y1};
        SDL_RenderCopy(renderer,sprite_texture[SPRITE_dialog],&src_rect,&dest_rect);
    }
    {
        SDL_Rect dest_rect = {x,y+y1*scale ,x1*scale,(h-y1)*scale};
        SDL_Rect src_rect = {0,y1,x1, sh-2*y1};
        SDL_RenderCopy(renderer,sprite_texture[SPRITE_dialog],&src_rect,&dest_rect);
    }
    {
        SDL_Rect dest_rect = {x,y+(h-y1)*scale ,x1*scale,y1*scale};
        SDL_Rect src_rect = {0,sh-y1,x1,y1};
        SDL_RenderCopy(renderer,sprite_texture[SPRITE_dialog],&src_rect,&dest_rect);
    }

    {
        SDL_Rect dest_rect = {x+(w-x1)*scale,y ,x1*scale,y1*scale};
        SDL_Rect src_rect = {sw-x1,0,x1, y1};
        SDL_RenderCopy(renderer,sprite_texture[SPRITE_dialog],&src_rect,&dest_rect);
    }
    {
        SDL_Rect dest_rect = {x+(w-x1)*scale,y+y1*scale ,x1*scale,(h-2*y1)*scale+1.f};
        SDL_Rect src_rect = {sw-x1,y1,x1, sh-2*y1};
        SDL_RenderCopy(renderer,sprite_texture[SPRITE_dialog],&src_rect,&dest_rect);
    }
    {
        SDL_Rect dest_rect = {x+(w-x1)*scale,y+(h-y1)*scale ,x1*scale,y1*scale};
        SDL_Rect src_rect = {sw-x1,sh-y1,x1, y1};
        SDL_RenderCopy(renderer,sprite_texture[SPRITE_dialog],&src_rect,&dest_rect);
    }

    {
        SDL_Rect dest_rect = {x+(x1*scale),y ,(w-2*x1)*scale+1.f,y1*scale};
        SDL_Rect src_rect = {x1,0 ,sw*(1.f-2*border_x) ,y1};
        SDL_RenderCopy(renderer,sprite_texture[SPRITE_dialog],&src_rect,&dest_rect);
    }
    {
        SDL_Rect dest_rect = {x+x1*scale,y+y1*scale ,(w-2*x1)*scale+1.f,(h-2*y1+1.f)*scale};
        SDL_Rect src_rect = {x1,y1 ,sw*(1.f-2*border_x) ,sh-2*y1};
        SDL_RenderCopy(renderer,sprite_texture[SPRITE_dialog],&src_rect,&dest_rect);
    }
    {
        SDL_Rect dest_rect = {x+x1*scale,y+(h-y1)*scale ,(w-2*x1)*scale+1.f,y1*scale};
        SDL_Rect src_rect = {x1,sh-y1,sw*(1.f-2*border_x) ,y1};
        SDL_RenderCopy(renderer,sprite_texture[SPRITE_dialog],&src_rect,&dest_rect);
    }
#pragma GCC diagnostic pop
}

