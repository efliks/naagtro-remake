/*****************************************
  Ticker

  ticker.c   Ticker facility

  (C) 2015  Felix Palmen
            https://github.com/Zirias
*****************************************/

#include <errno.h>
#include <time.h>
#include <dpmi.h>

#include "ticker.h"


static uclock_t nextTick;
static uclock_t tickTime;

static unsigned int ring;

enum wmethod {
    WM_NONE,
    WM_YIELD,
    WM_IDLE,
    WM_HLT
};

static enum wmethod method;

static int haveDosidle(void)
{
    __dpmi_regs regs;

    regs.x.ax = 0x1680;
    __dpmi_int(0x28, &regs);
    return regs.h.al ? 0 : 1;
}

static void dosidle(void)
{
    __dpmi_regs regs;

    /* the DOS Idle call is documented to return immediately if no other
     * program is ready to run, therefore do one HLT if we can */
    if (ring == 0)
        __asm__ volatile("hlt");

    regs.x.ax = 0x1680;
    __dpmi_int(0x28, &regs);
    if (regs.h.al) {
        errno = ENOSYS;
    }
}

void ticker_init(void)
{
    /* determine protection ring */
    __asm__("mov %%cs, %0\n\t"
            "and $3, %0"
            : "=r"(ring));

    errno = 0;
    __dpmi_yield();

    if (errno) {
        if (!haveDosidle()) {
            if (ring) {
                method = WM_NONE;
            } else {
                method = WM_HLT;
            }
        } else {
            method = WM_IDLE;
        }
    } else {
        method = WM_YIELD;
    }
}

void ticker_done(void)
{
}

void ticker_start(int msec)
{
    tickTime = msec * UCLOCKS_PER_SEC / 1000;
    nextTick = uclock() + tickTime;
}

void ticker_stop()
{
}

void ticker_wait(void)
{
    switch (method) {
    case WM_NONE:
        while (uclock() < nextTick)
            ;
        break;

    case WM_IDLE:
        while (uclock() < nextTick)
            dosidle();
        break;

    case WM_YIELD:
        while (uclock() < nextTick)
            __dpmi_yield();
        break;

    case WM_HLT:
        while (uclock() < nextTick)
            __asm__ volatile("hlt");
        break;
    }
    nextTick += tickTime;
}
