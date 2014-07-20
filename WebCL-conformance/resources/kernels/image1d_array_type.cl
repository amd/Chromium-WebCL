__kernel void image1d_array_type(
        __global float* input,
        image1d_array_t image1dArray,
        __global float* outputFloat,
        __global float* outputImage,
        unsigned int count)
{
    unsigned int i = get_global_id(0);
    if (i < count) {
        outputFloat[i] = input[i];
    }
}
