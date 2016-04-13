#include "route.h"
#include "lib_record.h"
#include "lib_time.h"
#include <string.h>  
#include <stdlib.h>
#include <stdio.h>
#define _Debug
static void parse_paths(char* topo[], unsigned int edge_num, struct path* paths,unsigned short matrix[][MAX_NODE_NUMBERS],unsigned short &maxnodeindex,unsigned short pathnummatrix[][MAX_NODE_NUMBERS],struct node_new nodeinfo[]);
static void parse_demand(char* demand,unsigned short vnode[], unsigned short *start_ptr, unsigned short* end_ptr, unsigned short* via_nodes_count_ptr) ;

//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{
	struct node_new nodeinfo[MAX_NODE_NUMBERS];
	memset(nodeinfo, 0, sizeof(nodeinfo));
	cal_usedtime(false);
	unsigned short matrix[MAX_NODE_NUMBERS][MAX_NODE_NUMBERS];     //邻接矩阵
	unsigned short visit[MAX_NODE_NUMBERS];          //标记是否被访问
	unsigned short demand_path[50];    //要求路径经过的点
	memset(matrix, SHORT_INF, sizeof(matrix));
	memset(visit, 0, sizeof(visit));

	unsigned short pathnummatrix[MAX_NODE_NUMBERS][MAX_NODE_NUMBERS];     //邻接矩阵
	memset(pathnummatrix, SHORT_INF, sizeof(pathnummatrix));
	//读取边
	struct path* paths = (struct path*) malloc(edge_num * sizeof(struct path));
	unsigned short max_node_index = 0;//所有点的最大序号
	parse_paths(topo, (unsigned int)edge_num, paths, matrix, max_node_index, pathnummatrix, nodeinfo);

	unsigned short start_index, end_index, via_nodes_count;
	parse_demand(demand, demand_path, &start_index, &end_index, &via_nodes_count);

	//int NewMatrix[MAX_NODE_NUMBERS][MAX_NODE_NUMBERS];     //邻接矩阵
	//memset(NewMatrix,SHORT_INF,sizeof(NewMatrix));  

	unsigned short A[MAX_NODE_NUMBERS][MAX_NODE_NUMBERS];//表示从特殊点i到j的距离
	memset(A, SHORT_INF, sizeof(A));

	unsigned short prev_of_middlept[MAX_NODE_NUMBERS][MAX_NODE_NUMBERS];
	memset(prev_of_middlept, SHORT_INF, sizeof(prev_of_middlept));
	//unsigned short prev_of_startpt[MAX_NODE_NUMBERS];//从起点到指定序号的中间节点的开环最短路径的上一个点
	//int dis_of_startpt[MAX_NODE_NUMBERS];//从起点到指定序号的中间节点的开环最短路径的距离
	Dijkstra(start_index, demand_path, via_nodes_count, max_node_index, matrix, nodeinfo, prev_of_middlept[0], A[0], &end_index);//求起点到中途点的最短距离，不经过其他中途点

																													   //int dis_of_middlept[MAX_NODE_NUMBERS][MAX_NODE_NUMBERS];
	for (int i = 0; i<via_nodes_count; ++i)
	{
		Dijkstra(demand_path[i], demand_path, via_nodes_count, max_node_index, matrix, nodeinfo, prev_of_middlept[i + 1], A[i + 1], &end_index, &start_index);
	}
	//cal_usedtime(true);

	//sll 2016.4.7 计算边的冲突关系
	/*构造起点、终点、中间点的图*/
	int mm = via_nodes_count*via_nodes_count + via_nodes_count;
	struct path_2 *NewPathMap= (struct path_2 *)malloc(mm * sizeof(struct path_2));
	memset(NewPathMap, 0, sizeof(mm * sizeof(struct path_2)));

	short **MapFromIndexToNewGraph = (short**)malloc((via_nodes_count + 1) * sizeof(short*));
	for (int i = 0; i < via_nodes_count + 1; ++i)
	{
		MapFromIndexToNewGraph[i] = (short*)malloc((via_nodes_count + 2) * sizeof(short));
		memset(MapFromIndexToNewGraph[i], SHORT_INF, (via_nodes_count + 2) * sizeof(short));
	}
		
	//int MapFromIndexToMiddlePointIndex[max_node_index+1]={-1};
	//unsigned short *edge1 = (unsigned short *)malloc(mm * sizeof(unsigned short));           //边的名称
	//memset(edge1, 0, mm * sizeof(unsigned short));
	//unsigned short *edgestart1 = (unsigned short *)malloc(mm * sizeof(unsigned short));      //起点
	//memset(edgestart1, 0, mm * sizeof(unsigned short));
	//unsigned short *edgeend1 = (unsigned short *)malloc(mm * sizeof(unsigned short));        //终点
	//memset(edgeend1, 0, mm * sizeof(unsigned short));
	//unsigned short *edgecost1 = (unsigned short *)malloc(mm * sizeof(unsigned short));       //边的权重
	//memset(edgecost1, 0, mm * sizeof(unsigned short));
	//unsigned short *edgetopo1[3000];       //每条边经过的路径点
	//int *edgecaculate1 = (int *)malloc(mm * sizeof(int));       //每条边经过的点数
	//memset(edgecaculate1, 0, sizeof(mm * sizeof(int)));

	int total = 0;
	for (int i = 0; i<1; i++)
	{
		for (int j = 0; j<via_nodes_count; j++)
		{
			if (A[0][demand_path[j]] == SHORT_INF)
				continue;
			NewPathMap[total].index = total;
			NewPathMap[total].weight = A[0][demand_path[j]];
			NewPathMap[total].from = start_index;
			NewPathMap[total].to = demand_path[j];
			MapFromIndexToNewGraph[0][j + 1] = total;
			int tempend = demand_path[j];
			int num=0;
			if (prev_of_middlept[0][tempend] != SHORT_INF)
			{
				while (tempend != start_index)
				{
					++num;
					tempend = prev_of_middlept[0][tempend];
				}
			}
			NewPathMap[total].node_on_path = (unsigned short *)malloc(num * sizeof(unsigned short));
			tempend = demand_path[j];
			for (int p = 0; p<num; p++)
			{
				*(NewPathMap[total].node_on_path + p) = tempend;
				tempend = prev_of_middlept[0][tempend];
			}
			NewPathMap[total].nodenum_on_path = num;
			total++;
		}
	}
	for (int i = 0; i<via_nodes_count; i++)
	{
		for (int j = 0; j<via_nodes_count+1; j++)
		{
			if (j != i)
			{
				unsigned short tempend;
				if (j == via_nodes_count)
					tempend = end_index;
				else
					tempend = demand_path[j];
				if (A[i + 1][tempend] == SHORT_INF)
					continue;
				NewPathMap[total].to = tempend;
				NewPathMap[total].index = total;
				NewPathMap[total].weight = A[i+1][demand_path[j]];
				NewPathMap[total].from = demand_path[i];
				MapFromIndexToNewGraph[i+1][j + 1] = total;
				int num = 0;
				if (prev_of_middlept[i + 1][tempend] != SHORT_INF)
				{
					while (tempend != demand_path[i])
					{
						++num;
						tempend = prev_of_middlept[i+1][tempend];
					}
				}
				NewPathMap[total].node_on_path = (unsigned short *)malloc(num * sizeof(unsigned short));
				tempend = NewPathMap[total].to;
				for (int p = 0; p<num; p++)
				{
					*(NewPathMap[total].node_on_path + p) = tempend;
					tempend = prev_of_middlept[i+1][tempend];
				}
				NewPathMap[total].nodenum_on_path = num;
				total++;
			}
		}
	}
	//record_result(total);

	stacks stack1(NULL);
	int visitm[50] = { 0 };    //中间点是否被访问
	int **repeat1 = (int **)malloc(total * sizeof(int *));
	for (int i = 0; i < total; ++i)
	{
		repeat1[i] = (int *)malloc(total * sizeof(int));
		memset(repeat1[i], 0, sizeof(total * sizeof(int)));
	}
		
	find(NewPathMap, stack1, visitm, via_nodes_count, total, repeat1);






	//cal_usedtime(true);
	unsigned short new_demand_path[51];    //要求路径经过的点
	new_demand_path[0] = start_index;
	for (int i = 0; i<via_nodes_count; ++i)
	{
		new_demand_path[i + 1] = demand_path[i];
	}
	//根据局部最优，
	unsigned short new_via_nodes_count = via_nodes_count + 1;

	int cost = 0;//目前的cost
	bool *visited_point = (bool*)malloc((max_node_index + 1) * sizeof(bool));
	memset(visited_point, false, (max_node_index + 1) * sizeof(bool));
	int MinCost = SHORT_INF;//最优解

	unsigned short *final_midnode_list = (unsigned short*)malloc((new_via_nodes_count) * sizeof(unsigned short));
	memset(final_midnode_list, false, (new_via_nodes_count) * sizeof(unsigned short));
	//int final_midnode_list[new_via_nodes_count];//包括起点

	int mdpoint_instack = 0;//不包括起点
	int *MapFromIndexToMiddlePointIndex = (int*)malloc((max_node_index + 1) * sizeof(int));
	memset(MapFromIndexToMiddlePointIndex, -1, (max_node_index + 1) * sizeof(int));
	//int MapFromIndexToMiddlePointIndex[max_node_index+1]={-1};
	for (int i = 0; i<new_via_nodes_count; ++i)
		MapFromIndexToMiddlePointIndex[new_demand_path[i]] = i;
	bool finished = false;
	int available_path_num = 0;

	//sll 2016.4.2 
	//inarray和outarray中元素加起来，就是0~new_via_nodes_count-1这些个元素
	std::vector<unsigned short> inarray;//用于存储已经遍历到的中途点的序号（在new_demand_path中）
	std::deque<unsigned short> outarray;//未遍历到的中途点的序号（在new_demand_path中）
	for (unsigned short i = 0; i<new_via_nodes_count; ++i)
		outarray.push_back(i);
	std::vector<unsigned short> newpath_inarray;

	//用于存储弹出的数据在outarray中的次序
	short *newlastpopup = (short *)malloc(new_via_nodes_count * sizeof(short));
	memset(newlastpopup, -1, new_via_nodes_count * sizeof(short));

	inarray.push_back(outarray[0]);
	outarray.erase(outarray.begin());
	++newlastpopup[0];
	//sll 2016.4.8 测试
	int u=5;
	if(u<new_via_nodes_count-1)
	{
		//newlastpopup[1]+=u;
		unsigned short temp=outarray[0];
		outarray[0]=outarray[u];
		outarray[u]=temp;
	}

	/*visited_point[start_index] = true;*/

	while (!finished)
	{
		if (cal_usedtime()>20000)
		{
			break;
		}
		if (push_a_element(newlastpopup, inarray, mdpoint_instack, new_demand_path, repeat1, cost,MinCost, outarray,NewPathMap, newpath_inarray,MapFromIndexToNewGraph))
		{
			//若n个数都压入队
			//判断是否为有效解
			//有效解则压入结果
			//不管是否为有效解，都要把最后2个元素弹出
			if (mdpoint_instack == new_via_nodes_count - 1)
			{
				int tempbeginindex_in_midnode = inarray.back();
				int templineindex = MapFromIndexToNewGraph[tempbeginindex_in_midnode][new_via_nodes_count];
				unsigned short tempcost = A[tempbeginindex_in_midnode][end_index];
				if (tempcost<SHORT_INF)//查找l->demand_path[i]的连接性
				{
					if (!PathComflict(newpath_inarray, templineindex, repeat1))//一个有效解
					{
						//把pointlist[l][demand_path[i]]中元素的序号在visited_point列表中对应元素全都至为true；
						//Add_node(prev_of_middlept[MapFromIndexToMiddlePointIndex[l]],visitedpoint,l,q);					
						cost += tempcost;
						if (cost<MinCost)//找到或者更新有用解
						{
							for (int i = 0; i<new_via_nodes_count; ++i)
							{
								final_midnode_list[i] = new_demand_path[inarray[i]];
							}
							MinCost = cost;
							++available_path_num;
							#ifdef _Debug
							printf("Found an Answer With COST%d", MinCost);
							cal_usedtime(true);
							#endif
						}
						cost -= tempcost;
					}
				}
				//不管是否有解
				pop_a_element(newlastpopup, inarray, mdpoint_instack, cost,outarray, NewPathMap, newpath_inarray);
				pop_a_element(newlastpopup, inarray, mdpoint_instack, cost, outarray, NewPathMap, newpath_inarray);
			}
		}
		else
		{
			if (mdpoint_instack == 0)//队为空，遍历结束
				finished = true;
			else
			{
				pop_a_element(newlastpopup, inarray, mdpoint_instack, cost, outarray, NewPathMap, newpath_inarray);
			}
		}
	}

	//找到可行结果
	if (available_path_num)
	{
		/*for (int i = 0; i<new_via_nodes_count; ++i)
		{
			int templineindex;
			if(i==new_via_nodes_count-1)
				templineindex = MapFromIndexToNewGraph[final_midnode_list[i]][new_via_nodes_count];
			else
				templineindex = MapFromIndexToNewGraph[final_midnode_list[i]][final_midnode_list[i + 1]];
			int tempbegin = new_demand_path[final_midnode_list[i]];
			int tempend;
			for (int j = NewPathMap[templineindex].nodenum_on_path-1; j >=0; --j)
			{
				tempend = *(NewPathMap[templineindex].node_on_path + j);
				record_result(pathnummatrix[tempbegin][tempend]);
				tempbegin = tempend;
			}
		}*/
		stacks stack;
		CollectAllPathInfo(pathnummatrix, prev_of_middlept, MapFromIndexToMiddlePointIndex, final_midnode_list, new_via_nodes_count, end_index, stack);
		while (stack.top)
		{
			record_result(stack.top->value);
			stack.pop();
		}
	}
	//未找到可行结果，说明要么时间到了，要么完成搜索
	else if (finished)
	{
		//可能是题目4那种状况
		DFS(start_index, matrix, visit);      //深度优先遍历
		stacks stack1(NULL);       //建立栈
		stacks stack_2(NULL);    //用来存放通过指定点的路径     
		unsigned short stacknote[600];    //标记点是否入栈
		for (int i = 0; i<600; i++)
		{
			stacknote[i] = 0;
			if (visit[i] == 0)       //若是被剔除的点
				stacknote[i] = 1;
		}
		stack1.push(start_index);   //起点入栈
		stacknote[start_index] = 1;   //起点入栈标志
		search(stack1, stack_2, matrix, visit, stacknote, demand_path, via_nodes_count, start_index, end_index);


		stacks stack_target(NULL);       //最终选择路径
		unsigned short cost_target = SHORT_INF;      //最终的权重和
		unsigned short cost_target_tmp = 0;
		unsigned short edge_target[100];  //最终选出的路径
		unsigned short edge_target_tmp[100];
		for (int i = 0; i<100; i++)
		{
			edge_target[i] = 0;
			edge_target_tmp[i] = 0;
		}
		int edge_number_tmp = 0;
		int edge_number = 0;        //最终路径的条数
		while (stack_2.top != NULL)
		{
			while (stack_2.top->value != start_index)
			{
				stack_target.push(stack_2.top->value);
				stack_2.pop();
			}
			stack_target.push(stack_2.top->value);
			stack_2.pop();
			// while(stack_target.top!=NULL)
			// {
			//     record_result(stack_target.top->value);
			//     stack_target.pop();
			// }
			unsigned short n0, n1;
			while (stack_target.top != NULL)
			{
				n0 = stack_target.top->value;
				stack_target.pop();
				if (stack_target.top != NULL)
				{
					n1 = stack_target.top->value;
					for (int i = 0; i<edge_num; i++)
					{
						if ((paths[i].from == n0) && (paths[i].to == n1))
						{
							cost_target_tmp = cost_target_tmp + paths[i].weight;          //权重相加
							edge_target_tmp[edge_number_tmp++] = paths[i].index;   //边存储
						}
					}
				}
			}
			if (cost_target_tmp<cost_target)       //找到权重和更小的路径
			{
				cost_target = cost_target_tmp;
				edge_number = edge_number_tmp;
				for (int i = 0; i<100; i++)
				{
					edge_target[i] = 0;
				}
				for (int i = 0; i<edge_number_tmp; i++)
				{
					edge_target[i] = edge_target_tmp[i];
				}
			}
			for (int i = 0; i<100; i++)
			{
				edge_target_tmp[i] = 0;
			}
			cost_target_tmp = 0;
			edge_number_tmp = 0;
			stack_target.clear();
		}
		for (int i = 0; i<edge_number; i++)
			record_result(edge_target[i]);
	}
	//for(int i=0;i<edge_number;i++)
	//	record_result(edge_target[i]);

	free(paths);
	//free(visited_point);
	free(final_midnode_list);
	free(MapFromIndexToMiddlePointIndex);
	free(newlastpopup);
	for (int i = 0; i < total; ++i)
		free(repeat1[i]);
	free(repeat1);

	//free(edge1); //边的名称
	//free(edgestart1);//起点
	//free(edgeend1);//终点
	//free(edgecost1);//边的权重
	for (int i = 0; i < total; ++i)//每条边经过的路径点
		free(NewPathMap[i].node_on_path);
	free(NewPathMap);

	for (int i = 0; i < via_nodes_count + 1; ++i)
	{
		free(MapFromIndexToNewGraph[i]);
	}
	free(MapFromIndexToNewGraph);
	//free(edgetopo1);
}

