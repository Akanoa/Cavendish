#include "vertice.h"
#include <cstdlib>

using namespace std;

struct Node* initNode(float x, float y, int type)
{
    struct Node *node = new struct Node;
    node->x    = x;
    node->y    = y;
    node->id   = 0;
    node->type = type;
    node->next = NULL;
    return node;
}

struct Segment* initSegment(Node *node1, Node *node2, int type)
{
    struct Segment *segment = new struct Segment;
    segment->node1 = node1;
    segment->node2 = node2;
    segment->type  = type;
    segment->id    = 0;
    segment->zone  = 0;
    segment->next  = NULL;

    return segment;
}

struct Element* initElement(Segment *segment1, Segment *segment2, Segment *segment3, int type)
{
    struct Element *element = new struct Element;
    element->segment1 = segment1;
    element->segment2 = segment2;
    element->segment3 = segment3;
    element->type  = type;
    element->id    = 0;
    element->zone  = 0;
    element->next  = NULL;

    return element;
}

float getAngle(struct Segment* segment1, struct Segment* segment2)
{
    return atan2(segment2->node2->y - segment2->node1->y, segment2->node2->x - segment2->node1->x) - atan2(segment1->node2->y - segment1->node1->y, segment1->node2->x - segment1->node1->x);
}

void sortSegment(struct List<struct Segment> *segments)
{
    //get how many segments must be sorted
    int left_segments = segments->nb-1;
    struct List<struct Segment> *computed = initList<struct Segment>();
    struct Segment *element = NULL;
    struct Segment *inserted = NULL;
    struct Node *swap = NULL; 
    struct Segment *last_computed = segments->first;

    inserted = popElement(segments, segments->first->id);
    addElement(computed, inserted);

    while(left_segments)
    {
        element = segments->first;
        do
        {
            if(last_computed->node2->id == element->node1->id)
            {
                inserted = popElement(segments, element->id);
                addElement(computed, inserted);
                inserted->next = NULL;
                last_computed = element;
            }
            else if(last_computed->node2->id == element->node2->id)
            {
                swap = element->node2;
                element->node2 = element->node1;
                element->node1 = swap;
                inserted = popElement(segments, element->id);
                addElement(computed, inserted);
                inserted->next = NULL;
                last_computed = element;
                break;
            }
        }while((element = element->next));
        left_segments--;
    }

    *segments = *computed;
    delete computed;

    for(struct Segment *seg = segments->first; seg; seg=seg->next)
    {
        cout << "segment: " << seg->id << endl << "\tnode 1: "<< seg->node1->id << endl << "\tnode 2: "<< seg->node2->id << endl << endl;
    }


    if(travelingDirection(segments))
        cout << "CW" << endl;
    else
        cout << "CCW" << endl;


    if(!travelingDirection(segments))
    {
        reverse(segments);

        element = segments->first;
        do
        {
            swap = element->node2;
            element->node2 = element->node1;
            element->node1 = swap;
        }while((element = element->next));
    }


    cout << endl << "----------------------" << endl << endl;

    for(struct Segment *seg = segments->first; seg; seg=seg->next)
    {
        cout << "segment: " << seg->id << endl << "\tnode 1: "<< seg->node1->id << endl << "\tnode 2: "<< seg->node2->id << endl << endl;
    }


    if(travelingDirection(segments))
        cout << "CW" << endl;
    else
        cout << "CCW" << endl;

}

bool travelingDirection(struct List<struct Segment> *segments)
{
    float sum = 0.0;
    struct Segment *last = NULL;
    for(struct Segment *seg = segments->first; seg->next; seg=seg->next)
    {
        sum += getAngle(seg, seg->next);
        last=seg->next;
    }

    sum += getAngle(last, segments->first);

    return (sum>=0)?true:false; 
}

