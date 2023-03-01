#ifndef TFLITE_TOP_H_
#define TFLITE_TOP_H_

// Expose a C friendly interface for main functions.
#ifdef __cplusplus
extern "C" {
#endif

void setup_tf_system(void);
void evaluate_tf_model(float operand);

#ifdef __cplusplus
}
#endif

#endif