void find(struct path_2 *newpath,stacks &stack1, int visitm[50], int m, int total, int **repeat1)
{
	for (int i = 0; i<total; i++)
	{
		for (int j = i+1; j<total; j++)
		{
			int caculate = newpath[i].nodenum_on_path + newpath[j].nodenum_on_path;
			unsigned short *topo = (unsigned short *)malloc(caculate*sizeof(unsigned short));
			for (int p = 0; p< newpath[i].nodenum_on_path; p++)
				topo[p] = *(newpath[i].node_on_path + p);
			for (int p = 0; p< newpath[j].nodenum_on_path; p++)
				topo[newpath[i].nodenum_on_path + p] = *(newpath[j].node_on_path + p);
			repeat1[i][j] = repeat1[j][i] = isrepeat(topo, caculate);
			//if (repeat1[i][j] == 10000)
			//	record_result(1);
			//else
			//	record_result(0);
			free(topo);
		}

	}
}

int isrepeat(unsigned short teminalpath[600], int caculate)
{
	for (int i = 0; i<caculate; i++)
	{
		for (int j = i + 1; j<caculate; j++)
			if (teminalpath[i] == teminalpath[j])
				return 1;
	}
	return 0;
}

/*字符转数字*/
void char_int(char character,char character1,char character2,short unsigned int &number)
{
	unsigned short num,num1,num2;
	switch(character)
	{
	case '0':num=0;break;
	case '1':num=1;break;
	case '2':num=2;break;
	case '3':num=3;break;
	case '4':num=4;break;
	case '5':num=5;break;
	case '6':num=6;break;
	case '7':num=7;break;
	case '8':num=8;break;
	case '9':num=9;break;
	default:num=0;
	}
	switch(character1)
	{
	case '0':num1=0;break;
	case '1':num1=1;break;
	case '2':num1=2;break;
	case '3':num1=3;break;
	case '4':num1=4;break;
	case '5':num1=5;break;
	case '6':num1=6;break;
	case '7':num1=7;break;
	case '8':num1=8;break;
	case '9':num1=9;break;
	default:num1=0;
	}
	switch(character2)
	{
	case '0':num2=0;break;
	case '1':num2=1;break;
	case '2':num2=2;break;
	case '3':num2=3;break;
	case '4':num2=4;break;
	case '5':num2=5;break;
	case '6':num2=6;break;
	case '7':num2=7;break;
	case '8':num2=8;break;
	case '9':num2=9;break;
	default:num2=0;
	}
	number=num*100+num1*10+num2;
}

