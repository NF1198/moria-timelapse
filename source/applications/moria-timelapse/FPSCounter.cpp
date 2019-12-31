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


#include "FPSCounter.h"
#include <chrono>
#include <iostream>

FPSCounter::FPSCounter()
    : t0(std::chrono::high_resolution_clock::now()), frames(0), _fps(0.0f) {}

FPSCounter::~FPSCounter() {}

FPSCounter &FPSCounter::reset() {
  this->t0 = std::chrono::high_resolution_clock::now();
  this->frames = 0;
  return *this;
}

FPSCounter &FPSCounter::update() {
  this->frames++;
  return *this;
}

float FPSCounter::fps() {
  if (this->frames == 0) {
    return this->_fps;
  }
  auto now = std::chrono::high_resolution_clock::now();
  auto elapsed_time = (std::chrono::nanoseconds(now - this->t0).count()) / 1e9f;
  this->_fps = static_cast<float>(this->frames / elapsed_time);
  this->frames = 0;
  this->t0 = now;
  return this->_fps;
}
