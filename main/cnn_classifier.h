#ifndef CNN_CLASSIFIER_H
#define CNN_CLASSIFIER_H

#include <stdint.h> 

#ifdef __cplusplus
extern "C" {
#endif

void init_tflite_model();

int run_inference(uint8_t* imageData);


#ifdef __cplusplus
}
#endif

#endif 