/*深度优先遍历*/
void DFS(unsigned short v0,unsigned short matrix[MAX_NODE_NUMBERS][MAX_NODE_NUMBERS],unsigned short visit[MAX_NODE_NUMBERS])     
{
	visit[v0]=1;
	for(unsigned short j=0;j<MAX_NODE_NUMBERS;j++)
		if(matrix[v0][j]==1&&visit[j]!=1)
		{
			DFS(j,matrix,visit);
		}
}

//遍历搜索路径
void search(stacks &stack1,stacks &stack_2,unsigned short matrix[MAX_NODE_NUMBERS][MAX_NODE_NUMBERS],unsigned short visit[600],unsigned short stacknote[600],
unsigned short demand_path[50],int m, short demand_start,unsigned short demand_end)
{
	node *top1=stack1.top;
	unsigned short topvalue=top1->value;
	stacks stack_1(NULL);    //中间过渡
	stacks stack2(NULL);
	for(unsigned short j=0;j<MAX_NODE_NUMBERS;j++)
	{
		if(matrix[topvalue][j]==1&&stacknote[j]!=1)
		{
			stack1.push(j);
			stacknote[j]=1;
			if(j!=demand_end)
				search(stack1,stack_2,matrix,visit,stacknote,demand_path,m,demand_start,demand_end);
			else                     //当遍历到要求的终点，将当前终点压入栈stack2
			{
				stacks stack_tmp(NULL);
				while(stack1.top!=NULL)
				{
					stack_tmp.push(stack1.top->value);
					stack1.pop();
				}
				while(stack_tmp.top!=NULL)
				{
					stack1.push(stack_tmp.top->value);
					stack2.push(stack_tmp.top->value);
					//    record_result(stack2.top->value);
					stack_tmp.pop();
				}
				stacknote[stack1.top->value]=0;
				stack1.pop();

				while(stack2.top!=NULL)  //将通过指定点的路径找出来
				{
					int demand_pathmark[50];
					for(int i=0;i<50;i++)
					{
						demand_pathmark[i]=0;
					}
					while(stack2.top->value!=demand_start)
					{
						stack_1.push(stack2.top->value);
						for(int i=0;i<m;i++)
						{
							if(stack2.top->value==demand_path[i])
								demand_pathmark[i]=1;
						}
						stack2.pop();
					}
					stack_1.push(stack2.top->value);
					stack2.pop();
					int a=1;
					for(int i=0;i<m;i++)
					{
						a=a&&demand_pathmark[i];
					}
					if(a==1)
					{
						while(stack_1.top!=NULL)
						{
							stack_2.push(stack_1.top->value);
							//   record_result(stack_2.top->value);
							stack_1.pop();
						}
					}
					stack_1.clear();
				}
			}
		}
	}
	stacknote[stack1.top->value]=0;
	stack1.pop();
}
//matrix 邻接矩阵，存权值
//pathnummatrix 权值矩阵
static void parse_paths(char* topo[], unsigned int edge_num, struct path* paths,unsigned short matrix[][MAX_NODE_NUMBERS],unsigned short &maxnodeindex,unsigned short pathnummatrix[][MAX_NODE_NUMBERS], struct node_new nodeinfo[])
{
	unsigned short index;
	unsigned short from, to;
	unsigned char weight;
	struct path* path;
	char* p;
	for (unsigned short i = 0; i < edge_num; ++i) 
	{
		path = &paths[i];
		p = topo[i];
		index=0;
		do 
		{
			index *= 10;
			index += *p - '0';
			p++;
		} while (*p != ',');
		p++;
		path->index = index;
		from = 0;
		do 
		{
			from *= 10;
			from += *p - '0';
			p++;
		} while (*p != ',');
		p++;
		path->from = from;
		if (maxnodeindex<from)
		{
			maxnodeindex=from;
		}

		to = 0;
		do 
		{
			to *= 10;
			to += *p - '0';
			p++;
		} while (*p != ',');
		p++;
		path->to = to;
		if (maxnodeindex<to)
		{
			maxnodeindex=to;
		}
		//matrix[from][to]=1;

		weight = 0;
		do 
		{
			weight *= 10;
			weight += *p - '0';
			p++;
		} while (*p != '\r'&&*p != '\n'&&*p != '\0');
		path->weight = weight;
		if (matrix[from][to]>weight)
		{
			matrix[from][to]=weight;
			if (pathnummatrix[from][to] !=SHORT_INF)
			{
				//说明压入非最小的边
				int num = nodeinfo[from].outward_paths_size;
				for (int i = 0; i < num; ++i)
				{
					if (nodeinfo[from].outward_paths[i]->to == to)
					{
						nodeinfo[from].outward_paths[i] = path;
						break;
					}
				}
			}
			else
			{
				nodeinfo[from].outward_paths[nodeinfo[from].outward_paths_size++]=path;
			}
			pathnummatrix[from][to]=index;
		}	
	}
	for (unsigned short i = 0; i <= maxnodeindex; ++i) 
	{
		matrix[i][i]=0;
	}
}

