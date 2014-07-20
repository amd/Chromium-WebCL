__kernel void test_stream_write_uchar(
    __global uchar *src,
    __global uchar *dst)
{
    int  tid = get_global_id(0);
    dst[tid] = src[tid];
}
