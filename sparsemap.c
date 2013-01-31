#include <stdlib.h>
#include <stdio.h>

#include "sparsemap.h"

Smap* emptysmp()
{
    Smap *s=malloc(sizeof(Smap));

    if (s==NULL)
	return NULL;

    s->len=MIN_LIN;
    s->size=0;
    s->list=malloc(MIN_LIN*sizeof(int*));
    return s;
}

void freesmp(Smap* s)
{
    int i;
    for (i=0 ; i<s->size ; i++)
	free(s->list[i]);
    free(s);
}

int open_line(Smap* s, int i)
{
    // Have to extend queue
    if (s->size==s->len)
    {
        int _i;
        int **_list=malloc(2*s->len*sizeof(int*));

        if (_list==NULL)
            return -1;

        for (_i=0 ; _i<s->len ; _i++)
            _list[_i]=s->list[_i];

        free(s->list);
        s->list=_list;
        s->len<<=1;
    }

    int i_;
    i_=find_line(s->list,i,0,s->size);

    if (i_==s->size || s->list[i_][0]!=i)
    {
        int _i;

        for (_i=s->size ; _i>i_ ; i--)
            s->list[_i]=s->list[_i-1];

        s->list[i_]=new_line(i, MIN_COL);

        if (s->list==NULL)
            return -1;

        s->size++;
#ifdef DEBUG_SMP
        printf("NEW LINE %d at %d\n", i, i_);
#endif
        return i_;
    }
    else
        return i_;
}

int push_line(Smap* s, int i)
{
    // Have to extend queue
    if (s->size==s->len)
    {
        int _i;
        int **_list=malloc(2*s->len*sizeof(int*));

	if (_list==NULL)
	    return -1;

        for (_i=0 ; _i<s->len ; _i++)
            _list[_i]=s->list[_i];

        free(s->list);
        s->list=_list;
        s->len<<=1;
    }

    if (!s->size || i>s->list[s->size-1][0]) // Lazy evaluation
    {
        s->list[s->size]=new_line(i,MIN_COL);
        return s->size++;
    }
    else
        return open_line(s,i);
}


int add_point(Smap* s, int i_s, int j)
{
#ifdef DEBUG_SMP
    printf("Added %d %d\n", s->list[i_s][0], j);
#endif
    return insert_point(&s->list[i_s],j);
}

int push_point(Smap* s, int i_s, int j)
{
    return _push_point(&s->list[i_s],j);
    
}

void qadd_point(Smap* s, int i, int j)
{
    add_point(s,open_line(s,i),j);
}

/***********************************************/
/******************Conversion*******************/
/***********************************************/
Smap* smp_of_bmp(int** b, int m, int n)
{
    int i, j, i_;
    Smap *s=emptysmp();


    for (i=0 ; i<m ; i++)
    {
        i_=-1;
        for (j=0 ; j<n ; j++)
            if (b[i][j])
            {
                if (i_==-1)
                    if ((i_=push_line(s,i))==-1)
		    {
			freesmp(s);
			return NULL;
		    }
                if (!push_point(s,i_,j))
		{
		    freesmp(s);
		    return NULL;
		}
            }
    }
    return s;
}


/************************************************/
/******************* Printing *******************/
/************************************************/

void print_smap(Smap s, int xM, int xm, int yM, int ym)
{
    int i,i_=xm,j_;

    char *empty_line=malloc((yM-ym+4)*sizeof(char));

    empty_line[0]=LIMITER;
    empty_line[yM-ym+1]=LIMITER;
    empty_line[yM-ym+2]='\n';
    empty_line[yM-ym+3]='\0';
    for (j_=1 ; j_<yM-ym+1 ; j_++)
        empty_line[j_]=DEDCELL;

    for (j_=ym-1 ; j_<yM+1 ; j_++)
        printf("%c",LIMITER);
    printf("\n");

    for (i=0 ; i<s.size ; i++)
    {
        if (s.list[i][0]<xm)
            continue;
        else if (s.list[i][0]>=xM)
            break;
        else
        {
            while (i_++<s.list[i][0])
              printf("%s",empty_line);
            print_line(s.list[i],yM,ym);
        }
    }

    while (i_++<xM)
        printf("%s",empty_line);

    for (j_=ym-1 ; j_<yM+1 ; j_++)
        printf("%c",LIMITER);
    printf("\n");

    return;
}


/************************************************/
/************ Auxiliary functions ***************/
/************************************************/

void print_line(int* l, int yM, int ym)
{
    int j,j_=ym;

    printf("%c",LIMITER);

    for (j=3 ; j<l[2]+3 ; j++)
    {
        if (l[j]<ym)
            continue;
        else if (l[j]>=yM)
            break;
        else
        {
            while (j_++<l[j])
                printf("%c",DEDCELL);
            printf("%c",ALIVE);
        }
    }

    while (j_++<yM)
        printf("%c",DEDCELL);

    printf("%c\n",LIMITER);
    return;
}

// find min(i_|s[i_][0]>=i), a<=i<b
int find_line(int** s, int i, int a, int b)
{
    if (a<b)
    {
        int c = (a+b)/2;
        if (s[c][0]>i)
            return find_line(s, i, a, c);
        else if (s[c][0]<i)
            return find_line(s, i, c+1, b);
        else
            return c;
    }
    else return a;
}

int* new_line(int i, int len)
{
    int *l=malloc((len+3)*sizeof(int*));

    if (l==NULL)
	return NULL;

    l[0]=i;
    l[1]=len;
    l[2]=0;
    return l;
}

int find_col(int* s, int j, int a, int b)
{
    if (a<b)
    {
        int c = (a+b)/2;
        if (s[c]<j)
            return find_col(s,j,c+1,b);
        else if (s[c]>j)
            return find_col(s,j,a,c-1);
        else
            return c;
    }
    else
        return a;
}

int insert_point(int** list, int j)
{
    int *l=*list;
    // Have to extend
    if (l[2]==l[1])
    {
        int _j;
        int *l2=new_line(l[0],2*l[1]);

        if (l2==NULL)
            return 0;

        for (_j=3 ; _j<l[1]+3 ; _j++)
            l2[_j]=l[_j];

        free(l);
        l=l2;
    }

    int j_;
    j_=find_col(l,j,3,l[2]+3);

    if (j_==l[2]+3 || l[j_]!=j)
    {
        int _j;

        l[2]++;

        for (_j=l[2]+3 ; _j>j_ ; _j--)
            l[_j]=l[_j-1];

        l[j_]=j;

#ifdef DEBUG_SMP
        printf("NEW COL %d\n", j_);
#endif
        return j_;
    }
    else
        return j_;
}

int _push_point(int** list, int j)
{
    int *l=*list;
    // Have to extend
    if (l[2]==l[1])
    {
        int _j;
        int *l2=new_line(l[0],2*l[1]);

        if (l2==NULL)
            return 0;

        for (_j=3 ; _j<l[1]+3 ; _j++)
            l2[_j]=l[_j];

        free(l);
        l=l2;
    }

    if (!l[2] || j>l[3+l[2]-1])
    {
        l[3+l[2]]=j;
        return 3+l[2]++;
    }
    else
        return 0;
}
