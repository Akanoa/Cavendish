#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#ifdef _WIN32
#include <direct.h>
#endif
#include <sys/stat.h>
#include "vertice.h"
#include "utils.h"


enum {HEADER, NB_POINT, POINT, COURBE, SEGMENT, END};
enum {LINUX, WINDOW};

const char *sep =
#ifdef _WIN32
    "\\\\";
#else
    "/";
#endif

using namespace std;

int main(int argc, char **argv)
{
    char src_[100], dst_[100];
    int nb_points_wanted = 30;
    sprintf(src_, "..%s%s%s%s", sep, "docs", sep, "MPmaille.geo");
    sprintf(dst_, "..%s%s%s%s", sep, "results", sep, "maillage.mai");

    string src(src_), dst(dst_);

    //generate result dir
    #ifdef _WIN32
        mkdir(dst.substr(0, dst.find_last_of(sep)-1).c_str());
    #else
        mkdir(dst.substr(0, dst.find_last_of(sep)).c_str(), 0700);
    #endif

    cout << "file: " << getFileName(dst, sep) << endl;

    if (argc > 1)
        src = argv[1];

    if (argc > 2)
        dst = argv[2];

    if(argc > 3)
        nb_points_wanted = atoi(argv[3]);

    // variables declaration
    ifstream input(src.c_str(), ifstream::in);

    int nb_final_points(0);
    int nb_elements(0);

    string line;
    vector<string> lines;
    vector<string> original_points;
    vector<string> points;
    vector<string> generated_points;
    vector<string> original_segments;

    int state = HEADER;

    struct List<struct Node> *nodes = initList<struct Node>();
    struct List<struct Segment> *segments = initList<struct Segment>();
    struct List<struct Element> *elements = initList<struct Element>();
    struct Node *node = NULL;
    struct Segment *segment = NULL;


    //parsing
    while(!input.eof())
    {
        safegetline(input, line);

        switch(state)
        {
        case HEADER:

            if(!line.compare("$points"))
                state = NB_POINT;
            else
                lines.push_back(line);
            break;
        case NB_POINT:
            state = POINT;
            break;
        case POINT:
            if(!line.compare("$courbes"))
                state = SEGMENT;
            else
            {
                float x, y;
                sscanf(line.c_str(), "%*s %f %f %*s", &x, &y);
                node = initNode(x, y, ORIGINAL);
                addElement<struct Node>(nodes, node);
            }
            break;
        case SEGMENT:
            if(!line.compare("//// fin"))
                state = END;
            else
            {
                int node_id1, node_id2;
                sscanf(line.c_str(), "%*s %d %d %*s", &node_id1, &node_id2);
                segment = initSegment(getElement<struct Node>(nodes, node_id1), getElement<struct Node>(nodes, node_id2), ORIGINAL);
                addElement<struct Segment>(segments, segment);
            }
            break;
        default:
            break;
        }
    }

    input.close();

    struct Node *tmp;
    char line_tmp[200];
    string str_tmp;
    int i=1;
    for(struct Node *node = nodes->first; node; node=node->next)
    {
        sprintf(line_tmp, "%d  %s  %s", i, float2scientific(node->x, 9).c_str(), float2scientific(node->y, 9).c_str());
        str_tmp = line_tmp;
        original_points.push_back(str_tmp);
        i++;
    }


    i=1;
    for(struct Segment *seg = segments->first; seg; seg=seg->next)
    {
        seg->zone = i;
        sprintf(line_tmp, "%3d%4d%4d", i, seg->node1->id, seg->node2->id);
        str_tmp = line_tmp;
        original_segments.push_back(str_tmp);
        i++;
    }


    sortSegment(segments);


/*    tmp= nodes->first;
    do
    {
        cout << "node: " << tmp->id << endl << "\tx:"<< tmp->x << endl << "\ty:"<< tmp->y << endl;
    }while((tmp = tmp->next));*/

    subdiviseOutline(segments, nodes, nb_points_wanted);

    // cout << endl << "*****************" << endl << endl;

    // for(struct Segment *seg = segments->first; seg; seg=seg->next)
    // {
    //     cout << "segment: " << seg->id << endl << "\tnode 1: "<< seg->node1->id << endl << "\tnode 2: "<< seg->node2->id << endl << endl;
    // }


    //generate list of points
    i=1;
    tmp = nodes->first;
    do
    {
        sprintf(line_tmp, "%4d  %s  %s%6d%6d", i, float2scientific(tmp->x, 10).c_str(), float2scientific(tmp->y, 10).c_str(), i, tmp->type);
        str_tmp = line_tmp;
        generated_points.push_back(str_tmp);
        nb_final_points++;
        i++;
    }while((tmp = tmp->next));

    while (segments->nb >= 4){
    Cavendish(segments, nodes, elements);
}

    //add not general inforamation
    string date, hour;
    currentDate(date, hour);

    lines[1] = "   RDM - Eléments finis";

    //remove file name informations
    lines.erase(lines.end()-4, lines.end());

    //convert int/float to string


    lines.push_back("$nom du fichier");
    lines.push_back(getFileName(dst, sep)+".mai");
    lines.push_back("$date");
    lines.push_back(date);
    lines.push_back("$HEURE");
    lines.push_back(hour);
    lines.push_back("$PROBLEME");
    lines.push_back("THPLAN");
    lines.push_back("$NOEUDS");
    lines.push_back(num2string(nb_final_points));
    lines.insert(lines.end(), generated_points.begin(), generated_points.end());
    lines.push_back("$limites de zones");
    lines.insert(lines.end(), original_segments.begin(), original_segments.end());
    lines.push_back("$points a mailler");
    lines.insert(lines.end(), original_points.begin(), original_points.end());
    lines.push_back("0");
    lines.push_back("$ELEMENTS");
    lines.push_back(num2string(nb_elements));
    lines.push_back("####TODO####");
    string footer = "$materiaux\n11\nMOD  2.100000E+11\nPOI  3.000000E-01\nMAS  7.800000E+03\nDIL  1.300000E-05\nCON  5.000000E+01\nLIM  2.500000E+08\nCAP  4.500000E+02\nNOM Acier\n///\n0\n$epaisseurs\n11 0.00000E+00\n0\n$fin du fichier";
    lines.push_back(footer);

    //generate output file
    ofstream output;
    output.open(dst.c_str(), ofstream::out | ofstream::trunc);

    for(unsigned int i=0; i< lines.size(); i++)
    {
        output<<lines.at(i)<<endl;
    }

    return 0;
}
