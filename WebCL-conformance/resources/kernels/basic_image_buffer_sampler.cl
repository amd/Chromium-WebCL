__kernel void basic(
   __global float* input,
   image2d_t imageInput,
   sampler_t samplerInput,
   __global float* outputFloat,
   __global float* outputImage,
   __global float* outputSampler,
   unsigned int count)
{
    unsigned int i = get_global_id(0);
    if (i < count) {
        outputFloat[i] = input[i];
    }
}
