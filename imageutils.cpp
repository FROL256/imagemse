#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <FreeImage.h>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <math.h>

#include <numeric> // accumulate

///////////////////////////////////////////////////////////////////////////////


void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
{
  std::cout << "\n***\n";
  std::cout << message;
  std::cout << "\n***\n";
}


bool LoadLDRImageFromFile(const char* a_fileName, int* pW, int* pH, std::vector<int32_t>& a_data)
{
  FREE_IMAGE_FORMAT fif = FIF_PNG; // image format

  fif = FreeImage_GetFileType(a_fileName, 0);

  if (fif == FIF_UNKNOWN)
    fif = FreeImage_GetFIFFromFilename(a_fileName);

  FIBITMAP* dib = nullptr;
  if (FreeImage_FIFSupportsReading(fif))
    dib = FreeImage_Load(fif, a_fileName);
  else
  {
    std::cout << "LoadLDRImageFromFile() : FreeImage_FIFSupportsReading/FreeImage_Load failed!" << std::endl;
    return false;
  }

  FIBITMAP* converted = FreeImage_ConvertTo32Bits(dib);
  BYTE* bits          = FreeImage_GetBits(converted);
  auto width          = FreeImage_GetWidth(converted);
  auto height         = FreeImage_GetHeight(converted);
  //auto bitsPerPixel   = FreeImage_GetBPP(converted);

  a_data.resize(width*height);
  BYTE* data = (BYTE*)&a_data[0];

  for (size_t y = 0; y < height; ++y)
  {
    int lineOffset1 = y*width;
    int lineOffset2 = y*width;

    for (size_t x = 0; x < width; ++x)
    {
      int offset1 = lineOffset1 + x;
      int offset2 = lineOffset2 + x;

      data[4 * offset1 + 0] = bits[4 * offset2 + 2];
      data[4 * offset1 + 1] = bits[4 * offset2 + 1];
      data[4 * offset1 + 2] = bits[4 * offset2 + 0];
      data[4 * offset1 + 3] = bits[4 * offset2 + 3];
    }
  }

  FreeImage_Unload(dib);
  FreeImage_Unload(converted);

  (*pW) = width;
  (*pH) = height;

  return true;
}


bool SaveLDRImageToFile(const char* a_fileName, int w, int h, int32_t* data)
{
  FIBITMAP* dib = FreeImage_Allocate(w, h, 32);

  BYTE* bits = FreeImage_GetBits(dib);
  //memcpy(bits, data, w*h*sizeof(int32_t));
  BYTE* data2 = (BYTE*)data;

  for (size_t i = 0; i < (size_t)(w*h); ++i)
  {
    bits[4 * i + 0] = data2[4 * i + 2];
    bits[4 * i + 1] = data2[4 * i + 1];
    bits[4 * i + 2] = data2[4 * i + 0];
    bits[4 * i + 3] = 255; // data2[4 * i + 3]; // 255 to kill alpha channel
  }

	auto imageFileFormat = FIF_PNG;

	std::string fileName(a_fileName);
	if (fileName.size() > 4)
	{
		std::string resolution = fileName.substr(fileName.size() - 4, 4);

		if (resolution.find(".bmp") != std::string::npos || resolution.find(".BMP") != std::wstring::npos)
			imageFileFormat = FIF_BMP;
	}

  if (!FreeImage_Save(imageFileFormat, dib, a_fileName))
  {
    FreeImage_Unload(dib);
    std::cout << "SaveImageToFile(): FreeImage_Save error on " << a_fileName << std::endl;
    return false;
  }

  FreeImage_Unload(dib);

  return true;
}


