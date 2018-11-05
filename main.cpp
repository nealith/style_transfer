#include "oRGB.h"


void createAlphaImage(const cv::Mat& mat, cv::Mat & dst)
{
  std::vector<cv::Mat> matChannels;
  cv::split(mat, matChannels);

  // create alpha channel
  cv::Mat alpha = Mat(mat.rows,mat.cols,CV_8UC1,Scalar(0));
  matChannels.push_back(alpha);

  cv::merge(matChannels, dst);
}

void search_in_hist(const Mat & hist,int canal,int val, unsigned int & i_min, unsigned int & i_max)
{

  if (i_min != i_max && i_min != i_max - 1){
    unsigned int i_middle = (i_max - i_min) / 2 + i_min;
    int v_middle = hist.ptr<Vec3i>(i_middle)[0].val[canal];
    if (val == v_middle){
      i_min = i_middle;
      i_max = i_middle;
    } else {
      int v_min = hist.ptr<Vec3i>(i_min)[0].val[canal];
      int v_max = hist.ptr<Vec3i>(i_max)[0].val[canal];
      if (v_min < val && val < v_middle){
        i_max = i_middle;
      } else {
        i_min = i_middle;
      }
      search_in_hist(hist,canal,val,i_min,i_max);
    }
  }
}

void compute_stats(const Mat & input ,Mat & hist, Mat & c_hist, Vec3d & mean, Vec3d & sigma){

  hist = Mat(256,1,CV_32SC3,Scalar(0));
  c_hist = Mat(256,1,CV_32SC3,Scalar(0));

  for (unsigned int i = 0; i < input.rows; i++) {
    const Vec4d * i_ptr_input = input.ptr<Vec4d>(i);
    for (unsigned int j = 0; j < input.cols; j++) {
      mean.val[0] += i_ptr_input[j].val[0];
      mean.val[1] += i_ptr_input[j].val[1];
      mean.val[2] += i_ptr_input[j].val[2];

      sigma.val[0] += i_ptr_input[j].val[0] * i_ptr_input[j].val[0];
      sigma.val[1] += i_ptr_input[j].val[1] * i_ptr_input[j].val[1];
      sigma.val[2] += i_ptr_input[j].val[2] * i_ptr_input[j].val[2];

      hist.ptr<Vec3i>(i_ptr_input[j].val[0])[0].val[0] += 1;
      hist.ptr<Vec3i>(i_ptr_input[j].val[1])[0].val[1] += 1;
      hist.ptr<Vec3i>(i_ptr_input[j].val[2])[0].val[2] += 1;
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

  c_hist.ptr<Vec3i>(0)[0] = hist.ptr<Vec3i>(0)[0];
  for (unsigned int i = 1; i < 256; i++) {
    c_hist.ptr<Vec3i>(i)[0] = c_hist.ptr<Vec3i>(i-1)[0] + hist.ptr<Vec3i>(i)[0];
  }

}



int main( int argc, char** argv )
{
    Mat input_, input, style_, style, output;
    if( argc != 3 || !(input_=imread(argv[1], IMREAD_UNCHANGED)).data || !(style_=imread(argv[2], IMREAD_UNCHANGED)).data )
        return -1;

        /*std::vector<cv::Mat> matChannels;
        cv::split(input_, matChannels);

        namedWindow( "lol", 1 );
        imshow( "lol", matChannels[0] );

        namedWindow( "lol2", 1 );
        imshow( "lol2", matChannels[1] );

        namedWindow( "lol3", 1 );
        imshow( "lol3", matChannels[2] );*/

    if(input_.channels() < 4){
      createAlphaImage(input_,input);
    } else {
      input = input_;
    }

    if(style_.channels() < 4){
      createAlphaImage(style_,style);
    } else {
      style = style_;
    }

    //namedWindow( "Input", 1 );
    //imshow( "Input", input );
    //namedWindow( "style", 1 );
    //imshow( "style", style );
    //waitKey();

    output = Mat(input.rows,input.cols,CV_8UC4);

    Mat input_oRGB, style_oRGB, output_oRGB;

    convert_BGR_to_oRGB(input,input_oRGB);
    convert_BGR_to_oRGB(style,style_oRGB);

    Mat histInput, histStyle, c_histInput, c_histStyle;
    Vec3d input_means,style_means,input_sigma, style_sigma;

    compute_stats(input_oRGB,histInput,c_histInput,input_means,input_sigma);
    compute_stats(style_oRGB,histStyle,c_histStyle,style_means,style_sigma);

    output_oRGB = Mat(input.rows,input.cols,CV_64FC4);
    Mat hist_map(256,1,CV_8UC1,Scalar(0));



    for (unsigned int i = 0; i < 256; i++) {
      int val_input = c_histInput.ptr<Vec3i>(i)[0].val[0];
      unsigned int i_min = 0;
      unsigned int i_max = 255;
      search_in_hist(c_histStyle,0,val_input,i_min,i_max);
      hist_map.ptr<unsigned char>(i)[0] = (unsigned char) i_max;
    }

    for (unsigned int i = 0; i < input.rows; i++) {
      Vec4d * i_ptr_input = input_oRGB.ptr<Vec4d>(i);
      Vec4d * i_ptr_output = output_oRGB.ptr<Vec4d>(i);
      for (unsigned int j = 0; j < input.cols; j++) {
        i_ptr_output[j].val[0] = hist_map.ptr<unsigned char>((int)i_ptr_input[j].val[0])[0];
        i_ptr_output[j].val[1] = (i_ptr_input[j].val[1] - input_means.val[1])*(style_sigma.val[1]/input_sigma.val[1]) + style_means.val[1];
        i_ptr_output[j].val[2] = (i_ptr_input[j].val[2] - input_means.val[2])*(style_sigma.val[2]/input_sigma.val[2]) + style_means.val[2];
        i_ptr_output[j].val[3] = i_ptr_input[j].val[3];
      }

    }



    convert_oRGB_to_BGR(output_oRGB,output);

    std::cout << "test" << '\n';
    //namedWindow( "output", 1 );
    //imshow( "output", output );
    //waitKey();

    imwrite("output.png",output);

    return 0;
}
