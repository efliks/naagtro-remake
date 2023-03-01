/*
 * FlatGrd2 06/02/02
 * Mikolaj Felix a.k.a. Majuma
 * mfelix@polbox.com
 */

#include <dos.h>
#include <dpmi.h>
#include <mem.h>
#include <sys/nearptr.h>

#include <string.h> // for memset

#include <conio.h>


int is_key_pressed()
{
    return kbhit() ? 1 : 0;
}

char get_key_code()
{
    return getch();
}

void set_mode13h()
{
    __dpmi_regs r;

    memset(&r, 0, sizeof(__dpmi_regs));
    r.x.ax = 0x13;
    __dpmi_int(0x10, &r);
}

void unset_mode13h()
{
    __dpmi_regs r;

    memset(&r, 0, sizeof(__dpmi_regs));
    r.x.ax = 0x03;
    __dpmi_int(0x10, &r);
}

void copy_buffer(unsigned char* frame_buffer)
{
    unsigned char* screen;

    __djgpp_nearptr_enable();
    screen = (unsigned char*)0x0a0000 + __djgpp_conventional_base;
    memcpy(screen, frame_buffer, 64000);
    __djgpp_nearptr_disable();
}

void clear_buffer(unsigned char* frame_buffer)
{
    memset(frame_buffer, 0, 64000);
}

void screen_retrace()
{
    while (!(inp(0x03da) & 8))
        ;
    while ((inp(0x03da) & 8))
        ;
}

void set_palette(unsigned char* palette)
{
    int i;

    outp(0x03c8, 0);
    for (i = 0; i < 768; i++) {
        outp(0x03c9, palette[i]);
    }
}

void load_palette(unsigned char* palette)
{
    int i;

    outp(0x03c7, 0);
    for (i = 0; i < 768; i++) {
        palette[i] = inp(0x03c9);
    }
}

void soft_segment(unsigned char* frame_buffer)
{
    unsigned char* ptr = frame_buffer;
    int i, color;

    for (i = 0; i < 0xffff; i++) {
        color = *(ptr + ((i - 1) & 0xffff));
        color += *(ptr + ((i + 1) & 0xffff));
        color += *(ptr + ((i - 320) & 0xffff));
        color += *(ptr + ((i + 320) & 0xffff));
        color >>= 2;
        *ptr = (unsigned char)color;
        ptr++;
    }

    /*
    memset(ptr, 0, 320);
    ptr += 320;

    for (i = 0; i < 64000 - 320 * 2; i++) {
        color = *(ptr - 1) + *(ptr + 1) + *(ptr - 320) + *(ptr + 320);
        color >>= 2;
        *ptr = (unsigned char)color;
        ptr++;
    }

    memset(ptr, 0, 320);
    */
}
