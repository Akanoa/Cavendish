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
    node->tag = 0;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

struct Segment* initSegment(Node *node1, Node *node2)
{
    struct Segment *segment = new struct Segment;
    segment->node1 = node1;
    segment->node2 = node2;
    segment->id    = 0;
    segment->next  = NULL;
    segment->prev = NULL;
    segment->tag = 0;

    return segment;
}


struct Element* initElement(Node *node1, Node *node2, Node *node3)
{
    struct Element *element = new struct Element;
    element->node1 = node1;
    element->node2 = node2;
    element->node3 = node3;
    element->id    = 0;
    element->next  = NULL;
    element->prev  = NULL;

    return element;
}

float getAngle(struct Segment* segment1, struct Segment* segment2)
{
    float x1 = segment1->node2->x - segment1->node1->x;
    float y1 = segment1->node2->y - segment1->node1->y;
    float x2 = segment2->node2->x - segment2->node1->x;
    float y2 = segment2->node2->y - segment2->node1->y;
    float dot = x1*x2 + y1*y2;
    float det = x1*y2 - y1*x2;
    return atan2(det, dot);
}

float getAngle(struct Segment* segment)
{
    return atan2(segment->node2->y - segment->node1->y, segment->node2->x - segment->node1->x);
}

void sortSegment(struct List<struct Segment> *segments, struct List<struct Element> *arcs)
{
    //get how many segments must be sorted
    int left_segments = segments->nb-1;
    struct List<struct Segment> *computed = initList<struct Segment>();
    struct Segment *element = NULL;
    struct Segment *inserted = NULL;
    struct Node *swap = NULL; 
    struct Segment *last_computed = segments->first;
    struct Element *arc = arcs->first;

    //add potentials arcs
    cout << "nb arcs" << arcs->nb << endl;
    for(int i=0; i< arcs->nb; i++)
    {
        inserted = initSegment(arc->node1, arc->node2);
        addElement(segments, inserted);
        inserted->tag = 1;
        left_segments++;
        arc = arc ->next;
    }

    inserted = popElement(segments, segments->first->id);
    addElement(computed, inserted);

    while(left_segments)
    {
        element = segments->first;
        for(int i=0; i<segments->nb; i++)
        {
            if(last_computed->node2->id == element->node1->id)
            {
                inserted = popElement(segments, element->id);
                addElement(computed, inserted);
                last_computed = element;
                break;
            }
            else if(last_computed->node2->id == element->node2->id)
            {
                swap = element->node2;
                element->node2 = element->node1;
                element->node1 = swap;
                inserted = popElement(segments, element->id);
                addElement(computed, inserted);
                last_computed = element;
                break;
            }
            element = element->next;
        };
        left_segments--;
    }

    *segments = *computed;
    delete computed;

    if(!travelingDirection(segments))
    {
        reverse(segments);

        element = segments->first;
        for(int i=0; i<segments->nb; i++)
        {
            swap = element->node2;
            element->node2 = element->node1;
            element->node1 = swap;
            element = element->next;
        }
    }


}

bool travelingDirection(struct List<struct Segment> *segments)
{
    float sum = 0.0;

    struct Segment *seg = segments->first;

    for(int i=0; i<segments->nb; i++)
    {
        sum += getAngle(seg, seg->next);
        seg = seg->next;
    }

    sum += getAngle(segments->last, segments->first);

    return (sum>=0)?true:false; 
}

float minAngle(struct List<struct Segment> *segments, struct Segment *segment)
{
    float min = PI;
    float tmp = min;
    struct Segment *seg=segments->first, *found= NULL;

    for(int i=0; i<segments->nb; i++)
    {
        tmp =  PI - getAngle(seg, seg->next);
        if(tmp > 0 && tmp<min)
        {
            found = seg;
            min = tmp;
        }
        seg = seg->next;
    }

    if(found)
        *segment = *found;

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
    }while((segment = segment->next)!=segments->last);

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
    int nb_points = n;
    struct Segment *segment = segments->first;
    float perimeter = getPerimeter(segments);

    int nb_segment = segments->nb; 

    for(int i=0; i< nb_segment; i++)
    {
        cout << "segment: " << segment->id << endl << "\tnode 1: "<< segment->node1->id << endl << "\tnode 2: "<< segment->node2->id << "\ttag: "<< segment->tag << endl << endl;
        segment = subdivise(segment, perimeter, nb_points, nodes, segments);
        cout << "segment: " << segment->id << endl << "\tnode 1: "<< segment->node1->id << endl << "\tnode 2: "<< segment->node2->id << "\ttag: "<< segment->tag << endl << endl;
        cout << "-------------" << endl;
    }

}

