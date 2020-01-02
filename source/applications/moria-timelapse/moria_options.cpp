#include "moria_options.h"
#include "moria_options_boost.h"

std::shared_ptr<MoriaOptions> MoriaOptions::create(int argc, char *argv[]) {
  return std::shared_ptr<MoriaOptions>(new MoriaOptionsBoost(argc, argv));
}

MoriaOptions::~MoriaOptions() {}