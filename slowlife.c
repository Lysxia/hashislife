#include <stdlib.h>
#include <stdio.h>

#include "read_gol.h"
#include "sparsemap.h"

int main(int argc, char** argv)
{
    if (argc<2)
    {
	printf("Not enough arguments\n");
        return 1;
    }
    else if (argc>2)
    {
	printf("Too many arguments\n");
	return 1;
    }

    int m,n,**bmp,B[9]={0},S[9]={0};

    if (NULL==(bmp=read_gol(argv[1],&m,&n,B,S)))
    {
	printf("Failure\n");
	return 1;
    }

    Smap s=smp_of_bmp(bmp,m,n);

    printf("Map size : %d %d\n",m,n);
    printf("Occupied lines : %d\n", s.size);
    printf("Queue size : %d\n", s.len);
    print_smap(s,10,-5,10,-5);

    return 0;
}
