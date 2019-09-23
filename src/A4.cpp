#include "A4.h"

unsigned char * Assignment4::PepperSaltNoise(int w, int h, unsigned char * data, double pa, double pb)
{
  std::srand(static_cast<unsigned>(std::time(nullptr)));
  unsigned char* output = static_cast<unsigned char*>(malloc(w * h * 3));
  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      float probability = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
      unsigned pixel = ((i * w) + j) * 3;
      int red = data[pixel];
      int green = data[pixel + 1];
      int blue = data[pixel + 2];

      if (probability < pa)
        blue = green = red = static_cast<int>(PEPPERNOISE * 255);
      else if (probability > pb)
        blue = green = red = static_cast<int>(SALTNOISE * 255);

      output[pixel] = static_cast<unsigned char>(red);
      output[pixel + 1] = static_cast<unsigned char>(green);
      output[pixel + 2] = static_cast<unsigned char>(blue);

    }
  }
  return output;
}

unsigned char * Assignment4::MedianFilter(int w, int h, unsigned char * data)
{
  unsigned char* output = static_cast<unsigned char*>(malloc(w * h * 3));
  std::vector<int> filter;
  
  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      // median filter 3x3
      for (int startY = -1; startY < 2; ++startY)
      {
        int y = i + startY;
        for (int startX = -1; startX < 2; ++startX)
        {
          int x = j + startX;

          // value repeat
          x = x < 0 ? 0 : x;
          y = y < 0 ? 0 : y;
          x = x >= w ? w - 1 : x;
          y = y >= h ? h - 1 : y;

          unsigned pixel = ((y * w) + x) * 3;

          int value = static_cast<int>(data[pixel]);
          filter.push_back(value);
        }
      }

      // find median
      std::sort(filter.begin(), filter.end());
      auto value = filter.begin();
      for (int k = 0; k < 4; ++k) ++value;

      unsigned pixel = ((i * w) + j) * 3;
      output[pixel] = static_cast<unsigned char>(*value);
      output[pixel + 1] = static_cast<unsigned char>(*value);
      output[pixel + 2] = static_cast<unsigned char>(*value);

      filter.clear();

    }
  }

  free(data);

  return output;
}

unsigned char * Assignment4::LocalNoiseReduction(int w, int h, unsigned char * data, double NoiseVariance, int mask)
{
  unsigned char* output = static_cast<unsigned char*>(malloc(w * h * 3));

  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      // mean filter 7x7
      double mean = 0;
      int sz = mask / 2;
      for (int startY = -sz; startY < sz + 1; ++startY)
      {
        int y = i + startY;
        for (int startX = -sz; startX < sz + 1; ++startX)
        {
          int x = j + startX;

          // value repeat
          x = x < 0 ? 0 : x;
          y = y < 0 ? 0 : y;
          x = x >= w ? w - 1 : x;
          y = y >= h ? h - 1 : y;

          unsigned pixel = ((y * w) + x) * 3;

          mean += static_cast<double>(data[pixel]);
        }
      }

      // find mean
      mean /= static_cast<double>(mask * mask);

      double sum = 0.0;

      // sum of deviation
      for (int startY = -sz; startY < sz + 1; ++startY)
      {
        int y = i + startY;
        for (int startX = -sz; startX < sz + 1; ++startX)
        {
          int x = j + startX;

          // value repeat
          x = x < 0 ? 0 : x;
          y = y < 0 ? 0 : y;
          x = x >= w ? w - 1 : x;
          y = y >= h ? h - 1 : y;

          unsigned pixel = ((y * w) + x) * 3;

          double value = static_cast<double>(data[pixel]);
          value -= mean;
          sum += (value * value);
        }
      }

      // find variance
      double localVariance = sum / static_cast<double>((mask * mask));


      unsigned pixel = ((i * w) + j) * 3;
      double coeffiecent = NoiseVariance / localVariance;
      if (NoiseVariance < 0.0) coeffiecent = 1.0;
      double noise = static_cast<double>(data[pixel]) - mean;
      noise *= coeffiecent;
      if (!localVariance) noise = 0.0;
      double result = static_cast<double>(data[pixel]) - noise;

      result = result > 255.0 ? 255.0 : result;
      result = result < 0 ? 0 : result;

      output[pixel] = static_cast<unsigned char>(result);
      output[pixel + 1] = static_cast<unsigned char>(result);
      output[pixel + 2] = static_cast<unsigned char>(result);
    }
  }


  return output;
}

