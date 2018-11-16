#include "oRGB.h"

#include <map>
#include <list>
#include <vector>

void createAlphaImage(const cv::Mat& mat, cv::Mat & dst)
{
  std::vector<cv::Mat> matChannels;
  cv::split(mat, matChannels);

  // create alpha channel
  cv::Mat alpha = Mat(mat.rows,mat.cols,CV_8UC1,Scalar(255));
  matChannels.push_back(alpha);

  cv::merge(matChannels, dst);
}

void match_luminance(const std::vector<std::pair<double,double>> & cdf, double val, unsigned int & i_min, unsigned int & i_max)
{
  if (i_min != i_max && i_min != i_max - 1){
    unsigned int i_middle = ((i_max - i_min) / 2.0) + i_min;
    double v_middle = cdf[i_middle].second;
    if (val == v_middle){
      i_min = i_middle;
      i_max = i_middle;
    } else {
      double v_min = cdf[i_min].second;
      double v_max = cdf[i_max].second;
      if (v_min <= val && val < v_middle){
        i_max = i_middle;
      } else {
        i_min = i_middle;
      }
      match_luminance(cdf,val,i_min,i_max);
    }
  }
}

bool compare_keys(std::pair<double,double> first,std::pair<double,double> second){
  return (first.first < second.first);
}

bool compare_values(std::pair<double,double> first,std::pair<double,double> second){
  return (first.second < second.second);
}

void compute_transfer_data(
  const Mat & oRGB_input,
  std::vector<std::pair<double,double>> & cumulative_distribution_function,
  double & mean_c1,
  double & mean_c2,
  double & std_c1,
  double & std_c2)
{
  cumulative_distribution_function = std::vector<std::pair<double,double>>();
  std::map<double,unsigned int> hist;
  mean_c1 = 0.0;
  mean_c2 = 0.0;
  std_c1 = 0.0;
  std_c2 = 0.0;
  for (unsigned int i = 0; i < oRGB_input.rows; i++) {
    const Vec4d * i_ptr_input = oRGB_input.ptr<Vec4d>(i);
    for (unsigned int j = 0; j < oRGB_input.cols; j++) {
      mean_c1 += i_ptr_input[j].val[1];
      mean_c2 += i_ptr_input[j].val[2];
      std_c1 += i_ptr_input[j].val[1] * i_ptr_input[j].val[1];
      std_c2 += i_ptr_input[j].val[2] * i_ptr_input[j].val[2];

      if (hist.count(i_ptr_input[j].val[0]) == 0) {
        hist[i_ptr_input[j].val[0]] = 1;
      } else {
        hist[i_ptr_input[j].val[0]] += 1;
      }
    }

  }

  std::list<std::pair<double,double>> distribution_function;

  for (std::map<double,unsigned int>::iterator it=hist.begin(); it != hist.end(); it++) {
    std::pair<double,double> kv = std::pair<double,double>(it->first,(double)it->second/(double)(oRGB_input.rows * oRGB_input.cols));
    distribution_function.push_front(kv);
  }
  hist.clear();

  distribution_function.sort(compare_keys);
  std::pair<double,double> last = std::pair<double,double>(-1.0,-1.0);
  std::pair<double,double> current;
  for (std::list<std::pair<double,double>>::iterator it=distribution_function.begin(); it != distribution_function.end(); it++) {
    if (last.first != -1.0) {
      current = (*it);
      current.second += last.second;
    }
    last = current;
    cumulative_distribution_function.push_back(last);
  }

  mean_c1 /= oRGB_input.rows * oRGB_input.cols;
  mean_c2 /= oRGB_input.rows * oRGB_input.cols;
  std_c1 -= mean_c1 * mean_c1;
  std_c2 -= mean_c2 * mean_c2;
  std_c1 = sqrt(std_c1);
  std_c2 = sqrt(std_c2);
}