void subdiviseArc(struct List<struct Segment> *segments, struct List<struct Node> *nodes, struct List<Element> *arcs, float delta)
{
    reverse(arcs);
    struct Element *arc = arcs->first;
    struct Segment *seg = segments->first, *seg_tmp = NULL;
    struct Segment *inserted = NULL, *seg1 = NULL, *seg2 = NULL;
    float beta1=0.0f, beta2=0.0f, beta=0.0f, gamma=0.0f, r=0.0f;
    float x_c = 0.0f, y_c=0.0f;
    struct Node *node = NULL;

    int original_segments_nb = segments->nb;


    for(int i=0; i < original_segments_nb; i++)
    {

        if(seg->tag)
        {
            seg1 = initSegment(arc->node3, seg->node1);
            seg2 = initSegment(arc->node3, seg->node2);

            beta1 = getAngle(seg1);
            beta2 = getAngle(seg2);
            beta  = getAngle(seg1, seg2);

            r = getDistance(seg1);
            gamma = acos((2.0*r*r-2.0*delta*delta)/(2.0*r*r));

            // cout << "delta: " << delta << endl;
            // cout << "r: " << r << endl;
            // cout << "beta1: " << beta1*180/PI << " deg" << endl;
            // cout << "beta2: " << beta2*180/PI << " deg" << endl;
            // cout << "beta: " << beta*180/PI << " deg" << endl;
            // cout << "gamma: " << gamma*180/PI << " deg" << endl;

            int n_segs = (int)round(beta/gamma);
            gamma = beta/((float)n_segs);

            for(int j=1; j < n_segs+1; j++)
            {

                x_c = arc->node3->x + r*cos(beta1+gamma*j);
                y_c = arc->node3->y + r*sin(beta1+gamma*j);

                if(j != n_segs)
                {
                    node = initNode(x_c, y_c, ORIGINAL);
                    addElement(nodes, node);
                }
                else
                    node = arc->node2;

                inserted = initSegment(seg->prev->node2, node);
                insertElement(segments, inserted, seg->prev->id);
                cout << "inserted segment: " << inserted->id << endl << "\tnode 1: "<< inserted->node1->id << endl << "\tnode 2: "<< inserted->node2->id << endl << endl;
                inserted->tag = 0;
                seg = inserted->next;
            }

            cout << "##############" << endl;
            arc = arc->next;
        }
        seg = seg->next;
    }

    seg = segments->first;
    for(int i=0; i < segments->nb; i++)
    {
        if(seg->tag)
        {
            seg_tmp = seg->next;
            popElement(segments, seg->id);
            seg = seg_tmp;
        }
        else
            seg = seg->next;
    }

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


    if(n_segs-1>0)
    {    
        for(int i=0; i<n_segs-1; i++)
        {
            //Node generation
            node = generateNewPointOnSegment(segment_, L, delta*(i+1));
            addElement(nodes, node);

            //Segment generation
            segment = initSegment(start, node);
            insertElement(segments, segment, last_segment->id);

            last_segment = segment;
            start = node;

        }
    
        segment = initSegment(start, segment_->node2);
        insertElement(segments, segment, last_segment->id);

        delete popElement(segments, segment_->id);
    }

    return last_segment->next->next;
}

