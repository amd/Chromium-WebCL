struct date {
    int dd, mm, yyyy;
};

__kernel void kernel_with_struct(
    struct date Mydate,
    __global int* output)
{
    output[0] = Mydate.dd;
    output[1] = Mydate.mm;
    output[2] = Mydate.yyyy;
}