int main( int argc, char** argv )
{
    Mat input_, input, style_, style, output;
    if( argc != 3 || !(input_=imread(argv[1], IMREAD_UNCHANGED)).data || !(style_=imread(argv[2], IMREAD_UNCHANGED)).data )
        return -1;

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

    Mat input_oRGB, style_oRGB, output_oRGB;

    convert_BGR_to_oRGB(input,input_oRGB);
    convert_BGR_to_oRGB(style,style_oRGB);


    std::vector<std::pair<double,double>> input_cumulative_distribution_function;
    double input_mean_c1;
    double input_mean_c2;
    double input_std_c1;
    double input_std_c2;

    std::cout << "compute transfer data for input..." << '\n';
    compute_transfer_data(
      input_oRGB,
      input_cumulative_distribution_function,
      input_mean_c1,
      input_mean_c2,
      input_std_c1,
      input_std_c2
    );

    std::vector<std::pair<double,double>> style_cumulative_distribution_function;
    double style_mean_c1;
    double style_mean_c2;
    double style_std_c1;
    double style_std_c2;

    std::cout << "compute transfer data for style..." << '\n';
    compute_transfer_data(
      style_oRGB,
      style_cumulative_distribution_function,
      style_mean_c1,
      style_mean_c2,
      style_std_c1,
      style_std_c2
    );

    std::cout << "begin histogramm matching..." << '\n';

    std::map<double,double> match;

    for (std::vector<std::pair<double,double>>::iterator it=input_cumulative_distribution_function.begin(); it != input_cumulative_distribution_function.end(); it++) {
      unsigned int min = 0;
      unsigned int max = style_cumulative_distribution_function.size();

      match_luminance(style_cumulative_distribution_function,it->second,min,max);

      double match_key = -1.0;

      if (min == max ) {
        match_key = style_cumulative_distribution_function[max].first;
      } else {
        double dmin = it->second - style_cumulative_distribution_function[min].second;
        double dmax = style_cumulative_distribution_function[max].second - it->second;

        match_key = (style_cumulative_distribution_function[min].first*dmin + style_cumulative_distribution_function[max].first*dmax)/(dmin+dmax);
      }

      match[it->first] = match_key;
    }

    std::cout << "begin output oRGB..." << '\n';

    output_oRGB = Mat(input.rows,input.cols,CV_64FC4);

    double max_1 = 0;
    double max_2 = 0;

    for (unsigned int i = 0; i < input_oRGB.rows; i++) {
      Vec4d * i_ptr_input = input_oRGB.ptr<Vec4d>(i);
      Vec4d * i_ptr_output = output_oRGB.ptr<Vec4d>(i);
      for (unsigned int j = 0; j < input_oRGB.cols; j++) {
        i_ptr_output[j].val[0] = match[i_ptr_input[j].val[0]]; // i_ptr_input[j].val[0];
        i_ptr_output[j].val[1] = (i_ptr_input[j].val[1] - input_mean_c1)*(style_std_c1/input_std_c1) + style_mean_c1;
        if (max_1 < i_ptr_output[j].val[1]) {
          max_1 = i_ptr_output[j].val[1];
        }
        i_ptr_output[j].val[2] = (i_ptr_input[j].val[2] - input_mean_c2)*(style_std_c2/input_std_c2) + style_mean_c2;
        if (max_2 < i_ptr_output[j].val[2]) {
          max_2 = i_ptr_output[j].val[2];
        }
        i_ptr_output[j].val[3] = i_ptr_input[j].val[3];
      }

    }

    if (max_1 > 1 || max_2 > 1) {
      std::cout << "ooooooooooo" << '\n';
    }

    std::cout << "convert to RGBA..." << '\n';

    convert_oRGB_to_BGR(output_oRGB,output);



    std::cout << "Writing image in output.png...." << '\n';

    bool result = false;
    try
    {
        result = imwrite("output.png", output);
    }
    catch (const cv::Exception& ex)
    {
        fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
    }
    if (result)
        printf("Saved PNG file with alpha data.\n");
    else
        printf("ERROR: Can't save PNG file.\n");
    return result ? 0 : 1;
}
