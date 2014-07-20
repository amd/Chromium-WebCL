__kernel void test_stream_write_uint(
    __global uint *src,
    __global uint *dst)
{
    int  tid = get_global_id(0);
    dst[tid] = src[tid];
}
