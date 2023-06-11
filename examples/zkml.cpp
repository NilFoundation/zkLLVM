#include <nil/crypto3/algebra/curves/pallas.hpp>

using namespace nil::crypto3::algebra::curves;

[[circuit]] typename pallas::base_field_type::value_type
    zkml_example(typename pallas::base_field_type::value_type unused) {

    std::size_t step_size = 2;
    float** input_layer;
    float** mask;
    void** output_layer_v = __builtin_assigner_zkml_convolution(reinterpret_cast<void**>(input_layer), reinterpret_cast<void**>(mask), step_size);
    float** output_layer = reinterpret_cast<float**>(output_layer);

    std::size_t pool_size = 3;
    void** input_pooled_v = __builtin_assigner_zkml_pooling(reinterpret_cast<void**>(input_layer), pool_size);
    float** input_pooled = reinterpret_cast<float**>(input_pooled_v);

    float fa;
    float fb = __builtin_assigner_zkml_ReLU(fa);

    void** input_normalized_v = __builtin_assigner_zkml_batch_norm(reinterpret_cast<void**>(input_layer));
    float** input_normalized = reinterpret_cast<float**>(input_normalized_v);


    return unused;
}