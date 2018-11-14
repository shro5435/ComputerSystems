/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/*
 * Please fill in the following student struct
 */
student_t student = {
    "Shania Roy",     /* First member full name */
    "shro5435@colorado.edu"  /* First member email address */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/*
 * naive_rotate - The naive baseline version of rotate
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst)
{
    int i, j;
    for (i = 0; i < dim; i++){
        for (j = 0; j < dim; j++){
            dst[RIDX(dim-1-j, i, dim)].red   = src[RIDX(i, j, dim)].red;
            dst[RIDX(dim-1-j, i, dim)].green = src[RIDX(i, j, dim)].green;
            dst[RIDX(dim-1-j, i, dim)].blue  = src[RIDX(i, j, dim)].blue;
        }
    }
}

/*
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
 char rotate_descr[] = "rotate: Current working version";
 void rotate(int dim, pixel *src, pixel *dst)
 {
   int i, j;
   int buffer = dim*dim, dim32 = (dim*31), dim32m = (dim*31-1);

    dst += buffer - dim; //code motion-> move code out of the loop as it is an invariant computative variable
    for (i = 0; i < dim; i += 32) //we know it's in multiples of 32 so we can start here... and move in blocks of 32
    {
            for (j = 0; j < dim; j++)
            {
                *dst = *src;            //Directly referencing the addresses via pointers avoids the read method used when handling arrays- huge speedup
                src += dim; //Find source pixel/destination pixel- origin

                *(dst + 1)= *src;       //32 part loop unrolling, combined with 32 loop blocking, allows for us to greatly improve performance/speed.
                src += dim; //shift address of destination pixel and offset location being observed-> blocking

                *(dst + 2)= *src; //Loop unrolling, yay...
                src += dim;       //Use this to reduce computation time by making it ugly

                *(dst + 3)= *src;
                src += dim;

                *(dst + 4)= *src;
                src += dim;

                *(dst + 5)= *src;
                src += dim;

                *(dst + 6)= *src;
                src += dim;

                *(dst + 7)= *src; //Just repeat these steps....continually
                src += dim;

                *(dst + 8)= *src;
                src += dim;

                *(dst + 9)= *src;
                src += dim;

                *(dst + 10)= *src;
                src += dim;

                *(dst + 11)= *src;
                src += dim;

                *(dst + 12)= *src;
                src += dim;

                *(dst + 13)= *src;
                src += dim;

                *(dst + 14)= *src;
                src += dim;

                *(dst + 15)= *src;
                src += dim;

                *(dst + 16)= *src;
                src += dim;

                *(dst + 17)= *src;
                src += dim;

                *(dst + 18)= *src;
                src += dim;

                *(dst + 19)= *src;
                src += dim;

                *(dst + 20)= *src;
                src += dim;

                *(dst + 21)= *src;
                src += dim;

                *(dst + 22)= *src;
                src += dim;

                *(dst + 23)= *src;
                src += dim;

                *(dst + 24)= *src;
                src += dim;

                *(dst + 25)= *src;
                src += dim;

                *(dst + 26)= *src;
                src += dim;

                *(dst + 27)= *src;
                src += dim;

                *(dst + 28)=*src;
                src += dim;

                *(dst +29)=*src;
                src += dim;

                *(dst + 30)=*src;
                src += dim;

                *(dst + 31)=*src;

                dst -= dim;
                src -= dim32m; //We're doing this up to 32 bytes
            }
            dst += 32 + buffer;
            src += dim32;
        }
    return;
 }

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_rotate_functions()
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);
    add_rotate_function(&rotate, rotate_descr);
    /* ... Register additional test functions here */
}


/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* Compute min of two integers */
static int min(int a, int b) {
    if(a<b){
        return a;
    }else{
        return b;
    }
}

/* Compute max of two integers */
static int max(int a, int b) {
    if(a>b){
        return a;
    }else{
        return b;
    }
}

