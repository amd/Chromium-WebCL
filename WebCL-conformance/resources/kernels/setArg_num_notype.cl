__kernel void setArgNumNoType(
    float input,
    __global float* output)
{
    int i = get_global_id(0);
    if(i < 10)
        output[i] = input;
}
