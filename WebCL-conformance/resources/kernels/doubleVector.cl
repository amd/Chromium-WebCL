#pragma OPENCL EXTENSION cl_khr_fp64
__kernel void kernelDoubleVector(
        double2 input,
        __global double2* output)
{
    output[0] = input;
}
