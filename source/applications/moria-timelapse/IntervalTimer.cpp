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

#include "IntervalTimer.h"
#include <cmath>

IntervalTimer::IntervalTimer(std::chrono::nanoseconds interval,
                             std::function<void(std::chrono::nanoseconds)> fn)
    : interval(interval), nextT(std::chrono::high_resolution_clock::now()),
      lastT(std::chrono::high_resolution_clock::now()), fn(fn) {}

IntervalTimer &IntervalTimer::update() {
  auto before = std::chrono::high_resolution_clock::now();
  if (before >= this->nextT) {
    this->fn(before - this->lastT);
    this->lastT = before;
    auto after = std::chrono::high_resolution_clock::now();
    this->nextT += std::chrono::nanoseconds{
        static_cast<long>(this->interval.count() *
                          std::ceil((after - nextT).count() /
                                    static_cast<float>(interval.count())))};
  }
  return *this;
}

IntervalTimer &IntervalTimer::reset() {
  auto before = std::chrono::high_resolution_clock::now();
  this->fn(before - this->lastT);
  this->lastT = before;
  auto after = std::chrono::high_resolution_clock::now();
  this->nextT += std::chrono::nanoseconds{
      static_cast<long>(this->interval.count() *
                        std::ceil((after - nextT).count() /
                                  static_cast<float>(interval.count())))};
  return *this;
}