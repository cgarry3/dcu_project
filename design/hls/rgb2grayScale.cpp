// -------------------------------------------------------------------------
//       DCU Custom RGB to Grayscale Filter
//
//  Author:       Cathal Garry
//  Description:  This is a custom RGB to Grayscale filter for
//                720P images. It contains an input and output AXI stream
//                ports.
// -------------------------------------------------------------------------

#include "rgb2grayScale.h"


using namespace std;


void rgb2grayScale_filter(AXI_STREAM& INPUT_STREAM, AXI_STREAM& OUTPUT_STREAM)
{
	// Creates an input and output stream
	#pragma HLS INTERFACE axis register both port=INPUT_STREAM
	#pragma HLS INTERFACE axis register both port=OUTPUT_STREAM

	// Removes ap_ctrl interface
	#pragma HLS INTERFACE ap_ctrl_none port=return

	// Synthesis data flow improvement
        #pragma HLS dataflow

	const int rows = MAX_HEIGHT;
	const int cols = MAX_WIDTH;

	// Input and output image
	RGB_IMAGE  input_image    (rows, cols);
	GRAY_IMAGE gray_image     (rows, cols);
	RGB_IMAGE  output_image   (rows, cols);

	// Convert AXI4 Stream data to hls::mat format
	hls::AXIvideo2Mat(INPUT_STREAM, input_image);

	hls::CvtColor<HLS_RGB2GRAY>(input_image, gray_image);
	hls::CvtColor<HLS_GRAY2RGB>(gray_image, output_image);

	// Convert the hls::mat format to AXI4 Stream format with SOF, EOL signals
	hls::Mat2AXIvideo(output_image, OUTPUT_STREAM);

}
