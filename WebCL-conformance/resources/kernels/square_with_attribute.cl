__kernel __attribute__((reqd_work_group_size(1,1,1))) void square_with_attribute(
        __global float* input,
        __global float* output,
        const unsigned int count)
{
    int i = get_global_id(0);
    if (i < count)
        output[i] = input[i] * input[i];
}
