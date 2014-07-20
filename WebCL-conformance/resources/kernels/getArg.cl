__kernel void getArg_sample(
    __global float* input,
    __global float* output,
    constant float* count,
    read_only image2d_t webCLImage,
    private sampler_t webCLSampler)
{
    unsigned int i = get_global_id(0);
    output[i] = input[i];
}
