__kernel void image2d_array_type(
        __global float* input,
        image2d_array_t image2dArray,
        __global float* outputFloat,
        __global float* outputImage,
        unsigned int count)
{
    unsigned int i = get_global_id(0);
    if (i < count) {
        outputFloat[i] = input[i];
    }
}
