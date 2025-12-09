#include <cstdint>
#include <vector>

#include "util/uint_types.hpp"
#include "util/macros.hpp"
#include "DDCX_lib/DDCX_lib_export.h"


using char_type = uint8_t;

DDCX_LIB_EXPORT std::vector<dsss::UIntPair<uint8_t>>& get_sa(int32_t argc, char const* argv[], kamping::Communicator<> comm);

DDCX_LIB_EXPORT std::vector<char_type>& get_local_string();

DDCX_LIB_EXPORT size_t get_text_size();