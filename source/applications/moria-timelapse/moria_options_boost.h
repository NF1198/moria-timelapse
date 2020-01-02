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

#ifndef ACD85D0E_F3F5_4E95_BE84_604E9EA841A5
#define ACD85D0E_F3F5_4E95_BE84_604E9EA841A5

#include "moria_options.h"
#include <memory>
#include <string>

class MoriaOptionsBoost : public MoriaOptions {
private:
  int deviceId_;
  std::string gstreamerDevice_;
  int frameWidth_;
  int frameHeight_;
  float captureFPS_;
  float saveInterval_;
  float filterPeriod_;
  std::string outDir_;
  bool showFps_ = false;
  bool showFpsChange_ = false;
  bool useUTC_;
  bool writeTimestamp_;
  bool verbose_;

public:
  virtual int deviceID();
  virtual std::string gstPipeline();
  virtual int apiID();
  virtual int frameWidth();
  virtual int frameHeight();
  virtual float captureFPS();
  virtual float saveInterval();
  virtual std::string outDir();
  virtual float filterPeriod();
  virtual bool showFps();
  virtual bool showFpsChange();
  virtual bool recordImages();
  virtual bool useUTCtime();
  virtual bool writeTimestampInImage();
  virtual bool verbose();

  MoriaOptionsBoost(int argc, char *argv[]);
  virtual ~MoriaOptionsBoost();
};

#endif /* ACD85D0E_F3F5_4E95_BE84_604E9EA841A5 */
