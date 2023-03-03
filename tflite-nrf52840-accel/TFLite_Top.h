#ifndef TFLITE_TOP_H_
#define TFLITE_TOP_H_

// Expose a C friendly interface for main functions.
#ifdef __cplusplus
extern "C" {
#endif

void setup_tf_system(void);
int32_t run_tf_model(float* new_accel_data);

#ifdef __cplusplus
}
#endif

#endif