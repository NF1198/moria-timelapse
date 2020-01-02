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

#ifndef AFD72442_BDEF_4481_89F5_19CADB0909FA
#define AFD72442_BDEF_4481_89F5_19CADB0909FA

#include <memory>
#include <string>

class MoriaOptions {
public:
  virtual int deviceID() = 0;
  virtual std::string gstPipeline() = 0;
  virtual int apiID() = 0;
  virtual int frameWidth() = 0;
  virtual int frameHeight() = 0;
  virtual float captureFPS() = 0;
  virtual float saveInterval() = 0;
  virtual std::string outDir() = 0;
  virtual float filterPeriod() = 0;
  virtual bool showFps() = 0;
  virtual bool showFpsChange() = 0;
  virtual bool recordImages() = 0;
  virtual bool useUTCtime() = 0;
  virtual bool writeTimestampInImage() = 0;
  virtual bool verbose() = 0;
  virtual u_int flip() = 0;
  virtual bool noGUI() = 0;
  virtual u_int decimate() = 0;

  virtual ~MoriaOptions();

  static std::shared_ptr<MoriaOptions> create(int argc, char *argv[]);
};

#endif /* AFD72442_BDEF_4481_89F5_19CADB0909FA */