bool LoadHDRImageFromFile(const char* a_fileName, int* pW, int* pH, std::vector<float>& a_data)
{  
    const char* filename = a_fileName;

    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN; // image format
    FIBITMAP *dib(NULL), *converted(NULL);
    BYTE* bits(NULL);                    // pointer to the image data
    unsigned int width(0), height(0);    //image width and height

    //check the file signature and deduce its format
    //if still unknown, try to guess the file format from the file extension
    //
    fif = FreeImage_GetFileType(filename, 0);
  
    
    if (fif == FIF_UNKNOWN)
    {
      fif = FreeImage_GetFIFFromFilename(filename);
    }
    
    if (fif == FIF_UNKNOWN)
    {
      std::cerr << "FreeImage failed to guess file image format: " << filename << std::endl;
      return false;
    }

    //check that the plugin has reading capabilities and load the file
    //
    if (FreeImage_FIFSupportsReading(fif))
    {
      dib = FreeImage_Load(fif, filename);
    }
    else
    {
      std::cerr << "FreeImage does not support file image format: " << filename << std::endl;
      return false;
    }

    //bool invertY = false; //(fif != FIF_BMP);

    if (!dib)
    {
      std::cerr << "FreeImage failed to load image: " << filename << std::endl;
      return false;
    }

    converted = FreeImage_ConvertToRGBF(dib);


    bits   = FreeImage_GetBits(converted);
    width  = FreeImage_GetWidth(converted);
    height = FreeImage_GetHeight(converted);

    const float* fbits = (const float*)bits;
    a_data.resize(width*height * 4);

    for (size_t i = 0; i < width * height; ++i)
    {
      a_data[4 * i + 0] = fbits[3 * i + 0];
      a_data[4 * i + 1] = fbits[3 * i + 1];
      a_data[4 * i + 2] = fbits[3 * i + 2];
      a_data[4 * i + 3] = 0.0f;
    }

    if(pW != nullptr) (*pW) = width;
    if(pH != nullptr) (*pH) = height;

    FreeImage_Unload(dib);
    FreeImage_Unload(converted);
    return true;
}


float MSE_RGB_LDR(const std::vector<int32_t>& image1, const std::vector<int32_t>& image2)
{
  if(image1.size() != image2.size())
    return 0.0f;

  double accum = 0.0;

  for (size_t i = 0; i < image1.size(); ++i)
  {
    const int pxData1 = image1[i];
    const int pxData2 = image2[i];
    const int r1      = (pxData1 & 0x00FF0000) >> 16;
    const int g1      = (pxData1 & 0x0000FF00) >> 8;
    const int b1      = (pxData1 & 0x000000FF);
     
    const int r2      = (pxData2 & 0x00FF0000) >> 16;
    const int g2      = (pxData2 & 0x0000FF00) >> 8;
    const int b2      = (pxData2 & 0x000000FF);

    const float diffR = r1-r2;
    const float diffG = b1-b2;
    const float diffB = g1-g2;

    accum += double(diffR * diffR + diffG * diffG + diffB * diffB);
  }

  return float(accum / double(size_t(3)*image1.size()));
}


float MSE_RGB_HDR(const std::vector<float>& image1, const std::vector<float>& image2)
{
  if(image1.size() != image2.size())
    return 0.0f;

  double accum = 0.0;

  for (size_t i = 0; i < image1.size(); i += 4)
  {
    const float r1    = image1[i+0];
    const float g1    = image1[i+1];
    const float b1    = image1[i+2];
       
    const float r2    = image2[i+0];
    const float g2    = image2[i+1];
    const float b2    = image2[i+2];

    const float diffR = r1-r2;
    const float diffG = b1-b2;
    const float diffB = g1-g2;

    accum += double(diffR * diffR + diffG * diffG + diffB * diffB);
  }

  return float(4.0 * accum / double(size_t(3)*image1.size())); // we mult by 4 due to image2.size() == w*h*4, but we actually want w*h
}


float Luminance(const float r, const float g, const float b) { return r * 0.2126F + g * 0.7152F + b * 0.0722F; }


double MathExp(const std::vector<float> a_array, const bool a_square, const bool a_generalAggregate) 
{
  const double sizeArray = a_generalAggregate ? (int)a_array.size() : (int)a_array.size() - 1;

  if (a_square)
  {
    double summ = 0.0F;

    for (auto& i : a_array)    
      summ += (i * i);
    
    return summ / sizeArray ;
  }
  else
    return std::accumulate(a_array.begin(), a_array.end(), 0.0F) / sizeArray;
}


double DotMathExp(const std::vector<float> a_array1, const std::vector<float> a_array2) 
{
  const size_t sizeArray = a_array1.size();  
  double summ = 0.0F;

  for (size_t i = 0; i < sizeArray; ++i)
    summ += (a_array1[i] * a_array2[i]);

  return summ / (double)sizeArray;
}


double Covariance(const double dotMathExp, const double mathExpImg1, const double mathExpImg2)
{
  const double covariance = dotMathExp - (mathExpImg1 * mathExpImg2);
  return covariance;
}


double Dispersion(const double mathExp, const double mathExpSqr)
{
  const double dispersion = mathExpSqr - (mathExp * mathExp);
  return fmax(dispersion, 0.0F);
}


