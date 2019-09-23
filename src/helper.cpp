#include "helper.h"

std::pair<unsigned char*, Package> LoadHelper(std::string & filename)
{
  Package p;
  unsigned char* data = nullptr;
  std::ifstream ifs;
  ifs.open(filename.c_str(), std::ifstream::in);
  std::string str((std::istreambuf_iterator<char>(ifs)),
    std::istreambuf_iterator<char>());
  if (ifs.fail())
  {
    ifs.close();
    return std::pair<unsigned char*, Package>(data, p);
  }
  if (str.empty())
  {
    ifs.close();
    return std::pair<unsigned char*, Package>(data, p);
  }
  auto type = str.substr(0, str.find_first_of('\n'));
  str.erase(0, str.find_first_of('\n') + 1);
  auto mess = str.substr(0, str.find_first_of('\n'));
  str.erase(0, str.find_first_of('\n') + 1);
  auto width = str.substr(0, str.find_first_of(' '));
  while (width[0] < 48 || width[0] > 57) width.erase(width.begin());
  str.erase(0, str.find_first_of(' ') + 1);
  auto height = str.substr(0, str.find_first_of('\n'));
  str.erase(0, str.find_first_of('\n') + 1);
  auto max = str.substr(0, str.find_first_of('\n'));
  str.erase(0, str.find_first_of('\n') + 1);
  p.type = type;
  p.message = mess;
  p.width = std::stoi(width);
  p.height = std::stoi(height);
  p.max = std::stoi(max);
  ifs.close();
  data = (unsigned char*)malloc(p.width * p.height * 3);

  char* token;

  token = strtok(const_cast<char*>(str.c_str()), "\n ");
  unsigned i = 0;

  while (token != NULL) {
    auto clr = atoi(token);
    data[i] = static_cast<unsigned char>((clr / static_cast<float>(p.max)) * 255);
    ++i;
    token = strtok(NULL, "\n ");
  }
  return std::pair<unsigned char*, Package>(data, p);
}

void SaveHelper(Package p, unsigned char * output, std::string filename)
{
  std::ofstream outfile(filename);
  std::string data;

  int w = p.width > 0 ?  p.width : 0;
  int h = p.height > 0 ? p.height : 0;

  data += p.type;
  data += "\n";
  data += p.message;
  data += "\n";
  data += std::to_string(w);
  data += " ";
  data += std::to_string(h);
  data += "\n";
  data += std::to_string(p.max);
  data += "\n";

  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      data += std::to_string(static_cast<unsigned char>((output[(i * w + j) * 3] / 255.f) * (p.max)));
      data += " ";
      data += std::to_string(static_cast<unsigned char>(((output[(i * w + j) * 3 + 1]) / 255.f) * (p.max)));
      data += " ";
      data += std::to_string(static_cast<unsigned char>(((output[(i * w + j) * 3 + 2]) / 255.f) * (p.max)));
      if (j != w) data += " ";
    }
    data += "\n";
  }
  outfile << data;
  outfile.close();


}

unsigned char * Resize(int w, int h, unsigned char * data, Package p)
{
  unsigned char* out = (unsigned char*)malloc(w * h * 3);
  int ow = p.width;
  int oh = p.height;
  float sw = (float)ow / (float)w;
  float sh = (float)oh / (float)h;
  for (int i = 0; i < h; ++i)
  {
    int floor_i = (int)(i * sh);
    for (int j = 0; j < w; ++j)
    {
      int floor_j = (int)(j * sw);
      unsigned pixel = ((i * w) + j) * 3;
      unsigned o_pixel = ((floor_i * ow) + floor_j) * 3;
      out[pixel] = data[o_pixel];
      out[pixel + 1] = data[o_pixel + 1];
      out[pixel + 2] = data[o_pixel + 2];
    }
  }
  free(data);
  return out;
}

void Inverse(int w, int h, unsigned char * data)
{
  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      unsigned pixel = ((i * w) + j) * 3;
      data[pixel] = 255 - data[pixel];
      data[pixel + 1] = 255 - data[pixel + 1];
      data[pixel + 2] = 255 - data[pixel + 2];
    }
  }
}

unsigned char * Addition(int w, int h, unsigned char* data1, unsigned char* data2, unsigned char* im)
{
  unsigned char* output = static_cast<unsigned char*>(malloc(w * h * 3));
  if (im)
  {
    for (int i = 0; i < h; ++i)
    {
      int tmp = (int)(i * w);
      for (int j = 0; j < w; ++j)
      {
        unsigned pixel = (tmp + j) * 3;
        int red = static_cast<int>(data1[pixel]) + static_cast<int>(data2[pixel]);
        int green = static_cast<int>(data1[pixel + 1]) + static_cast<int>(data2[pixel + 1]);
        int blue = static_cast<int>(data1[pixel + 2]) + static_cast<int>(data2[pixel + 2]);
        red = red > 255 ? 255 : red;
        green = green > 255 ? 255 : green;
        blue = blue > 255 ? 255 : blue;
        im[pixel] = output[pixel] = static_cast<unsigned char>(red);
        im[pixel + 1] = output[pixel + 1] = static_cast<unsigned char>(green);
        im[pixel + 2] = output[pixel + 2] = static_cast<unsigned char>(blue);
      }
    }
  }
  else
  {
    for (int i = 0; i < h; ++i)
    {
      int tmp = (int)(i * w);
      for (int j = 0; j < w; ++j)
      {
        unsigned pixel = (tmp + j) * 3;
        int red = static_cast<int>(data1[pixel]) + static_cast<int>(data2[pixel]);
        int green = static_cast<int>(data1[pixel + 1]) + static_cast<int>(data2[pixel + 1]);
        int blue = static_cast<int>(data1[pixel + 2]) + static_cast<int>(data2[pixel + 2]);
        red = red > 255 ? 255 : red;
        green = green > 255 ? 255 : green;
        blue = blue > 255 ? 255 : blue;
        output[pixel] = static_cast<unsigned char>(red);
        output[pixel + 1] = static_cast<unsigned char>(green);
        output[pixel + 2] = static_cast<unsigned char>(blue);
      }
    }
  }
  return output;
}

