__kernel void sample_copy_kernel(
    __global int *src,
    __global int *dst)
{
    int  tid = get_global_id(0);
    dst[tid] = src[tid];
}