static void parse_demand(char* demand,unsigned short vnode[], unsigned short * start_ptr, unsigned short* end_ptr, unsigned short* via_nodes_count_ptr)
{
	unsigned short start = 0, end = 0, index = 0, via_nodes_count = 0;
	char* p = demand;

	do {
		start = start * 10 + *p - '0';
		p++;
	} while (*p != ',');
	*start_ptr = start;
	p++;

	do {
		end = end * 10 + *p - '0';
		p++;
	} while (*p != ',');
	*end_ptr = end;
	p++;

	do {
		index = index * 10 + *p - '0';
		p++;
		if (*p == '|') 
		{
			vnode[via_nodes_count++]=index;
			p++;
			index = 0;
			continue;
		}
	} while (*p != '\r'&&*p != '\n'&&*p != '\0');
	vnode[via_nodes_count++]=index;
	*via_nodes_count_ptr = via_nodes_count;
}
void Dijkstra(unsigned short v0,unsigned short a[],unsigned short M,unsigned short max_node_index,unsigned short matrix[][MAX_NODE_NUMBERS], struct node_new nodeinfo[MAX_NODE_NUMBERS], unsigned short prev[MAX_NODE_NUMBERS],unsigned short dislist[MAX_NODE_NUMBERS],const unsigned short * const end,const unsigned short * const begin)
{
	int N=M+2;//修改
	int curN=0;//N个元素中目前求道的个数
	unsigned short (*A)[MAX_NODE_NUMBERS]=matrix;
	unsigned short *dist=dislist;
	bool S[MAX_NODE_NUMBERS];  // 判断是否已存入该点到S集合中
	memset(S,false,sizeof(S));
	int n=max_node_index+1;
	bool bMidPoint[MAX_NODE_NUMBERS]; //初始化后表示是否为中间点，如果是中间点，有些特殊处理
	memset(bMidPoint,false,sizeof(bMidPoint));
	for (unsigned short i=0;i<M;++i)
	{
		bMidPoint[a[i]]=true;
	}
	if (end)
	{
		bMidPoint[*end]=true;
	}
	if (begin)
	{
		bMidPoint[*begin]=true;
	}
	bMidPoint[v0]=false;

	for(int i=0; i<n; ++i)
	{
		dist[i] = A[v0][i];
		//S[i] = false;                                // 初始都未用过该点
		if(dist[i] == SHORT_INF)    
			prev[i] = SHORT_INF;
		else 
			prev[i] = v0;
	}
	unsigned int num = nodeinfo[v0].outward_paths_size;
	for (unsigned int k = 0; k < num; ++k)
	{
		unsigned short to = nodeinfo[v0].outward_paths[k]->to;
		dist[to]= A[v0][to];
		prev[to] = v0;
	}
	dist[v0] = 0;
	S[v0] = true; 
	++curN;
	for(int i=0; i<n; i++)
	{
		int mindist = SHORT_INF;
		int u = v0;// 找出当前未使用的点j的dist[j]最小值
		for(int j=0; j<n; ++j)
			if((!S[j]) && dist[j]<mindist)
			{
				u = j;  // u保存当前邻接点中距离最小的点的号码 
				mindist = dist[j];
			}
		S[u] = true; 
			
		if ((bMidPoint[u]))//新增结点为中途点，不更新dist列表
		{
			++curN;
			if(curN>=N)//搜索完这些个关键点（包括起点、终点，中途必经点），退出
				break;
			else
				continue;
		}

		int num = nodeinfo[u].outward_paths_size;
		for (int k = 0; k < num; ++k)
		{
			int to = nodeinfo[u].outward_paths[k]->to;
			if (!S[to] && dist[u] + A[u][to] < dist[to])
			{
				dist[to] = dist[u] + A[u][to];    //更新dist 
				prev[to] = u;                    //记录前驱顶点 
			}
		}
	}
}



