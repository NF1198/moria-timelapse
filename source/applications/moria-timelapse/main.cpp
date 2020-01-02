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

#include "moria_errors.hpp"
#include "moria.h"
#include "moria_options.h"
#include <iostream>

int main(int argc, char *argv[]) {

  try {
    auto options = MoriaOptions::create(argc, argv);
    Moria().run(options);
  } catch (const exit_success &ex) {
    // pass
  } catch (const std::runtime_error &ex) {
    std::cerr << "Runtime exception: " << ex.what() << "\n";
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
