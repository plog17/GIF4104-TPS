__kernel
void convolve(__global float* input, __global float* output){

    int i = get_global_id(0);
    int j = get_global_id(1);

    int p = get_global_size(0);
    int q = get_global_size(1);

     //Identification of work-item
    int idX = get_local_id(0);
    int idY = get_local_id(1);

    //printf("\ni, j = %d, %d\n", i, j);
    //printf("p, q = %d, %d\n", p, q);
    //printf("idX, idY = %d, %d\n", idX, idY);
    output[0] = p;
    output[1] = q;
}
