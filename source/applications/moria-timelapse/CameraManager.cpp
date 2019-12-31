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


#include "CameraManager.h"
#include <iostream>

CameraManager::CameraManager(cv::VideoCapture &&cam) : c(std::move(cam)) {}

CameraManager &CameraManager::open(int deviceId, int apiId) {
  this->c.open(deviceId + apiId);
  return *this;
}

CameraManager &CameraManager::set(cv::VideoCaptureProperties prop,
                                  double value) {
  if (this->c.isOpened()) {
    this->c.set(prop, value);
  } else {
    std::cerr
        << "Warning: Attempted to set property on unopended video capture."
        << "\n";
  }
  return *this;
}

bool CameraManager::isOpened() { return this->c.isOpened(); }

CameraManager &
CameraManager::with_frames(std::function<bool(cv::Mat &frame)> handler) {
  if (!this->isOpened()) {
    return *this;
  }
  for (cv::Mat frame; handler(frame); c >> frame) {
  }
  return *this;
}

CameraManager::~CameraManager() {
  if (c.isOpened()) {
    c.release();
  }
}
