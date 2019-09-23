#include "A3.h"

using namespace Assignment3;

unsigned char * Assignment3::ImaginaryToChar(int w, int h, ImaginaryNum * data)
{
  unsigned char * output = static_cast<unsigned char *>(malloc(w * h * 3));
  // float max = -std::numeric_limits<float>::max();
  // float min = std::numeric_limits<float>::max();
  // for (int i = 0; i < h; ++i)
  // {
  //   int tmp = (int)(i * w);
  //   for (int j = 0; j < w; ++j)
  //   {
  //     unsigned p = (tmp + j);
  //     if (max < data[p].real) max = data[p].real;
  //     if (min > data[p].real) min = data[p].real;
  //   }
  // }

  for (int i = 0; i < h; ++i)
  {
    int tmp = (int)(i * w);
    for (int j = 0; j < w; ++j)
    {
      unsigned p = (tmp + j);
      unsigned pixel = p * 3;
      float val = data[p].real;
      // float norm = (val - min) / (max - min);
      // float clr = norm * 255.f;
      output[pixel] = static_cast<unsigned char>(val);
      output[pixel + 1] = output[pixel];
      output[pixel + 2] = output[pixel];
    }
  }

  return output;
}

unsigned char * Assignment3::LogImaginaryToChar(int w, int h, ImaginaryNum * data)
{
  unsigned char * output = static_cast<unsigned char *>(malloc(w * h * 3));
  
  float max = -std::numeric_limits<float>::max();
  float min = std::numeric_limits<float>::max();

  for (int i = 0; i < h; ++i)
  {
    int tmp = (int)(i * w);
    for (int j = 0; j < w; ++j)
    {
      unsigned p = (tmp + j);
      float realsq = data[p].real * data[p].real;
      float imasq = data[p].imaginary * data[p].imaginary;
      float result = sqrtf(realsq + imasq) + 1;
      float clr = logf(result);
      if (max < clr) max = clr;
      if (min > clr) min = clr;
      //if (clr > 255.f) clr = 255.f;
    }
  }

  for (int i = 0; i < h; ++i)
  {
    int tmp = (int)(i * w);
    for (int j = 0; j < w; ++j)
    {
      unsigned p = (tmp + j);
      unsigned pixel = p * 3;
      float realsq = data[p].real * data[p].real;
      float imasq = data[p].imaginary * data[p].imaginary;
      float result = sqrtf(realsq + imasq) + 1;
      float val = logf(result);
      float norm = (val - min) / (max - min);
      float clr = norm * 255.f;
      output[pixel] = static_cast<unsigned char>(clr);
      output[pixel + 1] = output[pixel];
      output[pixel + 2] = output[pixel];
    }
  }

  return output;
}

ImaginaryNum * Assignment3::DirectFourierTransform(int w, int h, unsigned char * data)
{
  ImaginaryNum* output = static_cast<ImaginaryNum*>(malloc(w * h * sizeof(ImaginaryNum)));
  for (int i = 0; i < h; ++i)
  {
    int tmp = (int)(i * w);
    for (int j = 0; j < w; ++j)
    {
      unsigned pixel = (tmp + j);
      output[pixel] = DirectFourierHelper(w, h, data, j, i);
    }
  }
  return output;
}

ImaginaryNum Assignment3::DirectFourierHelper(int w, int h, unsigned char * data, int x, int y)
{
  ImaginaryNum output;

  float kM = static_cast<float>(y) / static_cast<float>(h);
  float lN = static_cast<float>(x) / static_cast<float>(w);

  for (int i = 0; i < h; ++i)
  {
    int tmp = (int)(i * w);
    for (int j = 0; j < w; ++j)
    {
      unsigned pixel = (tmp + j) * 3;
      int sum = (i + j) % 2;
      float nega = sum == 0 ? 1.f : -1.f;
      float fmn = data[pixel] * nega;
      float pwr = kM * static_cast<float>(i) +
                  lN * static_cast<float>(j);
      pwr *= (-2.f * static_cast<float>(M_PI));
      output.real += (fmn * cosf(pwr));
      output.imaginary += (fmn * sinf(pwr));
    }
  }

  output.real = output.real;
  output.imaginary = output.imaginary;
  return output;
}