unsigned char * Assignment4::AdaptiveMedianFilter(int w, int h, unsigned char * data, int maxSize)
{
  unsigned char* output = static_cast<unsigned char*>(malloc(w * h * 3));
  
  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      int windowSize = 3;
      int median = 0;
      int min = std::numeric_limits<int>::max();
      int max = std::numeric_limits<int>::min();
      std::vector<int> filter;
	    bool complete = false;
      while(!complete)
      {
        median = 0;
        min = std::numeric_limits<int>::max();
        max = std::numeric_limits<int>::min();

        int sz = windowSize / 2;
        for (int startY = -sz; startY < sz + 1; ++startY)
        {
          
          for (int startX = -sz; startX < sz + 1; ++startX)
          {
			  int y = i + startY;
            int x = j + startX;

            // value repeat
            x = x < 0 ? 0 : x;
            y = y < 0 ? 0 : y;
            x = x >= w ? w - 1 : x;
            y = y >= h ? h - 1 : y;

            unsigned pixel = ((y * w) + x) * 3;

            int value = static_cast<int>(data[pixel]);
            filter.push_back(value);

            if (value < min) min = value;
            if (value > max) max = value;
          }
        }

        std::sort(filter.begin(), filter.end());
	     	median = filter[filter.size() / 2];

        filter.clear();

        // STAGE A

        int A1 = median - min;
        int A2 = median - max;

        if (A1 > 0 && A2 < 0)
        {
          // STAGE B
          unsigned pixel = ((i * w) + j) * 3;
          int intensity = static_cast<int>(data[pixel]);
          int B1 = intensity - min;
          int B2 = intensity - max;
          if (B1 > 0 && B2 < 0)
          {
            output[pixel] = static_cast<unsigned char>(intensity);
            output[pixel + 1] = static_cast<unsigned char>(intensity);
            output[pixel + 2] = static_cast<unsigned char>(intensity);
          }
          else
          {
            output[pixel] = static_cast<unsigned char>(median);
            output[pixel + 1] = static_cast<unsigned char>(median);
            output[pixel + 2] = static_cast<unsigned char>(median);
          }
		      complete = true;
          break;
        }
        else
          windowSize += 2;
		if (windowSize > maxSize) break;
	  };
      if (!complete)
      {
        unsigned pixel = ((i * w) + j) * 3;
        output[pixel] = static_cast<unsigned char>(median);
        output[pixel + 1] = static_cast<unsigned char>(median);
        output[pixel + 2] = static_cast<unsigned char>(median);
      }
    }
  }

  return output;
}

unsigned char * Assignment4::GaussianNoise(int w, int h, unsigned char * data, double mean, double variance)
{
  std::random_device rd{};
  std::mt19937 gen{ rd() };
  std::normal_distribution<> d(mean, sqrt(variance));

  unsigned char* output = static_cast<unsigned char*>(malloc(w * h * 3));

  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      unsigned pixel = ((i * w) + j) * 3;
      double value = d(gen);
      double result = static_cast<double>(data[pixel]) + value;

      result = result < 0 ? 0 : result;
      result = result > 255.0 ? 255.0 : result;

      output[pixel] = static_cast<unsigned char>(result);
      output[pixel + 1] = static_cast<unsigned char>(result);
      output[pixel + 2] = static_cast<unsigned char>(result);
    }
  }

  return output;
}
