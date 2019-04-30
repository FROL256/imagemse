#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <FreeImage.h>

#include <cassert>

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message);

bool LoadLDRImageFromFile(const char* a_fileName, int* pW, int* pH, std::vector<int32_t>& a_data);
bool SaveLDRImageToFile(const char* a_fileName, int w, int h, int32_t* data);


bool LoadHDRImageFromFile(const char* a_fileName, int* pW, int* pH, std::vector<float>& a_data);

float MSE_RGB_LDR(const std::vector<int32_t>& image1, const std::vector<int32_t>& image2);
float MSE_RGB_HDR(const std::vector<float>& image1, const std::vector<float>& image2);

/*
void imagecut(std::string dir, int* xywh, int quads, std::string dir_out)
{
  
  // (2) cut images
  //
  FreeImage_SetOutputMessage(FreeImageErrorHandler);

  std:: vector<std:: string> files = listfiles(dir.c_str());
  std:: sort(files.begin(), files.end());
 
  if(files.size() < 4)
  {
    std:: cerr << "imagecut, bad file number: " << files.size() << std:: endl;
    return;
  }

  int wr,hr;
  std:: vector<int32_t> imageRef;
  std:: vector<int32_t> imageRef2;
  {
    std::string refName  = dir + "/" + files[5];
    std::string refName2 = dir + "/" + files[6];

    std:: cout << "refName  = " << refName.c_str()  << std:: endl;
    std:: cout << "refName2 = " << refName2.c_str() << std:: endl;
    std:: cout << std:                                     : endl;
  
    LoadLDRImageFromFile(refName.c_str(),  &wr, &hr, imageRef);
    LoadLDRImageFromFile(refName2.c_str(), &wr, &hr, imageRef2);
  }

  std:: cout << "process folder : " << dir.c_str() << std:: endl;

  for(auto& p : files)
  {
    if(p == "." || p == ".." || p.find(".png") == std::string::npos)
      continue;

    int w,h;
    std:: vector<int32_t> image;
    
    std::string fileIn = dir     + "/" + p;
    LoadLDRImageFromFile(fileIn.c_str(), &w, &h, image);
    
    std:: cout << " --> process file : " << p.c_str() << ", MSE = " << MSE_RGB_LDR(image, imageRef)*256.0f << std:: endl;

    for(int quad=0;quad<quads;quad++)
    {
      std:: stringstream strOut;
      strOut << quad;
      std::string fileOut = dir_out + "/" + std::string("zcut_") + strOut.str() + "_" + p;

      int minX = xywh[0 + quad*4];
      int minY = xywh[1 + quad*4];
      int maxX = xywh[2 + quad*4];
      int maxY = xywh[3 + quad*4];

      if(minX < 0)  minX = 0;
      if(minY < 0)  minY = 0;
      if(maxX >= w) maxX = w-1;
      if(maxY >= h) maxY = h-1;

      std:: vector<int32_t> imageCut(xywh[2]*xywh[3]);
      std:: vector<int32_t> imageCutRef(xywh[2]*xywh[3]);

      for(int y=0;y<xywh[3];y++)
      {
        for(int x=0;x<=xywh[2];x++)
          imageCut[xywh[2]*y + x] = image[(minY+y)*w + minX + x];
      }

      for(int y=0;y<xywh[3];y++)
      {
        for(int x=0;x<=xywh[2];x++)
          imageCutRef[xywh[2]*y + x] = imageRef[(minY+y)*w + minX + x];
      }
 
      std:: cout << "cut(" << quad << "), MSE = " << MSE_RGB_LDR(imageCut, imageCutRef)*256.0f << std:: endl;

      SaveLDRImageToFile(fileOut.c_str(), xywh[2], xywh[3], &imageCut[0]);

    }
  }

}
*/



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std:: vector<int32_t> cut(const std::vector<int32_t>& a_image, int minX, int minY, int sizeX, int sizeY, int w, int h)
{
  int maxX = minX + sizeX;
  int maxY = minY + sizeY;

  assert(minX >= 0);
  assert(minY >= 0);
  assert(maxX < w);
  assert(maxY < h);

  std:: vector<int32_t> imageCut(sizeX*sizeY);

  for (int y = 0; y < sizeY; y++)
  {
    for (int x = 0; x < sizeX; x++)
      imageCut[sizeX * y + x] = a_image[(minY + y) * w + minX + x];
  }

  return imageCut;
}


std:: vector<float> cut(const std::vector<float>& a_image, int minX, int minY, int sizeX, int sizeY, int w, int h)
{
  int maxX = minX + sizeX;
  int maxY = minY + sizeY;

  assert(minX >= 0);
  assert(minY >= 0);
  assert(maxX < w);
  assert(maxY < h);

  std:: vector<float> imageCut(size_t(sizeX * sizeY * 4));

  for (int y = 0; y < sizeY; ++y)
  {
    for (int x = 0; x < sizeX; ++x)
    {
      const size_t offset1  = 4 * (sizeX * y + x);
      const size_t offset2  = 4 * ((minY + y) * w + minX + x);

      imageCut[offset1 + 0] = a_image[offset2 + 0];
      imageCut[offset1 + 1] = a_image[offset2 + 1];
      imageCut[offset1 + 2] = a_image[offset2 + 2];
      imageCut[offset1 + 3] = a_image[offset2 + 3];
    }
  }

  return imageCut;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, const char** argv)
{
  try 
  { 
    if(argc < 3 + 4)
    {
      std:: cout << "argc = " << argc << std:: endl;
      std:: cout << "must freater or equal 3" << std:: endl;
      std:: cout << "example: 'imagecut imageIn.png subImageOut.png 20 50 100 100'" << std:: endl;
      std:: cout << "args: 'imagecut input.png output.png boxMinX boxMinY boxSizeX boxSizeY'" << std::endl;
      return 0; 
    }

    std::string fileImg0 = argv[1];
    std::string fileImg1 = argv[2];

    const int boxMinX    = atoi(argv[3]);
    const int boxMinY    = atoi(argv[4]);
    
    const int boxSizeX   = atoi(argv[5]);
    const int boxSizeY   = atoi(argv[6]);


    int w,h;
    std:: vector<int32_t> image1;
    std:: vector<float>   image1f;

    

    if(fileImg0.find(".hdr") != std::string::npos)
    {
      LoadHDRImageFromFile(fileImg0.c_str(), &w, &h, image1f);
      auto dataCut = cut(image1f, boxMinX, boxMinY, boxSizeX, boxSizeY, w, h);
      //SaveHDRImageToFile(fileImg1.c_str(), boxSizeX, boxSizeY, dataCut.data());
      std:: cout << "SaveHDRImageToFile: " << "not implemented" << std:: endl;
    }
    else
    {
      LoadLDRImageFromFile(fileImg0.c_str(), &w, &h, image1);
      auto dataCut = cut(image1, boxMinX, boxMinY, boxSizeX, boxSizeY, w, h);
      SaveLDRImageToFile(fileImg1.c_str(), boxSizeX, boxSizeY, dataCut.data());
    }
  }
  catch(std::bad_alloc& err)
  {
    std:: cout << "bad_alloc: " << err.what() << std:: endl;
  }
  catch(...)
  {
    std:: cout << "unknown error" << std:: endl;
  }
  
  return 0;
}