unsigned char * Subtraction(int w, int h, unsigned char * data1, unsigned char * data2, unsigned char * im)
{
  unsigned char* output = static_cast<unsigned char*>(malloc(w * h * 3));
  if (im)
  {
    for (int i = 0; i < h; ++i)
    {
      int tmp = (int)(i * w);
      for (int j = 0; j < w; ++j)
      {
        unsigned pixel = (tmp + j) * 3;
        int red = static_cast<int>(data1[pixel]) - static_cast<int>(data2[pixel]);
        int green = static_cast<int>(data1[pixel + 1]) - static_cast<int>(data2[pixel + 1]);
        int blue = static_cast<int>(data1[pixel + 2]) - static_cast<int>(data2[pixel + 2]);
        red = red < 0 ? 0 : red;
        green = green < 0 ? 0 : green;
        blue = blue < 0 ? 0 : blue;
        im[pixel] = output[pixel] = static_cast<unsigned char>(red);
        im[pixel + 1] = output[pixel + 1] = static_cast<unsigned char>(green);
        im[pixel + 2] = output[pixel + 2] = static_cast<unsigned char>(blue);
      }
    }
  }
  else
  {
    for (int i = 0; i < h; ++i)
    {
      int tmp = (int)(i * w);
      for (int j = 0; j < w; ++j)
      {
        unsigned pixel = (tmp + j) * 3;
        int red = static_cast<int>(data1[pixel]) - static_cast<int>(data2[pixel]);
        int green = static_cast<int>(data1[pixel + 1]) - static_cast<int>(data2[pixel + 1]);
        int blue = static_cast<int>(data1[pixel + 2]) - static_cast<int>(data2[pixel + 2]);
        red = red < 0 ? 0 : red;
        green = green < 0 ? 0 : green;
        blue = blue < 0 ? 0 : blue;
        output[pixel] = static_cast<unsigned char>(red);
        output[pixel + 1] = static_cast<unsigned char>(green);
        output[pixel + 2] = static_cast<unsigned char>(blue);
      }
    }
  }
  return output;
}

unsigned char * Product(int w, int h, unsigned char * data1, unsigned char * data2, unsigned char * im)
{
  unsigned char* output = static_cast<unsigned char*>(malloc(w * h * 3));
  if (im)
  {
    for (int i = 0; i < h; ++i)
    {
      int tmp = (int)(i * w);
      for (int j = 0; j < w; ++j)
      {
        unsigned pixel = (tmp + j) * 3;
        int red = static_cast<int>(static_cast<int>(data1[pixel])     * static_cast<float>((data2[pixel]) / 255.f));
        int green = static_cast<int>(static_cast<int>(data1[pixel + 1]) * static_cast<float>((data2[pixel + 1]) / 255.f));
        int blue = static_cast<int>(static_cast<int>(data1[pixel + 2]) * static_cast<float>((data2[pixel + 2]) / 255.f));
        red = red < 0 ? 0 : red;
        green = green < 0 ? 0 : green;
        blue = blue < 0 ? 0 : blue;
        red = red > 255 ? 255 : red;
        green = green > 255 ? 255 : green;
        blue = blue > 255 ? 255 : blue;
        im[pixel] = output[pixel] = static_cast<unsigned char>(red);
        im[pixel + 1] = output[pixel + 1] = static_cast<unsigned char>(green);
        im[pixel + 2] = output[pixel + 2] = static_cast<unsigned char>(blue);
      }
    }
  }
  else
  {
    for (int i = 0; i < h; ++i)
    {
      int tmp = (int)(i * w);
      for (int j = 0; j < w; ++j)
      {
        unsigned pixel = (tmp + j) * 3;
        int red = static_cast<int>(static_cast<int>(data1[pixel])     * static_cast<float>((data2[pixel]) / 255.f));
        int green = static_cast<int>(static_cast<int>(data1[pixel + 1]) * static_cast<float>((data2[pixel + 1]) / 255.f));
        int blue = static_cast<int>(static_cast<int>(data1[pixel + 2]) * static_cast<float>((data2[pixel + 2]) / 255.f));
        red = red < 0 ? 0 : red;
        green = green < 0 ? 0 : green;
        blue = blue < 0 ? 0 : blue;
        red = red > 255 ? 255 : red;
        green = green > 255 ? 255 : green;
        blue = blue > 255 ? 255 : blue;
        output[pixel] = static_cast<unsigned char>(red);
        output[pixel + 1] = static_cast<unsigned char>(green);
        output[pixel + 2] = static_cast<unsigned char>(blue);
      }
    }
  }
  return output;
}

void H_Equalization(int w, int h, unsigned char * data, unsigned int max)
{
  unsigned size = w * h * 3;
  std::vector<unsigned> count;
  std::vector<float> histogram;
  std::vector<unsigned char> map;
  count.resize(max + 1);
  histogram.resize(max + 1);
  map.resize(max + 1);
  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      unsigned pixel = ((i * w) + j) * 3;
      data[pixel]     = static_cast<unsigned char>((data[pixel] / 255.f    ) * max);
      ++count[data[pixel]];
      data[pixel + 1] = static_cast<unsigned char>((data[pixel + 1] / 255.f) * max);
      ++count[data[pixel + 1]];
      data[pixel + 2] = static_cast<unsigned char>((data[pixel + 2] / 255.f) * max);
      ++count[data[pixel + 2]];
    }
  }

  float accumulate = 0;

  for (unsigned int i = 0; i < (max + 1); ++i)
  {
    histogram[i] = static_cast<float>(count[i]) / size;
    accumulate += histogram[i];
    map[i] = static_cast<unsigned char>(max * accumulate);
  }

  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      unsigned pixel = ((i * w) + j) * 3;
      unsigned char red   = static_cast<unsigned char>((static_cast<float>(map[data[pixel]]) / max) * 255);
      unsigned char green = static_cast<unsigned char>((static_cast<float>(map[data[pixel + 1]]) / max) * 255);
      unsigned char blue  = static_cast<unsigned char>((static_cast<float>(map[data[pixel+ 2]]) / max) * 255);
      data[pixel]     = red;
      data[pixel + 1] = green;
      data[pixel + 2] = blue;
    }
  }

}

unsigned char * LogTransform(int w, int h, unsigned char * data, double c, double base)
{
  unsigned char* output = static_cast<unsigned char*>(malloc(w * h * 3));
  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      unsigned pixel = ((i * w) + j) * 3;
      int red = data[pixel];
      int green = data[pixel + 1];
      int blue = data[pixel + 2];

      red =   static_cast<int>(c * (log10(red + 1)   / log10(base)));
      green = static_cast<int>(c * (log10(green + 1) / log10(base)));
      blue =  static_cast<int>(c * (log10(blue + 1)  / log10(base)));

      red = red > 255 ? 255 : red;
      green = green > 255 ? 255 : green;
      blue = blue > 255 ? 255 : blue;

      output[pixel] = static_cast<unsigned char>(red);
      output[pixel + 1] = static_cast<unsigned char>(green);
      output[pixel + 2] = static_cast<unsigned char>(blue);

    }
  }
  return output;
}

