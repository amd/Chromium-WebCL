__kernel void options_test_kernel(
    __global float *src,
    __global int *dst)
{
    size_t tid = get_global_id(0);
    dst[tid] = src[tid];
}
