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

#ifndef F0CCC341_BBC3_43F0_A73F_39BAA61BCEED
#define F0CCC341_BBC3_43F0_A73F_39BAA61BCEED

template <typename T> class Butterworth2ndOrderIIRFilterParams {
private:
  T passband_;
  T samplerate_;
  T gain_;
  T b1_;
  T b2_;
  void calculate_filter_parameters();

public:
  Butterworth2ndOrderIIRFilterParams(T passband, T rate);

  Butterworth2ndOrderIIRFilterParams<T> &passband(T value);
  Butterworth2ndOrderIIRFilterParams<T> &samplerate(T rate);

  T passband();
  T samplerate();
  T gain();
  T B1();
  T B2();
};

#endif /* F0CCC341_BBC3_43F0_A73F_39BAA61BCEED */
