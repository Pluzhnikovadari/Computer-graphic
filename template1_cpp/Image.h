#ifndef MAIN_IMAGE_H
#define MAIN_IMAGE_H

#include <cstring>
#include <string>

constexpr int tileSize = 32;


struct Pixel
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

constexpr Pixel backgroundColor{255, 215, 0, 0};

Pixel blend(const Pixel &oldPix, Pixel newPix);

struct Image
{
  explicit Image(const std::string &a_path);
  Image(int a_width, int a_height, int a_channels);

  int Save(const std::string &a_path);

  int Width()    const { return width; }
  int Height()   const { return height; }
  int Channels() const { return channels; }
  size_t Size()  const { return size; }
  Pixel* Data()        { return  data; }

  Pixel GetPixel(int x, int y) { return data[width * y + x];}
  Pixel GetBufPixel(int x, int y) { return buffer[width * y + x];}
  void  PutPixel(int x, int y, const Pixel &pix) { data[width* y + x] = pix; }
  void  PutBufPixel(int x, int y, const Pixel &pix) { buffer[width* y + x] = pix; }
  void BufUpdate();

  ~Image();

private:
  int width = -1;
  int height = -1;
  int channels = 4;
  size_t size = 0;
  Pixel *data = nullptr;
  Pixel *buffer = nullptr;
  bool self_allocated = false;
};



#endif //MAIN_IMAGE_H
