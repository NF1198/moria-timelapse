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
#include "moria_options.h"
#include <iostream>
#include <memory>
#include <opencv2/core/version.hpp>
#include <sstream>

#define ENDL "\n"

CameraManager::CameraManager(cv::VideoCapture &&cam) : c(std::move(cam)) {}

void CameraManager::configure(std::shared_ptr<MoriaOptions> options) {
  if (options->gstPipeline().empty()) {
    c.open(options->deviceID(), options->apiID());
    c.set(cv::CAP_PROP_FRAME_WIDTH, options->frameWidth());
    c.set(cv::CAP_PROP_FRAME_HEIGHT, options->frameHeight());
    c.set(cv::CAP_PROP_FPS, options->captureFPS());
  } else {
    c.open(options->gstPipeline());
  }
  if (options->verbose()) {
    int fourcc = c.get(cv::CAP_PROP_FOURCC);
#ifdef CV_VERSION_MAJOR && CV_VERSION_MINOR && CV_VERSION_REVISION
#if CV_VERSION_MAJOR == 3 && CV_VERSION_MINOR == 4 && CV_VERSION_REVISION >= 4 || CV_VERSION_MAJOR >= 4
    std::cerr << "Opened camera using " << c.getBackendName() << " backend."
              << ENDL;
#endif
#endif
    if (options->gstPipeline().empty()) {
      std::cerr << "Camera deviceID: " << options->deviceID() << ENDL;
    } else {
      std::cerr << "gstreamer pipeline: \"" << options->gstPipeline() << "\""
                << ENDL;
    }
    std::cerr << "frame width:  " << c.get(cv::CAP_PROP_FRAME_WIDTH) << ENDL;
    std::cerr << "frame height: " << c.get(cv::CAP_PROP_FRAME_HEIGHT) << ENDL;
    std::cerr << "FPS:          " << c.get(cv::CAP_PROP_FPS) << ENDL;
    if (fourcc)
      std::cerr << "FOURCC:       " << static_cast<char>(fourcc)
                << static_cast<char>(fourcc >> 8)
                << static_cast<char>(fourcc >> 16)
                << static_cast<char>(fourcc >> 24) << ENDL;
  }
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
    throw std::runtime_error("Moria: camera not open.");
  }
  for (cv::Mat frame; handler(frame);) {
    try {
      c >> frame;
    } catch (const std::exception &ex) {
      std::stringstream what("Moria: Error grabbing frame. ");
      what << ex.what();
      throw std::runtime_error(what.str());
    }
  }
  return *this;
}

CameraManager::~CameraManager() {
  if (c.isOpened()) {
    c.release();
  }
}
