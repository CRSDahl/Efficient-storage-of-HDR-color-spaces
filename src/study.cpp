#include <iostream>
#include "Magick++.h"
#include <fstream>
#include <array>
#include <list>
#include <string>
//#include <filesystem>
#include <sys/stat.h>

//namespace fs = std::filesystem;
using namespace std;
using namespace Magick;

list < array < string, 8 >> csv = {
  {
    "Image",
    "Depth",
    "ColorSpace",
    "Size",
    "FuzzErrorMetric",
    "MeanSquareErrorMetric",
    "PeakSignalToRatioErrorMetric",
    "PerceptualErrorMetric"
  }
};

ColorspaceType colorSpaces[11] = {
    CMYKColorspace,
    YCbCrColorspace,
    HCLColorspace,
    LabColorspace,
    LuvColorspace,
    RGBColorspace,
    sRGBColorspace,
    XYZColorspace,
    YCCColorspace,
    YPbPrColorspace,
    YUVColorspace
};

string colorSpaceNames[11] = {
    "CMYK",
    "YCbC",
    "HCL",
    "Lab",
    "Luv",
    "RGB",
    "sRGB",
    "XYZ",
    "YCC",
    "YPbPr",
    "YUV"
};

int depths[4] = {
  4,
  6,
  8,
  10
};

// Converts the Image to the given color space reduces the Bitdepth of the given 
// Image and the increases the depth to the original depth after this the Image 
//gets converted back to RGB
Image convertReduceIncreasConvertback(Image image, int depth, ColorspaceType colorSpace){
  ColorRGB px = image.pixelColor(1,1);
  std::cout << px.red() << px.green() << px.blue() << std::endl;
  
  image.colorSpace(colorSpace);
  image.modulusDepth(depth);
  //image.depth(depth);
  //image.depth(32);
  //image.modulusDepth(32);
  std::cout << image.colorSpace() << std::endl;
  if (colorSpace != RGBColorspace){
    image.colorSpace(RGBColorspace);
  }
  px = image.pixelColor(1,1);
  std::cout << px.red() << px.green() << px.blue() << std::endl;
  return image;
}

//calculates the size of the image
int getImageSize(Image image, int depth, string colorSpace){
  int w = image.columns();
  int h = image.rows();
  int channels = image.channels();
  int numvalues = w * h * channels;
  int size = (numvalues * depth) / 8;
  if (colorSpace=="CMYK"){
    size = size / 3 * 4;
  }
  return size;
}

// compares the two images with (FuzzError, MeanSquaredErrorMetric, 
// PeakSignalToNoiseRatioErrorMetric, PerceptualHashErrorMetric) and saves the results
// in a csv document
void compareImagesAndSaveInCSV(Image originalImage, Image changedImage, string colorSpace, int depth){
  array < string, 8 > line;
  line[0] = changedImage.fileName();
  line[1] = to_string(depth); //depth
  line[2] = colorSpace; //colorspace
  line[3] = to_string(getImageSize(changedImage, depth, colorSpace)); //filesize
  line[4] = to_string(changedImage.compare(originalImage, FuzzErrorMetric));
  line[5] = to_string(changedImage.compare(originalImage, MeanSquaredErrorMetric));
  line[6] = to_string(changedImage.compare(originalImage, PeakSignalToNoiseRatioErrorMetric));
  line[7] = to_string(changedImage.compare(originalImage, PerceptualHashErrorMetric));
  csv.push_back(line);
}

//saves the informations of the original image to the csv
void imageInformationToCSV(Image image){
  array < string, 8 > line;
  line[0] = image.fileName(); //filename
  line[1] = to_string(image.depth()); //depth
  line[2] = "RGB(A)"; //colorspace
  line[3] = to_string(image.fileSize()); //filesize
  csv.push_back(line);
}

// compares the image with all color spaces and bit depth
void compareColorSpaces(Image image, Image changedImage){
  imageInformationToCSV(image);
  for(int i = 0; i < 11; i++){
    for(int j = 0; j < 4; j++){
      //Image changedImage = image;
      changedImage = convertReduceIncreasConvertback(changedImage, depths[j], colorSpaces[i]);
      compareImagesAndSaveInCSV(image, changedImage, colorSpaceNames[i], depths[j]);
    }
  }
}

// saves the array in a csv file
void saveToCSV(){
  ofstream results("results.csv");
  for (const array < string, 8 > & line: csv) {
    for (const string & field: line) {
      results << field << ",";
    }
    results << "\n";
  }
}

// reads in the Image and Compares them in diffrent color Spaces
void readeInImageAndCompare(string path){
  std::cout << "readeInImageAndCompare \n";
  Image image;
  image.depth(32);
  image.modulusDepth(32);
  image.read(path);
  Image image2;
  image2.depth(32);
  image2.modulusDepth(32);
  image2.read(path);
  std::cout << image.colorSpace() << std::endl;
  compareColorSpaces(image, image2);
}

// reads in all Images in the Folder and Compares them in diffrent color Spaces
void readeInFolderAndCompareAll(string path){
  //for (const auto & entry : fs::directory_iterator(path)){
  //  std::cout << entry.path() << std::endl;
  //  readeInImageAndCompare(entry.path());
  //}
}

// handels input and starts the comparison of the images
int main(int argc, char ** argv) {
  //case if the user gives no input
  if (argc < 2) {
    cout << "Please pass: -pic ImagePath \n or: -all FolderPath \n";
    return 0;
  }

  string arg1 = argv[1];

  //case if the user inputs -help -h
  if (arg1 == "-h" || arg1 == "-help"){
    cout << "Please pass: -pic ImagePath \n or: -all FolderPath \n";
    return 0;
  }

  //normal case if the user inputs a file or a folder
  if (arg1 == "-pic" && argc == 3){
    readeInImageAndCompare(argv[2]);
    saveToCSV();
    return 0;
  } else if(arg1 == "-all" && argc == 3) {
    readeInFolderAndCompareAll(argv[2]);
    saveToCSV();
    return 0;
  }else if(arg1 == "-comp" && argc == 4) {
    Image image1;
    image1.read(argv[2]);
    Image image2;
    image2.read(argv[3]);
    compareImagesAndSaveInCSV(image1, image2, "", 32);
    saveToCSV();
    return 0;
  }

  return -1;
}
