// Copyright (c) 2020 Nicholas Folse
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef C34B2E44_EC72_46CD_B573_F61A40F34B0B
#define C34B2E44_EC72_46CD_B573_F61A40F34B0B

#include <opencv2/videoio.hpp>
#include <utility>

class CameraManager {
private:
  cv::VideoCapture c;

public:
  CameraManager(cv::VideoCapture &&cam);
  CameraManager &open(int deviceId, int apiId);
  CameraManager &set(cv::VideoCaptureProperties prop, double value);
  bool isOpened();
  CameraManager &with_frames(std::function<bool(cv::Mat &frame)> handler);
  ~CameraManager();
};

#endif /* C34B2E44_EC72_46CD_B573_F61A40F34B0B */
