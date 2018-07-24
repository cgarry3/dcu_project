// -------------------------------------------------------------------------
//       DCU Custom Corner Detector Filter
//
//  Author:       Cathal Garry
//  Description:
// ------------------------------------------------------------------------

#include "cornerDetection.h"

void cornerDetection(AXI_STREAM& stream_in, AXI_STREAM& stream_out)
{

    // ----------------------------------------
    //  Local Storage
    // ----------------------------------------

    // Image width and height
    int const rows = MAX_HEIGHT;
    int const cols = MAX_WIDTH;

    // RGB Image storage
    RGB_IMAGE  imgInput(rows, cols);
    RGB_IMAGE  imgDuplicate0(rows, cols);
    RGB_IMAGE  imgDuplicate1(rows, cols);
    RGB_IMAGE  imgRGB(rows, cols);
    RGB_IMAGE  imgOutput(rows, cols);

    // Gray Image storage
    GRAY_IMAGE imgGray(rows, cols);
    GRAY_IMAGE imgNormalize(rows, cols);
    GRAY_IMAGE imgScale(rows, cols);
    GRAY_IMAGE imgThres(rows, cols);

    // Harris Filter Storage
    HARRIS_IMAGE	harris_resp(rows,cols);
    HARRIS_IMAGE	harris_resp0(rows,cols);
    HARRIS_IMAGE	harris_resp1(rows,cols);


    // ----------------------------------------
    //  Directives
    // ----------------------------------------

    #pragma HLS INTERFACE axis register both port=stream_out
    #pragma HLS INTERFACE axis register both port=stream_in

    // Removes ap_ctrl interface
    #pragma HLS INTERFACE ap_ctrl_none port=return

    // Synthesis data flow improvement
    #pragma HLS dataflow

     // used to deal with delay between imgDuplicate1 and imgDuplicate0
     #pragma HLS stream depth=20000 variable=imgDuplicate1.data_stream

    // ----------------------------------------
    //  Convert input stream
    // ----------------------------------------

    hls::AXIvideo2Mat(stream_in, imgInput);

    // ----------------------------------------
    //  Duplicate input stream
    // ----------------------------------------

    hls::Duplicate(imgInput, imgDuplicate0, imgDuplicate1);

    // ----------------------------------------
    //  RGB to Gray
    // ----------------------------------------

    hls::CvtColor<HLS_RGB2GRAY>(imgDuplicate0, imgGray);

    // ----------------------------------------
    //  Harris Corner Filter
    // ----------------------------------------

	float k = 0.04;
	hls::CornerHarris<5,3>(imgGray, harris_resp, k);
	hls::Duplicate(harris_resp,harris_resp0,harris_resp1);
	hls::Scalar<1,float>	s;
	float min_temp = 1.0f;
	float max_temp = 0.0f;


	loop_height1:for(int i = 0; i < rows; i++)
	{
    	loop_width1:for(int j = 0; j < cols; j++)
    	{
			#pragma HLS PIPELINE II=1
			#pragma HLS LOOP_FLATTEN

    		harris_resp1 >> s;

    		if(s.val[0] < min_temp)
    		{
    			min_temp = s.val[0];
    		}
    		if(s.val[0] > max_temp)
    		{
    			max_temp = s.val[0];
    		}
    	}
    }

	int r = (int)((max_temp - min_temp) * 1000);
	float alpha = (255000.0f)/r;
	float beta = (-1)*min_temp*alpha;
	hls::Scale(harris_resp0,imgScale,alpha,beta);

    // ----------------------------------------
    //  Threshold
    // ----------------------------------------

	hls::Threshold(imgScale, imgThres, 40, 255, HLS_THRESH_BINARY);

    // ----------------------------------------
    //  Gray to RGB
    // ----------------------------------------

    hls::CvtColor<HLS_GRAY2RGB>(imgThres, imgRGB);

    // ----------------------------------------
    //  Set Corners Red
    // ----------------------------------------

    setCornersRed<RGB_IMAGE,RGB_PIXEL>(imgRGB, imgDuplicate1, imgOutput, rows, cols);

    // ----------------------------------------
    //  Output image
    // ----------------------------------------

    hls::Mat2AXIvideo(imgOutput, stream_out);
}

// -----------------------------------
//  Set Corners Red
// -----------------------------------

template<typename IMG_T, typename PIXEL_T>
void setCornersRed(IMG_T& img_in0, IMG_T& img_in1, IMG_T& img_out, int rows, int cols) {

	PIXEL_T pin0, pin1;
	PIXEL_T pout;

	L_row: for(int row = 0; row < rows; row++) {
	#pragma HLS LOOP_TRIPCOUNT min=720 max=1080
		L_col: for(int col = 0; col < cols; col++) {
		#pragma HLS LOOP_TRIPCOUNT min=1280 max=1920
		#pragma HLS pipeline rewind

				   img_in0 >> pin0;
				   img_in1 >> pin1;

				   // set pixel red
				   if(pin0.val[0]>200){
						pout.val[0] = 0;
						pout.val[1] = 0;
						pout.val[2] = 255;
				   }
				   // pass through pixel
				   else{
						pout.val[0] = pin1.val[0];
						pout.val[1] = pin1.val[1];
						pout.val[2] = pin1.val[2];
				   }

				   img_out << pout;
				}
		}

}