unsigned char * GammaTransform(int w, int h, unsigned char * data, double c, double sigma)
{
  unsigned char* output = static_cast<unsigned char*>(malloc(w * h * 3));
  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      unsigned pixel = ((i * w) + j) * 3;
      int red = data[pixel];
      int green = data[pixel + 1];
      int blue = data[pixel + 2];

      red =   static_cast<int>(c * pow(red, sigma));
      green = static_cast<int>(c * pow(green, sigma));
      blue =  static_cast<int>(c * pow(blue, sigma));

      red = red > 255 ? 255 : red;
      green = green > 255 ? 255 : green;
      blue = blue > 255 ? 255 : blue;

      output[pixel] = static_cast<unsigned char>(red);
      output[pixel + 1] = static_cast<unsigned char>(green);
      output[pixel + 2] = static_cast<unsigned char>(blue);

    }
  }
  return output;
}

unsigned char * GaussianBlur(int w, int h, unsigned char * data, int N, double sigma)
{
  matrix kernel;
  kernel.resize(N);
  for (int i = 0; i < N; ++i) kernel[i].resize(N);
  CreateGaussianKernel(N, kernel, sigma);
  unsigned char* output = static_cast<unsigned char*>(malloc(w * h * 3));
  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      unsigned pixel = ((i * w) + j) * 3;
      Color val = MaskingAssistant(j, i, kernel, data, N, w, h);
      val.Clamp();
      output[pixel] =     static_cast<unsigned char>(val.r);
      output[pixel + 1] = static_cast<unsigned char>(val.g);
      output[pixel + 2] = static_cast<unsigned char>(val.b);
    }
  }
  return output;
}

void CreateGaussianKernel(int N, matrix& m, double sigma)
{
  int startx = -(N / 2);
  int starty = -(N / 2);
  double deno = 2 * (pow(sigma, 2));
  double total = 0.0;
  for (int i = 0; i < N; ++i)
  {
    startx = -(N / 2);
    for (int j = 0; j < N; ++j)
    {
      double val = -(((pow(startx, 2)) + (pow(starty, 2))) / deno);
      m[i][j] = exp(val);
      total += m[i][j];
      ++startx;
    }
    ++starty;
  }
  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N; ++j)
      m[i][j] /= total;
}

Color MaskingAssistant(int x, int y, matrix m, unsigned char * data, int N, int w, int h)
{
  int startx = -(N / 2);
  int starty = -(N / 2);
  Color finalVal{};

  for (int i = 0; i < N; ++i)
  {
    startx = -(N / 2);
    for (int j = 0; j < N; ++j)
    {
      int offSetX = x + startx;
      int offSetY = y + starty;
      // value repeat
      offSetX = offSetX < 0 ? 0 : offSetX;
      offSetY = offSetY < 0 ? 0 : offSetY;
      offSetX = offSetX >= w ? w - 1 : offSetX;
      offSetY = offSetY >= h ? h - 1 : offSetY;
      
      unsigned pixel = ((offSetY * w) + offSetX)* 3;

      finalVal.r += (data[pixel] * m[i][j]);
      finalVal.g += (data[pixel + 1] * m[i][j]);
      finalVal.b += (data[pixel + 2] * m[i][j]);

      ++startx;
    }
    ++starty;
  }
  return finalVal;
}

unsigned char* EdgeDetetectionSobel(int w, int h, unsigned char * data)
{
  unsigned char* output = static_cast<unsigned char*>(malloc(w * h * 3));

  matrix vertical;
  matrix horizontal;

  matrix added;

  vertical.resize(3);
  horizontal.resize(3);
  added.resize(3);

  for (int i = 0; i < 3; ++i)
  {
    vertical[i].resize(3);
    horizontal[i].resize(3);
    added[i].resize(3);
  }

  vertical[0] = { -1.0,-2.0,-1.0 };
  vertical[1] = {  0.0, 0.0, 0.0 };
  vertical[2] = {  1.0, 2.0, 1.0 };

  horizontal[0] = { -1.0, 0.0, 1.0 };
  horizontal[1] = { -2.0, 0.0, 2.0 };
  horizontal[2] = { -1.0, 0.0, 1.0 };

  added[0] = { -2.0,-2.0, 0.0 };
  added[1] = { -2.0, 0.0, 2.0 };
  added[2] = {  0.0, 2.0, 2.0 };

  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      unsigned pixel = ((i * w) + j) * 3;

      Color vert_val = MaskingAssistant(j, i, vertical, data, 3, w, h);
      
      Color hori_val = MaskingAssistant(j, i, horizontal, data, 3, w, h);
      
      vert_val.fabs();
      hori_val.fabs();

      Color val = vert_val + hori_val;

      // Color val = MaskingAssistant(j, i, added, data, 3, w, h);

      val.Clamp();

      output[pixel] = static_cast<unsigned char>(val.r);
      output[pixel + 1] = static_cast<unsigned char>(val.g);
      output[pixel + 2] = static_cast<unsigned char>(val.b);
    }
  }
  return output;
}

unsigned char * UnsharpMaskOperation(int w, int h, unsigned char * data, int N, double sigma, double K)
{
  matrix kernel;
  kernel.resize(N);
  for (int i = 0; i < N; ++i) kernel[i].resize(N);
  CreateGaussianKernel(N, kernel, sigma);
  unsigned char* output = static_cast<unsigned char*>(malloc(w * h * 3));
  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      unsigned pixel = ((i * w) + j) * 3;

      Color val = MaskingAssistant(j, i, kernel, data, N, w, h);
      val.Clamp();

      Color originalVal{};
      originalVal.r = static_cast<double>(data[pixel]);
      originalVal.g = static_cast<double>(data[pixel + 1]);
      originalVal.b = static_cast<double>(data[pixel + 2]);

      Color mask{};
      mask.r = originalVal.r - val.r;
      mask.g = originalVal.g - val.g;
      mask.b = originalVal.b - val.b;
      mask.Clamp();

      originalVal.r = originalVal.r + (K * mask.r);
      originalVal.g = originalVal.g + (K * mask.g);
      originalVal.b = originalVal.b + (K * mask.b);
      originalVal.Clamp();

      output[pixel] =     static_cast<unsigned char>(originalVal.r);
      output[pixel + 1] = static_cast<unsigned char>(originalVal.g);
      output[pixel + 2] = static_cast<unsigned char>(originalVal.b);
    }
  }
  return output;
}

