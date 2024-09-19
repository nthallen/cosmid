#ifndef POPS_H_INCLUDED
#define POPS_H_INCLUDED

#include <time.h>
#include <stdint.h>

typedef struct __attribute__((__packed__)) {
  double Time;
  uint32_t Part_Num;
  float    PartCon_num_cc;
  uint32_t Baseline;
  float    STD;
  float    P_mbar;
  float    Flow;
  float    LDTemp;
  float    LD_Mon;
  float    Temp;
  uint32_t Bin01;
  uint32_t Bin02;
  uint32_t Bin03;
  uint32_t Bin04;
  uint32_t Bin05;
  uint32_t Bin06;
  uint32_t Bin07;
  uint32_t Bin08;
  uint32_t Bin09;
  uint32_t Bin10;
  uint32_t Bin11;
  uint32_t Bin12;
  uint32_t Bin13;
  uint32_t Bin14;
  uint32_t Bin15;
  uint32_t Bin16;
  uint8_t  Srvr; // Server status enum {POPS_init, POPS_idle, POPS_active, POPS_shutdown}
  uint8_t  Stale;
} POPS_t;
extern POPS_t POPS;

#endif