bool Cavendish(struct List<struct Segment> *segments, struct List<struct Node> *nodes, struct List<struct Element> *elements, int iter)
{
    struct Segment *segment = new struct Segment, *seg1 = NULL, *seg2 = NULL, *seg=NULL;
    struct Segment *generated = NULL;
    struct Node *node = NULL;
    float angle = 0.0f;
    float r = 0.0f, a=0.0f, b=0.0f, x_int=0.0f, y_int=0.0f, diag1 = 0.0f, diag2 = 0.0f;
    float x_a=0.0f, y_a=0.0f, x_b=0.0f, y_b=0.0f, x_d =0.0f, y_d=0.0f;
    int state;
    int i = 0;
    float ratio = sqrt(3)/2.0;

    do
    {
        if(i == iter && iter>0)
            return true;
        angle = minAngle(segments, segment)*180/PI;

        //switch between cases
        if(angle < 90)
            state = FIRST_CASE;
        else if (angle >= 90 && angle < 120)
            state = SECOND_CASE;
        else if (angle >= 120 && angle < 180)
            state = THIRD_CASE;
        else if (angle>=180)
            return false;

        if (segments->nb == 4)
            state = END_CASE;

        switch(state)
        {
            case FIRST_CASE:
                // cout << "first" << endl;
                //add new element
                addElement(elements, initElement(segment->node1, segment->node2, segment->next->node2));
                //generate new segment between first node of first segment and second node of second segment
                generated = initSegment(segment->node1, segment->next->node2);
                insertElement(segments, generated, segment->id);
                seg1 = segments->first;

                seg1 = popElement(segments, segment->id);
                seg2 = popElement(segments, segment->next->id);

                break;
            case SECOND_CASE:
                // cout << "second" << endl;
                //compute NN* distance
                r =  (getDistance(segment->prev)+2*(getDistance(segment)+getDistance(segment->next))+getDistance(segment->next->next))/6.0;
                //a = r/l1
                a = r/getDistance(segment);
                //b = r/l2
                b = r/getDistance(segment->next);
                //x_d = r/l1*x_a + r/l2*x_c + x_b*(1-r/l1-r/l2) 
                x_int = a*segment->node1->x + b*segment->next->node2->x + (segment->node2->x)*(1-a-b);
                //x_d = r/l1*x_a + r/l2*x_c + x_b*(1-r/l1-r/l2) 
                y_int = a*segment->node1->y + b*segment->next->node2->y + (segment->node2->y)*(1-a-b);

                node = initNode(x_int, y_int, ORIGINAL);

                //append new node
                addElement(nodes, node);

                //generate 2 new elements
                addElement(elements, initElement(segment->node1, segment->node2, node));
                addElement(elements, initElement(segment->node2, segment->next->node2, node));

                //generate new outline
                generated = initSegment(segment->node1, node);
                insertElement(segments, generated, segment->prev->id);

                generated = initSegment(node, segment->next->node2);
                insertElement(segments, generated, segment->id);

                //remove old segment to create new computed outline
                seg1 = popElement(segments, segment->id);
                seg2 = popElement(segments, segment->next->id);

                delete seg1;
                delete seg2;
                break;
            case THIRD_CASE:
                // cout << "third" << endl;
                //compute NN* distance
                seg =  getDistance(segment)<getDistance(segment->next)?segment:segment->next;

                x_a = seg->node1->x;
                x_b = seg->node2->x;
                y_a = seg->node1->y;
                y_b = seg->node2->y;

                x_d = (x_a + x_b)/2.0;
                y_d = (y_a + y_b)/2.0;

                a = getDistance(seg);

                x_int = ratio*(y_a - y_b) + x_d;
                y_int = ratio*(x_b - x_a) + y_d;

                node = initNode(x_int, y_int, ORIGINAL);

                //append new node
                addElement(nodes, node);

                //generate 2 new elements
                addElement(elements, initElement(segment->node1, segment->node2, node));
                addElement(elements, initElement(segment->node2, segment->next->node2, node));

                //generate new outline
                seg1 = initSegment(segment->node1, node);
                insertElement(segments, seg1, segment->prev->id);

                seg2 = initSegment(node, segment->next->node2);
                insertElement(segments, seg2, segment->id);

                //remove old segment to create new computed outline
                seg1 = popElement(segments, segment->id);
                seg2 = popElement(segments, segment->next->id);

                delete seg1;
                delete seg2;
                break;
            case END_CASE:
                //get 2 diagonales
                diag1 = getDistance(initSegment(segment->node1, segment->next->node2));
                diag2 = getDistance(initSegment(segment->node2, segment->prev->node2));

                if(diag1>diag2)
                {
                    // cout << "case 1" << endl;
                    addElement(elements, initElement(segment->node1, segment->node2, segment->next->next->node2));
                    addElement(elements, initElement(segment->node2, segment->next->node2, segment->next->next->node2));
                }
                else
                {

                    addElement(elements, initElement(segment->node1, segment->node2, segment->next->node2));
                    addElement(elements, initElement(segment->node1, segment->next->node2, segment->prev->node1));
                }
                popElement(segments, segments->first->id);
                popElement(segments, segments->first->id);
                popElement(segments, segments->first->id);
                popElement(segments, segments->first->id);
                break;
            default:
                break;
        }
        i++;
    }while((segments->nb));

    return true;

}