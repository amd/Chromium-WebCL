__kernel void sample_test(
    __global float *src,
    __global int *dst)
{
    int  tid = get_global_id(0);
    dst[tid] = (int)src[tid];
}

__kernel void sample_test2(
    __global int *src,
    __global float *dst)
{
    int  tid = get_global_id(0);
    dst[tid] = (float)src[tid];
}

