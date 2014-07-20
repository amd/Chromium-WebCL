#pragma OPENCL EXTENSION cl_khr_fp64
__kernel void kernelDouble(
        double input,
        __global double* output)
{
    output[0] = input;
}
