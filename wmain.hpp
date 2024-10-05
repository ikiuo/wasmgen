/*
 * wmain.hpp
 */
#pragma once

#ifndef USE_WMAIN
#  if defined(_WIN32) || defined(_WIN64)
#    define USE_WMAIN  1
#    define main main_utf8
#  else
#    define USE_WMAIN  0
#  endif
#endif
