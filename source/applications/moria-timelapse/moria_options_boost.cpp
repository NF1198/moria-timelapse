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

#include "moria_options_boost.h"
#include "moria-timelapse/moria-timelapse-version.h"
#include "moria_errors.hpp"
#include "util.h"
#include <boost/program_options.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <opencv2/videoio.hpp>

#ifdef __unix__
#define DEFAULT_CAPTURE cv::CAP_V4L2
#elif defined(_WIN32) || defined(WIN32)
#define DEFAULT_CAPTURE cv::CAP_VFW
#else
#define DEFUALT_CAPTURE cv::CAP_ANY
#endif

#define ENDL "\n";

namespace po = boost::program_options;

MoriaOptionsBoost::MoriaOptionsBoost(int argc, char *argv[]) {
  std::string config_file;

  // command-line only options
  po::options_description generic("Generic options");
  generic.add_options()("version", "print version string")(
      "help", "show help message")("config,c",
                                   po::value<std::string>(&config_file),
                                   "name of configuration file.");

  // command-line and config-file options
  po::options_description config("Configuration");
  config.add_options()("device,d", po::value<int>(&deviceId_)->default_value(0),
                       "default device ID (uses system default backend, e.g. "
                       "v4l2 on *nix or vfw on win32)");
  config.add_options()("gst", po::value<std::string>(&gstreamerDevice_),
                       "gstreamer pipeline (will be used instead of deviceID, "
                       "if specified); pipeline must end with \"! appsink\"");
  config.add_options()("width,w",
                       po::value<int>(&frameWidth_)->default_value(320),
                       "frame width (ignored if using gst pipeline)");
  config.add_options()("height,h",
                       po::value<int>(&frameHeight_)->default_value(240),
                       "frame height (ignored if using gst pipeline)");
  config.add_options()(
      "fps", po::value<float>(&captureFPS_)->default_value(10),
      "capture frames per second (target; ignored if using gst pipeline)");
  config.add_options()("save-interval",
                       po::value<float>(&saveInterval_)->default_value(10),
                       "interval (seconds) at which frames are saved to disk");
  config.add_options()("filter-period",
                       po::value<float>(&filterPeriod_)->default_value(1),
                       "virtual shutter speed (seconds)");
  config.add_options()("output,O", po::value<std::string>(&outDir_),
                       "output directory");
  config.add_options()("utc", po::value<bool>(&useUTC_)->default_value(false),
                       "use UTC timestamps");
  config.add_options()("timestamp",
                       po::value<bool>(&writeTimestamp_)->default_value(true),
                       "write timestamp in frame");
  config.add_options()(
      "flip,f", po::value<u_int>(&flip_)->default_value(0),
      "flip frame {0: no flip, 1: horizontal, 2: vertical, 3: both}");
  config.add_options()("noGUI", po::bool_switch(&noGUI_), "don't show the GUI");
  config.add_options()(
      "decimate", po::value<u_int>(&decimate_)->default_value(1),
      "only capture 1:N frames (useful if framerate is set by camera)");
  config.add_options()("verbose,v", po::bool_switch(&verbose_),
                       "verbose output");

  // hidden options
  po::options_description hidden("Hidden options");

  // command-line options
  po::options_description cmdline_options;
  cmdline_options.add(generic).add(config).add(hidden);

  // config file options
  po::options_description config_file_options;
  config_file_options.add(config).add(hidden);

  // visible options
  po::options_description visible("Allowed options");
  visible.add(generic).add(config);

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, cmdline_options), vm);
  po::notify(vm);

  if (vm.count("config")) {
    std::ifstream ifs(config_file.c_str());
    if (!ifs) {
      throw std::runtime_error("Can not open config file.");
    } else {
      store(po::parse_config_file(ifs, config_file_options), vm);
      notify(vm);
    }
  }

  if (vm.count("help")) {
    std::cout << "================================================" << ENDL;
    std::cout << MORIA_TIMELAPSE_NAME_VERSION << ENDL;
    std::cout << MORIA_TIMELAPSE_PROJECT_DESCRIPTION << ENDL;
    std::cout << "================================================" << ENDL;
    std::cout << ENDL;
    std::cout << visible << "\n";
    std::cout << "\nRun with GST_DEBUG=3 for additional info.\n";
    throw exit_success();
  }

  if (vm.count("version")) {
    print_version();
    throw exit_success();
  }
}

MoriaOptionsBoost::~MoriaOptionsBoost() {}
int MoriaOptionsBoost::deviceID() { return deviceId_; }
int MoriaOptionsBoost::apiID() { return DEFAULT_CAPTURE; }
std::string MoriaOptionsBoost::gstPipeline() { return gstreamerDevice_; }
int MoriaOptionsBoost::frameWidth() { return frameWidth_; }
int MoriaOptionsBoost::frameHeight() { return frameHeight_; }
float MoriaOptionsBoost::captureFPS() { return captureFPS_; }
float MoriaOptionsBoost::saveInterval() { return saveInterval_; }
std::string MoriaOptionsBoost::outDir() { return outDir_; }
float MoriaOptionsBoost::filterPeriod() { return filterPeriod_; }
bool MoriaOptionsBoost::showFps() { return showFps_; }
bool MoriaOptionsBoost::showFpsChange() { return showFpsChange_; }
bool MoriaOptionsBoost::recordImages() { return !outDir().empty(); }
bool MoriaOptionsBoost::useUTCtime() { return useUTC_; }
bool MoriaOptionsBoost::writeTimestampInImage() { return writeTimestamp_; }
bool MoriaOptionsBoost::verbose() { return verbose_; }
u_int MoriaOptionsBoost::decimate() { return decimate_; }
bool MoriaOptionsBoost::noGUI() { return noGUI_; }
u_int MoriaOptionsBoost::flip() { return flip_; }
