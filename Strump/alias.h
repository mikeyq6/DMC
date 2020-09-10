#pragma once

// #ifndef _MSC_VER
// typedef __int8 int8_t;
// typedef unsigned __int8 uint8_t;
// typedef __int16 int16_t;
// typedef unsigned __int16 uint16_t;
// typedef __int32 int32_t;
// typedef unsigned __int32 uint32_t;
// typedef __int64 int64_t;
// typedef unsigned __int64 uint64_t;
// #endif

/* Memory Location Shortcuts */
constexpr auto P1 = 0xff00;
constexpr auto SB = 0xff01;
constexpr auto SC = 0xff02;
constexpr auto DIV = 0xff04;
constexpr auto TIMA = 0xff05;
constexpr auto TMA = 0xff06;
constexpr auto TAC = 0xff07;
constexpr auto IF = 0xff0f;
constexpr auto NR10 = 0xff10;
constexpr auto NR11 = 0xff11;
constexpr auto NR12 = 0xff12;
constexpr auto NR13 = 0xff13;
constexpr auto NR14 = 0xff14;
constexpr auto NR21 = 0xff16;
constexpr auto NR22 = 0xff17;
constexpr auto NR23 = 0xff18;
constexpr auto NR24 = 0xff19;
constexpr auto NR30 = 0xff1a;
constexpr auto NR31 = 0xff1b;
constexpr auto NR32 = 0xff1c;
constexpr auto NR33 = 0xff1d;
constexpr auto NR34 = 0xff1e;
constexpr auto NR41 = 0xff20;
constexpr auto NR42 = 0xff21;
constexpr auto NR43 = 0xff22;
constexpr auto NR44 = 0xff23;
constexpr auto NR50 = 0xff24;
constexpr auto NR51 = 0xff25;
constexpr auto NR52 = 0xff26;

constexpr auto LCDC = 0xff40;
constexpr auto STAT = 0xff41;
constexpr auto SCY = 0xff42;
constexpr auto SCX = 0xff43;
constexpr auto LY = 0xff44;
constexpr auto LYC = 0xff45;
constexpr auto DMA = 0xff46;
constexpr auto BGP = 0xff47;
constexpr auto OBP0 = 0xff48;
constexpr auto OBP1 = 0xff49;
constexpr auto WY = 0xff4a;
constexpr auto WX = 0xff4b;
constexpr auto ENDSTART = 0xff50;
constexpr auto HDMA1 = 0xff50;
constexpr auto HDMA2 = 0xff51;
constexpr auto HDMA3 = 0xff52;
constexpr auto HDMA4 = 0xff53;
constexpr auto HDMA5 = 0xff54;
constexpr auto BCPS = 0xff68;
constexpr auto BCPD = 0xff69;
constexpr auto OCPS = 0xff6a;
constexpr auto OCPD = 0xff6b;
constexpr auto VBK = 0xFF4F;

constexpr auto IE = 0xffff;
