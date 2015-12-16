#include "map.h"

Map load_map(const char *filename)
{
    //TODO(Vidar): map tile types
    Map map = {};
    FILE *f = fopen(filename,"rb");
    if(f){
        fread(&map.w,sizeof(s32),1,f);
        fread(&map.h,sizeof(s32),1,f);
        fread(&map.layers,sizeof(s32),1,f);
        map.tiles = malloc(map.w*map.h*map.layers*sizeof(s16));
        fread(map.tiles,map.w*map.h*map.layers,sizeof(s16),f);
        fclose(f);
    }
    return map;
}

void save_map(const char *filename, Map map)
{
    FILE *f = fopen(filename,"wb");
    if(f){
        fwrite(&map.w,sizeof(s32),1,f);
        fwrite(&map.h,sizeof(s32),1,f);
        fwrite(&map.layers,sizeof(s32),1,f);
        fwrite(map.tiles,map.w*map.h*map.layers,sizeof(s16),f);
        fclose(f);
    }
}



void delete_map(Map map)
{
    free(map.tiles);
}