bool push_a_element(short *newlastpopup, std::vector<unsigned short> &inarray, int &mdpoint_instack,unsigned short demand_path[],int **repeat, int &cost,int MinCost, std::deque<unsigned short> &outarray, struct path_2 *NewPathMap,std::vector<unsigned short>&NewpathInArray,short **MapFromIndexToNewGraph)
{
	int costleft=MinCost-cost;
	int tempbeginindex_in_midnode = inarray.back();
	int tempbegin = demand_path[tempbeginindex_in_midnode];//局部起点index
	short *nextpopup = &newlastpopup[mdpoint_instack + 1];
	for (short i = *nextpopup + 1; i<(short)outarray.size(); i++)
	{
		int tempend = demand_path[outarray[i]];
		int templineindex = MapFromIndexToNewGraph[tempbeginindex_in_midnode][outarray[i]];
		if (templineindex<SHORT_INF)//查找tempbegin->tempend的连接性
		{
			int tempcost=NewPathMap[templineindex].weight;
			if (tempcost<costleft&&!PathComflict(NewpathInArray, templineindex,repeat))//判断有无重复点
			{
				NewpathInArray.push_back(templineindex);
				cost += tempcost;
				inarray.push_back(outarray[i]);
				outarray.erase(outarray.begin() + i);
				*nextpopup = i;
				++mdpoint_instack;
				return true;
			}
		}
	}
	return false;
}

