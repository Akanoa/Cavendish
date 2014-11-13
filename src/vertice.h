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
void addElement(struct List<T> *list, T *element)
{
    //find last node from nodes
    T *element_ = list->first;

    if(list->first)
    {
        while(element_->next)
            element_ = element_->next;

        list->nb++;
        element->id = list->nb;
        element_->next = element;
    }
    //This is the first element of list
    else
    {
        list->nb++;
        element->id = list->nb;
        list->first = element;
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
    T *element  = list->first;
    T *previous = element;

    //if list owns at least one element
    if(list->first)
    {
        if(list->first->id == id)
        {
            element = list->first;
            list->first = element->next;
            list->nb--;
        }
        else
        {
            do
            {
                if(element->id == id)
                {
                    previous->next = element->next;
                    list->nb--;
                    break;
                }
                previous = element;

            }while((element = element->next));
        }
    }
    return element;

}

template<typename T>
void reverse(struct List<T> *list)
{
    struct List<T> *computed = initList<T>();
    T *element = NULL, *inserted = NULL;

    while(list->nb)
    {
        element = list->first;
        while(element->next)
            element = element->next;
        inserted = popElement(list, element->id);
        addElement(computed, inserted);
        inserted->next = NULL;
    }

    *list = *computed;
    delete computed;
}


//nodes stuff
struct Node* initNode(float x, float y, int type);

//segments stuff
struct Segment* initSegment(Node *node1, Node *node2,int type);
float getDistance(struct Segment *segment);
float getDistance(struct Node *node1, struct Node *node2);
float getPerimeter(struct List<struct Segment> *segments);
void subdiviseOutline(struct List<struct Segment> *segments, int n);
struct List<struct Segment> * subdivise(struct Segment *segment, float perimiter, int n);
void sortSegment(struct List<struct Segment> *segments);
bool travelingDirection(struct Segment *seg1, struct Segment *seg2);

