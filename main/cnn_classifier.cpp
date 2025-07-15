#include "cnn_classifier.h"
#include "esp_log.h"

#include "modelo_int8.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

static const char *TAG = "CNN_CLASSIFIER";
constexpr int kTensorArenaSize = 40 * 1024;
static uint8_t tensor_arena[kTensorArenaSize];
int fire_value = 0;

tflite::MicroInterpreter *interpreter = nullptr;
TfLiteTensor *input = nullptr;
TfLiteTensor *output = nullptr;

void init_tflite_model()
{
    const tflite::Model *model = tflite::GetModel(modelo_int8_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        ESP_LOGE(TAG, "Incompatible TFLite model.");
        return;
    }

    static tflite::MicroMutableOpResolver<8> resolver;

    resolver.AddConv2D();
    resolver.AddDepthwiseConv2D();
    resolver.AddFullyConnected();
    resolver.AddSoftmax();
    resolver.AddReshape();
    resolver.AddMaxPool2D();
    resolver.AddMean();
    resolver.AddLogistic();

    static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena, kTensorArenaSize, nullptr);
    interpreter = &static_interpreter;

    if (interpreter->AllocateTensors() != kTfLiteOk)
    {
        ESP_LOGE(TAG, "Error allocating tensors.");
        return;
    }

    input = interpreter->input(0);
    output = interpreter->output(0);

    ESP_LOGI(TAG, "TFLite model initialized successfully.");
}

int run_inference(uint8_t *imageData)
{
    if (!interpreter || !input || !output)
    {
        ESP_LOGE(TAG, "Interpreter not initialized.");
        return 0;
    }

    for (int i = 0; i < 32 * 24; i++)
    {
        float input_float = imageData[i] / 255.0f;
        int val = roundf(input_float / input->params.scale) + input->params.zero_point;
        if (val > 127)
            val = 127;
        if (val < -128)
            val = -128;
        input->data.int8[i] = (int8_t)val;
    }

    if (interpreter->Invoke() != kTfLiteOk)
    {
        ESP_LOGE(TAG, "Error performing inference.");
        return 0;
    }

    int8_t sem_fogo = output->data.int8[0];
    int8_t fogo = output->data.int8[1];

    int predicted = (fogo > sem_fogo) ? 1 : 0;
    ESP_LOGI("TFLite", "Raw output int8 → no fire: %d, fire: %d", sem_fogo, fogo);
    ESP_LOGI("TFLite", "Result: %s\n", predicted ? "FIRE DETECTED" : "NO FIRE");
    return predicted;
}
