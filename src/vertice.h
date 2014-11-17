#include <iostream>
#include <cmath>

enum type {ORIGINAL, GENERATED};

struct Node
{
    float x;
    float y;
    int id;
    int type;
    struct Node *next;
};

struct Segment
{
    Node *node1;
    Node *node2;
    int id;
    int zone;
    int type;
    struct Segment *next;
};

template<typename T>
struct List
{
    int nb;
    T *first;
};

//templates stuff


template<typename T>
struct List<T>* initList()
{
    struct List<T> *list = new List<T>;
    list->nb= 0;
    list->first = NULL;

    return list;
}

template<typename T>
void addElement(struct List<T> *list, T *element_)
{
    //find last node from nodes
    T *element = list->first;

    list->nb++;
    element_->id = list->nb;

    if(element)
    {
        while(element->next)
            element = element->next;

        element->next = element_;
    }
    else
    {
        list->first = element_;
    }
}


template<typename T>
T* getElement(struct List<T> *list, int id)
{
    T *element = list->first;

    if(list->first)
    {
        while(element->next)
        {
            if(element->id == id)
                break;
            element = element->next;
        }
    }

    return element;
}

template<typename T>
T* popElement(struct List<T> *list, int id)
{
    T *element = NULL;
    T *previous = list->first;
    for(element = list->first; element; element = element->next)
    {
        if(element->id == id)
        {
            if(element == list->first)
                list->first = element->next;

            else
                previous->next = element->next;

            break;
        }

        previous = element;
    }

    if(element)
    {
        element->next = NULL;
    }
    
    return element;

}

template<typename T>
void reverse(struct List<T> *list)
{
    struct List<T> *computed = initList<T>();
    T *element = NULL, *inserted = NULL;

    while(list->first)
    {
        element = list->first;
        while(element->next)
            element = element->next;
        inserted = popElement(list, element->id);
        addElement(computed, inserted);
    }

    *list = *computed;
    delete computed;
}

template<typename T>
void insertElement(struct List<T> *list, T* element_, int id)
{
    T *element = NULL;
    T *next = NULL;
    for(element = list->first; element; element = element->next)
    {
        if(element->id == id)
        {
            list->nb++;
            element->next = element_;
            element_->next = next;
            element_->id = list->nb;

            break;
        }

        if(element->next->next)
        {
            list->nb++;
            next = element->next->next;
        }
    }
}


//nodes stuff
struct Node* initNode(float x, float y, int type);

//segments stuff
struct Segment* initSegment(Node *node1, Node *node2,int type);
float getDistance(struct Segment *segment);
float getDistance(struct Node *node1, struct Node *node2);
float getPerimeter(struct List<struct Segment> *segments);
void subdiviseOutline(struct List<struct Segment> *segments, struct List<struct Node> *nodes, int n);
void subdivise(struct Segment *segment_, float perimiter, int n, struct List<struct Node> *nodes, struct List<struct Segment> *segments, struct Segment *last_inserted);
void sortSegment(struct List<struct Segment> *segments);
bool travelingDirection(struct Segment *seg1, struct Segment *seg2);
struct Node* generateNewPointOnSegment(struct Segment *segment, float length, float distance);
