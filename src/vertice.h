#include <iostream>
#include <cmath>
#include <cstring>
#include <vector>

#define PI 3.14159265

enum type {ORIGINAL, GENERATED};
enum cavendish {FIRST_CASE, SECOND_CASE, THIRD_CASE, END_CASE};

struct Node
{
    float x;
    float y;
    int id;
    int type;
    struct Node *next;
    struct Node *prev;
};

struct Segment
{
    Node *node1;
    Node *node2;
    int id;
    int zone;
    int type;
    struct Segment *next;
    struct Segment *prev;
};

struct Element
{
    Node *node1;
    Node *node2;
    Node *node3;
    int id;
    int zone;
    int type;
    struct Element *next;
    struct Element *prev;
};

template<typename T>
struct List
{
    int nb;
    int last_id;
    T *first;
    T *last;
};

//templates stuff


template<typename T>
struct List<T>* initList()
{
    struct List<T> *list = new List<T>;
    list->nb = 0;
    list->last_id = 1;
    list->first = NULL;
    list->last = NULL;

    return list;
}

template<typename T>
void addElement(struct List<T> *list, T *element)
{
    if(list->first)
    {
        element->id = list->last_id;
        list->last_id++;
        element->next = list->first;
        element->prev = list->last;
        list->first->prev = element;
        list->last->next  = element;
        list->last = element;
    }
    else
    {
        element->id = list->last_id;
        list->last_id++;
        list->first = element;
        list->last = element;
        element->prev = element;
        element->next = element;
    }

    list->nb++;
}


template<typename T>
T* getElement(struct List<T> *list, int id)
{
    T *element = list->first;
    bool ok = false;

    if(list->first)
    {
        do
        {
            if(element->id == id)
            {
                ok = true;
                break;
            }
            element = element->next;
        }while(element!=list->first);
    }

    return ok?element:NULL;
}

template<typename T>
T* popElement(struct List<T> *list, int id)
{
    T *element = getElement(list, id);

    if(element)
    {
        if(list->nb == 1)
        {
            list->last = NULL;
            list->first = NULL;
        }
        else
        {
            element->next->prev = element->prev;
            element->prev->next = element->next;

            if(element == list->last)
                list->last = element->prev;

            if(element == list->first)
                list->first = element->next;
        }
        element->next = NULL;
        element->prev = NULL;

        list->nb--;
    }

    return element;

}

template<typename T>
void reverse(struct List<T> *list)
{
    struct List<T> *computed = initList<T>();
    T *inserted = NULL;

    while(list->nb)
    {

        inserted = popElement(list, list->last->id);
        addElement(computed, inserted);
    }

    *list = *computed;
    delete computed;
}

template<typename T>
void insertElement(struct List<T> *list, T* element_, int id)
{
    T *element = getElement(list, id);
    T* next = NULL;

    if(element)
    {
        next = element->next;

        element_->id = list->last_id;
        list->last_id++;
        
        element->next = element_;
        element_->prev = element;

        next->prev = element_;
        element_->next = next;
 
        list->nb++;
    }

}

//initilises a node with (x, y) cartesians coordinates
struct Node* initNode(float x, float y, int type);
//initilises a segment compounded of node1 and node2
struct Segment* initSegment(Node *node1, Node *node2);
//initilises a triangular element compounded of node1, node2 and node3
struct Element* initElement(Node *node1, Node *node2, Node *node3);
//retrieves segment's norm
float getDistance(struct Segment *segment);
//retrieves segment's norm declares with 2 nodes
float getDistance(struct Node *node1, struct Node *node2);
//retrives polygon perimeter compounded of n segments
float getPerimeter(struct List<struct Segment> *segments);
//from a polygon generates another polygon with n subdivisions
void subdiviseOutline(struct List<struct Segment> *segments, struct List<struct Node> *nodes, int n);
//cuts a segment in n parts
struct Segment* subdivise(struct Segment *segment_, float perimiter, int n, struct List<struct Node> *nodes, struct List<struct Segment> *segments);
//reorganises a list of segment to make a polygon
void sortSegment(struct List<struct Segment> *segments);
//determines if polygon turn in couter-clockwise or clockwise direction
bool travelingDirection(struct List<struct Segment> *segments);
//generates a new node along the segment using a distance
struct Node* generateNewPointOnSegment(struct Segment *segment, float length, float distance);
//computes angle between two segments in CW direction
float getAngle(struct Segment* segment1, struct Segment* segment2);
//Meshes a geometry using cavendish algorithm
bool Cavendish(struct List<struct Segment> *segments, struct List<struct Node> *nodes, struct List<struct Element> *elements);
//retrieves the smallest angle in a polygon, segment initialises as first segment of min angle
float minAngle(struct List<struct Segment> *segments, struct Segment *segment);