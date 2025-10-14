#ifndef GPIO_H
#define GPIO_H

//axi 0x81200000 -> led 4 bits
#include "stdio.h"
#include "inttypes.h"
#include "unistd.h"
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

class gpio
{
    uint32_t Vsize;
    uint32_t* Vaddr;
    uint32_t dir;
    int fd;

    int set_fd_handle(const char* fd_path);
    int map_Vsize(const char* fd_path);
    int map_Vaddr(const char* fd_path);
public:
    inline void setDir(bool d){dir = d;}
    inline bool getDir(){return dir;}
    gpio(const char* fd_path);
    ~gpio();
    inline const uint32_t readReg32(size_t idx){idx/=(!idx)?1:4; return Vaddr[idx];}
    inline void writeReg32(uint32_t v, size_t idx){idx/=(!idx)?1:4; Vaddr[idx] = v;}
};


class led_simple : gpio
{
    const uint32_t RegWriteOffst;
    const uint32_t RegstatusOffst;
public:
    led_simple(const char* fd_path, uint32_t _RW, uint32_t _RS)
          : gpio{fd_path},
            RegWriteOffst{_RW},
        RegstatusOffst{_RS}{setDir(0); writeReg32(getDir(), 0x4); }
    inline uint32_t readLedx(size_t offst){ return readReg32(offst);};
    inline void writeLedx(uint32_t v, size_t offst){ writeReg32(v,offst); }
};

class led_RGB : gpio
{
public:

};

#endif // GPIO_H
