__kernel void test_stream_read_ulong(
    __global ulong *dst)
{
    dst[0] = ((1UL<<32)+1UL);
}