ImaginaryNum * Assignment3::InverseFourierTransform(int w, int h, ImaginaryNum * data)
{
  ImaginaryNum* output = static_cast<ImaginaryNum*>(malloc(w * h * sizeof(ImaginaryNum)));
  for (int i = 0; i < h; ++i)
  {
    int tmp = (int)(i * w);
    for (int j = 0; j < w; ++j)
    {
      unsigned pixel = (tmp + j);
      int sum = (i + j) % 2;
      float nega = sum == 0 ? 1.f : -1.f;
      output[pixel] = InverseFourierHelper(w, h, data, j, i);
      output[pixel].real *= nega;
    }
  }
  return output;
}

ImaginaryNum Assignment3::InverseFourierHelper(int w, int h, ImaginaryNum * data, int x, int y)
{
  int MN = w * h;

  ImaginaryNum output;

  float kM = static_cast<float>(y) / static_cast<float>(h);
  float lN = static_cast<float>(x) / static_cast<float>(w);

  for (int i = 0; i < h; ++i)
  {
    int tmp = (int)(i * w);
    for (int j = 0; j < w; ++j)
    {
      unsigned pixel = (tmp + j);
      ImaginaryNum fmn = data[pixel];
      float pwr = kM * static_cast<float>(i) +
                  lN * static_cast<float>(j);
      pwr *= (2.f * static_cast<float>(M_PI));
      output.real += (fmn.real * cosf(pwr));
      output.real += (-fmn.imaginary * sinf(pwr));
    }
  }

  output.real /= MN;
  return output;
}

ImaginaryNum * Assignment3::SeparableFourierTransform(int w, int h, unsigned char * data)
{
  ImaginaryNum* output = static_cast<ImaginaryNum*>(malloc(w * h * sizeof(ImaginaryNum)));
  ImaginaryNum* rows = static_cast<ImaginaryNum*>(malloc(w * h * sizeof(ImaginaryNum)));

  for (int i = 0; i < h; ++i)
  {
    int tmp = (int)(i * w);
    for (int j = 0; j < w; ++j)
    {
      unsigned p = (tmp + j);
      ImaginaryNum out;
      for (int k = 0; k < w; ++k)
      {
        unsigned pixel = (tmp + k) * 3;
        int sum = (i + k) % 2;
        float nega = sum == 0 ? 1.f : -1.f;
        float fmn = data[pixel] * nega;
        float pwr = (static_cast<float>(j) * static_cast<float>(k)) / static_cast<float>(w);
        pwr *= (-2.f * static_cast<float>(M_PI));
        out.real += fmn * cosf(pwr);
        out.imaginary += fmn * sinf(pwr);
      }
      rows[p] = out;
    }
  }

  for (int i = 0; i < h; ++i)
  {
    int tmp = (int)(i * w);
    for (int j = 0; j < w; ++j)
    {
      unsigned p = (tmp + j);
      ImaginaryNum out;
      for (int k = 0; k < h; ++k)
      {
        int pixel = (int)(k * w);
        pixel += j;
        ImaginaryNum fmn = rows[pixel];
        float pwr = (static_cast<float>(i) * static_cast<float>(k)) / static_cast<float>(h);
        pwr *= (-2.f * static_cast<float>(M_PI));
        float real = cosf(pwr);
        float imaginary = sinf(pwr);
        out.real += ((real * fmn.real) - (imaginary * fmn.imaginary));
        out.imaginary += ((real * fmn.imaginary) + (imaginary * fmn.real));
      }
      output[p] = out;
    }
  }
  free(rows);
  return output;
}

