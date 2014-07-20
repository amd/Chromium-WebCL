__kernel void image1d_type(
        __global float* input,
        image1d_t image1dInput,
        __global float* outputFloat,
        __global float* outputImage,
        unsigned int count)
{
    unsigned int i = get_global_id(0);
    if (i < count) {
        outputFloat[i] = input[i];
    }
}
