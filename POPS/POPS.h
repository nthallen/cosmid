#ifndef POPS_H_INCLUDED
#define POPS_H_INCLUDED

#include <time.h>
#include <stdint.h>

typedef struct __attribute__((__packed__)) {
  double   Time;
  uint32_t Part_Num;
  uint32_t HistSum;
  float    PartCon_num_cc;
  uint32_t Baseline;
  uint32_t BLTH;
  float    STD;
  float    MaxSTD;
  float    P_mbar;
  float    MSTemp;
  float    PumpLife;
  float    WidthSTD;
  float    WidthAvg;
  float    Flow;
  float    PumpFB;
  float    LDTemp;
  float    LaserFB;
  float    LD_Mon;
  float    AirTemp;
  float    BatV;
  float    Laser_Current;
  float    Flow_Set;
  uint32_t BL_Start;
  float    TH_Mult;
  float    logmin;
  float    logmax;
  int32_t  Skip_Save;
  uint32_t MinPeakPts;
  uint32_t MaxPeakPts;
  int32_t  RawPts;
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
  uint16_t NBins;
  uint8_t  IntStatus;
  uint8_t  DataStatus;
  uint8_t  Srvr; // Server status enum {POPS_init, POPS_idle, POPS_active, POPS_shutdown}
  uint8_t  Stale;
} POPS_t;
extern POPS_t POPS;

#endif
