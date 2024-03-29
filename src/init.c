#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "datadef.h"

void load_flag_from_pgm(char **flag, int imax, int jmax, char *filename)
{
    char buf[80];
    char *pix;
    int width, height, max, i, j;
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("Couldn't open file '%s'\n", filename);
        return;
    }
    fscanf(fp, "%s %d %d %d", buf, &width, &height, &max);
    if (strcmp("P5", buf) != 0) {
        printf("'%s' is not a PGM file.\n", filename);
        return;
    }
    if (width < 1 || height < 1 || max < 1) {
        printf("'%s' has invalid headers.\n", filename);
        return;
    }
    pix = malloc(width);
    for (j = 1; j < jmax+2; j++) {
        if (j <= height) {
            fread(pix, width, 1, fp);
        }
        for (i = 1; i < imax+2; i++) {
            if (j >= height+1 || i >= width+1) {
                flag[i][j] = C_F;
            } else {
                if (pix[i-1] == 0) {
                    flag[i][j] = C_B;
                } else {
                    flag[i][j] = C_F;
                }
            }
        }
    }
    free(pix);
    fclose(fp);
}

/* Initialize the flag array, marking any obstacle cells and the edge cells
 * as boundaries. The cells adjacent to boundary cells have their relevant
 * flags set too.
 */
void init_flag(char **flag, int imax, int jmax, float delx, float dely,
    int *ibound)
{
    int i, j;
    float mx, my, x, y, rad1;

    /* Mark a circular obstacle as boundary cells, the rest as fluid */
    mx = 20.0/41.0*jmax*dely; // 2
    my = mx; // 2
    rad1 = 5.0/41.0*jmax*dely; // 0.5

    // printf("mx = %f\n", mx);
    // printf("my = %f\n", my);
    // printf("rad1 = %f\n", rad1);

    // exit(0);

    for (i=1;i<=imax;i++) {
        for (j=1;j<=jmax;j++) {
            x = (i-0.5)*delx - mx;
            y = (j-0.5)*dely - my;
            flag[i][j] = (x*x + y*y <= rad1*rad1)?C_B:C_F;
        }
    }
    
    /* Mark the north & south boundary cells */
    for (i=0; i<=imax+1; i++) {
        flag[i][0]      = C_B;
        flag[i][jmax+1] = C_B;
    }
    /* Mark the east and west boundary cells */
    for (j=1; j<=jmax; j++) {
        flag[0][j]      = C_B;
        flag[imax+1][j] = C_B;
    }

    /* flags for boundary cells */
    *ibound = 0;
    for (i=1; i<=imax; i++) {
        for (j=1; j<=jmax; j++) {
            if (!(flag[i][j] & C_F)) {
                (*ibound)++;
                if (flag[i-1][j] & C_F) flag[i][j] |= B_W; // flag[i][j] = flag[i][j] | B_W 0004
                if (flag[i+1][j] & C_F) flag[i][j] |= B_E; // flag[i][j] = flag[i][j] | B_E 0008
                if (flag[i][j-1] & C_F) flag[i][j] |= B_S; // flag[i][j] = flag[i][j] | B_S 0002
                if (flag[i][j+1] & C_F) flag[i][j] |= B_N; // flag[i][j] = flag[i][j] | B_N 0001
            }
        }
    }

// printf("%d\n", *ibound);
// // print all flags 
//     for (i=0;i<=imax+1;i++) {
//         for (j=0;j<=jmax+1;j++) {
//             printf("%x ", flag[i][j]);
//         }
//         printf("\n");
//     }
//     exit(1);
}