unsigned char * ConnectedComponentLabeling(int w, int h, unsigned char * data, int min, int max)
{
  // first is the conflicted value, second is the value to replace first
  std::map<unsigned, unsigned> conflicts;
  // background is label 0, background is black (0,0,0)
  unsigned currLabel = 1;
  // set all labels to be black
  unsigned* labels = static_cast<unsigned*>(malloc(w * h * sizeof(unsigned)));
  memset(labels, 0, w * h * sizeof(unsigned));

  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      unsigned pixel = ((i * w) + j) * 3;
      unsigned char r, g, b;
      r = data[pixel];
      g = data[pixel + 1];
      b = data[pixel + 2];
      if (!IsInsideSet(min, max, r, g, b)) continue;
      std::set<unsigned> neigh = GetNeighbours(labels, data, j, i, w, h, min, max);
      if (neigh.empty()) // no label
      {
        unsigned pos = ((i * w) + j);
        labels[pos] = currLabel;
        ++currLabel;
      }
      else // got label
      {
        unsigned pos = ((i * w) + j);
        unsigned l = *neigh.begin();
        labels[pos] = l;
        auto start = neigh.begin();
        for (++start; start != neigh.end(); ++start)
        {
          // resolve conflict
          // if (conflicts.find(*start) == conflicts.end())
          // {
            conflicts[*start] = l;
          // }
          // else
          // {
          //   unsigned val = conflicts[*start];
          //   unsigned prevVal = val;
          //   while (conflicts.find(val) != conflicts.end())
          //   {
          //     prevVal = val;
          //     val = conflicts[val];
          //   }
          //   if (val > l) conflicts[prevVal] = l;
          // }
        }
      }
    }
  }

  // Two Pass Second Pass
  // Conflict Resolving

  std::vector<labelColor> LColors;
  LColors.resize(currLabel);
  LColors[0] = labelColor{};
  GenerateLabelColors(LColors);

  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      unsigned pos = ((i * w) + j);
      unsigned labelValue = labels[pos];
      if (conflicts.find(labelValue) != conflicts.end())
      {
        unsigned val = conflicts[labelValue];
        while (conflicts.find(val) != conflicts.end())
        {
          val = conflicts[val];
        }
        labels[pos] = val;
      }
      unsigned label = ResolveConflict(labels, data, j, i, w, h, min, max);
      labels[pos] = label;
    }
  }

  conflicts.clear();

  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      unsigned pos = ((i * w) + j);
      unsigned labelValue = labels[pos];
      unsigned label = ResolveConflict(labels, data, j, i, w, h, min, max);
      if (label < labelValue) conflicts[labelValue] = label;
    }
  }

  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      unsigned pos = ((i * w) + j);
      unsigned labelValue = labels[pos];
      if (conflicts.find(labelValue) != conflicts.end())
      {
        unsigned val = conflicts[labelValue];
        while (conflicts.find(val) != conflicts.end())
        {
          val = conflicts[val];
        }
        labels[pos] = val;
      }
    }
  }

  unsigned char* output = static_cast<unsigned char*>(malloc(w * h * 3));

  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      unsigned pixel = ((i * w) + j) * 3;
      unsigned pos = ((i * w) + j);
      unsigned label = labels[pos];
      labelColor c = LColors[label];
      output[pixel] = c.r;
      output[pixel + 1] = c.g;
      output[pixel + 2] = c.b;
    }
  }

  free(labels);

  return output;;
}

std::set<unsigned> GetNeighbours(unsigned * labels, unsigned char * data, int x, int y, int w, int h, int min, int max)
{
  std::set<unsigned> neighbours;
  unsigned pixel = ((y * w) + x) * 3;
  unsigned char r, g, b;
  r = data[pixel];
  g = data[pixel + 1];
  b = data[pixel + 2];

  // check m neighbours
  int topx = x;     int topy = y - 1;
  int btmx = x;     int btmy = y + 1;
  int lftx = x - 1; int lfty = y;
  int rgtx = x + 1; int rgty = y;

  // top
  if (topy >= 0)
  {
    unsigned tmp = ((topy * w) + topx) * 3;
    unsigned char tr, tg, tb;
    tr = data[tmp];
    tg = data[tmp + 1];
    tb = data[tmp + 2];
    if (IsInsideSet(min, max, tr, tg, tb))
    {
      unsigned pos = ((topy * w) + topx);
      unsigned l = labels[pos];
      if (l) neighbours.insert(l);
    }
  }

  // btm
  if (btmy < h)
  {
    unsigned tmp = ((btmy * w) + btmx) * 3;
    unsigned char tr, tg, tb;
    tr = data[tmp];
    tg = data[tmp + 1];
    tb = data[tmp + 2];
    if (IsInsideSet(min, max, tr, tg, tb))
    {
      unsigned pos = ((btmy * w) + btmx);
      unsigned l = labels[pos];
      if (l) neighbours.insert(l);
    }
  }

  // left
  if (lftx >= 0)
  {
    unsigned tmp = ((lfty * w) + lftx) * 3;
    unsigned char tr, tg, tb;
    tr = data[tmp];
    tg = data[tmp + 1];
    tb = data[tmp + 2];
    if (IsInsideSet(min, max, tr, tg, tb))
    {
      unsigned pos = ((lfty * w) + lftx);
      unsigned l = labels[pos];
      if (l) neighbours.insert(l);
    }
  }

  // right
  if (rgtx < w)
  {
    unsigned tmp = ((rgty * w) + rgtx) * 3;
    unsigned char tr, tg, tb;
    tr = data[tmp];
    tg = data[tmp + 1];
    tb = data[tmp + 2];
    if (IsInsideSet(min, max, tr, tg, tb))
    {
      unsigned pos = ((rgty * w) + rgtx);
      unsigned l = labels[pos];
      if (l) neighbours.insert(l);
    }
  }

  // start of m neighbour neighbour
  unsigned tlresult = CheckMNeighbour(labels, data, x - 1, y + 1, w, h, TOPLEFT , min, max);
  unsigned trresult = CheckMNeighbour(labels, data, x + 1, y + 1, w, h, TOPRIGHT, min, max);
  unsigned brresult = CheckMNeighbour(labels, data, x + 1, y - 1, w, h, BTMRIGHT, min, max);
  unsigned blresult = CheckMNeighbour(labels, data, x - 1, y - 1, w, h, BTMLEFT , min, max);

  if (tlresult) neighbours.insert(tlresult);
  if (trresult) neighbours.insert(trresult);
  if (brresult) neighbours.insert(brresult);
  if (blresult) neighbours.insert(blresult);

  return neighbours;

}

void GenerateLabelColors(std::vector<labelColor>& lc)
{
  for (size_t i = 1; i < lc.size(); ++i)
  {
    labelColor tmp;
    tmp.r = std::rand() % 255;
    tmp.g = std::rand() % 255;
    tmp.b = std::rand() % 255;
    lc[i] = tmp;
  }
}

