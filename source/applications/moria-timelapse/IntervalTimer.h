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

#ifndef AFA27D69_9611_4472_A8B5_6D7C9FC3825C
#define AFA27D69_9611_4472_A8B5_6D7C9FC3825C

#include <chrono>
#include <functional>

class IntervalTimer {
private:
  std::chrono::nanoseconds interval;
  std::chrono::time_point<std::chrono::high_resolution_clock> nextT, lastT;
  std::function<void(std::chrono::nanoseconds)> fn;

public:
  IntervalTimer(std::chrono::nanoseconds interval,
                std::function<void(std::chrono::nanoseconds)> fn);

  IntervalTimer &update();
  IntervalTimer &reset();
};

#endif /* AFA27D69_9611_4472_A8B5_6D7C9FC3825C */
