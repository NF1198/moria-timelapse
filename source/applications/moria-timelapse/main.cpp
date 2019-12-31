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
#include "ChangeDetector.hpp"
#include "FPSCounter.h"
#include "IIR_2nd_temporal_filter.hpp"
#include "IntervalTimer.h"
#include "butterworth_2nd_IIR_params.hpp"
#include <cmath>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/core/utils/filesystem.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <sstream>
#include <string>
#include <vector>

#define endl "\n"

int main(int, char **) {

  int deviceID = 2;        // 0 = open default camera
  int apiID = cv::CAP_ANY; // 0 = autodetect default API
  int frameWidth = 960;  // 1600
  int frameHeight = 700;  // 1200
  float captureFPS = 5.0f;  // camera acquisition FPS target
  float recordPeriod = 60; // image record interval
  std::string outDir{"/tmp/moria"};
  float filterPeriod = 300; // temporal filter seconds
  bool showFps = true;
  bool showFpsChange = true;
  bool recordImages = true;
  bool useUTCtime = false;
  bool writeTimestampInImage = true;

  // font for image text
  int fontFace = cv::FONT_HERSHEY_PLAIN;
  double fontScale = 1;
  int thickness = 1;

  // try to initialize output directory
  if (!cv::utils::fs::exists(outDir)) {
    cv::utils::fs::createDirectories(outDir);
    if (!cv::utils::fs::isDirectory(outDir)) {
      std::cerr << "Output path is not a directory.\n";
      return -1;
    }
  }

  //--- INITIALIZE VIDEOCAPTURE
  cv::VideoCapture camera;

  camera.open(deviceID, apiID);

  // check if we succeeded
  if (!camera.isOpened()) {
    std::cerr << "ERROR! Unable to open camera\n";
    return -1;
  }

  // setup camera
  // cap.set(cv::CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
  camera.set(cv::CAP_PROP_FRAME_WIDTH, frameWidth);
  camera.set(cv::CAP_PROP_FRAME_HEIGHT, frameHeight);
  camera.set(cv::CAP_PROP_FPS, captureFPS);

  CameraManager cap{std::move(camera)};

  // State
  Butterworth2ndOrderIIRFilterParams<float> filterParams(1.0 / filterPeriod,
                                                         1); // 30 fps
  IIR_2nd_temporal_filter<float> filter[3];

  // FPS Counter
  FPSCounter fpscounter;

  // FPS Change Detector
  ChangeDetector<float> fpsChangeDetector(
      0.12f, /*threshhold pct*/
      [&]() { return fpscounter.fps(); },
      [&](auto pct_ch, auto from, auto to) {
        (void)pct_ch;
        auto old_gain = filterParams.gain();
        filterParams.samplerate(static_cast<float>(to));
        filter[0].resetgain(old_gain, filterParams.gain());
        filter[1].resetgain(old_gain, filterParams.gain());
        filter[2].resetgain(old_gain, filterParams.gain());
        if (showFpsChange) {
          std::cerr << "fps changed: {from: " << from << ", to: " << to << "}"
                    << endl;
          // std::cerr << "new filterParams: {gain: " << filterParams.gain()
          //           << ", B1: " << filterParams.B1()
          //           << ", B2: " << filterParams.B2()
          //           << ", fc: " << filterParams.passband()
          //           << ", fs: " << filterParams.samplerate() << "}" << endl;
        }
      });

  // Timers
  IntervalTimer fps_printer{
      std::chrono::seconds{10}, [&](std::chrono::nanoseconds elapsed) {
        (void)elapsed;
        if (showFps)
          std::cerr << "fps: " << fpscounter.fps() << endl;
      }};

  cv::Mat floatFrame;
  cv::Mat frameChannels[3];
  cv::Mat outFloatFrame;
  cv::Mat outFrame;

  std::vector<int> compression_params;
  compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
  compression_params.push_back(95);

  IntervalTimer image_writer{
      std::chrono::milliseconds{static_cast<int64_t>(recordPeriod * 1000)},
      [&](std::chrono::nanoseconds elapsed) {
        (void)elapsed;
        if (recordImages && cv::utils::fs::exists(outDir) &&
            cv::utils::fs::isDirectory(outDir)) {
          std::stringstream imgDir, imgName;
          auto timestamp = std::time(nullptr);
          auto timestamp_to_print =
              useUTCtime ? std::gmtime(&timestamp) : std::localtime(&timestamp);

          imgDir << std::put_time(timestamp_to_print, "%Y-%m-%d");
          imgName << std::put_time(timestamp_to_print, "%Y-%m-%d_%H-%M-%S")
                  << ".jpg";

          auto imgOutPath = cv::utils::fs::join(outDir, imgDir.str());
          auto imgOutFilePath = cv::utils::fs::join(imgOutPath, imgName.str());

          cv::utils::fs::createDirectories(imgOutPath);

          cv::imwrite(imgOutFilePath, outFrame, compression_params);
        }
      }};

  std::cout << "Press 'q' key to terminate" << endl;

  //--- GRAB AND WRITE LOOP
  cap.with_frames([&](cv::Mat &frame) {
    if (frame.empty()) {
      std::cerr << "Empty frame!\n";
      return true;
    }

    fpscounter.update();
    fps_printer.update();
    fpsChangeDetector.update();

    cv::cvtColor(frame, frame, cv::COLOR_RGB2XYZ);
    frame.convertTo(floatFrame, CV_32FC3, 1.0 / 255.0);
    cv::split(floatFrame, frameChannels);
    filter[0].apply(filterParams, frameChannels[0]);
    filter[1].apply(filterParams, frameChannels[1]);
    filter[2].apply(filterParams, frameChannels[2]);
    cv::Mat filteredlayers[] = {filter[0].value(), filter[1].value(),
                                filter[2].value()};
    cv::merge(filteredlayers, 3, outFloatFrame);
    outFloatFrame.convertTo(outFrame, CV_8UC3, 255.0);
    cv::cvtColor(outFrame, outFrame, cv::COLOR_XYZ2RGB);

    if (writeTimestampInImage) {
      auto timestamp = std::time(nullptr);
      auto timestamp_to_print =
          useUTCtime ? std::gmtime(&timestamp) : std::localtime(&timestamp);
      std::stringstream timeText;
      timeText << std::put_time(timestamp_to_print, "%a %b %e, %Y %T %Z");

      int baseline = 0;
      cv::Size textSize = cv::getTextSize(timeText.str(), fontFace, fontScale,
                                          thickness, &baseline);

      // center the text
      cv::Point textOrg((outFrame.cols - textSize.width) / 2,
                                      (outFrame.rows - textSize.height - 2));

      cv::putText(outFrame, timeText.str(), textOrg, fontFace, fontScale, cv::Scalar::all(255),
              thickness, CV_AA);
    }

    image_writer.update();

    int keyCode = cv::waitKey(5);
    if (keyCode >= 0) {
      switch (keyCode) {
      case 114: /*r*/
        filter[0].reset(filterParams.gain(), frameChannels[0]);
        filter[1].reset(filterParams.gain(), frameChannels[1]);
        filter[2].reset(filterParams.gain(), frameChannels[2]);
        break;
      case 113: /*q*/
        return false;
        break;
      case 102: /*f*/
        showFps = !showFps;
        break;
      case 99: /*c*/
        showFpsChange = !showFpsChange;
        break;
      case 93: /*]*/
        filterPeriod *= 1.1f;
        filterPeriod = std::round(filterPeriod * 10) / 10;
        filterParams.passband(1.0 / filterPeriod);
        std::cerr << "filter time: " << filterPeriod << " seconds." << endl;
        // std::cerr << "new filterParams: {gain: " << filterParams.gain()
        //           << ", B1: " << filterParams.B1()
        //           << ", B2: " << filterParams.B2()
        //           << ", fc: " << filterParams.passband()
        //           << ", fs: " << filterParams.samplerate() << "}" << endl;
        break;
      case 91: /*[*/
        filterPeriod /= 1.1f;
        filterPeriod = std::round(filterPeriod * 10) / 10;
        filterParams.passband(1.0 / filterPeriod);
        std::cerr << "filter time: " << filterPeriod << " seconds." << endl;
        // std::cerr << "new filterParams: {gain: " << filterParams.gain()
        //           << ", B1: " << filterParams.B1()
        //           << ", B2: " << filterParams.B2()
        //           << ", fc: " << filterParams.passband()
        //           << ", fs: " << filterParams.samplerate() << "}" << endl;
        break;
      default:
        std::cout << "key: " << keyCode << endl;
        break;
      }
    }

    // show live and wait for a key with timeout long enough to show images
    imshow("Live", outFrame);

    return true;
  });

  return 0;
}
