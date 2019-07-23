#include <iostream>
#include "Magick++.h"
#include <fstream>
#include <array>
#include <list>
#include <sys/stat.h>

using namespace std;
using namespace Magick;

list < array < string, 15 >> csv = {
  {
    "Image",
    "Depth",
    "ColorSpace",
    "Size",
    "Relative Size to Original",
    "AbsoluteErrorMetric",
    "FuzzErrorMetric",
    "MeanAbsoluteErrorMetric",
    "MeanErrorPerPixelMetric",
    "MeanSquareErrorMetric",
    "NormalizedCrossCorrelationErrorMetric",
    "PeakAbsoluteErrorMetric",
    "PeakSignalToRatioErrorMetric",
    "PerceptualErrorMetric",
    "RootMeanSquaredErrorMetric"
  }
};

int depths[] = {
  4,
  6,
  8,
  10
};

void convertImageToColorSpace(Image images[], string colorspace, ColorspaceType colorSpaceEnum, Image originalImage) {
  for (int i = 0; i < sizeof(depths) / sizeof(depths[0]); i++) {
    images[i].colorSpace(colorSpaceEnum);
    images[i].modulusDepth(depths[i]);
    string originalImageSource = originalImage.fileName().substr(originalImage.fileName().find_last_of("/\\") + 1);
    string savingString = "ConvertedPictures/_" + originalImageSource + colorspace + to_string(depths[i]);
    int w = images[i].columns();
    int h = images[i].rows();
    int channels = images[i].channels();
    int numvalues = w * h * channels;
    int size = (numvalues * depths[i]) / 8;
    MagickCore::Quantum * pixels = images[i].getPixels(0, 0, w, h);
    unsigned offset = images[i].channels() * (w * 0 + 0);
    int range = pow(2, images[i].modulusDepth()) - 1;
    //save the converted image
    std::ofstream ofile(savingString, std::ios::binary);
    for (int j = 0; j < numvalues; j++) {
      int pixel = (pixels[j] * range / QuantumRange);
      ofile.write((char * ) & pixel, sizeof(int));
    }
    //generate line in csv file
    array < string, 15 > line;
    line[0] = images[i].fileName() + "_" + colorspace + to_string(images[i].modulusDepth()); //filename
    line[1] = to_string(images[i].modulusDepth()); //depth
    line[2] = colorspace; //colorspace
    line[3] = to_string(size); //filesize
    double relativesize = (double) size / (double) originalImage.fileSize();
    line[4] = to_string(relativesize); //size relative to original
    line[5] = to_string(images[i].compare(originalImage, AbsoluteErrorMetric));
    line[6] = to_string(images[i].compare(originalImage, FuzzErrorMetric));
    line[7] = to_string(images[i].compare(originalImage, MeanAbsoluteErrorMetric));
    line[8] = to_string(images[i].compare(originalImage, MeanErrorPerPixelErrorMetric));
    line[9] = to_string(images[i].compare(originalImage, MeanSquaredErrorMetric));
    line[10] = to_string(images[i].compare(originalImage, NormalizedCrossCorrelationErrorMetric));
    line[11] = to_string(images[i].compare(originalImage, PeakAbsoluteErrorMetric));
    line[12] = to_string(images[i].compare(originalImage, PeakSignalToNoiseRatioErrorMetric));
    line[13] = to_string(images[i].compare(originalImage, PerceptualHashErrorMetric));
    line[14] = to_string(images[i].compare(originalImage, RootMeanSquaredErrorMetric));
    csv.push_back(line);
  }
}

void addImage(string path) {
  try {
    // Read a file into image object 
    Image readInImage;
    readInImage.read(path);
    // create clone images with different depths (16bit, 24bit, 32bit)
    Image tempImages[sizeof(depths) / sizeof(depths[0])] = {};
    for (int i = 0; i < sizeof(depths) / sizeof(depths[0]); i++) {
      tempImages[i].read(path);
    }

    //first set the data of the original file
    array < string, 15 > line;
    line[0] = readInImage.fileName(); //filename
    line[1] = to_string(readInImage.depth()); //depth
    line[2] = "RGB(A)"; //colorspace
    line[3] = to_string(readInImage.fileSize()); //filesize
    csv.push_back(line);

    convertImageToColorSpace(tempImages, "CMYK", CMYKColorspace, readInImage);
    convertImageToColorSpace(tempImages, "YCbCr", YCbCrColorspace, readInImage);
    convertImageToColorSpace(tempImages, "HCL", HCLColorspace, readInImage);
    convertImageToColorSpace(tempImages, "Lab", LabColorspace, readInImage);
    convertImageToColorSpace(tempImages, "Luv", LuvColorspace, readInImage);
    convertImageToColorSpace(tempImages, "RGB", RGBColorspace, readInImage);
    convertImageToColorSpace(tempImages, "sRGB", sRGBColorspace, readInImage);
    convertImageToColorSpace(tempImages, "XYZ", XYZColorspace, readInImage);
    convertImageToColorSpace(tempImages, "YCC", YCCColorspace, readInImage);
    convertImageToColorSpace(tempImages, "YPbPr", YPbPrColorspace, readInImage);
    convertImageToColorSpace(tempImages, "YUV", YUVColorspace, readInImage);
  } catch (Exception & error_) {
    cout << "Caught exception: " << error_.what() << endl;
  }
}

int main(int argc, char ** argv) {
  #ifdef __linux__
    mkdir("ConvertedPictures", 0777);
  #else
	_wmkdir(L"ConvertedPictures");
  #endif

  if (argc < 2)
    cout << "Please pass a number of pictures as parameters\n";

  for (int i = 1; i < argc; ++i) {
    cout << argv[i] << "\n";
    addImage(argv[i]);
  }

  ofstream results("results.csv");
  for (const array < string, 15 > & line: csv) {
    for (const string & field: line) {
      results << field << ",";
    }
    results << "\n";
  }
  return 0;
}