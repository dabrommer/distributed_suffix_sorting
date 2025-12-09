#pragma once

#include "DDCX_lib/DDCX_lib_export.h"
#include <cstdint>
#include <vector>

#include "util/macros.hpp"
#include "util/uint_types.hpp"


using char_type = uint8_t;

DDCX_LIB_EXPORT std::vector<dsss::UIntPair<uint8_t>>& get_sa(int32_t argc, char const* argv[], kamping::Communicator<> comm);

DDCX_LIB_EXPORT std::vector<char_type>& get_local_string();

DDCX_LIB_EXPORT size_t get_text_size();

DDCX_LIB_EXPORT std::string get_input_path();