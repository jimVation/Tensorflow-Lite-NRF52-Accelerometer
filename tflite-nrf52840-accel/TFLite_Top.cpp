#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"

#include "TFLite_Top.h"
#include "sine_wave_model_data.h"

const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

constexpr int kTensorArenaSize = 2056;
alignas(16) uint8_t tensor_arena[kTensorArenaSize];

extern "C" void setup_tf_system(void)
{
   tflite::InitializeTarget(); 

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_sine_wave_data);

  if (model->version() != TFLITE_SCHEMA_VERSION) 
  {
    MicroPrintf("Model provided is schema version %d not equal to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // This pulls in all the operation implementations we need.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::AllOpsResolver resolver;

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) 
  {
    MicroPrintf("AllocateTensors() failed");
    return;
  }

  // Obtain pointers to the model's input and output tensors.
  input = interpreter->input(0);
  output = interpreter->output(0);
}

extern "C" void evaluate_tf_model(float operand)
{
  // Place the input in the model's input tensor
  input->data.f[0] = operand;

  // Run inference, and report any error
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) 
  {
    MicroPrintf("Invoke failed on operand: %f\n", static_cast<double>(operand));
    return;
  }

  // Obtain the  output from model's output tensor
  float y = output->data.f[0];

  // Log the current X and Y values
  MicroPrintf("x_value: %f, y_value: %f", operand, y);  
}

