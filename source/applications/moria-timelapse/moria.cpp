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

#include "moria.h"
#include "CameraManager.h"
#include "ChangeDetector.hpp"
#include "FPSCounter.h"
#include "IIR_2nd_temporal_filter.hpp"
#include "IntervalTimer.h"
#include "butterworth_2nd_IIR_params.hpp"
#include "util.h"
#include <algorithm>
#include <chrono>
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

#define ENDL "\n"

Moria::Moria() {}

Moria::~Moria() {}

void Moria::run(std::shared_ptr<MoriaOptions> options) {
  float filterPeriod = options->filterPeriod();
  bool showFps = options->showFps() || options->verbose();
  bool showFpsChange = options->showFpsChange() || options->verbose();
  float saveInterval = options->saveInterval();
  bool recordImages = options->recordImages();
  bool useUTCtime = options->useUTCtime();
  bool writeTimestampInImage = options->writeTimestampInImage();
  std::string outDir = options->outDir();
  bool verbose = options->verbose();
  u_int flip = std::min(3u, std::max(0u, options->flip()));
  bool noGUI = options->noGUI();
  u_int decimate = std::max(1u, options->decimate());

  // font for time text
  int fontFace = cv::FONT_HERSHEY_PLAIN;
  double fontScale = 1;
  int thickness = 1;

  // print usage info
  print_version();
  if (!noGUI) {
    std::cout << "Press 'q' key to exit" << ENDL;
    std::cout << "Press 'f' key to toggle FPS display" << ENDL;
    std::cout << "Press 'c' key to toggle FPS change display" << ENDL;
    std::cout << "Press 'r' key to reset frame" << ENDL;
    std::cout << "Press 't' key to toggle timestamp" << ENDL;
    std::cout << "Press 'u' key to toggle UTC timestamp" << ENDL;
    std::cout << "Press 'v' key to toggle verbose display" << ENDL;
    std::cout << "Press '[' key to decrease filter period" << ENDL;
    std::cout << "Press ']' key to increase filter period" << ENDL;
  }

  if (!options->recordImages()) {
    std::cerr << ENDL;
    std::cerr
        << "Warning: No output directory specified. Images will not be saved."
        << ENDL;
    std::cerr << ENDL;
  }

  // create butterworth filter parameters
  Butterworth2ndOrderIIRFilterParams<float> filterParams(
      1.0 / filterPeriod, // cut-off freq.
      1);                 // sample rate (FPS)

  // initialize IIR filters; one for each video channel
  IIR_2nd_temporal_filter<float> filter[3];

  // try to initialize output directory
  if (recordImages && !cv::utils::fs::exists(outDir)) {
    cv::utils::fs::createDirectories(outDir);
    if (!cv::utils::fs::isDirectory(outDir)) {
      std::stringstream errs("Moria: Output path is not a directory (");
      errs << outDir << ")";
      throw std::runtime_error(errs.str());
    }
    if (verbose) {
      std::cerr << "Initialized output directory: " << outDir << ENDL;
    }
  }

  //--- Initialize VideoCapture
  CameraManager cap{cv::VideoCapture()};
  try {
    cap.configure(options);
  } catch (...) {
    throw std::runtime_error("Moria: Error configuring camera");
  }

  // check if we succeeded
  if (!cap.isOpened()) {
    throw std::runtime_error("Moria: Unable to open camera");
  }

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
                    << ENDL;
          if (verbose) {
            std::cerr << "new filterParams: {gain: " << filterParams.gain()
                      << ", B1: " << filterParams.B1()
                      << ", B2: " << filterParams.B2()
                      << ", fc: " << filterParams.passband()
                      << ", fs: " << filterParams.samplerate() << "}" << ENDL;
          }
        }
      });

  // Timers
  IntervalTimer fps_printer{
      std::chrono::seconds{5}, [&](std::chrono::nanoseconds elapsed) {
        (void)elapsed;
        if (showFps)
          std::cerr << "fps: " << fpscounter.fps() << ENDL;
      }};

  auto imprint_timestamp = [&](cv::Mat &frame) {
    auto timestamp = std::time(nullptr);
    auto timestamp_to_print =
        useUTCtime ? std::gmtime(&timestamp) : std::localtime(&timestamp);
    std::stringstream timeText;
    timeText << std::put_time(timestamp_to_print, "%a %b %e, %Y %T %Z");

    int baseline = 0;
    cv::Size textSize = cv::getTextSize(timeText.str(), fontFace, fontScale,
                                        thickness, &baseline);

    // center the text
    cv::Point textOrg((frame.cols - textSize.width) / 2,
                      (frame.rows - textSize.height - 2));

    cv::putText(frame, timeText.str(), textOrg, fontFace, fontScale,
                cv::Scalar::all(255), thickness, cv::LINE_AA);
  };

  // Frame buffers
  cv::Mat floatFrame;
  cv::Mat frameChannels[3];
  cv::Mat outFloatFrame;
  cv::Mat outFrame;

  std::vector<int> compression_params;
  compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
  compression_params.push_back(95);

  IntervalTimer image_writer{
      std::chrono::milliseconds{static_cast<int64_t>(saveInterval * 1000)},
      [&](std::chrono::nanoseconds elapsed) {
        (void)elapsed;
        if (recordImages && cv::utils::fs::exists(outDir) &&
            cv::utils::fs::isDirectory(outDir)) {
          std::stringstream imgDir, imgName;

          auto now_chrono{std::chrono::system_clock::now()};
          auto since_epoch{now_chrono.time_since_epoch()};
          auto count_since_epoch = since_epoch.count();
          auto seconds_since_epoch{(count_since_epoch / 1000000000UL) *
                                   1000000000UL};
          auto nanoseconds_since_epoch{count_since_epoch - seconds_since_epoch};

          auto timestamp = std::chrono::system_clock::to_time_t(now_chrono);
          auto timestamp_to_print =
              useUTCtime ? std::gmtime(&timestamp) : std::localtime(&timestamp);

          imgDir << std::put_time(timestamp_to_print, "%Y-%m-%d");
          imgName << std::put_time(timestamp_to_print, "%Y-%m-%d_%H-%M-%S")
                  << "-"
                  << std::setw(3) << std::setfill('0')
                  << static_cast<int>(nanoseconds_since_epoch / 1.0e9 * 1000)
                  << ".jpg";

          auto imgOutPath = cv::utils::fs::join(outDir, imgDir.str());
          auto imgOutFilePath = cv::utils::fs::join(imgOutPath, imgName.str());

          cv::utils::fs::createDirectories(imgOutPath);

          cv::imwrite(imgOutFilePath, outFrame, compression_params);
          if (verbose) {
            std::cerr << "save image: " << imgOutFilePath << ENDL;
          }
        }
      }};

  int empty_frames = 0;
  int decimate_counter = 0;

  //--- GRAB AND WRITE LOOP
  cap.with_frames([&](cv::Mat &frame) {
    if (decimate_counter-- == 0) {
      decimate_counter = decimate;

      if (frame.empty()) {
        decimate_counter = 0; // ensure we capture the next valid frame
        if (verbose) {
          std::cerr << "Empty frame!\n";
        }
        empty_frames++;
        if (empty_frames > 10) {
          throw std::runtime_error("Moria: encountered too many empty frames.");
        }
        return true; // continue capture
      }

      fpscounter.update();
      fps_printer.update();
      fpsChangeDetector.update();

      if (flip) {
        switch (flip) {
        case 1:
          cv::flip(frame, frame, 1);
          break;
        case 2:
          cv::flip(frame, frame, 0);
          break;
        case 3:
          cv::flip(frame, frame, -1);
          break;
        default:
          break;
        }
      }

      // apply low pass filter to frame channels
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
        imprint_timestamp(outFrame);
      }

      image_writer.update();
    }

    if (!noGUI) {
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
          if (showFps)
            fps_printer.reset();
          break;
        case 99: /*c*/
          showFpsChange = !showFpsChange;
          break;
        case 93: /*]*/
          filterPeriod *= 1.25f;
          filterPeriod = std::round(filterPeriod * 10) / 10;
          filterParams.passband(1.0 / filterPeriod);
          std::cerr << "filter time: " << filterPeriod << " seconds." << ENDL;
          if (verbose) {
            std::cerr << "new filterParams: {gain: " << filterParams.gain()
                      << ", B1: " << filterParams.B1()
                      << ", B2: " << filterParams.B2()
                      << ", fc: " << filterParams.passband()
                      << ", fs: " << filterParams.samplerate() << "}" << ENDL;
          }
          break;
        case 91: /*[*/
          filterPeriod /= 1.25f;
          filterPeriod = std::round(filterPeriod * 10) / 10;
          filterParams.passband(1.0 / filterPeriod);
          std::cerr << "filter time: " << filterPeriod << " seconds." << ENDL;
          if (verbose) {
            std::cerr << "new filterParams: {gain: " << filterParams.gain()
                      << ", B1: " << filterParams.B1()
                      << ", B2: " << filterParams.B2()
                      << ", fc: " << filterParams.passband()
                      << ", fs: " << filterParams.samplerate() << "}" << ENDL;
          }
          break;
        case 118: /*v*/
          verbose = !verbose;
          break;
        case 116: /*t*/
          writeTimestampInImage = !writeTimestampInImage;
          break;
        case 117: /*u*/
          useUTCtime = !useUTCtime;
          break;
        case 92: /*\*/
          flip++;
          flip %= 4;
          break;
        default:
          std::cout << "key: " << keyCode << ENDL;
          break;
        }
      }

      // show live and wait for a key with timeout long enough to show images
      if (!outFrame.empty()) {
        imshow("Live", outFrame);
      } else if (verbose) {
        std::cerr << "Moria: unable to display empty frame." << ENDL;
      }
    }

    return true;
  });
}
