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

#ifndef A1D38A46_F2DF_4D0E_9162_CD837FD2E34F
#define A1D38A46_F2DF_4D0E_9162_CD837FD2E34F

#include "butterworth_2nd_IIR_params.h"
#include <iostream>
#include <opencv2/core.hpp>

#define NZEROS 2
#define NPOLES 2

template <typename T> class IIR_2nd_temporal_filter {
private:
  cv::Mat X[NZEROS + 1], Y[NPOLES + 1];
  cv::Mat tempB, tempC;

  void check_init(const cv::Mat &ref, const T &gain) {
    for (int idx = 1; idx < NZEROS + 1; idx++) {
      if (X[idx].size() != ref.size() || X[idx].type() != ref.type()) {
        ref.copyTo(X[idx]);
        X[idx] /= gain;
      }
    }
    for (int idx = 1; idx < NPOLES + 1; idx++) {
      if (Y[idx].size() != ref.size() || Y[idx].type() != ref.type()) {
        ref.copyTo(Y[idx]);
      }
    }
  }

public:
  IIR_2nd_temporal_filter() {}
  IIR_2nd_temporal_filter &apply(Butterworth2ndOrderIIRFilterParams<T> &params,
                                 const cv::Mat &frame) {                        
    check_init(frame, params.gain());

    // rotate inputs array
    cv::Mat tempX = X[0];
    X[0] = X[1];
    X[1] = X[2];
    X[2] = tempX;

    // rotate outputs array
    cv::Mat tempY = Y[0];
    Y[0] = Y[1];
    Y[1] = Y[2];
    Y[2] = tempY;

    // apply input
    frame.copyTo(X[2]);
    X[2] /= params.gain();

    // compute output frame
    Y[0].copyTo(tempB);
    tempB *= params.B2();
    Y[1].copyTo(tempC);
    tempC *= params.B1();
    X[1].copyTo(Y[2]);
    Y[2] *= 2;
    Y[2] += X[0];
    Y[2] += X[2];
    Y[2] += tempB;
    Y[2] += tempC;

    return *this;
  }

  IIR_2nd_temporal_filter &reset(const T &gain, const cv::Mat &ref) {
    for (int idx = 1; idx < NZEROS + 1; idx++) {
      ref.copyTo(X[idx]);
      X[idx] /= gain;
    }
    for (int idx = 1; idx < NPOLES + 1; idx++) {
      ref.copyTo(Y[idx]);
    }
    return *this;
  }

  IIR_2nd_temporal_filter &resetgain(const T &old_gain, const T &new_gain) {
    if (old_gain != 0 && !std::isnan(old_gain)) {
      T gain_raio = old_gain / new_gain;
      X[0] *= gain_raio;
      X[1] *= gain_raio;
      X[2] *= gain_raio;
      // std::cerr << "baked: {from: " << old_gain << ", to: " << new_gain << ",
      // ratio: " << gain_raio << "}\n";
    }
    return *this;
  }

  const cv::Mat &value() { return Y[2]; }
};

#endif /* A1D38A46_F2DF_4D0E_9162_CD837FD2E34F */
