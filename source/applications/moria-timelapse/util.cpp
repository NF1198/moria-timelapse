
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

#include "util.h"
#include "moria-timelapse/moria-timelapse-version.h"
#include <iostream>

#define ENDL "\n"

void print_version() {
  std::cout << "================================================" << ENDL;
  std::cout << MORIA_TIMELAPSE_NAME_VERSION << ENDL;
  std::cout << MORIA_TIMELAPSE_PROJECT_DESCRIPTION << ENDL;
  std::cout << ENDL;
  std::cout << MORIA_TIMELAPSE_AUTHOR_ORGANIZATION << ENDL;
  std::cout << MORIA_TIMELAPSE_AUTHOR_DOMAIN << ENDL;
  std::cout << MORIA_TIMELAPSE_AUTHOR_MAINTAINER << ENDL;
  std::cout << ENDL;
  std::cout << "Copyright (c) 2020 " << MORIA_TIMELAPSE_AUTHOR_ORGANIZATION
            << ENDL;
  std::cout << "Licensed under the " << MORIA_TIMELAPSE_LICENSE << ENDL;
  std::cout << "================================================" << ENDL;
}