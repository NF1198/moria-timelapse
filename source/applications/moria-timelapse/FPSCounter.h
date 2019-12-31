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

#ifndef BA8FE04C_D06D_48E6_BBDA_42552B2B96FE
#define BA8FE04C_D06D_48E6_BBDA_42552B2B96FE

#include <chrono>
#include <functional>

class FPSCounter {
private:
  std::chrono::time_point<std::chrono::high_resolution_clock> t0;
  int frames;
  float _fps;

public:
  FPSCounter();
  ~FPSCounter();
  FPSCounter &reset();
  FPSCounter &update();
  float fps();
};

#endif /* BA8FE04C_D06D_48E6_BBDA_42552B2B96FE */