unsigned CheckMNeighbour(unsigned * labels, unsigned char * data, int x, int y, int w, int h, M_NEIGHBOUR n, int min, int max)
{
  if (x >= w || x < 0 || y < 0 || y >= h) return 0;
  switch (n)
  {
  case TOPLEFT :
    {
      int topx = x + 1; int topy = y;
      int lftx = x;     int lfty = y - 1;
      if (topx < w)
      {
        int pos = ((topy * w) + topx);
        if (labels[pos]) return 0;
      }
      if (lfty >= 0)
      {
        int pos = ((lfty * w) + lftx);
        if (labels[pos]) return 0;
      }
      unsigned pos = ((y * w) + x);
      unsigned tmp = pos * 3;
      unsigned char tr, tg, tb;
      tr = data[tmp];
      tg = data[tmp + 1];
      tb = data[tmp + 2];
      if (IsInsideSet(min, max, tr, tg, tb))
         return labels[pos];
      break;
    }
  case TOPRIGHT:
    {
      int topx = x - 1; int topy = y;
      int rgtx = x;     int rgty = y - 1;
      if (topx >= 0)
      {
        int pos = ((topy * w) + topx);
        if (labels[pos]) return 0;
      }
      if (rgty >= 0)
      {
        int pos = ((rgty * w) + rgtx);
        if (labels[pos]) return 0;
      }
      unsigned pos = ((y * w) + x);
      unsigned tmp = pos * 3;
      unsigned char tr, tg, tb;
      tr = data[tmp];
      tg = data[tmp + 1];
      tb = data[tmp + 2];
      if (IsInsideSet(min, max, tr, tg, tb))
        return labels[pos];
      break;
    }
  case BTMLEFT:
    {
      int btmx = x + 1; int btmy = y;
      int lftx = x;     int lfty = y + 1;
      if (btmx < w)
      {
        int pos = ((btmy * w) + btmx);
        if (labels[pos]) return 0;
      }
      if (lfty < h)
      {
        int pos = ((lfty * w) + lftx);
        if (labels[pos]) return 0;
      }
      unsigned pos = ((y * w) + x);
      unsigned tmp = pos * 3;
      unsigned char tr, tg, tb;
      tr = data[tmp];
      tg = data[tmp + 1];
      tb = data[tmp + 2];
      if (IsInsideSet(min, max, tr, tg, tb))
        return labels[pos];
      break;
    }
  case BTMRIGHT:
    {
      int btmx = x - 1; int btmy = y;
      int rgtx = x;     int rgty = y + 1;
      if (btmx >= 0)
      {
        int pos = ((btmy * w) + btmx);
        if (labels[pos]) return 0;
      }
      if (rgty < h)
      {
        int pos = ((rgty * w) + rgtx);
        if (labels[pos]) return 0;
      }
      unsigned pos = ((y * w) + x);
      unsigned tmp = pos * 3;
      unsigned char tr, tg, tb;
      tr = data[tmp];
      tg = data[tmp + 1];
      tb = data[tmp + 2];
      if (IsInsideSet(min, max, tr, tg, tb))
        return labels[pos];
      break;
    }
  }
  return 0;
}

unsigned ResolveConflict(unsigned* labels, unsigned char* data, int x, int y, int w, int h, int min, int max)
{
  bool top, btm, lft, rgt;
  top = btm = lft = rgt = true;
  unsigned Opos = ((y * w) + x);
  unsigned label = labels[Opos];
  int topx = x;     int topy = y - 1;
  int btmx = x;     int btmy = y + 1;
  int lftx = x - 1; int lfty = y;
  int rgtx = x + 1; int rgty = y;

  // top
  if (topy >= 0)
  {
    unsigned pos = ((topy * w) + topx);
    unsigned l = labels[pos];
    unsigned dPos = pos * 3;
    int tr = data[dPos];
    int tg = data[dPos + 1];
    int tb = data[dPos + 2];
    if (l && IsInsideSet(min, max, tr,tg,tb))
    {
      label = label < l ? label : l;
      top = false;
    }
  }

  // btm
  if (btmy < h)
  {
    unsigned pos = ((btmy * w) + btmx);
    unsigned l = labels[pos];
    unsigned dPos = pos * 3;
    int tr = data[dPos];
    int tg = data[dPos + 1];
    int tb = data[dPos + 2];
    if (l && IsInsideSet(min, max, tr, tg, tb))
    {
      label = label < l ? label : l;
      btm = false;
    }
  }


  // left
  if (lftx >= 0)
  {
    unsigned pos = ((lfty * w) + lftx);
    unsigned l = labels[pos];
    unsigned dPos = pos * 3;
    int tr = data[dPos];
    int tg = data[dPos + 1];
    int tb = data[dPos + 2];
    if (l && IsInsideSet(min, max, tr, tg, tb))
    {
      label = label < l ? label : l;
      lft = false;
    }
  }

  // right
  if (rgtx < w)
  {
    unsigned pos = ((rgty * w) + rgtx);
    unsigned l = labels[pos];
    unsigned dPos = pos * 3;
    int tr = data[dPos];
    int tg = data[dPos + 1];
    int tb = data[dPos + 2];
    if (l && IsInsideSet(min, max, tr, tg, tb))
    {
      label = label < l ? label : l;
      rgt = false;
    }
  }

  // M-Adjancy Conflict Resolve
  if (top && lft && topy >= 0 && lftx >= 0)
  {
    unsigned pos = ((topy * w) + lftx);
    unsigned l = labels[pos];
    unsigned dPos = pos * 3;
    int tr = data[dPos];
    int tg = data[dPos + 1];
    int tb = data[dPos + 2];
    if (l && IsInsideSet(min, max, tr, tg, tb))
      label = label < l ? label : l;
  }
  if (top && rgt && topy >= 0 && rgtx < w)
  {
    unsigned pos = ((topy * w) + rgtx);
    unsigned l = labels[pos];
    unsigned dPos = pos * 3;
    int tr = data[dPos];
    int tg = data[dPos + 1];
    int tb = data[dPos + 2];
    if (l && IsInsideSet(min, max, tr, tg, tb))
      label = label < l ? label : l;
  }
  if (btm && lft && btmy < h && lftx >= 0)
  {
    unsigned pos = ((btmy * w) + lftx);
    unsigned l = labels[pos];
    unsigned dPos = pos * 3;
    int tr = data[dPos];
    int tg = data[dPos + 1];
    int tb = data[dPos + 2];
    if (l && IsInsideSet(min, max, tr, tg, tb))
      label = label < l ? label : l;
  }
  if (btm && rgt && btmy < h && rgtx < w)
  {
    unsigned pos = ((btmy * w) + rgtx);
    unsigned l = labels[pos];
    unsigned dPos = pos * 3;
    int tr = data[dPos];
    int tg = data[dPos + 1];
    int tb = data[dPos + 2];
    if (l && IsInsideSet(min, max, tr, tg, tb))
      label = label < l ? label : l;
  }

  return label;
}

