#include "general_algorithm.h"


int GeneralAlgorithm::MostFreqNum(std::vector<int> &nums) {
  if (nums.empty()) return 0;
  int res = 0, mx = -1;
  std::map<int, int> m;
  for (int &num: nums) {
    m[num]++;
  }
  for (int i = 0; i < m.size(); ++i) {
    if (mx < m[i]) {
      mx = m[i];
      res = i;
    }
  }
  return res;
}

std::vector<std::string> GeneralAlgorithm::Split(const std::string &in_str, char delim) {
  std::vector<std::string> res;
  std::stringstream ss(in_str);
  std::string line;
  while (std::getline(ss, line, delim)) {
    res.push_back(line);
  }
  return res;
}


float GeneralAlgorithm::Cross2d(glm::vec2 v1, glm::vec2 v2) {
  return v1.x * v2.y - v2.x * v1.y;
}

float GeneralAlgorithm::Cross2d(glm::vec3 v1, glm::vec3 v2) {
  return Cross2d(glm::vec2(v1), glm::vec2(v2));
}


bool GeneralAlgorithm::IsInsideTriangle(glm::vec2 &point, std::vector<glm::vec2> &triangle_vertices) {
  if (triangle_vertices.size() < 3) return false;
  auto a = triangle_vertices[0];
  auto b = triangle_vertices[1];
  auto c = triangle_vertices[2];
  auto r1 = Cross2d(c - a, point - a);
  auto r2 = Cross2d(b - c, point - c);
  auto r3 = Cross2d(a - b, point - b);
  if ((r1 >= 0 && r2 >= 0 && r3 >= 0) || (r1 < 0 && r2 < 0 && r3 < 0)) {
    return true;
  }
  return false;
}

std::tuple<float, float, float>
GeneralAlgorithm::ComputeBaryCentric2d(glm::vec2 &point, std::vector<glm::vec2> &triangle_vertices) {
//  std::tuple<float, float, float> res;
  float alpha, beta, gamma;
  auto a = triangle_vertices[0];
  auto b = triangle_vertices[1];
  auto c = triangle_vertices[2];
  auto x = point.x;
  auto y = point.y;

  float div1 = a[0] * (b[1] - c[1]) + (c[0] - b[0]) * a[1] + b[0] * c[1] - c[0] * b[1];
  float div2 = b[0] * (c[1] - a[1]) + (a[0] - c[0]) * b[1] + c[0] * a[1] - a[0] * c[1];
  float div3 = c[0] * (a[1] - b[1]) + (b[0] - a[0]) * c[1] + a[0] * b[1] - b[0] * a[1];

  alpha = (x * (b[1] - c[1]) + (c[0] - b[0]) * y + b[0] * c[1] - c[0] * b[1]) / div1;
  beta = (x * (c[1] - a[1]) + (a[0] - c[0]) * y + c[0] * a[1] - a[0] * c[1]) / div2;
  gamma = (x * (a[1] - b[1]) + (b[0] - a[0]) * y + a[0] * b[1] - b[0] * a[1]) / div3;

  return {alpha, beta, gamma};
}

std::string GeneralAlgorithm::PrintGlmVec3(const glm::vec3 &v) {
  return fmt::format("{} {} {}", v.x, v.y, v.z);
}

std::string GeneralAlgorithm::PrintGlmMat4(const glm::mat4 &m) {
  std::string res;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      res += fmt::format("{} ", m[j][i]);
    }
    res += "\n";
  }
  return res;
}

void GeneralAlgorithm::Snapshot(const std::string &filename, const std::string &output_dir, int width, int height,
                                bool is_rgb) {
  // create output dir if not exists
  if (!std::filesystem::exists(output_dir)) {
    std::filesystem::create_directories(output_dir);
  }
  // create opencv image
  cv::Mat img(height, width, CV_8UC3);
  glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
  // opencv mat: BGR
  glReadPixels(0, 0, img.cols, img.rows, GL_BGR, GL_UNSIGNED_BYTE, img.data);
  // flip image
  cv::Mat flipped;
  // flip top-bottom
  cv::flip(img, flipped, 0);
  // not rgb image, use gray image
  if (!is_rgb) {
    cv::cvtColor(flipped, flipped, cv::COLOR_RGB2GRAY);
  }
  cv::imwrite(output_dir + "\\" + filename, flipped);
  std::cout << output_dir + "\\" + filename << " saved" << std::endl;
}