ImaginaryNum * Assignment3::SeperableInverseFourierTransform(int w, int h, ImaginaryNum * data)
{
  int MN = w * h;
  for (int i = 0; i < h; ++i)
  {
    int tmp = (int)(i * w);
    for (int j = 0; j < w; ++j)
    {
      unsigned p = (tmp + j);
      data[p].Conjugate();
    }
  }

  ImaginaryNum* output = static_cast<ImaginaryNum*>(malloc(w * h * sizeof(ImaginaryNum)));
  ImaginaryNum* rows = static_cast<ImaginaryNum*>(malloc(w * h * sizeof(ImaginaryNum)));

  for (int i = 0; i < h; ++i)
  {
    int tmp = (int)(i * w);
    for (int j = 0; j < w; ++j)
    {
      unsigned p = (tmp + j);
      ImaginaryNum out;
      for (int k = 0; k < w; ++k)
      {
        unsigned pixel = (tmp + k);
        // int sum = (i + k) % 2;
        // float nega = sum == 0 ? 1.f : -1.f;
        ImaginaryNum fmn = data[pixel];
        // fmn.real *= nega;
        // fmn.imaginary *= nega;
        float pwr = (static_cast<float>(j) * static_cast<float>(k)) / static_cast<float>(w);
        pwr *= (-2.f * static_cast<float>(M_PI));
        float real = cosf(pwr);
        float imaginary = sinf(pwr);
        out.real += ((real * fmn.real) - (imaginary * fmn.imaginary));
        out.imaginary += ((real * fmn.imaginary) + (imaginary * fmn.real));
      }
      rows[p] = out;
    }
  }

  for (int i = 0; i < h; ++i)
  {
    int tmp = (int)(i * w);
    for (int j = 0; j < w; ++j)
    {
      unsigned p = (tmp + j);
      ImaginaryNum out;
      for (int k = 0; k < h; ++k)
      {
        int pixel = (int)(k * w);
        pixel += j;
        ImaginaryNum fmn = rows[pixel];
        float pwr = (static_cast<float>(i) * static_cast<float>(k)) / static_cast<float>(h);
        pwr *= (-2.f * static_cast<float>(M_PI));
        float real = cosf(pwr);
        float imaginary = sinf(pwr);
        out.real += ((real * fmn.real) - (imaginary * fmn.imaginary));
        out.imaginary += ((real * fmn.imaginary) + (imaginary * fmn.real));
      }
      int sum = (i + j) % 2;
      float nega = sum == 0 ? 1.f : -1.f;
      out.real /= (MN * nega);
      out.imaginary /= (MN * nega);
      out.Conjugate();
      output[p] = out;
    }
  }

  free(rows);
  return output;
}

ImaginaryNum * Assignment3::FastFourierTransform(int w, int h, unsigned char * data)
{
  ImaginaryNum* output = static_cast<ImaginaryNum*>(malloc(w * h * sizeof(ImaginaryNum)));
  ImaginaryNum* rows = static_cast<ImaginaryNum*>(malloc(w * h * sizeof(ImaginaryNum)));
  std::vector<unsigned> indexes;
  indexes.resize(w);

  unsigned twoPwr = static_cast<unsigned>(ceilf(log2f(static_cast<float>(w))));

  // creating preprocess indexes
  for (unsigned i = 0; i < static_cast<unsigned>(w); ++i)
    indexes[i] = ReverseBits(i, twoPwr);

  std::vector<ImaginaryNum> tmp_input;
  tmp_input.resize(w);
  std::vector<ImaginaryNum> tmp_output;
  tmp_output.resize(w);

  int N = static_cast<int>(log2(w));

  for (int i = 0; i < h; ++i)
  {
    int tmp = (int)(i * w);
    for (int l = 0; l < w; ++l)
    {
      unsigned newIndex = indexes[l];
      if (newIndex >= static_cast<unsigned>(w)) newIndex -= static_cast<unsigned>(w);
      int sum = (i + indexes[l]) % 2;
      float nega = sum == 0 ? 1.f : -1.f;
      tmp_input[l].real = data[(tmp + indexes[l]) * 3] * nega;
      tmp_input[l].imaginary = 0.f * nega;
    }

    for (int j = 0; j < N; ++j)
    {
      int M = w / (1 << (j + 1));
      int idx = 0;
      int twoPwrJ = (1 << j);
      for (int k = 0; k < M; ++k)
      {
        for (int l = 0; l < twoPwrJ; ++l)
        {
          float pwr = (-static_cast<float>(l)) / static_cast<float>(twoPwrJ);
          pwr *= (static_cast<float>(M_PI));
          float real = cosf(pwr);
          float imaginary = sinf(pwr);

          ImaginaryNum out;
          out.real = (real * tmp_input[idx + l + twoPwrJ].real) - (imaginary * tmp_input[idx + l + twoPwrJ].imaginary);
          out.imaginary = (real * tmp_input[idx + l + twoPwrJ].imaginary) + (imaginary * tmp_input[idx + l + twoPwrJ].real);

          tmp_output[idx + l].real = tmp_input[idx + l].real + out.real;
          tmp_output[idx + l].imaginary = tmp_input[idx + l].imaginary + out.imaginary;

          tmp_output[idx + l + twoPwrJ].real = tmp_input[idx + l].real - out.real;
          tmp_output[idx + l + twoPwrJ].imaginary = tmp_input[idx + l].imaginary - out.imaginary;
        }
        idx += (1 << (j + 1));
      }
      // swap input and output
      tmp_input = tmp_output;
    }

    for (int z = 0; z < w; ++z)
    {
      int pixel = (i * w) + z;
      rows[pixel].real = tmp_output[z].real;
      rows[pixel].imaginary = tmp_output[z].imaginary;
    }
  }

  tmp_input.clear();
  tmp_output.clear();
  tmp_input.resize(h);
  tmp_output.resize(h);

  N = static_cast<int>(log2(h));

  twoPwr = static_cast<unsigned>(ceilf(log2f(static_cast<float>(h))));
  indexes.clear();
  indexes.resize(h);

  // creating preprocess indexes
  for (unsigned i = 0; i < static_cast<unsigned>(h); ++i)
    indexes[i] = ReverseBits(i, twoPwr);
  
  for (int i = 0; i < w; ++i)
  {

    for (int l = 0; l < h; ++l)
    {
      unsigned newIndex = indexes[l];
      if (newIndex >= static_cast<unsigned>(h)) newIndex -= static_cast<unsigned>(h);
      int tmp = (int)(newIndex * w);
      tmp += i;
      tmp_input[l] = rows[tmp];
    }

    for (int j = 0; j < N; ++j)
    {
      int M = h / (1 << (j + 1));
      int idx = 0;
      int twoPwrJ = (1 << j);
      for (int k = 0; k < M; ++k)
      {
        for (int l = 0; l < twoPwrJ; ++l)
        {
          float pwr = (-static_cast<float>(l)) / static_cast<float>(twoPwrJ);
          pwr *= (static_cast<float>(M_PI));
          float real = cosf(pwr);
          float imaginary = sinf(pwr);

          ImaginaryNum out;
          out.real = (real * tmp_input[idx + l + twoPwrJ].real) - (imaginary * tmp_input[idx + l + twoPwrJ].imaginary);
          out.imaginary = (real * tmp_input[idx + l + twoPwrJ].imaginary) + (imaginary * tmp_input[idx + l + twoPwrJ].real);

          tmp_output[idx + l].real = tmp_input[idx + l].real + out.real;
          tmp_output[idx + l].imaginary = tmp_input[idx + l].imaginary + out.imaginary;

          tmp_output[idx + l + twoPwrJ].real = tmp_input[idx + l].real - out.real;
          tmp_output[idx + l + twoPwrJ].imaginary = tmp_input[idx + l].imaginary - out.imaginary;
        }
        idx += (1 << (j + 1));
      }
      // swap input and output
      tmp_input = tmp_output;
    }

    for (int z = 0; z < h; ++z)
    {
      int pixel = (z * w) + i;
      output[pixel].real = tmp_output[z].real;
      output[pixel].imaginary = tmp_output[z].imaginary;
    }
  }
  
  free(rows);
  return output;

}

