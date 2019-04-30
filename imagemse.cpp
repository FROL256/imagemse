#include <iostream>
#include <vector>
#include <string>

bool LoadLDRImageFromFile(const char* a_fileName,
                          int* pW, int* pH, std::vector<int32_t>& a_data);
bool SaveLDRImageToFile(const char* a_fileName, int w, int h, int32_t* data);


bool LoadHDRImageFromFile(const char* a_fileName,
                          int* pW, int* pH, std::vector<float>& a_data);

float MSE_RGB_LDR(const std::vector<int32_t>& image1, const std::vector<int32_t>& image2);
float MSE_RGB_HDR(const std::vector<float>& image1, const std::vector<float>& image2);


int main(int argc, const char** argv)
{
  try 
  { 
    if(argc < 3)
    {
      std::cout << "argc = " << argc << std::endl;
      std::cout << "must freater or equal 3" << std::endl; 
      std::cout << "example: 'imagemse image1.png image2.png'" << std::endl;
      return 0; 
    }

    std::string fileImg0 = argv[1];
    std::string fileImg1 = argv[2];

    int w,h;
    std::vector<int32_t> image1, image2;
    std::vector<float>   image1f, image2f;

    float mse = 0.0f;

    if(fileImg0.find(".hdr") != std::string::npos)
    {
       LoadHDRImageFromFile(fileImg0.c_str(), &w, &h, image1f);
       LoadHDRImageFromFile(fileImg1.c_str(), &w, &h, image2f);
       mse = MSE_RGB_HDR(image1f, image2f);
    }
    else
    {
       LoadLDRImageFromFile(fileImg0.c_str(), &w, &h, image1);
       LoadLDRImageFromFile(fileImg1.c_str(), &w, &h, image2);
       mse = MSE_RGB_LDR(image1, image2);
    }

    std::cout << mse << std::endl;

  }
  catch(std::bad_alloc err)
  {
    std::cout << "bad_alloc: " << err.what() << std::endl;
  }
  catch(...)
  {
    std::cout << "unknown error" << std::endl;
  }
  
  return 0;
}
