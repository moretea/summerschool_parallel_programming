#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void put_pixel(int red, int green, int blue)  {
    fputc((char)red,stdout);
    fputc((char)green,stdout);
    fputc((char)blue,stdout);
}

int main()
{
    double x,xx,y,cx,cy;
    const int itermax = 10000;      /* how many iterations to do */
    const double magnify=2.0;     /* no magnification          */
    const int hxres = 1000;        /* horizonal resolution      */
    const int hyres = 1000;        /* vertical resolution       */
    int iteration;

    /* header for PPM output */
    printf("P6\n");
    printf("%d %d\n255\n",hxres,hyres);

    for (int hy=1; hy<=hyres; hy++)  {
        for (int hx=1; hx<=hxres; hx++)  {
            cx = (((float)hx)/((float)hxres)-0.5)/magnify*3.0-0.7;
            cy = (((float)hy)/((float)hyres)-0.5)/magnify*3.0;
            x = 0.0;
            y = 0.0;
            for (iteration=1; iteration < itermax; iteration++) {
                xx = x*x-y*y+cx;
                y = 2.0*x*y+cy;
                x = xx;
                if (x*x+y*y>100.0) {
                    iteration = 999999;
                }
            }
            if (iteration<99999) {
                put_pixel(0,255,255);
            } else {
                put_pixel(180,0,0);
            }
        }
    }

    return 0;
}
