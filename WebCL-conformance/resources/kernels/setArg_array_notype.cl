__kernel void kernel8VFloat(
    const float8 input,
    __global float8* output)
{
    int i = get_global_id(0);
    if(i < 10)
        output[i] = convert_float8(input);
}
