#include<stdio.h>
#include<stdlib.h>

int set_9(int k, int B[9])
{
    if (k<0 || k>8)
    {
        return 1;
    }
    B[k] = 1;
    return 0;
}

int** read_gol(char* filename, int* m, int* n, int B[9], int S[9])
{
    FILE *file = fopen(filename, "r");
    int **life, i, j;
    int a, b;
    char c;

    if (file == NULL)
    {
	printf("Failed to open file '%s'", filename);
	return NULL;
    }

    //map size
    fscanf(file, "%d %d ", m, n);
    a=*m;
    b=*n;

    //B rule
    fscanf(file, "B%c", &c);
    
    while (c != '/')
    {
	if (set_9(c-'0',B))
	{
	    printf("Bad B rule\n");
	    return NULL;
	}
	fscanf(file, "%c", &c);
    }

    //S rule
    fscanf(file, "S%c", &c);

    while (c!='\032' && c!='\n')
    {
	if (set_9(c-'0',S))
	{
	    printf("Bad S rule\n");
	    return NULL;
	}
	fscanf(file, "%c", &c);
    }

    if ( NULL == (life = malloc(a*sizeof(int*))) )
    {
	printf("Not enough memory\n");
	return NULL;
    }

    //map
    for (i=0 ; i<a ; i++)
    {
	if ( NULL == (life[i] = malloc(b*sizeof(int))) )
	{
	    printf("Not enough memory\n");
	    while (i-- > 0)
		free(life[i]);
	    return NULL;
	}
	for (j=0 ; j<b ; j++)
	{
	    fscanf(file, "%c ", &c);
	    switch (c)
	    {
		case '0':
		    life[i][j] = 0;
		    break;
		case '1':
		    life[i][j] = 1;
                    break;
		default:
		    printf("Bad map\n");
		    i++;
		    while (i-->0)
			free(life[i]);
		    return NULL;
	    }
	}
    }

    fclose(file);

    return life;
}

void print_map(int** map, int m, int n)
{
    int i,j;
    for (i=0 ; i<m ; i++)
    {
	for (j=0 ; j<n ; j++)
	    printf("%d", map[i][j]);
	printf("\n");
    }
    return;
}

