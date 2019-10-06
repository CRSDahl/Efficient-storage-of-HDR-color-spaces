#include <iostream>
#include "Magick++.h"
#include <fstream>
#include <array>
#include <list>
#include <string>
#include <sys/stat.h>

using namespace std;
using namespace Magick;

list < array < string, 12 >> csv = {
  {
    "Image",
    "Depth",
    "ColorSpace",
    "Size",
    "RGBFuzzErrorMetric",
    "RGBMeanSquareErrorMetric",
    "RGBPeakSignalToRatioErrorMetric",
    "RGBPerceptualErrorMetric",
    "LABFuzzErrorMetric",
    "LABMeanSquareErrorMetric",
    "LABPeakSignalToRatioErrorMetric",
    "LABPerceptualErrorMetric"
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

//saves the informations of the original image to the csv
void imageInformationToCSV(Image image){
  array < string, 12 > line;
  line[0] = image.fileName(); //filename
  line[1] = to_string(image.depth()); //depth
  line[2] = "RGB(A)"; //colorspace
  line[3] = to_string(image.fileSize()); //filesize
  csv.push_back(line);
}

// Converts the Image to the given color space reduces the Bitdepth of the given 
// Image and the increases the depth to the original depth after this the Image 
//gets converted back to RGB
Image convertReduceIncreasConvertback(Image image, int depth, ColorspaceType colorSpace, ColorspaceType conversionColorSpace){  
  image.colorSpace(colorSpace);
  image.modulusDepth(depth);
  image.modulusDepth(32);
  if (colorSpace != conversionColorSpace){
    image.colorSpace(conversionColorSpace);
  }
  return image;
}

// compares the two images with (FuzzError, MeanSquaredErrorMetric, 
// PeakSignalToNoiseRatioErrorMetric, PerceptualHashErrorMetric) and saves the results
// in a csv document
void compareImagesAndSaveInCSV(Image originalImage, Image changedImage, Image labImage, Image changedLabImage, string colorSpace, int depth){
  array < string, 12 > line;
  line[0] = changedImage.fileName();
  line[1] = to_string(depth); //depth
  line[2] = colorSpace; //colorspace
  line[3] = to_string(getImageSize(changedImage, depth, colorSpace)); //filesize
  line[4] = to_string(changedImage.compare(originalImage, FuzzErrorMetric));
  line[5] = to_string(changedImage.compare(originalImage, MeanSquaredErrorMetric));
  line[6] = to_string(changedImage.compare(originalImage, PeakSignalToNoiseRatioErrorMetric));
  line[7] = to_string(changedImage.compare(originalImage, PerceptualHashErrorMetric));
  line[8] = to_string(changedLabImage.compare(labImage, FuzzErrorMetric));
  line[9] = to_string(changedLabImage.compare(labImage, MeanSquaredErrorMetric));
  line[10] = to_string(changedLabImage.compare(labImage, PeakSignalToNoiseRatioErrorMetric));
  line[11] = to_string(changedLabImage.compare(labImage, PerceptualHashErrorMetric));
  csv.push_back(line);
}

// compares the image with all color spaces and bit depth
void compareColorSpaces(Image image, Image changedImage, Image labImage, Image changedLabImage){
  imageInformationToCSV(image);
  for(int i = 0; i < 11; i++){
    for(int j = 0; j < 4; j++){
      changedImage = convertReduceIncreasConvertback(changedImage, depths[j], colorSpaces[i], RGBColorspace);
      changedLabImage = convertReduceIncreasConvertback(changedLabImage, depths[j], colorSpaces[i], LabColorspace);
      compareImagesAndSaveInCSV(image, changedImage, labImage, changedLabImage, colorSpaceNames[i], depths[j]);
    }
  }
}

// reads in the Image and Compares them in diffrent color Spaces
void readeInImageAndCompare(string path){
  Image image;
  image.read(path);
  image.depth(32);
  image.modulusDepth(32);
  Image image2;
  image2.read(path);
  image2.depth(32);
  image2.modulusDepth(32);
  Image image3;
  image3.read(path);
  image3.colorSpace(LabColorspace);
  image3.depth(32);
  image3.modulusDepth(32);
  Image image4;
  image4.read(path);
  image4.colorSpace(LabColorspace);
  image4.depth(32);
  image4.modulusDepth(32);
  compareColorSpaces(image, image2, image3, image4);
}

// saves the array in a csv file
void saveToCSV(){
  ofstream results("results.csv");
  for (const array < string, 12 > & line: csv) {
    for (const string & field: line) {
      results << field << ";";
    }
    results << "\n";
  }
}

// handels input and starts the comparison of the images
int main(int argc, char ** argv) {
  //case if the user gives no input
  if (argc < 2) {
    cout << "Please input images to process.";
    return 0;
  }else{
        for (int i=1; i<argc; i++) {
            readeInImageAndCompare(argv[i]);
        }
        saveToCSV();
        return 0;
  }
  return -1;
}
