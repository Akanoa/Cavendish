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
    segment->next  = NULL;

    return segment;
}

void sortSegment(struct List<struct Segment> *segments)
{
    //get how many segments must be sorted
    int left_segments = segments->nb-1;
    int waiting_id = 0;
    struct List<struct Segment> *computed = initList<struct Segment>();
    struct Segment *element = getElement(segments, segments->first->id);
    struct Segment *inserted = NULL;
    struct Node *swap = NULL; 
    struct Segment *last_computed = segments->first;
    addElement(computed, element);
    popElement(segments, element->id);
    element->next = NULL;

    cout << "init computed list content:" << endl;
    for(struct Segment *seg = computed->first; seg; seg=seg->next)
    {
        cout << "segment: " << seg->id << endl << "\tnode 1: "<< seg->node1->id << endl << "\tnode 2: "<< seg->node2->id << endl << endl;
    }
    cout << "+++++++++++++++" << endl;

    cout << endl << "init segments list content:" << endl;
    for(struct Segment *seg = segments->first; seg; seg=seg->next)
    {
        cout << "segment: " << seg->id << endl << "\tnode 1: "<< seg->node1->id << endl << "\tnode 2: "<< seg->node2->id << endl << endl;
    }
    cout << "#########" << endl;

    while(left_segments)
    {
        element = segments->first;
        cout << "left_segments: " << left_segments << endl;
        cout << "waiting node: "<< last_computed->node2->id << endl; 
        do
        {
            if(last_computed->node2->id == element->node1->id)
            {
                cout << "found node1: " << element->id << endl;
                inserted = popElement(segments, element->id);
                addElement(computed, inserted);
                inserted->next = NULL;
                last_computed = element;

                cout << endl << "computed list content:" << endl;
                for(struct Segment *seg = computed->first; seg; seg=seg->next)
                {
                    cout << "segment: " << seg->id << endl << "\tnode 1: "<< seg->node1->id << endl << "\tnode 2: "<< seg->node2->id << endl << endl;
                }
                cout << "+++++++++++++++" << endl;
                cout << endl << "segments list content:" << endl;
                for(struct Segment *seg = segments->first; seg; seg=seg->next)
                {
                    cout << "segment: " << seg->id << endl << "\tnode 1: "<< seg->node1->id << endl << "\tnode 2: "<< seg->node2->id << endl << endl;
                }
                cout << "#########" << endl;
                break;
            }
            else if(last_computed->node2->id == element->node2->id)
            {
                cout << "found node2: " << element->id << endl;
                swap = element->node2;
                element->node2 = element->node1;
                element->node1 = swap;
                inserted = popElement(segments, element->id);
                addElement(computed, inserted);
                inserted->next = NULL;
                last_computed = element;

                cout << endl << "computed list content:" << endl;
                for(struct Segment *seg = computed->first; seg; seg=seg->next)
                {
                    cout << "segment: " << seg->id << endl << "\tnode 1: "<< seg->node1->id << endl << "\tnode 2: "<< seg->node2->id << endl << endl;
                }
                cout << "+++++++++++++++" << endl;
                cout << endl << "segments list content:" << endl;
                for(struct Segment *seg = segments->first; seg; seg=seg->next)
                {
                    cout << "segment: " << seg->id << endl << "\tnode 1: "<< seg->node1->id << endl << "\tnode 2: "<< seg->node2->id << endl << endl;
                }
                cout << "#########" << endl;
                break;
            }
        }while(element = element->next);
        left_segments--;
    }

    cout << "~~~~~~~~~~~~~~~~~~~~" << endl;
    for(struct Segment *seg = computed->first; seg; seg=seg->next)
    {
        cout << "segment: " << seg->id << endl << "\tnode 1: "<< seg->node1->id << endl << "\tnode 2: "<< seg->node2->id << endl << endl;
    }
    cout << "~~~~~~~~~~~~~~~~~~~~" << endl;
}