/* A struct used to compute averaged pixel value */
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/*
 * initialize_pixel_sum - Initializes all fields of sum to 0
 */
static void initialize_pixel_sum(pixel_sum *sum){
    sum->red = 0;
    sum->green = 0;
    sum->blue = 0;
    sum->num = 0;
}

static void accumulate_sum(pixel_sum *sum, pixel p){
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
}
/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst)
/*
 * Notes-
 ** This is already partially optimized over truly naive implementation of for (i = 0; i < dim; i++; {for (j = 0; j < dim; j++){dst[RIDX(i, j, dim)] = avg(dim, i, j, src);}}}
 ** To optimize this further, we MUST use loop unrolling to a high degree, as the PDF states this is uses many computations, and must use blocking to an effective degree
*/
{
    int i, j, ii, jj;
    pixel_sum ps;

    for (j = 0; j < dim; j++){
        for (i = 0; i < dim; i++){
            initialize_pixel_sum(&ps);
            for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++){
                for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++){
                    accumulate_sum(&ps, src[RIDX(ii,jj,dim)]);
                }
            }

            dst[RIDX(i, j, dim)].red   = ps.red/ps.num;
            dst[RIDX(i, j, dim)].green = ps.green/ps.num;
            dst[RIDX(i, j, dim)].blue  = ps.blue/ps.num;
        }
    }
}

/*
 * smooth - Your current working version of smooth.
 * IMPORTANT: This is the version you will be graded on
 */
