#ifndef _LogLevel_h_
#define _LogLevel_h_

extern int verbose_;
#define LOG_LEVEL(n) (verbose_ >= (n))

#endif