void pop_a_element(short *newlastpopup, std::vector<unsigned short> &inarray, int &mdpoint_instack,int &cost,std::deque<unsigned short> &outarray, struct path_2 *NewPathMap, std::vector<unsigned short>&NewpathInArray)
{
	if (outarray.size())
	{
		newlastpopup[mdpoint_instack + 1] = -1;
	}
	int tempend_index_in_midpt = inarray[mdpoint_instack];
	outarray.insert(outarray.begin() + newlastpopup[mdpoint_instack], inarray.back());
	inarray.pop_back();
	--mdpoint_instack;
	int tempbegin_index_in_midpt = inarray[mdpoint_instack];
	
	cost -= NewPathMap[NewpathInArray.back()].weight;
	NewpathInArray.pop_back();

}

bool Repass_node(unsigned short prev_of_pt[MAX_NODE_NUMBERS],bool visitedpoint[],int begin,int end)
{
	//pointlist[l][demand_path[i]]中元素的序号在visited_point列表中对应元素是否全为false
	int pt=end;
	while(visitedpoint[pt]==false)
	{
		pt=prev_of_pt[pt];
	}
	if(pt==begin)
		return false;
	else
		return true;
}

void Add_node(unsigned short prev_of_pt[],bool visitedpoint[],int begin,int end)
{
	int pt=end;
	while(pt!=begin)
	{
		visitedpoint[pt]=true;
		pt=prev_of_pt[pt];
	}
}

