#include "oRGB.h"


void compute_stats(const Mat & input ,Mat & hist, Vec3b & mean, Vec3b & sigma){

  hist = Mat(256,1,CV_8UC3,Scalar(0));

  for (size_t i = 0; i < input.rows; i++) {
    Vec4b * i_ptr_input = input.ptr<Vec4b>(i);
    for (size_t j = 0; j < input.cols; j++) {
      mean.val[0] += i_ptr_input[j].val[0];
      mean.val[1] += i_ptr_input[j].val[1];
      mean.val[2] += i_ptr_input[j].val[2];

      mean.val[0] += i_ptr_input[j].val[0] * i_ptr_input[j].val[0];
      mean.val[1] += i_ptr_input[j].val[1] * i_ptr_input[j].val[1];
      mean.val[2] += i_ptr_input[j].val[2] * i_ptr_input[j].val[2];

      Vec4b * 0_ptr = input.ptr<Vec4b>(i);
      Vec4b * 1_ptr = input.ptr<Vec4b>(i);
      Vec4b * 2_ptr = input.ptr<Vec4b>(i);

      input.ptr<Vec3b>(i_ptr_input[j].val[0])[0] += 1;
      input.ptr<Vec3b>(i_ptr_input[j].val[1])[1] += 1;
      input.ptr<Vec3b>(i_ptr_input[j].val[2])[2] += 1;
    }

  }

  mean.val[0] /= input.rows * input.cols;
  mean.val[1] /= input.rows * input.cols;
  mean.val[2] /= input.rows * input.cols;

  mean.val[0] /= input.rows * input.cols;
  mean.val[1] /= input.rows * input.cols;
  mean.val[2] /= input.rows * input.cols;

  mean.val[0] -= mean.val[0] * mean.val[0];
  mean.val[1] -= mean.val[1] * mean.val[1];
  mean.val[2] -= mean.val[2] * mean.val[2];



}



int main( int argc, char** argv )
{
    Mat input, style;
    if( argc != 3 || !(input=imread(argv[1], IMREAD_COLOR)).data || || !(style=imread(argv[2], IMREAD_COLOR)).data )
        return -1;

    Mat input_oRGB, style_oRGB;

    convert_BGR_to_oRGB(input,input_oRGB);
    convert_BGR_to_oRGB(style,style_oRGB);


    const float* ranges[] = { lranges, cybranges, crgranges };
    Mat histInput, histStyle;




    namedWindow( "Source", 1 );
    imshow( "Source", src );
    namedWindow( "H-S Histogram", 1 );
    imshow( "H-S Histogram", histImg );
    waitKey();
}
