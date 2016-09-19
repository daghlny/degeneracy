
/*
 * @Algorithm: This algorithm is designed by Batagelj & Zaversnik
 * the algorithm first published in the paper named "An O(m) Algorithm
 * for Cores Decomposition of Networks"
 */

#include <algorithm>
#include <iostream>
#include <set>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <fstream>

#include "inputbuffer.hpp"
#include "BZ.hpp"

using std::string;
using std::vector;

/* 
 * argv[1]: the input graph file with specified format
 * argv[2]: the output file with degeneracy ording 
 */
int main(int argc, char **argv)
{
    FILE *infile = fopen(argv[1], "r+");
    FILE *outfile = NULL;
    if( infile == NULL ){
        printf("open file error\n");
        exit(0);
    }
    char *outfilename = (char*)malloc(sizeof(char) * 100);
    strcpy(outfilename, argv[1]);
    if( argc < 3 ){
        strcat(outfilename, ".degeneracy.txt");
        outfile = fopen(outfilename, "w+");
    }
    else
        outfile = fopen(argv[2], "w+");

    /* get the total vertices number and use it to initilize arrays */
    int nodenum = 0;
    fscanf(infile, "%d\n", &nodenum);
    g = (vlist **)malloc(sizeof(vlist*) * nodenum);
    for( int i = 0; i != nodenum; ++i )
        g[i] = NULL;
    vid_t *vert = (vid_t*)malloc(sizeof(vid_t) * nodenum);
    vid_t *pos  = (vid_t*)malloc(sizeof(vid_t) * nodenum);
    vid_t *deg  = (vid_t*)malloc(sizeof(vid_t) * nodenum);

    /* reading graph data from file */
    inputbuffer ibuff(infile);
    vid_t md = init_g(ibuff, deg, nodenum);

    int *bin = (int*)malloc(sizeof(int) * (md+1));
    init_bin(bin, deg, pos, vert, md, nodenum);

    int degeneracy = 0;

    /* Main Loop */
    for( vid_t i = 0; i != nodenum; ++i )
    {
        /* the vertex @v is the chosen vertex in this iteration */
        vid_t v = vert[i];
        if(deg[v] > degeneracy)
            degeneracy = deg[v];
        for(vIt it = g[v]->begin(); it != g[v]->end(); ++it)
        {
            if(deg[*it] < deg[v]){
                continue;
                printf("the neighbor(id: %d)'s degree is smaller than @v(id: %d)\n", *it, v);
                exit(0);
            }

            /* output format */
            fprintf(outfile, "%d : %d\n", v, deg[v]);

            /* for each neighbor of @v, its degree should be decreased */
            vid_t du = deg[*it];    // degree of the neighbor
            int   pu = pos[*it];    // current position of neighbor
            int   pw = bin[du];     // begin position of neighbor's degree
            int   w  = vert[pw];    // first vertex in the @vert with @du degree

            if(*it != w)
            {
                /* exchange the two vertices' position in the list */
                pos[*it] = pw;
                pos[w]   = pu;
                vert[pu] = w;
                vert[pw] = *it;
            }

            ++bin[du];
            --deg[*it];
        }
    }
    printf("degeneracy: %d\n", degeneracy);

    return 1;
}

/*
 * return the maximum degree of all vertices */
vid_t
init_g(inputbuffer &ibuff, vid_t* deglist, int nodenum)
{
    int count = nodenum;
    vid_t md  = 0;
    while(--count >= 0)
    {
        char *start, *end;
        int result = ibuff.getline(start, end);
        if( result < 0 ) break;

        vid_t v = 0;
        do{
            v = (10 * v) + int(*start)-48;
        }while( *(++start) != ',' );

        //printf("nodenum: %d, v: %d\n", nodenum, v);
        if( v > nodenum || g[v] != NULL){
            printf("the value of @v is wrong\n");
            exit(0);
        }

        vid_t deg = 0;
        while( *(++start) != ':' ){
            deg = (10 * deg) + int(*start) - 48;
        }

        deglist[v] = deg;
        if( deg > md ) md = deg;
        g[v] = new vector<vid_t>(deg);
        for(int i = 0; i != deg; ++i)
        {
            vid_t u = 0;
            while( *(++start) != ':' && *(start) != '\n'){
                u = (10 * u) + int(*start) - 48;
            }
            g[v]->at(i) = u;
        }
    }

    return md;
}

void
init_bin(int *bin, vid_t *deglist, vid_t *pos, vid_t *vert, vid_t md, vid_t nodenum)
{
    for( int i = 0; i != md+1; ++i)
        bin[i] = 0;
    for( vid_t i = 0; i != nodenum; ++i )
        ++bin[ deglist[i] ];

    int start = 0;
    for( vid_t i = 0; i != md+1; ++i ){
        int num = bin[i];
        bin[i]  = start;
        start  += num;
    }

    for(vid_t i = 0; i != nodenum; ++i){
        pos[i] = bin[deglist[i]];
        vert[pos[i]] = i;
        ++bin[deglist[i]];
    }

    for(vid_t i = nodenum; i > 0; --i)
        bin[i] = bin[i-1];   
    bin[0] = 0;
    
}