void Remove_node(unsigned short prev_of_pt[],bool visitedpoint[],int begin,int end)
{
	int pt=end;
	while(pt!=begin)
	{
		visitedpoint[pt]=false;
		pt=prev_of_pt[pt];
	}
}

void collectpathbetwoonKeyNode(unsigned short pathnummatrix[][MAX_NODE_NUMBERS],unsigned short prev_of_pt[],int begin,int end,stacks &stack)
{
	int subbegin=prev_of_pt[end];
	int subend=end;
	while(subend!=begin)
	{
		stack.push(pathnummatrix[subbegin][subend]);
		subend=subbegin;
		subbegin=prev_of_pt[subend];
	}
}
void CollectAllPathInfo(unsigned short pathnummatrix[][MAX_NODE_NUMBERS],unsigned short prev_of_middlept[][MAX_NODE_NUMBERS],int MapFromIndexToMiddlePointIndex[],unsigned short midnode_list[],int nodenum,int end,stacks &stack)
{
	int begin=midnode_list[0];
	int lastnum=nodenum;
	int subend=end;
	int subbegin;
	int subbegin_index_inmidpt;
	while (subend!=begin)
	{
		int subbegin=midnode_list[--lastnum];
		int subbegin_index_inmidpt=MapFromIndexToMiddlePointIndex[subbegin];
		collectpathbetwoonKeyNode(pathnummatrix,prev_of_middlept[subbegin_index_inmidpt],subbegin,subend,stack);
		subend=subbegin;
	}
}

bool PathComflict(std::vector<unsigned short>&NewpathInArray, unsigned short newpathindex, int **repeat)
{
	for (int i = NewpathInArray.size()-1; i >= 0; --i)
	{
		if (repeat[NewpathInArray[i]][newpathindex] == 1)
			return true;
	}
	return false;
}