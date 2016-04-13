#ifndef __ROUTE_H__
#define __ROUTE_H__
#include <vector>
#include <deque>
#undef NULL
#if defined(__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#define MAX_OUTWARD_PATH_NUMBERS 8
#define MAX_NODE_NUMBERS 600
#define MAX_MIDNODE_NUMBERS 50
#define SHORT_INF 0x3f3f
struct path 
{
	unsigned short index;
	unsigned short from;
	unsigned short to;
	unsigned short weight;
};

struct path_2
{
	unsigned short index;
	unsigned short from;
	unsigned short to;
	unsigned short weight;
	unsigned short nodenum_on_path;
	unsigned short *node_on_path;
};

struct node_new {
	// Constant
	unsigned int outward_paths_size : 4;
	struct path* outward_paths[MAX_OUTWARD_PATH_NUMBERS];
	//bool is_via_node : 1;

	//// Variable
	//bool has_been_visited : 1;
	//unsigned int current_outward_path_index : 4;
};
class node
{
public:
	unsigned short value;
	node *next;

	node(unsigned short a,node *b)
	{
		value=a;
		next=b;
	}
};

class stacks
{
public:
	node *top;
	stacks(node *a=NULL)
	{
		top=NULL;
	}
	void push(unsigned short a)
	{
		if (top==NULL)
			top =new node(a,NULL);
		else top=new node(a,top);
	}
	void pop()
	{
		node *b=top;
		top=top->next;
		delete b;
	}
	void clear()
	{
		while(top!=NULL)
		{
			pop();
		}
	}
}; //保存已加入路径结点的栈
int isrepeat(unsigned short teminalpath[600], int caculate);
void find(struct path_2 *newpath, stacks &stack1, int visitm[50], int m, int total,int **repeat);
void char_int(char character,char character1,char character2,short unsigned int &number);
void DFS(unsigned short v0,unsigned short [MAX_NODE_NUMBERS][MAX_NODE_NUMBERS],unsigned short [MAX_NODE_NUMBERS]);
void Dijkstra(unsigned short v0,unsigned short a[],unsigned short M,unsigned short max_node_index,unsigned short matrix[][MAX_NODE_NUMBERS], struct node_new nodeinfo[MAX_NODE_NUMBERS], unsigned short prev[MAX_NODE_NUMBERS], unsigned short dislist[MAX_NODE_NUMBERS],const unsigned short *const end=NULL,const unsigned short * const begin=NULL);
void Add_node(unsigned short prev_of_pt[],bool visitedpoint[],int begin,int end);
void Remove_node(unsigned short prev_of_pt[],bool visitedpoint[],int begin,int end);
bool push_a_element(short *newlastpopup, std::vector<unsigned short> &inarray, int &mdpoint_instack, unsigned short demand_path[], int **repeat, int &cost, int MinCost,std::deque<unsigned short> &outarray, struct path_2 *NewPathMap, std::vector<unsigned short>&NewpathInArray, short **MapFromIndexToNewGraph);
void pop_a_element(short *newlastpopup, std::vector<unsigned short> &inarray, int &mdpoint_instack, int &cost, std::deque<unsigned short> &outarray, struct path_2 *NewPathMap, std::vector<unsigned short>&NewpathInArray);
bool Repass_node(unsigned short prev_of_pt[MAX_NODE_NUMBERS], bool visitedpoint[], int begin, int end);
void search_route(char *graph[5000], int edge_num, char *condition);
void collectpathbetwoonKeyNode(unsigned short pathnummatrix[][MAX_NODE_NUMBERS],unsigned short prev_of_pt[],int begin,int end,stacks &stack);
void CollectAllPathInfo(unsigned short pathnummatrix[][MAX_NODE_NUMBERS],unsigned short prev_of_middlept[][MAX_NODE_NUMBERS],int MapFromIndexToMiddlePointIndex[],unsigned short midnode_list[],int nodenum,int end,stacks &stack);
void search(stacks &stack1,stacks &stack_2,unsigned short [MAX_NODE_NUMBERS][MAX_NODE_NUMBERS],unsigned short [MAX_NODE_NUMBERS],unsigned short [MAX_NODE_NUMBERS],
			unsigned short [50],int m, short demand_start,unsigned short demand_end);
bool PathComflict(std::vector<unsigned short>&NewpathInArray, unsigned short newpathindex, int **repeat);
#endif
