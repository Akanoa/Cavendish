#include "vertice.h"

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

void sortSegment(struct List<struct Segment> *segments)
{
    //get how many segments must be sorted
    int left_segments = segments->nb-1;
    struct List<struct Segment> *computed = initList<struct Segment>();
    struct Segment *element = getElement(segments, segments->first->id);
    struct Segment *inserted = NULL;
    struct Node *swap = NULL; 
    struct Segment *last_computed = segments->first;
    struct Segment *seg1 = NULL, *seg2 = NULL;

    addElement(computed, element);
    popElement(segments, element->id);
    element->next = NULL;

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

    seg1 = segments->first;
    seg2 = segments->first->next;

    if((seg1->node2->x - seg1->node1->x)*(seg2->node2->y - seg2->node1->y) - (seg2->node2->x - seg2->node1->x)*(seg1->node2->y - seg1->node1->y))

    reverse(segments);

    for(struct Segment *seg = segments->first; seg; seg=seg->next)
    {
        cout << "segment: " << seg->id << endl << "\tnode 1: "<< seg->node1->id << endl << "\tnode 2: "<< seg->node2->id << endl << endl;
    }

}

bool travelingDirection(struct Segment *seg1, struct Segment *seg2)
{
    if(((seg1->node2->x - seg1->node1->x)*(seg2->node2->y - seg2->node1->y) - (seg2->node2->x - seg2->node1->x)*(seg1->node2->y - seg1->node1->y))==1)
        return true;
    return false;
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

void subdiviseOutline(struct List<struct Segment> *segments, int n)
{
    struct List<struct Segment> *computed = initList<struct Segment>();
    int nb_points = n - segments->nb;
    struct Segment *segment = segments->first;
    float perimeter = getPerimeter(segments);

    do
    {
        subdivise(segment, perimeter, nb_points);
    }while((segment = segment->next));
}

struct List<struct Segment> * subdivise(struct Segment *segment, float perimiter, int n)
{
    struct List<struct Segment> *computed = initList<struct Segment>();

    return computed;
}