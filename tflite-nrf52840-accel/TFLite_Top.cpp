#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "magic_wand_model_data.h"

#include "gesture_predictor.h"
#include "TFLite_Top.h"
#include "collect_accel_data.h"

#define CHANNEL_NUMBER 3

const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* model_input = nullptr;
int input_length;

// Create an area of memory to use for input, output, and intermediate arrays.
// The size of this will depend on the model you're using, and may need to be determined by experimentation.
constexpr int kTensorArenaSize = 60 * 1024;
alignas(16) uint8_t tensor_arena[kTensorArenaSize];

extern "C" void setup_tf_system(void)
{
   tflite::InitializeTarget(); 

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_magic_wand_model_data);

  if (model->version() != TFLITE_SCHEMA_VERSION) 
  {
    MicroPrintf("Model provided is schema version %d not equal to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will incur some penalty in 
  // code space for op implementations that are not needed by this graph.
  static tflite::MicroMutableOpResolver<5> micro_op_resolver;  // NOLINT
  micro_op_resolver.AddConv2D();
  micro_op_resolver.AddDepthwiseConv2D();
  micro_op_resolver.AddFullyConnected();
  micro_op_resolver.AddMaxPool2D();
  micro_op_resolver.AddSoftmax();

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();

  if (allocate_status != kTfLiteOk) 
  {
    MicroPrintf("AllocateTensors() failed");
    return;
  }

  // Obtain pointer to the model's input tensor.
  model_input = interpreter->input(0);
  if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
      (model_input->dims->data[1] != 128) || (model_input->dims->data[2] != CHANNEL_NUMBER) ||
      (model_input->type != kTfLiteFloat32)) 
  {
    MicroPrintf("Bad input tensor parameters in model");
    return;
  }

  input_length = model_input->bytes / sizeof(float);
}

extern "C" int32_t run_tf_model(float* new_accel_data)
{
    static int num_runs = 0;

    if (update_accel_data(model_input->data.f, new_accel_data, input_length, false))
    {
        // Run inference, and report any error.
        TfLiteStatus invoke_status = interpreter->Invoke();

        //if (invoke_status != kTfLiteOk) 
        //{
        //    MicroPrintf("Invoke failed");
        //    return -1;
        //}

        // Analyze the results to obtain a prediction
        int gesture_index = PredictGesture(interpreter->output(0)->data.f);

        num_runs++;

        MicroPrintf("Runs %d, %d", num_runs, gesture_index);

        return gesture_index;
    }
    else
    {
        return -1;
    }
}