float Dssim(const std::vector<float>& a_img1Lum, const std::vector<float>& a_img2Lum, const int bpp)
{
  // test
  //std::vector<float> img1Lum = {1, 3, 3, 14, 56};
  //std::vector<float> img2Lum = {3, 67, 1, 2, 24};
  // mathExpImg1    = 15.4
  // mathExpImg2    = 19.4
  // mathExpSqrImg1 = 670.2
  // mathExpSqrImg2 = 1015.8
  // dispImage1     = 433.04
  // dispImage2     = 639.44
  // dotMathExp     = 315.8
  // covariance     = 17.04
  // ssim           = 0.0797636
  // dssim          = 0.460118

  const double mathExpImg1    = MathExp(a_img1Lum, false, true); // mean
  const double mathExpImg2    = MathExp(a_img2Lum, false, true); // mean
  const double mathExpSqrImg1 = MathExp(a_img1Lum, true, true);
  const double mathExpSqrImg2 = MathExp(a_img2Lum, true, true);  
  const double dispImage1     = Dispersion(mathExpImg1, mathExpSqrImg1);  
  const double dispImage2     = Dispersion(mathExpImg2, mathExpSqrImg2);
  const double dotMathExp     = DotMathExp(a_img1Lum, a_img2Lum);
  const double covariance     = Covariance(dotMathExp, mathExpImg1, mathExpImg2);
        
  std::cout << "mathExpImg1    = " << mathExpImg1    << std::endl;
  std::cout << "mathExpImg2    = " << mathExpImg2    << std::endl;
  std::cout << "mathExpSqrImg1 = " << mathExpSqrImg1 << std::endl;
  std::cout << "mathExpSqrImg2 = " << mathExpSqrImg2 << std::endl;
  std::cout << "dispImage1     = " << dispImage1     << std::endl;
  std::cout << "dispImage2     = " << dispImage2     << std::endl;
  std::cout << "dotMathExp     = " << dotMathExp     << std::endl;
  std::cout << "covariance     = " << covariance     << std::endl;

  const double L      = pow(2, bpp) - 1;
  const double k1     = 0.01;
  const double k2     = 0.03;
  const double c1     = (k1 * L) * (k1 * L);
  const double c2     = (k2 * L) * (k2 * L);

  const double ssim   =    (2.0 * mathExpImg1 * mathExpImg2 + c1) * (2.0 * covariance + c2) / 
    ((mathExpImg1 * mathExpImg1 + mathExpImg2 * mathExpImg2 + c1) * (dispImage1 + dispImage2 + c2));

  const float dssim  = (1.0 - ssim) / 2.0;

  std::cout<< "ssim           = " << ssim     << std::endl;
  std::cout<< "dssim          = " << dssim    << std::endl;

  return dssim;
}


float DSSIM_RGB_LDR(const std::vector<int32_t>& a_image1, const std::vector<int32_t>& a_image2)
{
  if(a_image1.size() != a_image2.size())
    return 0.0F;

  const size_t sizeImg = a_image1.size();
  
  std::vector<float> img1Lum(sizeImg);
  std::vector<float> img2Lum(sizeImg);

  for (size_t i = 0; i < sizeImg; ++i)
  {
    const int pxData1 = a_image1[i];
    const int pxData2 = a_image2[i];
    const int r1      = (pxData1 & 0x00FF0000) >> 16;
    const int g1      = (pxData1 & 0x0000FF00) >> 8;
    const int b1      = (pxData1 & 0x000000FF);
     
    const int r2      = (pxData2 & 0x00FF0000) >> 16;
    const int g2      = (pxData2 & 0x0000FF00) >> 8;
    const int b2      = (pxData2 & 0x000000FF);

    img1Lum[i]        = Luminance(r1, g1, b1) / 255;
    img2Lum[i]        = Luminance(r2, g2, b2) / 255;  
  }  

  const float dssim  = Dssim(img1Lum, img2Lum, 8);

  return dssim;
}


float DSSIM_RGB_HDR(const std::vector<float>& a_image1, const std::vector<float>& a_image2)
{
  if(a_image1.size() != a_image2.size())
    return 0.0f;

  const size_t sizeImg = a_image1.size();
  
  std::vector<float> img1Lum(sizeImg);
  std::vector<float> img2Lum(sizeImg);

  for (size_t i = 0; i < sizeImg; i += 4)
  {
    const float r1    = a_image1[i+0];
    const float g1    = a_image1[i+1];
    const float b1    = a_image1[i+2];
       
    const float r2    = a_image2[i+0];
    const float g2    = a_image2[i+1];
    const float b2    = a_image2[i+2];

    img1Lum[i]        = Luminance(r1, g1, b1);
    img2Lum[i]        = Luminance(r2, g2, b2);  
  }

  const float dssim  = Dssim(img1Lum, img2Lum, 32);

  return dssim;  
}
