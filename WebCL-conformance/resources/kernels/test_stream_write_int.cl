__kernel void test_stream_write_int(
    __global int *src,
    __global int *dst)
{
    int  tid = get_global_id(0);
    dst[tid] = src[tid];
}
