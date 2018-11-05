#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

#include <cmath>

#define PI 3.1415926535897932384626433

using namespace cv;

void convert_oRGB_to_BGR(const Mat & input, Mat & output)
{

  output = Mat(input.rows,input.cols,CV_8UC4);

  for (unsigned int i = 0; i < input.rows; i++) {
    const Vec4d * i_ptr_input = input.ptr<Vec4d>(i);
    Vec4b * i_ptr_output = output.ptr<Vec4b>(i);
    for (unsigned int j = 0; j < input.cols; j++) {

      double teta = atan2(i_ptr_input[j].val[2],i_ptr_input[j].val[1]);

      double teta0 = 0.0;

      if (teta < PI/2.0){
        teta0 = (2/3.0) * teta;
      } else {
        teta0 = PI/3.0 + (4/3.0)*(teta - PI/2.0);
      }

      double angle = teta0 - teta;

      double C1 = cos(angle)*i_ptr_input[j].val[1] - sin(angle)*i_ptr_input[j].val[2];
      double C2 = sin(angle)*i_ptr_input[j].val[1] + cos(angle)*i_ptr_input[j].val[2];

      int R = i_ptr_input[j].val[0] + 0.1140 * C1 + 0.7436 * C2;
      int G = i_ptr_input[j].val[0] + 0.1140 * C1 - 0.4111 * C2;
      int B = i_ptr_input[j].val[0] - 0.8660 * C1 + 0.1663 * C2;



      i_ptr_output[j].val[0] =  (unsigned char) B;
      i_ptr_output[j].val[1] =  (unsigned char) G;
      i_ptr_output[j].val[2] =  (unsigned char) R;
      i_ptr_output[j].val[3] =  (unsigned char) i_ptr_input[j].val[3];
    }

  }


}

void convert_BGR_to_oRGB(const Mat & input, Mat & output)
{
  output = Mat(input.rows,input.cols,CV_64FC4);

  for (unsigned int i = 0; i < input.rows; i++) {
    const Vec4b * i_ptr_input = input.ptr<Vec4b>(i);
    Vec4d * i_ptr_output = output.ptr<Vec4d>(i);
    for (unsigned int j = 0; j < input.cols; j++) {
      int L = 0.2990 * i_ptr_input[j].val[2] + 0.5870 * i_ptr_input[j].val[1] + 0.1140 * i_ptr_input[j].val[0];
      double C1 = 0.5000 * i_ptr_input[j].val[2] + 0.5000 * i_ptr_input[j].val[1] - 1.0 * i_ptr_input[j].val[0];
      double C2 = 0.8660 * i_ptr_input[j].val[2] - 0.8660 * i_ptr_input[j].val[1] + 0 * i_ptr_input[j].val[0];

      double teta = atan2(C2,C1);

      double teta0 = 0.0;

      if (teta < PI/3.0){
        teta0 = (3/2.0) * teta;
      } else {
        teta0 = PI/2.0 + (3/4.0)*(teta - PI/3.0);
      }

      double angle = teta0 - teta;

      int Cyb = cos(angle)*C1 - sin(angle)*C2;
      int Crg = sin(angle)*C1 + cos(angle)*C2;
      i_ptr_output[j].val[0] = L;
      i_ptr_output[j].val[1] = Cyb;
      i_ptr_output[j].val[2] = Crg;
      i_ptr_output[j].val[3] = i_ptr_input[j].val[3];
    }

  }





}
