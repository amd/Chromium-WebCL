__kernel void sample_kernel_code_bad(
    __global float *src,
    __global int *dst)
{
    int  tid = get_global_id(0);thisisanerror,
    dst[tid] = (int)src[tid];
}
