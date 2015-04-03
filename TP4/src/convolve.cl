__kernel
void convolve(__global unsigned char* inputImage, __global double* inputFilter, __global unsigned char* output, const int filterSize){

    int x = get_global_id(0)+2;
    int y = get_global_id(1)+2;

    int p = get_global_size(0);
    int r = get_global_size(1);

     //Identification of work-item
    int idX = get_local_id(0);
    int idY = get_local_id(1);

<<<<<<< HEAD
    //printf("\ni, j = %d, %d\n", i, j);
    //printf("p, q = %d, %d\n", p, q);
    //printf("idX, idY = %d, %d\n", idX, idY);
    output[0] = p;
    output[1] = q;
}
=======
    int filterHalfSize = filterSize/2;

    int lR = 0.;
    int lG = 0.;
    int lB = 0.;


    for (int j = -filterHalfSize; j <= filterHalfSize; j++) {
        int fy = j + filterHalfSize;
        for (int i = -filterHalfSize; i <= filterHalfSize; i++) {
            int fx = i + filterHalfSize;
            //printf("%d\n", fx + fy*filterHalfSize);
            //printf("%f\n", inputFilter[3]);
            lR += convert_double(inputImage[(y + j)*973*4 + (x + i)*4]) * inputFilter[fx + fy*filterHalfSize];
            lG += convert_double(inputImage[(y + j)*973*4 + (x + i)*4 + 1]) * inputFilter[fx + fy*filterHalfSize];
            lB += convert_double(inputImage[(y + j)*973*4 + (x + i)*4 + 2]) * inputFilter[fx + fy*filterHalfSize];
        }
    }
    output[y*973*4 + x*4] = convert_char(lR);
    output[y*973*4 + x*4 + 1] = convert_char(lG);
    output[y*973*4 + x*4 + 2] = convert_char(lB);
    output[y*973*4 + x*4 + 3] = 255;
}
>>>>>>> f07fd2c733836abbcd12a87e43e8a702c20a015f