bool CommandLineInput(std::string s, std::string dir, std::string& myoutput)
{
  myoutput.clear();
  if (s[0] == 'a' && s[1] == 'd' && s[2] == 'd') // add
  {
    if (s[3] == ' ' && s[4] == '-' && s[5] == 'i' && s[6] == ' ')
    {
      std::string rest = s.substr(7);
      std::string input1 = rest.substr(0, rest.find(' '));
      rest.erase(0, rest.find(' ') + 1);
      std::string input2 = rest.substr(0, rest.find(' '));
      rest.erase(0, rest.find(' ') + 1);

      if (rest[0] != '-' || rest[1] != 'o' || rest[2] != ' ')
      {
        std::string errorMessage = "Invalid add Command, e.g. add -i input1.ppm input2.ppm -o output.ppm";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      rest.erase(0, 3);
      std::string output = rest;
      
      // Extension Checking
      if (!CheckExtension(input1)) return true;
      if (!CheckExtension(input2)) return true;
      if (!CheckExtension(output)) return true;

      if (dir.empty() || dir[dir.size() - 1] == '\\' || dir[dir.size() - 1] == '/')
      {
        input1 = dir + input1;
        input2 = dir + input2;
        output = dir + output;
      }
      else
      {
        input1 = dir + '\\' + input1;
        input2 = dir + '\\' + input2;
        output = dir + '\\' + output;
      }

      auto data1 = LoadHelper(input1);
      if (!data1.first)
      {
        std::string errorMessage = "Input1 file does not exists or wrong directory";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      auto data2 = LoadHelper(input2);
      if (!data2.first)
      {
        free(data1.first);
        std::string errorMessage = "Input2 file does not exists or wrong directory";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      unsigned size1 = data1.second.height * data1.second.width;
      unsigned size2 = data2.second.height * data2.second.width;
      if (size1 > size2)
      {
        data2.first = Resize(data1.second.width, data1.second.height, data2.first, data2.second);
        data2.second.width = data1.second.width;
        data2.second.height = data1.second.height;
      }
      else if (size2 > size1)
      {
        data1.first = Resize(data2.second.width, data2.second.height, data1.first, data1.second);
        data1.second.width = data2.second.width;
        data1.second.height = data2.second.height;
      }

      unsigned char* out = Addition(data1.second.width, data1.second.height, data1.first, data2.first, nullptr);

      SaveHelper(data1.second, out, output);

      free(out);
      free(data1.first);
      free(data2.first);

      myoutput = output;

      return true;
    }
    else
    {
      std::string errorMessage = "Invalid add Command, e.g. add -i input1.ppm input2.ppm -o output.ppm";
      wxLogMessage(errorMessage.c_str());
      return true;
    }
  }
  else if (s[0] == 's' && s[1] == 'u' && s[2] == 'b') // sub
  {
    if (s[3] == ' ' && s[4] == '-' && s[5] == 'i' && s[6] == ' ')
    {
      std::string rest = s.substr(7);
      std::string input1 = rest.substr(0, rest.find(' '));
      rest.erase(0, rest.find(' ') + 1);
      std::string input2 = rest.substr(0, rest.find(' '));
      rest.erase(0, rest.find(' ') + 1);

      if (rest[0] != '-' || rest[1] != 'o' || rest[2] != ' ')
      {
        std::string errorMessage = "Invalid sub Command, e.g. sub -i input1.ppm input2.ppm -o output.ppm";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      rest.erase(0, 3);
      std::string output = rest;

      // Extension Checking
      if (!CheckExtension(input1)) return true;
      if (!CheckExtension(input2)) return true;
      if (!CheckExtension(output)) return true;

      if (dir.empty() || dir[dir.size() - 1] == '\\' || dir[dir.size() - 1] == '/')
      {
        input1 = dir + input1;
        input2 = dir + input2;
        output = dir + output;
      }
      else
      {
        input1 = dir + '\\' + input1;
        input2 = dir + '\\' + input2;
        output = dir + '\\' + output;
      }

      auto data1 = LoadHelper(input1);
      if (!data1.first)
      {
        std::string errorMessage = "Input1 file does not exists or wrong directory";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      auto data2 = LoadHelper(input2);
      if (!data2.first)
      {
        free(data1.first);
        std::string errorMessage = "Input2 file does not exists or wrong directory";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      unsigned size1 = data1.second.height * data1.second.width;
      unsigned size2 = data2.second.height * data2.second.width;
      if (size1 > size2)
      {
        data2.first = Resize(data1.second.width, data1.second.height, data2.first, data2.second);
        data2.second.width = data1.second.width;
        data2.second.height = data1.second.height;
      }
      else if (size2 > size1)
      {
        data1.first = Resize(data2.second.width, data2.second.height, data1.first, data1.second);
        data1.second.width = data2.second.width;
        data1.second.height = data2.second.height;
      }

      unsigned char* out = Subtraction(data1.second.width, data1.second.height, data1.first, data2.first, nullptr);

      SaveHelper(data1.second, out, output);

      free(out);
      free(data1.first);
      free(data2.first);

      myoutput = output;

      return true;
    }
    else
    {
      std::string errorMessage = "Invalid sub Command, e.g. sub -i input1.ppm input2.ppm -o output.ppm";
      wxLogMessage(errorMessage.c_str());
      return true;
    }
  }
  else if (s[0] == 'm' && s[1] == 'u' && s[2] == 'l') // mul
  {
    if (s[3] == ' ' && s[4] == '-' && s[5] == 'i' && s[6] == ' ')
    {
      std::string rest = s.substr(7);
      std::string input1 = rest.substr(0, rest.find(' '));
      rest.erase(0, rest.find(' ') + 1);
      std::string input2 = rest.substr(0, rest.find(' '));
      rest.erase(0, rest.find(' ') + 1);

      if (rest[0] != '-' || rest[1] != 'o' || rest[2] != ' ')
      {
        std::string errorMessage = "Invalid mul Command, e.g. mul -i input1.ppm input2.ppm -o output.ppm";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      rest.erase(0, 3);
      std::string output = rest;

      // Extension Checking
      if (!CheckExtension(input1)) return true;
      if (!CheckExtension(input2)) return true;
      if (!CheckExtension(output)) return true;

      if (dir.empty() || dir[dir.size() - 1] == '\\' || dir[dir.size() - 1] == '/')
      {
        input1 = dir + input1;
        input2 = dir + input2;
        output = dir + output;
      }
      else
      {
        input1 = dir + '\\' + input1;
        input2 = dir + '\\' + input2;
        output = dir + '\\' + output;
      }

      auto data1 = LoadHelper(input1);
      if (!data1.first)
      {
        std::string errorMessage = "Input1 file does not exists or wrong directory";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      auto data2 = LoadHelper(input2);
      if (!data2.first)
      {
        free(data1.first);
        std::string errorMessage = "Input2 file does not exists or wrong directory";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      unsigned size1 = data1.second.height * data1.second.width;
      unsigned size2 = data2.second.height * data2.second.width;
      if (size1 > size2)
      {
        data2.first = Resize(data1.second.width, data1.second.height, data2.first, data2.second);
        data2.second.width = data1.second.width;
        data2.second.height = data1.second.height;
      }
      else if (size2 > size1)
      {
        data1.first = Resize(data2.second.width, data2.second.height, data1.first, data1.second);
        data1.second.width = data2.second.width;
        data1.second.height = data2.second.height;
      }

      unsigned char* out = Product(data1.second.width, data1.second.height, data1.first, data2.first, nullptr);

      SaveHelper(data1.second, out, output);

      free(out);
      free(data1.first);
      free(data2.first);

      myoutput = output;

      return true;
    }
    else
    {
      std::string errorMessage = "Invalid mul Command, e.g. mul -i input1.ppm input2.ppm -o output.ppm";
      wxLogMessage(errorMessage.c_str());
      return true;
    }
  }
  else if (s[0] == 'i' && s[1] == 'n' && s[2] == 'v') // inv
  {
    if (s[3] == ' ' && s[4] == '-' && s[5] == 'i' && s[6] == ' ')
    {
      std::string rest = s.substr(7);
      std::string input = rest.substr(0, rest.find(' '));
      rest.erase(0, rest.find(' ') + 1);

      if (rest[0] != '-' || rest[1] != 'o' || rest[2] != ' ')
      {
        std::string errorMessage = "Invalid inv Command, e.g. inv -i input.ppm -o output.ppm";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      rest.erase(0, 3);
      std::string output = rest;

      if (!CheckExtension(input)) return true;
      if (!CheckExtension(output)) return true;

      if (dir.empty() || dir[dir.size() - 1] == '\\' || dir[dir.size() - 1] == '/')
      {
        input = dir + input;
        output = dir + output;
      }
      else
      {
        input = dir + '\\' + input;
        output = dir + '\\' + output;
      }

      auto data = LoadHelper(input);
      if (!data.first)
      {
        std::string errorMessage = "Input file does not exists or wrong directory";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      Inverse(data.second.width, data.second.height, data.first);

      SaveHelper(data.second, data.first, output);

      free(data.first);

      myoutput = output;

      return true;

    }
    else
    {
      std::string errorMessage = "Invalid inv Command, e.g. inv -i input.ppm -o output.ppm";
      wxLogMessage(errorMessage.c_str());
      return true;
    }
  }
  else if (s[0] == 'l' && s[1] == 'o' && s[2] == 'g') // log
  {
    if (s[3] == ' ' && s[4] == '-' && s[5] == 'c' && s[6] == ' ')
    {
      std::string rest = s.substr(7);
      std::string constant = rest.substr(0, rest.find(' '));
      rest.erase(0, rest.find(' ') + 1);
      double c = std::stod(constant);

      if (rest[0] != '-' || rest[1] != 'b' || rest[2] != ' ')
      {
        std::string errorMessage = "Invalid log Command, e.g. log -c <constant> -b<base> -i input.ppm -o output.ppm";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      rest.erase(0, 3);
      std::string base = rest.substr(0, rest.find(' '));
      rest.erase(0, rest.find(' ') + 1);
      double b = std::stod(base);

      if (rest[0] != '-' || rest[1] != 'i' || rest[2] != ' ')
      {
        std::string errorMessage = "Invalid log Command, e.g. log -c <constant> -b<base> -i input.ppm -o output.ppm";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      rest.erase(0, 3);
      std::string input = rest.substr(0, rest.find(' '));
      rest.erase(0, rest.find(' ') + 1);

      if (rest[0] != '-' || rest[1] != 'o' || rest[2] != ' ')
      {
        std::string errorMessage = "Invalid log Command, e.g. log -c <constant> -b<base> -i input.ppm -o output.ppm";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      rest.erase(0, 3);
      std::string output = rest;

      if (!CheckExtension(input)) return true;
      if (!CheckExtension(output)) return true;

      if (dir.empty() || dir[dir.size() - 1] == '\\' || dir[dir.size() - 1] == '/')
      {
        input = dir + input;
        output = dir + output;
      }
      else
      {
        input = dir + '\\' + input;
        output = dir + '\\' + output;
      }

      auto data = LoadHelper(input);
      if (!data.first)
      {
        std::string errorMessage = "Input file does not exists or wrong directory";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      unsigned char* out = LogTransform(data.second.width, data.second.height, data.first, c, b);

      SaveHelper(data.second, out, output);

      free(out);
      free(data.first);

      myoutput = output;

      return true;

    }
    else
    {
      std::string errorMessage = "Invalid log Command, e.g. log -c <constant> -b<base> -i input.ppm -o output.ppm";
      wxLogMessage(errorMessage.c_str());
      return true;
    }
  }
  else if (s[0] == 'p' && s[1] == 'o' && s[2] == 'w') // pow
  {
    if (s[3] == ' ' && s[4] == '-' && s[5] == 'c' && s[6] == ' ')
    {
      std::string rest = s.substr(7);
      std::string constant = rest.substr(0, rest.find(' '));
      rest.erase(0, rest.find(' ') + 1);
      double c = std::stod(constant);

      if (rest[0] != '-' || rest[1] != 'g' || rest[2] != 'a' || rest[3] != 'm' || rest[4] != 'm' || rest[5] != 'a' || rest[6] != ' ')
      {
        std::string errorMessage = "Invalid pow Command, e.g. pow -c <constant> -gamma<exponent value> -i input.ppm -o output.ppm";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      rest.erase(0, 7);
      std::string gamma = rest.substr(0, rest.find(' '));
      rest.erase(0, rest.find(' ') + 1);
      double g = std::stod(gamma);

      if (rest[0] != '-' || rest[1] != 'i' || rest[2] != ' ')
      {
        std::string errorMessage = "Invalid pow Command, e.g. pow -c <constant> -gamma<exponent value> -i input.ppm -o output.ppm";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      rest.erase(0, 3);
      std::string input = rest.substr(0, rest.find(' '));
      rest.erase(0, rest.find(' ') + 1);

      if (rest[0] != '-' || rest[1] != 'o' || rest[2] != ' ')
      {
        std::string errorMessage = "Invalid pow Command, e.g. pow -c <constant> -gamma<exponent value> -i input.ppm -o output.ppm";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      rest.erase(0, 3);
      std::string output = rest;

      if (!CheckExtension(input)) return true;
      if (!CheckExtension(output)) return true;

      if (dir.empty() || dir[dir.size() - 1] == '\\' || dir[dir.size() - 1] == '/')
      {
        input = dir + input;
        output = dir + output;
      }
      else
      {
        input = dir + '\\' + input;
        output = dir + '\\' + output;
      }

      auto data = LoadHelper(input);
      if (!data.first)
      {
        std::string errorMessage = "Input file does not exists or wrong directory";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      unsigned char* out = GammaTransform(data.second.width, data.second.height, data.first, c, g);

      SaveHelper(data.second, out, output);

      free(out);
      free(data.first);

      myoutput = output;

      return true;
    }
    else
    {
      std::string errorMessage = "Invalid pow Command, e.g. pow -c <constant> -gamma<exponent value> -i input.ppm -o output.ppm";
      wxLogMessage(errorMessage.c_str());
      return true;
    }
  }
  else if (s[0] == 'g' && s[1] == 'b' && s[2] == 'l' && s[3] == 'u' && s[4] == 'r') // gblur
  {
    if (s[5] == ' ' && s[6] == '-' && s[7] == 'N' && s[8] == ' ')
    {
      std::string rest = s.substr(9);
      std::string Nsize = rest.substr(0, rest.find(' '));
      rest.erase(0, rest.find(' ') + 1);
      double N = std::stod(Nsize);

      if (rest[0] != '-' || rest[1] != 's' || rest[2] != 'i' || rest[3] != 'g' || rest[4] != 'm' || rest[5] != 'a' || rest[6] != ' ')
      {
        std::string errorMessage = "Invalid gblur Command, e.g. gblur -N <kernel size> -sigma<floating point value> -i input.ppm -o output.ppm";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      rest.erase(0, 7);
      std::string sigma = rest.substr(0, rest.find(' '));
      rest.erase(0, rest.find(' ') + 1);
      double sig = std::stod(sigma);

      if (rest[0] != '-' || rest[1] != 'i' || rest[2] != ' ')
      {
        std::string errorMessage = "Invalid gblur Command, e.g. gblur -N <kernel size> -sigma<floating point value> -i input.ppm -o output.ppm";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      rest.erase(0, 3);
      std::string input = rest.substr(0, rest.find(' '));
      rest.erase(0, rest.find(' ') + 1);

      if (rest[0] != '-' || rest[1] != 'o' || rest[2] != ' ')
      {
        std::string errorMessage = "Invalid gblur Command, e.g. gblur -N <kernel size> -sigma<floating point value> -i input.ppm -o output.ppm";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      rest.erase(0, 3);
      std::string output = rest;

      if (!CheckExtension(input)) return true;
      if (!CheckExtension(output)) return true;

      if (dir.empty() || dir[dir.size() - 1] == '\\' || dir[dir.size() - 1] == '/')
      {
        input = dir + input;
        output = dir + output;
      }
      else
      {
        input = dir + '\\' + input;
        output = dir + '\\' + output;
      }

      auto data = LoadHelper(input);
      if (!data.first)
      {
        std::string errorMessage = "Input file does not exists or wrong directory";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      unsigned char* out = GaussianBlur(data.second.width, data.second.height, data.first, N, sig);

      SaveHelper(data.second, out, output);

      free(out);
      free(data.first);

      myoutput = output;

      return true;
    }
    else
    {
      std::string errorMessage = "Invalid gblur Command, e.g. gblur -N <kernel size> -sigma<floating point value> -i input.ppm -o output.ppm";
      wxLogMessage(errorMessage.c_str());
      return true;
    }
  }
  else if (s[0] == 's' && s[1] == 'o' && s[2] == 'b' && s[3] == 'e' && s[4] == 'l') // sobel
  {
    if (s[5] == ' ' && s[6] == '-' && s[7] == 'i' && s[8] == ' ')
    {
      std::string rest = s.substr(9);
      std::string input = rest.substr(0, rest.find(' '));
      rest.erase(0, rest.find(' ') + 1);

      if (rest[0] != '-' || rest[1] != 'o' || rest[2] != ' ')
      {
        std::string errorMessage = "Invalid sobel Command, e.g. sobel -i input.ppm -o output.ppm";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      rest.erase(0, 3);
      std::string output = rest;

      if (!CheckExtension(input)) return true;
      if (!CheckExtension(output)) return true;

      if (dir.empty() || dir[dir.size() - 1] == '\\' || dir[dir.size() - 1] == '/')
      {
        input = dir + input;
        output = dir + output;
      }
      else
      {
        input = dir + '\\' + input;
        output = dir + '\\' + output;
      }

      auto data = LoadHelper(input);
      if (!data.first)
      {
        std::string errorMessage = "Input file does not exists or wrong directory";
        wxLogMessage(errorMessage.c_str());
        return true;
      }

      unsigned char* out = EdgeDetetectionSobel(data.second.width, data.second.height, data.first);

      SaveHelper(data.second, out, output);

      free(data.first);
      free(out);

      myoutput = output;

      return true;

    }
    else
    {
      std::string errorMessage = "Invalid sobel Command, e.g. sobel -i input.ppm -o output.ppm";
      wxLogMessage(errorMessage.c_str());
      return true;
    }
  }
  return false;
}

bool CheckExtension(std::string f)
{
  size_t lastPos = f.size() - 1;
  if (f[lastPos - 3] != '.' || f[lastPos - 2] != 'p' || f[lastPos - 1] != 'p' || f[lastPos] != 'm')
  {
    std::string errorMessage = "Invalid file extension, e.g no .ppm";
    wxLogMessage(errorMessage.c_str());
    return false;
  }
  return true;
}

bool IsInsideSet(int min, int max, unsigned char r, unsigned char g, unsigned char b)
{
  int red = static_cast<int>(r);
  int green = static_cast<int>(g);
  int blue = static_cast<int>(b);
  if (IntheMiddle(min, max, red)
      && IntheMiddle(min, max, green)
      && IntheMiddle(min, max, blue)) return true;
  return false;
}

bool IntheMiddle(int min, int max, int val)
{
  if (val >= min && val <= max) return true;
  return false;
}

