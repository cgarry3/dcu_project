#include "rgb2grayScale.h"
#include <iostream>
#include <stdio.h>

using namespace std;


void rgb2grayScale_filter(AXI_STREAM& INPUT_STREAM, AXI_STREAM& OUTPUT_STREAM)
{
	//#pragma HLS INTERFACE axis depth=10000 port=INPUT_STREAM bundle=VIDEO_IN
	//#pragma HLS INTERFACE axis depth=10000 port=OUTPUT_STREAM bundle=VIDEO_OUT
    #pragma HLS dataflow

	const int rows = MAX_HEIGHT;
	const int cols = MAX_WIDTH;

	// Input and output image
	RGB_IMAGE  input_image  (rows, cols);
	GRAY_IMAGE output_image (rows, cols);
	RGB_IMAGE  output_image2  (rows, cols);

	// Convert AXI4 Stream data to hls::mat format
	hls::AXIvideo2Mat(INPUT_STREAM, input_image);

	hls::CvtColor<HLS_RGB2GRAY>(input_image, output_image);
	hls::CvtColor<HLS_GRAY2RGB>(output_image, output_image2);

	// Convert the hls::mat format to AXI4 Stream format with SOF, EOL signals
	hls::Mat2AXIvideo(output_image2, OUTPUT_STREAM);

}
