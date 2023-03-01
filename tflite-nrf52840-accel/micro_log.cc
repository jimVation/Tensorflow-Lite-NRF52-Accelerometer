/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/lite/micro/micro_log.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include <cstdarg>
#include <cstdint>
#include <new>

#if !defined(TF_LITE_STRIP_ERROR_STRINGS)
#include "tensorflow/lite/micro/debug_log.h"
#include "tensorflow/lite/micro/micro_string.h"
#endif

#define BUFFER_SIZE 50

void Log(const char* format, va_list args) 
{
  char log_buffer[BUFFER_SIZE];
  //vsnprintf(log_buffer, BUFFER_SIZE, format, args);
  MicroVsnprintf(log_buffer, BUFFER_SIZE, format, args);
  NRF_LOG_INFO("%s", log_buffer);
  NRF_LOG_FLUSH();
}

#if !defined(TF_LITE_STRIP_ERROR_STRINGS)
void MicroPrintf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  Log(format, args);
  va_end(args);
}
#endif
