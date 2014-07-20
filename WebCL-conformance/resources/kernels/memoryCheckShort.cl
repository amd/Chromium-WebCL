__kernel void memoryCheckShort(
    __global short* input,
    __global int* output,
    unsigned int count)
{
    unsigned int i;
    for (i = 0; i < count; i++) {
        if (input[i] != 0)
            output[0]++;
    }
}
