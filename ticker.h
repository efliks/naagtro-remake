#ifndef _TICKER_H
#define _TICKER_H

void ticker_init();
void ticker_done();

void ticker_start(int);
void ticker_stop();

void ticker_wait();

#endif  // _TICKER_H
