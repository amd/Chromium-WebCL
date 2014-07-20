__kernel void image1d_buffer_type(
        __global float* input,
        image1d_buffer_t image1dBuffer,
        __global float* outputFloat,
        __global float* outputImage,
        unsigned int count)
{
    unsigned int i = get_global_id(0);
    if (i < count) {
        outputFloat[i] = input[i];
    }
}