char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst)
{

  /*Now the question is, How do we approach this?
  Well, we could rewrite the functions above, but we know that the function provided
  was already optimized with blocking in mind, using functions. Therefore, to get any kind of
  considerable speed-up we're going to have to disassemble the functions altogther.
  So, Instead, we're going to pull it apart and manually perform the functions.
  */

  int i, j, jj, dimAdd = dim+1, dimsub = dim-1, dimMult = dim*dim;
   //Handling the corners
   dst[0].red = (src[0].red+src[1].red+src[dim].red+src[dimAdd].red)/4; //Moving along local variables
   dst[0].blue = (src[0].blue+src[1].blue+src[dim].blue+src[dimAdd].blue)/4; //All we are really doing is averaging the pixels surrounding the selected area
   dst[0].green = (src[0].green+src[1].green+src[dim].green+src[dimAdd].green)/4;//We do so manually and the challenge is in locating the pixels.

   //Left Corner ^

   i = dim*2-1;
   dst[dimsub].red = (src[dim-2].red+src[dimsub].red+src[i-1].red+src[i].red)/4; //Here we select the right corner, we grab the upper two and the lower two.
   dst[dimsub].blue = (src[dim-2].blue+src[dimsub].blue+src[i-1].blue+src[i].blue)/4; //A picture is just one large array. We can address this as a one or two dimensional array
   dst[dimsub].green = (src[dim-2].green+src[dimsub].green+src[i-1].green+src[i].green)/4; //In this case, it is faster to refer to it via 1 dimension and doing everything the painful mathematic way
   //Right Corner^

   j = dim*(dimsub);
   i = dim*(dim-2);
   dst[j].red = (src[j].red+src[j + 1].red+src[i].red+src[i + 1].red)/4;
   dst[j].blue = (src[j].blue+src[j + 1].blue+src[i].blue+src[i + 1].blue)/4;
   dst[j].green = (src[j].green+src[j + 1].green+src[i].green+src[i + 1].green)/4;
   //Bottom Left Corner^

   j = dimMult-1;
   i = dim*(dimsub)-1;
   dst[j].red = (src[j - 1].red+src[j].red+src[i - 1].red+src[i].red)/4;
   dst[j].blue = (src[j - 1].blue+src[j].blue+src[i - 1].blue+src[i].blue)/4;
   dst[j].green = (src[j - 1].green+src[j].green+src[i - 1].green+src[i].green)/4;
   //Bottom Right Corner^

   //Sides: The sides are not much more complicated than the corners- in this case we're just considering a longer chunk of information- accumulating six pixel-chunks at a time.
   i = dimsub;
   for (j = 1; j < i; j++)
   {
       dst[j].red = (src[j].red+src[j-1].red+src[j+1].red+src[j+dim].red+src[j+dimAdd].red+src[j-1+dim].red)/6;
       dst[j].green = (src[j].green+src[j-1].green+src[j+1].green+src[j+dim].green+src[j+dimAdd].green+src[j-1+dim].green)/6;
       dst[j].blue = (src[j].blue+src[j-1].blue+src[j+1].blue+src[j+dim].blue+src[j+dimAdd].blue+src[j-1+dim].blue)/6;
   }
   //Top Side/Edge/Border^

   i = dimMult-1;
   for (j = i - dim + 2; j < i; j++)
   {
       dst[j].red = (src[j].red+src[j-1].red+src[j+1].red+src[j-dim].red+src[j+1-dim].red+src[j-1-dim].red)/6;
       dst[j].green = (src[j].green+src[j-1].green+src[j+1].green+src[j-dim].green+src[j+1-dim].green+src[j-1-dim].green)/6;
       dst[j].blue = (src[j].blue+src[j-1].blue+src[j+1].blue+src[j-dim].blue+src[j+1-dim].blue+src[j-1-dim].blue)/6;
   }
   //Bottom Side/Border/Edge^

   for (j = dim+dimsub; j < dimMult-1; j+=dim)
    {
       dst[j].red = (src[j].red+src[j-1].red+src[j-dim].red+src[j+dim].red+src[j-dim-1].red+src[j-1+dim].red)/6;
       dst[j].green = (src[j].green+src[j-1].green+src[j-dim].green+src[j+dim].green+src[j-dim-1].green+src[j-1+dim].green)/6;
       dst[j].blue = (src[j].blue+src[j-1].blue+src[j-dim].blue+src[j+dim].blue+src[j-dim-1].blue+src[j-1+dim].blue)/6;
   }
   //Right Side/Border/Edge^

   i = i - (dimsub);
   for (j = dim; j < i; j+=dim)
    {
       dst[j].red = (src[j].red+src[j-dim].red+src[j+1].red+src[j+dim].red+src[j+dimAdd].red+src[j-dim+1].red)/6;
       dst[j].green = (src[j].green+src[j-dim].green+src[j+1].green+src[j+dim].green+src[j+dimAdd].green+src[j-dim+1].green)/6;
       dst[j].blue = (src[j].blue+src[j-dim].blue+src[j+1].blue+src[j+dim].blue+src[j+dimAdd].blue+src[j-dim+1].blue)/6;
   }
   //Left Edge/Border/Side^

   //Center of the image
   jj = dim;
   for (i = 1; i < dim-1; i++)
   {
       for (j = 1; j < dim-1; j++) //This is the only case that's different from the upper attempts, in that, we have to consider nine pixels at a time.
       //We can call this rectangular loop tiling (as is most efficient in today's architectures)
       {
           jj ++;
           dst[jj].red = (src[jj-1].red+src[jj].red+src[jj+1].red+src[jj-dim-1].red+src[jj-dim].red+src[jj-dim+1].red+src[jj+dim-1].red+src[jj+dim].red+src[jj+dim+1].red)/9;
           dst[jj].green = (src[jj-1].green+src[jj].green+src[jj+1].green+src[jj-dim-1].green+src[jj-dim].green+src[jj-dim+1].green+src[jj+dim-1].green+src[jj+dim].green+src[jj+dim+1].green)/9;
           dst[jj].blue = (src[jj-1].blue+src[jj].blue+src[jj+1].blue+src[jj-dim-1].blue+src[jj-dim].blue+src[jj-dim+1].blue+src[jj+dim-1].blue+src[jj+dim].blue+src[jj+dim+1].blue)/9;
       }
       jj += 2;
   }
}


/*********************************************************************
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&smooth, smooth_descr);
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    /* ... Register additional test functions here */
}