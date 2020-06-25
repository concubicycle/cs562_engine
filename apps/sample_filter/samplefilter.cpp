#include "math.h"
#include <vector>
#include <iostream>
#include <string.h>
#include <assert.h> 
using namespace std;

#include <rgbe.h>

const float PI = 3.1415927;


// Write an HDR image in .hdr (RGBE) format.
void write(float* image, const int rwidth, const int rheight,
           const string outName)
{
    rgbe_header_info info;
    char errbuf[100] = {0};

    FILE* fp  =  fopen(outName.c_str(), "wb");
    int rc = RGBE_WriteHeader(fp, rwidth, rheight, &info, errbuf);
    if (rc != RGBE_RETURN_SUCCESS) {
        printf("RGBE write error: %s\n", errbuf);
        exit(-1); }

    rc = RGBE_WritePixels_RLE(fp, &image[0], rwidth,  rheight, errbuf);
    if (rc != RGBE_RETURN_SUCCESS) {
        printf("RGBE write error: %s\n", errbuf);
        exit(-1); }
    fclose(fp);
}

int main(int argc, char** argv)
{    
    // Read in-file name from command line, create out-file name
    string inName = argv[1];
    string outName = inName.substr(0,inName.length()-4) + "-linear.hdr";

    std::cout << "in: " << inName << std::endl;

    // Read the input file:  expect a *.hdr image
    int width, height;
    rgbe_header_info info;
    char errbuf[100] = {0};
    FILE* fp = fopen(inName.c_str(), "rb");
    int rc = RGBE_ReadHeader(fp, &width, &height, &info, errbuf);
    if (rc != RGBE_RETURN_SUCCESS) {
        printf("RGBE read error: %s\n", errbuf);
        exit(-1); }
    printf("%dx%d\n", width, height);
    
    std::vector<float> image(3*width*height, 0.0);
    rc = RGBE_ReadPixels_RLE(fp, &image[0], width, height, errbuf);
    if (rc != RGBE_RETURN_SUCCESS) {
        printf("RGBE read error: %s\n", errbuf);
        exit(-1); }
    fclose(fp);

    // Gamma correct the image to linear color space.  Use gamma=2.2
    // if you have no specific gamma information. Skip this step if
    // you know image is already in linear space.

    // This is included to demonstrate the magic of OpenMP: This
    // pragma turns the following loop into a multi-threaded loop,
    // making use of all the cores your machine may have.
    #pragma omp parallel for schedule(dynamic, 1) // Magic: Multi-thread y loop
    for (int j=0;  j<height;  j++) {
        for (int i=0;  i<width; i++) {
            int p = (j*width+i);
            for (int c=0;  c<3;  c++) {
                image[3*p+c] *= pow(image[3*p+c], 1.8); } } }

    // Write out unchanged array for MIPMAP level 0
    write(&image[0], width, height, outName); 

    // Continue with images decreasing in size by a factor of two
    // (rounded down when necessary) filtered with cosine-power-lobes
    // of exponents decreasing from about 8192.  Stop at a
    // predetermined minimum size with a predetermined small exponent.

}
