__kernel void setArgLocal(
    __global float* input,
    __global float* output,
    __local float* temp,
    const unsigned int count)
{
    int gtid = get_global_id(0);
    int ltid = get_local_id(0);
    if (gtid < count)
    {
        temp[ltid] = input[gtid] + 2;
        output[gtid] = temp[ltid] * temp[ltid];
    }
}

