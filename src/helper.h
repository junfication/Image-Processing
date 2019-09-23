#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <utility>
#include <string>
#include <vector>
#include <cmath>
#include <set>
#include <random>
#include <map>
#include <wx/wx.h>

typedef std::vector<std::vector<double>> matrix;

struct Color
{
  double r;
  double g;
  double b;
  Color() : r(0.0), g(0.0), b(0.0) {}
  void Clamp()
  {
    r = r > 255.0 ? 255.0 : r;
    g = g > 255.0 ? 255.0 : g;
    b = b > 255.0 ? 255.0 : b;

    r = r < 0.0 ? 0.0 : r;
    g = g < 0.0 ? 0.0 : g;
    b = b < 0.0 ? 0.0 : b;
  }
  Color operator+(const Color& rhs)
  {
    Color out;
    out.r = r + rhs.r;
    out.g = g + rhs.g;
    out.b = b + rhs.b;
    return out;
  }
  void fabs()
  {
    r = std::abs(r);
    g = std::abs(g);
    b = std::abs(b);
  }
};

struct Package
{
  std::string type;
  std::string message;
  unsigned int width;
  unsigned int height;
  unsigned int max;
};

struct labelColor
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
  labelColor() : r(0), g(0), b(0) {}
};

enum M_NEIGHBOUR
{
  TOPLEFT = 0,
  TOPRIGHT = 1,
  BTMLEFT = 2,
  BTMRIGHT = 3
};

std::pair<unsigned char*, Package> LoadHelper(std::string& filename);
void               SaveHelper(Package p, unsigned char* output, std::string filename);
unsigned char*     Resize(int w, int h, unsigned char* data, Package p);
void               Inverse(int w, int h, unsigned char* data);
unsigned char*     Addition(int w, int h, unsigned char* data1, unsigned char* data2, unsigned char* im);
unsigned char*     Subtraction(int w, int h, unsigned char* data1, unsigned char* data2, unsigned char* im);
unsigned char*     Product(int w, int h, unsigned char* data1, unsigned char* data2, unsigned char* im);
void               H_Equalization(int w, int h, unsigned char* data, unsigned int max);
unsigned char*     LogTransform(int w, int h, unsigned char* data, double c, double base);
unsigned char*     GammaTransform(int w, int h, unsigned char* data, double c, double sigma);
unsigned char*     GaussianBlur(int w, int h, unsigned char* data, int N, double sigma);
void               CreateGaussianKernel(int N, matrix& m, double sigma);
Color              MaskingAssistant(int x, int y, matrix m, unsigned char* data, int N, int w, int h);
unsigned char*     EdgeDetetectionSobel(int w, int h, unsigned char* data);
unsigned char*     UnsharpMaskOperation(int w, int h, unsigned char* data, int N, double sigma, double K);
unsigned char*     ConnectedComponentLabeling(int w, int h, unsigned char* data, int min, int max);
std::set<unsigned> GetNeighbours(unsigned* labels, unsigned char* data, int x, int y, int w, int h, int min, int max);
void               GenerateLabelColors(std::vector<labelColor>& lc);
unsigned           CheckMNeighbour(unsigned* labels, unsigned char* data, int x, int y, int w, int h, M_NEIGHBOUR n, int min, int max);
unsigned           ResolveConflict(unsigned* labels, unsigned char* data, int x, int y, int w, int h,int min,int max);
bool               CommandLineInput(std::string s, std::string dir, std::string& myoutput);
bool               CheckExtension(std::string f);
bool               IsInsideSet(int min, int max, unsigned char r, unsigned char g, unsigned char b);
bool               IntheMiddle(int min, int max, int val);