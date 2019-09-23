#pragma once
#include <stdlib.h>
#ifndef _USE_MATH_DEFINES
  #define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <limits>
#include <vector>

namespace Assignment3
{
  struct ImaginaryNum
  {
    float real = 0.f;
    float imaginary = 0.f;
    void Conjugate()
    {
      imaginary *= -1.f;
    }

    // real + imaginary * j
  };
  unsigned char* ImaginaryToChar(int w, int h, ImaginaryNum * data);
  unsigned char* LogImaginaryToChar(int w, int h, ImaginaryNum* data);
  ImaginaryNum* DirectFourierTransform(int w, int h, unsigned char* data);
  ImaginaryNum  DirectFourierHelper(int w, int h, unsigned char* data, int x, int y);
  ImaginaryNum* InverseFourierTransform(int w, int h, ImaginaryNum* data);
  ImaginaryNum  InverseFourierHelper(int w, int h, ImaginaryNum* data, int x, int y);
  ImaginaryNum* SeparableFourierTransform(int w, int h, unsigned char* data);
  ImaginaryNum* SeperableInverseFourierTransform(int w, int h, ImaginaryNum* data);
  ImaginaryNum* FastFourierTransform(int w, int h, unsigned char* data);
  ImaginaryNum* InverseFastFourierTransform(int w, int h, ImaginaryNum* data);
  unsigned      ReverseBits(unsigned original, unsigned bitsize);

}