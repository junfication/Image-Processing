#pragma once

#ifndef _USE_MATH_DEFINES
  #define _USE_MATH_DEFINES
#endif

#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <random>

#define PEPPERNOISE 1.0
#define SALTNOISE 0.0

namespace Assignment4
{
  unsigned char* PepperSaltNoise(int w, int h, unsigned char* data, double pa, double pb);
  unsigned char* MedianFilter(int w, int h, unsigned char* data);
  unsigned char* LocalNoiseReduction(int w, int h, unsigned char* data, double NoiseVariance, int mask = 7);
  unsigned char* AdaptiveMedianFilter(int w, int h, unsigned char* data, int maxSize);
  unsigned char* GaussianNoise(int w, int h, unsigned char* data, double mean, double variance);
}