ImaginaryNum * Assignment3::InverseFastFourierTransform(int w, int h, ImaginaryNum * data)
{
  int MN = w * h;
  for (int i = 0; i < h; ++i)
  {
    int tmp = (int)(i * w);
    for (int j = 0; j < w; ++j)
    {
      unsigned p = (tmp + j);
      data[p].Conjugate();
    }
  }

  ImaginaryNum* output = static_cast<ImaginaryNum*>(malloc(w * h * sizeof(ImaginaryNum)));
  ImaginaryNum* rows = static_cast<ImaginaryNum*>(malloc(w * h * sizeof(ImaginaryNum)));
  std::vector<unsigned> indexes;
  indexes.resize(w);

  unsigned twoPwr = static_cast<unsigned>(ceilf(log2f(static_cast<float>(w))));

  // creating preprocess indexes
  for (unsigned i = 0; i < static_cast<unsigned>(w); ++i)
    indexes[i] = ReverseBits(i, twoPwr);

  std::vector<ImaginaryNum> tmp_input;
  tmp_input.resize(w);
  std::vector<ImaginaryNum> tmp_output;
  tmp_output.resize(w);

  int N = static_cast<int>(log2(w));

  for (int i = 0; i < h; ++i)
  {
    int tmp = (int)(i * w);
    for (int l = 0; l < w; ++l)
    {
      unsigned newIndex = indexes[l];
      if (newIndex >= static_cast<unsigned>(w)) newIndex -= static_cast<unsigned>(w);
      tmp_input[l].real = data[(tmp + newIndex)].real;
      tmp_input[l].imaginary = data[(tmp + newIndex)].imaginary;
    }

    for (int j = 0; j < N; ++j)
    {
      int M = w / (1 << (j + 1));
      int idx = 0;
      int twoPwrJ = (1 << j);
      for (int k = 0; k < M; ++k)
      {
        for (int l = 0; l < twoPwrJ; ++l)
        {
          float pwr = (-static_cast<float>(l)) / static_cast<float>(twoPwrJ);
          pwr *= (static_cast<float>(M_PI));
          float real = cosf(pwr);
          float imaginary = sinf(pwr);

          ImaginaryNum out;
          out.real = (real * tmp_input[idx + l + twoPwrJ].real) - (imaginary * tmp_input[idx + l + twoPwrJ].imaginary);
          out.imaginary = (real * tmp_input[idx + l + twoPwrJ].imaginary) + (imaginary * tmp_input[idx + l + twoPwrJ].real);

          tmp_output[idx + l].real = tmp_input[idx + l].real + out.real;
          tmp_output[idx + l].imaginary = tmp_input[idx + l].imaginary + out.imaginary;

          tmp_output[idx + l + twoPwrJ].real = tmp_input[idx + l].real - out.real;
          tmp_output[idx + l + twoPwrJ].imaginary = tmp_input[idx + l].imaginary - out.imaginary;
        }
        idx += (1 << (j + 1));
      }
      // swap input and output
      tmp_input = tmp_output;
    }

    for (int z = 0; z < w; ++z)
    {
      int pixel = (i * w) + z;
      rows[pixel].real = tmp_output[z].real;
      rows[pixel].imaginary = tmp_output[z].imaginary;
    }
  }

  tmp_input.clear();
  tmp_output.clear();
  tmp_input.resize(h);
  tmp_output.resize(h);

  N = static_cast<int>(log2(h));

  twoPwr = static_cast<unsigned>(ceilf(log2f(static_cast<float>(h))));
  indexes.clear();
  indexes.resize(h);

  // creating preprocess indexes
  for (unsigned i = 0; i < static_cast<unsigned>(h); ++i)
    indexes[i] = ReverseBits(i, twoPwr);

  for (int i = 0; i < w; ++i)
  {

    for (int l = 0; l < h; ++l)
    {
      unsigned newIndex = indexes[l];
      if (newIndex >= static_cast<unsigned>(h)) newIndex -= static_cast<unsigned>(h);
      int tmp = (int)(newIndex * w);
      tmp += i;
      tmp_input[l] = rows[tmp];
    }

    for (int j = 0; j < N; ++j)
    {
      int M = h / (1 << (j + 1));
      int idx = 0;
      int twoPwrJ = (1 << j);
      for (int k = 0; k < M; ++k)
      {
        for (int l = 0; l < twoPwrJ; ++l)
        {
          float pwr = (-static_cast<float>(l)) / static_cast<float>(twoPwrJ);
          pwr *= (static_cast<float>(M_PI));
          float real = cosf(pwr);
          float imaginary = sinf(pwr);

          ImaginaryNum out;
          out.real = (real * tmp_input[idx + l + twoPwrJ].real) - (imaginary * tmp_input[idx + l + twoPwrJ].imaginary);
          out.imaginary = (real * tmp_input[idx + l + twoPwrJ].imaginary) + (imaginary * tmp_input[idx + l + twoPwrJ].real);

          tmp_output[idx + l].real = tmp_input[idx + l].real + out.real;
          tmp_output[idx + l].imaginary = tmp_input[idx + l].imaginary + out.imaginary;

          tmp_output[idx + l + twoPwrJ].real = tmp_input[idx + l].real - out.real;
          tmp_output[idx + l + twoPwrJ].imaginary = tmp_input[idx + l].imaginary - out.imaginary;
        }
        idx += (1 << (j + 1));
      }
      // swap input and output
      tmp_input = tmp_output;
    }

    for (int z = 0; z < h; ++z)
    {
      int pixel = (z * w) + i;
      int sum = (i + z) % 2;
      float nega = sum == 0 ? 1.f : -1.f;
      output[pixel].real      = tmp_output[z].real;
      output[pixel].imaginary = tmp_output[z].imaginary;
      output[pixel].real /= (MN * nega);
      output[pixel].imaginary /= (MN * nega);
      output[pixel].Conjugate();
    }
  }

  free(rows);
  return output;
}

unsigned Assignment3::ReverseBits(unsigned original, unsigned bitsize)
{
  unsigned reversed = 0;

  for (unsigned i = 0; i < bitsize; i++)
    reversed |= ((original >> i) & 1) << ((bitsize - 1) - i);

  return reversed;
}
