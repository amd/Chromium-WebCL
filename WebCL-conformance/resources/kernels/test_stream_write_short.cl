__kernel void test_stream_write_short(
    __global short *src,
    __global short *dst)
{
    int  tid = get_global_id(0);
    dst[tid] = src[tid];
}
