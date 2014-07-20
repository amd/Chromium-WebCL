__kernel __attribute__((reqd_work_group_size(10, 10, 10)))
void copy(
    __global float* input,
   __global float* output)
{
    int i = get_global_id(0);
    output[i] = input[i];
}
