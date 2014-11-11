#include <iostream>

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
struct List<T>* initList()
{
    struct List<T> *list = new List<T>;
    list->nb= 0;
    list->first = NULL;

    return list;
}

//nodes stuff
struct Node* initNode(float x, float y, int type);

//segments stuff
struct Segment* initSegment(Node *node1, Node *node2,int type);


void sortSegment(struct List<struct Segment> *segments);
