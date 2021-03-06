#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
/*
Pixel blend(const Pixel &oldPix, Pixel newPix) {
    newPix.r = newPix.a / 255.0 * (newPix.r - oldPix.r) + oldPix.r;
    newPix.g = newPix.a / 255.0 * (newPix.g - oldPix.g) + oldPix.g;
    newPix.b = newPix.a / 255.0 * (newPix.b - oldPix.b) + oldPix.b;
    newPix.a = 255;
    return newPix;
}
*/

Image::Image(const std::string &a_path)
{
  if((data = (Pixel*)stbi_load(a_path.c_str(), &width, &height, &channels, 4)) != nullptr)
  {
    size = width * height * channels;
  }
  BufUpdate();
}

Image::Image(int a_width, int a_height, int a_channels)
{
  data = new Pixel[a_width * a_height ]{};

  if(data != nullptr)
  {
    width = a_width;
    height = a_height;
    size = a_width * a_height * a_channels;
    channels = a_channels;
    self_allocated = true;
    BufUpdate();
  }
}


int Image::Save(const std::string &a_path)
{
  auto extPos = a_path.find_last_of('.');
  if(a_path.substr(extPos, std::string::npos) == ".png" || a_path.substr(extPos, std::string::npos) == ".PNG")
  {
    stbi_write_png(a_path.c_str(), width, height, channels, data, width * channels);
  }
  else if(a_path.substr(extPos, std::string::npos) == ".jpg" || a_path.substr(extPos, std::string::npos) == ".JPG" ||
          a_path.substr(extPos, std::string::npos) == ".jpeg" || a_path.substr(extPos, std::string::npos) == ".JPEG")
  {
    stbi_write_jpg(a_path.c_str(), width, height, channels, data, 100);
  }
  else
  {
    std::cerr << "Unknown file extension: " << a_path.substr(extPos, std::string::npos) << "in file name" << a_path << "\n";
    return 1;
  }

  return 0;
}

void Image::BufUpdate() {
  if (buffer != nullptr) {
    delete [] buffer;
  }
  buffer = new Pixel[height * width * channels];
  std::memcpy(buffer, data, width * height * channels);
}


Image::~Image()
{
  if(self_allocated)
    delete [] data;
  else
  {
    stbi_image_free(data);
  }
}