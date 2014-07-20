__kernel void test_stream_write_ulong(
    __global ulong *src,
    __global ulong *dst)
{
    int  tid = get_global_id(0);
    dst[tid] = src[tid];
}
