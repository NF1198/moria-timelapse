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

#ifndef D1CF5BD8_6FCE_4518_952B_173917229B87
#define D1CF5BD8_6FCE_4518_952B_173917229B87

#include <cmath>
#include <functional>

template <typename T>
class ChangeDetector {
private:
  T v0, v1;
  float pct_threshold;
  std::function<T()> valueProvider;
  std::function<void(float, T, T)> fn;

public:
  ChangeDetector(float pct, std::function<T()> valueProvider, std::function<void(float, T, T)> fn)
      : pct_threshold(pct), 
      valueProvider(valueProvider),
      fn(fn) {}
  ChangeDetector &update() {
    v1 = valueProvider();
    float pct_change = (static_cast<float>((v1 - v0) / v0));
    if (std::abs(pct_change) > pct_threshold) {
      fn(pct_change, v0, v1);
      v0 = v1;
    }
    return *this;
  }
};

#endif /* D1CF5BD8_6FCE_4518_952B_173917229B87 */
