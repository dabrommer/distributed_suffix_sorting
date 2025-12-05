#include <cstdint>
#include <vector>

#include "util/uint_types.hpp"
#include "util/macros.hpp"
#include "DDCX_lib/DDCX_lib_export.h"


DDCX_LIB_EXPORT std::vector<dsss::UIntPair<uint8_t>> get_sa(int32_t argc, char const* argv[], kamping::Communicator<> comm);