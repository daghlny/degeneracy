
#include <iostream>
#include <set>
#include <fstream>
#include <cstdint>
#include <string>
#include <cstdlib>
#include <cstring>

using std::string;

typedef uint32_t vid_t;
typedef std::set<vid_t> vlist;
typedef std::set<vid_t>::iterator vlistIt;

vlist **G;         // points to a array of adjacency lists
int   *degree;

/*
 *  add a edge into the @datap and update @deg array
 *  called by @readGraph() during initialization
 */
inline void
addEdge(vid_t v, vid_t u, vlist **datap, int *deg)
{
    typedef std::pair<vlistIt, bool> vlistInsert_t;
    if(datap[v] == NULL) datap[v] = new vlist();
    if(datap[u] == NULL) datap[u] = new vlist();
    vlistInsert_t uit = datap[v]->insert(u);
    vlistInsert_t vit = datap[u]->insert(v);

    if(deg[v] < 0) ++deg[v]; 
    if(deg[u] < 0) ++deg[u]; 
    if( uit.second == true ) ++deg[v];
    if( vit.second == true ) ++deg[u];
}

/*
 *  read the Graph data and construct the degree array and the adjacent list
 */
void
readGraph(vlist** datap, char *inFileName, int *degree)
{
    
    std::ifstream inFile;
    inFile.open(inFileName);

    char inbuffer[1024];
    const char *flag = " \t";

    while(!inFile.eof())
    {
        inFile.getline(inbuffer, 1024);
        if(inbuffer[0] == '#') continue;
        if(!isdigit(inbuffer[0])) break;

        string line(inbuffer);
        std::string::size_type seperator = line.find_first_of(flag, 0);
        vid_t sour = atoi(line.substr(0, seperator).c_str());
        vid_t dest = atoi(line.substr(seperator, line.size()-seperator).c_str());

        addEdge(sour, dest, datap, degree);
    }
    inFile.close();
}

/*
 * return the vertex with the smallest degree
 * @len is the @memnode number
 * @mindeg will return the smallest degree
 */
vid_t
smallestDegVertex(int *deg, int len, int &mindeg)
{

    vid_t res_vertex = 0;
    mindeg = -1;
    for(int i = 0; i != len; ++i){
        if( deg[i] >= 0 && mindeg < 0 ){
            res_vertex = i;
            mindeg = deg[i];
            continue;
        }
        if( deg[i] < 0 ) continue;
        if( deg[i] < mindeg ){
            res_vertex = i;
            mindeg = deg[i];
        }
    }

    return res_vertex;
}

/*
 * @argv[1]: input file path
 * @argv[2]: the exact total number of the input graph
 * @argv[3]: whether output the degeneracy ordering
 * @argv[4]: the number of memory nodes should be assigned to give redundancy
 */
int
main(int argc, char **argv)
{
   
    if( argc < 4 )
    {
        std::cout << "Program inputfile totalvertex whetheroutput [InMemNode]";
        std::cout << std::endl;
        exit(1);
    }

    FILE *wfile;
    int ifoutput = atoi(argv[3]);
    if( ifoutput ){
        char *outputfilename = (char*)malloc(sizeof(char) * 200);
        strcpy( outputfilename, argv[1] );
        strcat( outputfilename, ".degeneracy.txt" );
        wfile = fopen(outputfilename, "w");
        if( wfile == NULL ){
            std::cout << "open output file error" << std::endl;
            exit(1);
        }
    }

    int nodenum = atoi(argv[2]);
    int memnode = nodenum+100;
    if( argc == 5 ){
        memnode = atoi(argv[4]);
    }
    G = (vlist**)malloc(sizeof(vlist*) * memnode);
    degree = (int*)malloc(sizeof(int*) * memnode);
    for( int i = 0; i != memnode; ++i )
        G[i] = NULL;
    for( int i = 0; i != memnode; ++i )
        degree[i] = -1;

    readGraph(G, argv[1], degree);
    std::cout << "finish reading graph data" << std::endl;

    int remaining_vertex = nodenum;
    vid_t currv = 0;
    int   mindeg = 0;

    while(remaining_vertex != 0){
     
        currv = smallestDegVertex(degree, memnode, mindeg);
        if( ifoutput ){
            fprintf(wfile, "%d:%d\n", currv, mindeg);
        }
        if( G[currv] == NULL )
        {
            std::cout << "current vertex's vlist pointer is NULL" << std::endl;
            std::cout << "The Vertex: " << currv << std::endl;
            exit(0);
        }
        for( vlistIt it = G[currv]->begin(); it != G[currv]->end(); it++) {
            if( G[*it] == NULL ) continue;
            --degree[*it];
        }


        delete G[currv];
        G[currv] = NULL;
        degree[currv] = -1;
        --remaining_vertex;
    }

    fclose(wfile);

    return 1;
}
