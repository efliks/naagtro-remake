/*****************************************
  NAAGTRO version 2

  tunnel.c   Rotating tunnel effect

  (C) March 5, 2023  M. Feliks
*****************************************/

#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "low.h"
#include "naagtro.h"

struct TunnelElement
{
    unsigned char x, y, t;
};

static struct TunnelElement* ptr_tunnel;
static unsigned char* ptr_texture;

static int tunnel_move;


void create_texture(unsigned char* ptr_texture)
{
    double angle_val = 0;

    unsigned char cosine_lookup[256];
    unsigned char* ptr_lookup = cosine_lookup;

    for (int i = 0; i < 256; i++) {
        *ptr_lookup = (unsigned char)(31.0 * cos(angle_val));
        ptr_lookup++;
        angle_val += 0.025;
    }

    unsigned char tex_a, tex_b, tex_c, tex_d;
    unsigned char* ptr_tex = ptr_texture;

    for (int i = 0; i < 256; i++) {
        unsigned char tt = *(cosine_lookup + (tex_c & 0xff)) + *(cosine_lookup + (tex_d & 0xff));

        for (int j = 0; j < 256; j++) {
            *ptr_tex = tt + *(cosine_lookup + (tex_a & 0xff)) + *(cosine_lookup + (tex_b & 0xff));
            ptr_tex++;
            tex_a += 4;
            tex_b += 3;
        }
        tex_c += 5;
        tex_d += 1;
    }
}

void create_tunnel(struct TunnelElement* ptr_tunnel)
{
    struct TunnelElement* ptr_tun = ptr_tunnel;

    for (double i = -100; i < 100; i++) {
        for (double j = -160; j < 160; j++) {
            double sqr_i2j2 = sqrt(i * i + j * j);

            ptr_tun->x = (unsigned char)( 4096.0 / (sqr_i2j2 + 1) ) & 0xff;
             ptr_tun->y = (unsigned char)( atan2(i, j) * M_PI / 256.0 ) & 0xff;

            int foo = (unsigned char)( (189 - 32 - (int)sqr_i2j2) >> 2 );  // ???
            if (foo < 0) {
               foo = 0;
            }
            ptr_tun->t = (unsigned char)foo;
            ptr_tun++;
        }
    }
}

void init_tunnel()
{
    ptr_texture = (unsigned char *)malloc(65536 * sizeof(unsigned char));
    create_texture(ptr_texture);

    ptr_tunnel = (struct TunnelElement *)malloc(64000 * sizeof(struct TunnelElement));
    create_tunnel(ptr_tunnel);
}

void deinit_tunnel()
{
    if (ptr_tunnel != NULL) {
        free(ptr_tunnel);
    }
    if (ptr_texture != NULL) {
        free(ptr_texture);
    }
}

void do_tunnel(unsigned char* frame_buffer)
{
    tunnel_move++;

    struct TunnelElement* ptr_tun = ptr_tunnel;
    unsigned char* ptr_fb = frame_buffer;

    for (int i = 0; i < 64000; i++, ptr_tun++, ptr_fb++) {
        int texx = (ptr_tun->x + tunnel_move) & 0xff;
         int texy = (ptr_tun->y + tunnel_move) & 0xff;
         *ptr_fb = ( *(ptr_texture + (texy << 8) + texx) ) - ptr_tun->t;
    }
}
