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

#ifndef BF039872_DA9C_40B9_AD7C_DD98041BDB25
#define BF039872_DA9C_40B9_AD7C_DD98041BDB25

#include "moria_options.h"
#include <memory>

class Moria {
public:
  Moria();
  ~Moria();
  void run(std::shared_ptr<MoriaOptions> options);
};


#endif /* BF039872_DA9C_40B9_AD7C_DD98041BDB25 */
