__kernel void test_stream_read_short(
    __global short *dst)
{
    dst[0] = (short)((1<<8)+1);
}