float minAngle(struct List<struct Segment> *segments, struct Segment *segment_n_1, struct Segment *segment1, struct Segment *segment2, struct Segment *segment_n1)
{
    //Nouveau noeuds
    //struct Node *new_node1 = NULL;
    //struct Node *new_node2 = NULL;
    //Noeuds a supprimer
    //int del_node = 0;
    //Elements à supprimer
    int del_seg1 = 0;
    int del_seg2 = 0;
    //Element précédent
    int seg_n_1 = 0;
    int prev = 0;

    float min = PI, tmp = 0.0; //max angle
    reverse(segments);
    for(struct Segment *seg = segments->first; seg->next; seg=seg->next)
    {
        tmp = getAngle(seg, seg->next);
        if(tmp<min){
            del_seg1 = seg->id;
            del_seg2 = seg->next->id;
           
            seg_n_1 = prev;

            //del_node = seg->node1->id;
            min = tmp;

        }
        prev = seg->id;
    }
    segment2 = getElement (segments, del_seg1);
    segment1 = getElement (segments, del_seg2);
    segment_n1 = getElement (segments, segment1->next->id);
            
            
    segment_n_1 = getElement(segments, seg_n_1);
    reverse(segments);

    return min;
}


float getDistance(struct Segment *segment)
{
    return sqrt((segment->node1->x - segment->node2->x)*(segment->node1->x - segment->node2->x)+(segment->node1->y - segment->node2->y)*(segment->node1->y - segment->node2->y));
}

float getDistance(struct Node *node1, struct Node *node2)
{
    return sqrt((node1->x - node2->x)*(node1->x - node2->x)+(node1->y - node2->y)*(node1->y - node2->y));
}

float getPerimeter(struct List<struct Segment> *segments)
{
    struct Segment *segment = segments->first;
    float res = 0.0;
    do
    {
        res += getDistance(segment);
    }while((segment = segment->next));

    return res;
}

struct Node* generateNewPointOnSegment(struct Segment *segment, float length, float distance)
{
    float x = segment->node1->x*(length-distance)/length + segment->node2->x*distance/length;
    float y = segment->node1->y*(length-distance)/length + segment->node2->y*distance/length;

    return initNode(x, y, segment->id);
}

void subdiviseOutline(struct List<struct Segment> *segments, struct List<struct Node> *nodes, int n)
{
    int nb_points = n - segments->nb;
    struct Segment *segment = segments->first;
    float perimeter = getPerimeter(segments);

    cout << "Delta: " << perimeter/nb_points << endl;

    do
    {
        segment = subdivise(segment, perimeter, nb_points, nodes, segments);
    }while((segment = segment->next));

}

struct Segment* subdivise(struct Segment *segment_, float perimiter, int n, struct List<struct Node> *nodes, struct List<struct Segment> *segments)
{
    float L = getDistance(segment_);
    int n_segs = (int)round(L*n/perimiter);
    float delta = L/n_segs;

    struct Segment *segment = NULL;
    struct Segment *last_segment = segment_;

    struct Node *node = NULL;
    struct Node *start = segment_->node1;

    cout << "id: " << segment_->id << " => ni: " << n_segs-1 << " => delta: " << delta << " => L: " << L << endl;


    if(n_segs-1>0)
    {    
        for(int i=0; i<n_segs-1; i++)
        {
            //Node generation
            node = generateNewPointOnSegment(segment_, L, delta*(i+1));
            cout << "new point generated:" << endl << "\tx: "<< node->x << "\ty: " << node->y << endl << endl;
            addElement(nodes, node);

            //Segment generation
            segment = initSegment(start, node, ORIGINAL);
            insertElement(segments, segment, last_segment->id);

            last_segment = segment;
            start = node;

        }
    
        segment = initSegment(start, segment_->node2, ORIGINAL);
        insertElement(segments, segment, last_segment->id);

        delete popElement(segments, segment_->id);
    }

    cout << "------------------------" << endl << endl;

    return last_segment->next;
}


