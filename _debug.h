//
// Created by Kaylor on 22-11-21.
//

#ifndef __CAN_DEBUG__
#define __CAN_DEBUG__

#ifdef _DEBUG
#define _debug(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define _debug(fmt, ...) ((void)0)
#endif

#endif
