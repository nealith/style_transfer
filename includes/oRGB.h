#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

#include <cmath>

#define PI 3.1415926535897932384626433

using namespace cv;

void convert_oRGB_to_BGR(const Mat & input, Mat & output)
{


  output = Mat(input.rows,input.cols,CV_8UC4);
  Mat output_tmp(input.rows,input.cols,CV_64FC3);

  double max0 = 0.0;
  double max1 = 0.0;
  double max2 = 0.0;


  for (unsigned int i = 0; i < input.rows; i++) {
    const Vec4d * i_ptr_input = input.ptr<Vec4d>(i);
    Vec3d * i_ptr_output_tmp = output_tmp.ptr<Vec3d>(i);
    for (unsigned int j = 0; j < input.cols; j++) {

      double teta0 = atan2(i_ptr_input[j].val[2],i_ptr_input[j].val[1]);

      double teta = 0.0;

      if (teta0 < PI/2.0){
        teta = (2.0/3.0) * teta0;
      } else {
        teta = PI/3.0 + (4.0/3.0)*(teta0 - PI/2.0);
      }

      double angle = teta - teta0;

      double C1 = cos(angle)*i_ptr_input[j].val[1] - sin(angle)*i_ptr_input[j].val[2];
      double C2 = sin(angle)*i_ptr_input[j].val[1] + cos(angle)*i_ptr_input[j].val[2];

      i_ptr_output_tmp[j].val[2] = i_ptr_input[j].val[0] + 0.1140 * C1 + 0.7436 * C2;
      if (max2 < i_ptr_output_tmp[j].val[2]) {
        max2 = i_ptr_output_tmp[j].val[2];
      }
      i_ptr_output_tmp[j].val[1] = i_ptr_input[j].val[0] + 0.1140 * C1 - 0.4111 * C2;
      if (max1 < i_ptr_output_tmp[j].val[1]) {
        max1 = i_ptr_output_tmp[j].val[1];
      }
      i_ptr_output_tmp[j].val[0] = i_ptr_input[j].val[0] - 0.8660 * C1 + 0.1663 * C2;
      if (max0 < i_ptr_output_tmp[j].val[0]) {
        max0 = i_ptr_output_tmp[j].val[0];
      }

    }
  }

  for (unsigned int i = 0; i < input.rows; i++) {
    const Vec4d * i_ptr_input = input.ptr<Vec4d>(i);
    const Vec3d * i_ptr_output_tmp = output_tmp.ptr<Vec3d>(i);
    Vec4b * i_ptr_output = output.ptr<Vec4b>(i);
    for (unsigned int j = 0; j < input.cols; j++) {

      i_ptr_output[j].val[0] =  round((i_ptr_output_tmp[j].val[0]/max0)*255);
      i_ptr_output[j].val[1] =  round((i_ptr_output_tmp[j].val[1]/max1)*255);
      i_ptr_output[j].val[2] =  round((i_ptr_output_tmp[j].val[2]/max2)*255);
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
      double L = 0.2990 * ((double)i_ptr_input[j].val[2]/255.0) + 0.5870 * ((double)i_ptr_input[j].val[1]/255.0) + 0.1140 * ((double)i_ptr_input[j].val[0]/255.0);
      double C1 = 0.5000 * (((double)i_ptr_input[j].val[2]/255.0) + ((double)i_ptr_input[j].val[1]/255.0)) - ((double)i_ptr_input[j].val[0]/255.0);
      double C2 = 0.8660 * (((double)i_ptr_input[j].val[2]/255.0) - ((double)i_ptr_input[j].val[1]/255.0));

      double teta = atan2(C2,C1);

      double teta0 = 0.0;

      if (teta < PI/3.0){
        teta0 = (3.0/2.0) * teta;
      } else {
        teta0 = PI/2.0 + (3.0/4.0)*(teta - PI/3.0);
      }

      double angle = teta0 - teta;

      double Cyb = cos(angle)*C1 - sin(angle)*C2;
      double Crg = sin(angle)*C1 + cos(angle)*C2;
      i_ptr_output[j].val[0] = L;
      i_ptr_output[j].val[1] = Cyb;
      i_ptr_output[j].val[2] = Crg;
      i_ptr_output[j].val[3] = i_ptr_input[j].val[3];
    }

  }





}
