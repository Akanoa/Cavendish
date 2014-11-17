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

void sortSegment(struct List<struct Segment> *segments)
{
    //get how many segments must be sorted
    int left_segments = segments->nb-1;
    struct List<struct Segment> *computed = initList<struct Segment>();
    struct Segment *element = NULL;
    struct Segment *inserted = NULL;
    struct Node *swap = NULL; 
    struct Segment *last_computed = segments->first;
    struct Segment *seg1 = NULL, *seg2 = NULL;

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

    seg1 = segments->first;
    seg2 = segments->first->next;

    if(travelingDirection(seg1, seg2))
        cout << "CW" << endl;
    else
        cout << "CCW" << endl;


    if(!travelingDirection(seg1, seg2))
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

    seg1 = segments->first;
    seg2 = segments->first->next;

    if(travelingDirection(seg1, seg2))
        cout << "CW" << endl;
    else
        cout << "CCW" << endl;

}

bool travelingDirection(struct Segment *seg1, struct Segment *seg2)
{
    float res = (seg1->node2->x - seg1->node1->x)*(seg2->node2->y - seg2->node1->y) - (seg2->node2->x - seg2->node1->x)*(seg1->node2->y - seg1->node1->y);
    if(res>0)
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

    cout << "------------------------" << endl << endl;

    return last_segment->next;
}