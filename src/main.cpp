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


enum {HEADER, NB_POINT, POINT, COURBE, END};
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
    int nb_points_wanted = 20;
    int iter = 0;
    bool only_outline = false;
    float angle = 1.0;
    sprintf(src_, "..%s%s%s%s", sep, "docs", sep, "polygon.geo");
    sprintf(dst_, "..%s%s%s%s", sep, "results", sep, "maillage.cal");

    string src(src_), dst(dst_);

    //generate result dir
    #ifdef _WIN32
        mkdir(dst.substr(0, dst.find_last_of(sep)-1).c_str());
    #else
        mkdir(dst.substr(0, dst.find_last_of(sep)).c_str(), 0700);
    #endif

    if (argc > 1)
        src = argv[1];

    if (argc > 2)
        dst = argv[2];

    if(argc > 3)
        nb_points_wanted = atoi(argv[3]);

    if(argc > 4)
        angle = atof(argv[4]);

    if(argc > 5)
        iter = atoi(argv[5]);

    if(argc > 6 && !strcmp(argv[6], "true"))
        only_outline = true;

    cout << "########################################" << endl;
    cout << "#            ENIB Meshing              #" << endl;
    cout << "########################################" << endl << endl;
    cout << "---------------------------------------------" << endl;
    cout << "usage: cavendish [src] [dst] [outiline_subdivions] [angle] [nb_iter] [only_outline]" << endl << endl;
    cout << "- src                 : RDM6 .geo file used as original outline geometry" << endl;
    cout << "- dst                 : RDM6 .cal file, output file " << endl;
    cout << "- outiline_subdivions : approximated number of outiline subdivions wanted" << endl;
    cout << "- angle               : angle size between two in case of arc discretisation" << endl;
    cout << "- nb_iter             : how many iteration" << endl;
    cout << "- only_outline        : if true, generates only outline subdivised" << endl;
    cout << "---------------------------------------------" << endl << endl;
    cout << "Input file:          " << src << endl;
    cout << "Output file:         " << dst << endl;
    cout << "outiline subdivions: " << nb_points_wanted << endl << endl;
    cout << "Meshing in progress..." << endl << endl;

    // variables declaration
    ifstream input(src.c_str(), ifstream::in);

    int nb_final_points(0);
    int nb_elements(0);

    string line;
    string startwith;
    char startwith_tmp[100] = ""; 
    vector<string> lines;
    vector<string> original_points;
    vector<string> points;
    vector<string> generated_points;
    vector<string> original_segments;

    vector<string> generated_elements;


    int state = HEADER;

    struct List<struct Node> *nodes = initList<struct Node>();
    struct List<struct Segment> *segments = initList<struct Segment>();
    struct List<struct Element> *arcs = initList<struct Element>();
    struct List<struct Element> *elements = initList<struct Element>();
    struct Node *node = NULL;
    struct Segment *segment = NULL;
    struct Element *arc = NULL;

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
                state = COURBE;
            else
            {
                float x, y;
                sscanf(line.c_str(), "%*s %f %f %*s", &x, &y);
                node = initNode(x, y, ORIGINAL);
                addElement<struct Node>(nodes, node);
            }
            break;
        case COURBE:
            if(!line.compare("//// fin"))
                state = END;
            else
            {
                sscanf(line.c_str(), "%s", startwith_tmp);
                startwith = startwith_tmp;
                if(!startwith.compare("segment"))
                {
                    int node_id1, node_id2;
                    sscanf(line.c_str(), "%*s %d %d %*s", &node_id1, &node_id2);
                    segment = initSegment(getElement<struct Node>(nodes, node_id1), getElement<struct Node>(nodes, node_id2));
                    addElement<struct Segment>(segments, segment);
                }
                else if (!startwith.compare("arc"))
                {
                    int node_id1=0, node_id2=0;
                    char cx_s[20], cy_s[20];
                    float cx=0.0, cy=0.0;
                    sscanf(line.c_str(), "%*s %d %d  %s  %s %*s", &node_id1, &node_id2, cx_s, cy_s);
                    cx = atof((const char*)cx_s);
                    cy = atof((const char*)cy_s);
                    arc = initElement(getElement(nodes, node_id1), getElement(nodes, node_id2), initNode(cx, cy, ORIGINAL));

                    addElement(arcs, arc);
                }
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
    for(struct Node *node = nodes->first; node!=nodes->last; node=node->next)
    {
        sprintf(line_tmp, "%d  %s  %s", i, float2scientific(node->x, 9).c_str(), float2scientific(node->y, 9).c_str());
        str_tmp = line_tmp;
        original_points.push_back(str_tmp);
        i++;
    }


    i=1;
    for(struct Segment *seg = segments->first; seg!=segments->last; seg=seg->next)
    {
        sprintf(line_tmp, "%3d%4d%4d", i, seg->node1->id, seg->node2->id);
        str_tmp = line_tmp;
        original_segments.push_back(str_tmp);
        i++;
    }

    sortSegment(segments, arcs);

    subdiviseArc(segments, arcs , angle);

    subdiviseOutline(segments, nodes, nb_points_wanted);

    bool ok = true;

    if(only_outline)
    {
        struct Segment *seg = segments->first;
        for(int i=0; i < segments->nb; i++)
        {
            addElement(elements, initElement(seg->node1, seg->node2, seg->node1));
            seg = seg->next;
        }
    }
    else
    {
        ok = Cavendish(segments, nodes, elements, iter);
    }

    //stop all cavendish had crashed
    if(!ok)
    {
        cout << "Sorry, but your paremeters inputed, could not generate a convenable meshing :<" << endl;
        cout << "Try another to reduce number of subdivions for example..." << endl;
        exit(1);
    }

    //generate list of points
    tmp = nodes->first;
    for(i=1; i<nodes->nb+1; i++)
    {
        sprintf(line_tmp, "%4d  %s  %s%6d%6d", i, float2scientific(tmp->x, 10).c_str(), float2scientific(tmp->y, 10).c_str(), i, tmp->type);
        str_tmp = line_tmp;
        generated_points.push_back(str_tmp);
        nb_final_points++;
        tmp = tmp->next;
    }


    //generate list of elements
    struct Element *tmp_el;
    tmp_el = elements->first;
    for(i=1; i<elements->nb+1; i++)
    {
        sprintf(line_tmp, "%4d TRI3 1   1  11  11     %2d     %2d     %2d", i, tmp_el->node1->id, tmp_el->node2->id, tmp_el->node3->id);
        str_tmp = line_tmp;
        generated_elements.push_back(str_tmp);
        tmp_el = tmp_el->next;
    }

    

    //add not general inforamation
    string date, hour;
    currentDate(date, hour);

    lines[1] = "   RDM - Eléments finis";

    //remove file name informations
    lines.erase(lines.end()-4, lines.end());

    //convert int/float to string
    nb_elements = elements->nb;


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
    lines.insert(lines.end(), generated_elements.begin(), generated_elements.end());
    string footer = "$materiaux\n11\nMOD  2.100000E+11\nPOI  3.000000E-01\nMAS  7.800000E+03\nDIL  1.300000E-05\nCON  5.000000E+01\nLIM  2.500000E+08\nCAP  4.500000E+02\nNOM Acier\n///\n0\n$epaisseurs\n11 0.00000E+00\n0\n$fin du fichier";
    lines.push_back(footer);

    //generate output file
    ofstream output;
    output.open(dst.c_str(), ofstream::out | ofstream::trunc);

    for(unsigned int i=0; i< lines.size(); i++)
    {
        output<<lines.at(i)<<endl;
    }

    cout << "Completed :D" << endl << endl;
    cout << nodes->nb << " nodes    generated" << endl;
    cout << elements->nb << " elements generated" << endl << endl;
    cout << "Be careful, RDM6 seems unable to handle too many elements!!" << endl;

    return 0;
}