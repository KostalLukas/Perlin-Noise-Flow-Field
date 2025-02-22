/*
 * Perlin Force Field Animation  v3.0
 * Lukas Kostal, 24.12.2023, BA
 */

 #include <iostream>
 #include <cmath>
 #include <vector>
 #include <fstream>
 #include <opencv2/opencv.hpp>

 // function to perform smoothened interpolation
 float interpolate(float x0, float x1, float w) {
   w = (6 * w * w - 15 * w + 10) * w * w * w;
   return x0 + w * (x1 - x0);
 }

 using namespace std;

 int main() {

   // iteration
   const int img_num = 11;

   // final resolution
   const int x_res = 6000;//10000;
   const int y_res = 6000;//10000;

   // seed value for pseudorandom generator
   const int seed = 2;

   // frequency of Perlin noise
   const float freq = 40;

   const int n_part = 2000000;//2000000;
   const int n_iter = 100;//2000;

   const float step = 20;//2;

   const int lum = 40;//16;

   // calculate increment for random vector grid
   const float inc = sqrt(x_res * x_res + y_res * y_res)  / freq;

  // calculate no of grid vertices to cover entire image
  const int nx_inc = int(x_res / inc) + 2;
  const int ny_inc = int(y_res / inc) + 2;

  // angle for generating vectors in random direction
  float ang;

  // array of random unit vectors at each grid vertex
  float vec_arr[nx_inc][ny_inc][2];

  srand(seed);

  // loop over each grid vertex and generate random unit vector
  for(int i=0; i < nx_inc; i++) {
      for(int j=0; j < ny_inc; j++) {
          // generate random angle
          ang = rand() / static_cast<float>(RAND_MAX) * 2 * M_PI;

          // write components of random unit vector to array
          vec_arr[i][j][0] = cos(ang);
          vec_arr[i][j][1] = sin(ang);
      }
  }

  //float pos_arr[n_part][2];
  vector<vector<float>> pos_arr(n_part, vector<float>(2, 0));

  for(int i=0; i < n_part; i++) {
      pos_arr[i][0] = rand() / static_cast<float>(RAND_MAX) * x_res;
      pos_arr[i][1] = rand() / static_cast<float>(RAND_MAX) * y_res;
  }

  // indices of bottom left corner of each grid cell
  int xi, yi, x0, y0;

  // local coordinates within a grid cell
  float x, y;

  // intermediate interpolated values, final value and its min and max
  float dot0, dot1, dot2, dot3, inter0, inter1;

  vector<uint8_t> img_vec(x_res * y_res, 0);

  for(int i=0; i < n_iter; i++) {
    for(int j=0; j < n_part; j++) {

      xi = int(pos_arr[j][0]);
      yi = int(pos_arr[j][1]);

      x0 = int(xi / inc);
      y0 = int(yi / inc);

      // calculate local cell coordinates
      x = xi / inc - x0;
      y = yi / inc - y0;

      // calculate dot product for each vertex of cell
      dot0 = vec_arr[x0][y0][0] * x + vec_arr[x0][y0][1] * y;
      dot1 = vec_arr[x0][y0 + 1][0] * x + vec_arr[x0][y0 + 1][1] * (y - 1);
      dot2 = vec_arr[x0 + 1][y0][0] * (x - 1) + vec_arr[x0 + 1][y0][1] * y;
      dot3 = vec_arr[x0 + 1][y0 + 1][0] * (x - 1) + vec_arr[x0 + 1][y0 + 1][1] * (y - 1);

      // interpolate dot products
      inter0 = interpolate(dot0, dot1, y);
      inter1 = interpolate(dot2, dot3, y);
      ang = interpolate(inter0, inter1, x);

      ang = (ang + sqrt(0.5)) / (2 * sqrt(0.5)) * 2 * M_PI;

      if ((0 < xi) and (0 < yi) and (xi < x_res) and (yi < y_res)) {

        pos_arr[j][0] += cos(ang) * step;
        pos_arr[j][1] += sin(ang) * step;

        if (img_vec[xi * y_res + yi] + lum < 255) {
          img_vec[xi * y_res + yi] += lum;
        }
      }

    }
  }

  const string img_nam = "img_" + to_string(img_num);

  cv::Mat img = cv::Mat(x_res, y_res, CV_8UC1, img_vec.data());

  cv::namedWindow(img_nam, cv::WINDOW_NORMAL);
  cv::rotate(img, img, cv::ROTATE_90_CLOCKWISE);
  cv::imwrite("../output/" + img_nam + ".tiff", img);
  cv::imshow(img_nam, img);
  cv::waitKey();

  return 0;
 }
