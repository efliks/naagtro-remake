/*****************************************
  NAAGTRO version 2

  bumpmap.c   Bump mapping effect

  (C) March 5, 2023  M. Feliks
*****************************************/

#include <stdlib.h>
#include <string.h>  // for memset
#include <time.h>
#include <math.h>

#include "low.h"
#include "naagtro.h"

struct WayLocation
{
    int x, y;
};

#define WAYTABLE_SIZE 256
static struct WayLocation way_table[WAYTABLE_SIZE];

static unsigned char *ptr_bumpmap, *ptr_envmap;

static int light_loc;


void init_bumpmap(unsigned char* bumpmap)
{
    const int bmap_size = 65536;
    const int bmap_visible_size = 320 * 200;  // 64000

    const int naaglogo_size = 320 * 70;  // 22400

    const int filler_size = (bmap_visible_size - naaglogo_size) / 2;  // 20800

    const int otherfill_size = bmap_size - 2 * filler_size - naaglogo_size;  // 1536
    const int prefill_size = ( otherfill_size / 2 / 320 ) * 320;  // 640
    const int postfill_size = otherfill_size - prefill_size;  // 896

    unsigned char* ptr_bumpmap = bumpmap;
    for (int i = 0; i < prefill_size + filler_size; i++, ptr_bumpmap++) {
        *ptr_bumpmap = 255;
    }

    //unpack NAAG logo
    unsigned char* ptr_packed_logo = naaglogo;

    for (int i = 0; i < naaglogo_size / 8; i++, ptr_packed_logo++) {
        for (int j = 7; j >= 0; j--, ptr_bumpmap++) {
            if ((*ptr_packed_logo >> j) & 1) {
                *ptr_bumpmap = 0;
            }
            else {
                *ptr_bumpmap = 255;
            }
        }
    }

    for (int i = 0; i < filler_size + postfill_size; i++, ptr_bumpmap++) {
        *ptr_bumpmap = 255;
    }

    //add random noise
    ptr_bumpmap = bumpmap;
    for (int i = 0; i < bmap_size; i++, ptr_bumpmap++) {
        if (*ptr_bumpmap != 0) {
           *ptr_bumpmap = (unsigned char)(rand() & 255);
        }
    }

    //blur several times
    for (int i = 0; i < 4; i++) {
        do_segment_blur(bumpmap, 320);
    }

    //done initializing bump map
}

void init_envmap(unsigned char* envmap)
{
    unsigned char* ptr_envmap = envmap;

    for (double y = -128; y < 128; y++) {
        for (double x = -128; x < 128; x++, ptr_envmap++) {
            double v = 63 * (1 - sqrt(pow(x / 128, 2) + pow(y / 128, 2)));
            char w = (char)v;
            if (w < 0) {
                w = 0;
            }
            else if (w > 63) {
                w = 63;
            }
            *ptr_envmap = (unsigned char)w;
        }
    }
}

void init_way(struct WayLocation* way_table, int size_table)
{
    struct WayLocation* ptr_table = way_table;

    for (int i = 0; i < size_table; i++) {
        ptr_table->x = (int)(120 * cos(i * M_PI / (size_table / 2)) ) + 320 / 2;
        ptr_table->y = (int)(35 * sin(i * M_PI / (size_table / 2)) ) + 200 / 2;
        ptr_table++;
    }
}

void init_bump_mapping()
{
    //initialize RNG
    srand(time(NULL));

    //initialize bump map
    ptr_bumpmap = (unsigned char *)malloc(65536 * sizeof(unsigned char));
    init_bumpmap(ptr_bumpmap);

    //initialize environ map
    ptr_envmap = (unsigned char *)malloc(65536 * sizeof(unsigned char));
    init_envmap(ptr_envmap);

    //initialize way table
    init_way(way_table, WAYTABLE_SIZE);
}

void deinit_bump_mapping()
{
    if (ptr_envmap != NULL) {
        free(ptr_envmap);
    }
    if (ptr_bumpmap != NULL) {
        free(ptr_bumpmap);
    }
}

void do_bump_mapping(unsigned char* frame_buffer)
{
    light_loc = (light_loc + 1) % WAYTABLE_SIZE;

    unsigned char* ptr_fb = frame_buffer + 320;
    unsigned char* ptr_bump = ptr_bumpmap + 3 * 320;

    memset(frame_buffer, 0, 320);
    memset(frame_buffer + 199 * 320, 0, 320);

    for (int y = 1; y < 199; y++) {
        for (int x = 0; x < 320; x++, ptr_fb++, ptr_bump++) {
            int v = *(ptr_bump - 1) - *(ptr_bump + 1) - x + way_table[light_loc].x + 128;
            if (v < 0 || v > 255) {
                v = 0;
            }
            int w = *(ptr_bump - 320) - *(ptr_bump + 320) - y + way_table[light_loc].y + 128;
            if (w < 0 || w > 255) {
                w = 0;
            }
            unsigned char pix = *(ptr_envmap + (w << 8) + v);
            *ptr_fb = pix;
        }
    }
}