void Cavendish(struct List<struct Segment> *segments, struct List<struct Node> *nodes, struct List<struct Element> *elements){


    struct Segment* segment1 = NULL;
    struct Segment* segment2 = NULL;

    struct Segment* previous = NULL;
    struct Segment* next = NULL;

    float angle_min = 0.0;

    angle_min = minAngle(segments, previous, segment1, segment2, next);

    ///////////////////////////////////////// PREMIER CAS //////////////////////////////////////
    if (angle_min < (PI/2)){
        struct Segment* new_seg = initSegment(segment1->node1, segment2->node2, ORIGINAL);
        struct Element* new_element = initElement(segment1, segment2, new_seg, ORIGINAL);
        addElement(segments, new_seg);
        popElement(segments, segment1->id);
        popElement(segments, segment2->id);
        popElement(nodes, segment1->node2->id);
        insertElement(segments,new_seg, previous->id);
        addElement(elements, new_element);
    }

    ///////////////////////////////////////// SECOND CAS //////////////////////////////////////

    if ((angle_min>=(PI/2)) && (angle_min<(2*PI/3))){
        struct Node* node_1 = segment2->node1;
        struct Node* node_3 = NULL;
        float x_1 = node_1->x;
        float y_1 = node_1->y;
        float x_3 = 0.0;
        float y_3 = 0.0;

        float x_int = node_1->x + 10;
        float y_int = node_1->y;
        struct Node* node_int = initNode(x_int, y_int, ORIGINAL);
        struct Segment* intermediaire = initSegment(node_1, node_int, ORIGINAL);

        float angle_int = (angle_min/2 ) + getAngle(intermediaire, segment2);

        float r = (1/6)*(getDistance(previous)+2*(getDistance(segment1)+getDistance(segment2))+getDistance(next));

        x_3 = x_1 + r*cos(angle_int);
        y_3 = y_1 + r*sin(angle_int);

        node_3 = initNode(x_3, y_3, ORIGINAL);

        addElement(nodes, node_3);

        struct Segment* new_seg1 = initSegment(node_1, node_3, ORIGINAL);
        struct Segment* new_seg2 = initSegment(segment1->node1, node_3, ORIGINAL);
        struct Segment* new_seg3 = initSegment(node_3, segment2->node2, ORIGINAL);
        struct Element* new_element1 = initElement(segment1, new_seg1, new_seg2, ORIGINAL);
        struct Element* new_element2 = initElement(segment1, new_seg1, new_seg3, ORIGINAL);

        addElement(segments, new_seg2);
        addElement(segments, new_seg3);
        popElement(segments, segment1->id);
        popElement(segments, segment2->id);
        popElement(nodes, segment1->node2->id);
        insertElement(segments, new_seg2, previous->id);
        insertElement(segments, new_seg3, new_seg2->id);
        addElement(elements, new_element1);
        addElement(elements, new_element2);
    }

    if (angle_min > (2*PI/3)){

        float r = getDistance(segment1);
        float t = getDistance(segment2);
        float x_3 = 0.0;
        float y_3 = 0.0;

        struct Segment* equi = NULL;
        struct Node* node_3 = NULL;

        if (r > t){
            equi = segment2;
        }
        else equi = segment1;

        struct Node* node_1 = equi->node1;

        float x_int = node_1->x + 10;
        float y_int = node_1->y;
        struct Node* node_int = initNode(x_int, y_int, ORIGINAL);

        struct Segment* intermediaire = initSegment(node_1, node_int, ORIGINAL);
        float angle_int = (angle_min/3 ) + getAngle(intermediaire, equi);

        x_3 = equi->node1->x + r*cos(angle_int);
        y_3 = equi->node1->y + r*sin(angle_int);

        node_3 = initNode(x_3, y_3, ORIGINAL);

        addElement(nodes, node_3);

        struct Segment* new_seg1 = initSegment(equi->node1, node_3, ORIGINAL);
        struct Segment* new_seg2 = initSegment(node_3, equi->node2, ORIGINAL);
        struct Element* new_element1 = initElement(segment1, new_seg1, new_seg2, ORIGINAL);

        addElement(segments, new_seg1);
        addElement(segments, new_seg2);
        popElement(segments, segment1->id);
        insertElement(segments, new_seg1, previous->id);
        insertElement(segments, new_seg2, new_seg1->id);
        addElement(elements, new_element1);


    }


    
    

}