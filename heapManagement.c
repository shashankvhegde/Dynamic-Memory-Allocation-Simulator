/* The program accepts from user the amount of space to be allocated from heap and the name of pointer to the same. 
Also in case of deletion, it asks the user for the name of the pointer of the location to be freed. There is a free list
that keeps record of free memory locations and an alloc list that keeps record of filled memory locations.*/
#include <stdio.h>
#include <string.h>
#include<math.h>
#include<stdlib.h>
#define N 3
#define gran 5

/* we assume that the memory is divided into 1000 memory locations each consisting of gran bytes. So minimum space allocated 
on request is gran bytes.*/

typedef struct nodeTagAloc   // this is a node of aloc list
{
	char name[100];
	int startIndex;
	int fragSpace;          // the total space is space occupied in bytes by a particular chunk.
	int totalSpace;
	struct nodeTagAloc *next;
}Aloc_node;

typedef struct nodeTagFree  // this is a node of free list
{
	int startIndex;
	int freeSpace;         // free space is number of free memory locations available per unoccupied chunk.
	struct nodeTagFree *next;
}Free_node;

printFreelist(Free_node * headFree)
{
	Free_node * ptr=headFree;
	printf("Free list:\n");
	while(ptr!=NULL)
	{
		printf("Starting index: ");
		printf("%d\n",ptr->startIndex);
		printf("Free space is: ");
		printf("%d locations = %d bytes\n",ptr->freeSpace,ptr->freeSpace*gran);
		printf("-------------------------------------------------------------\n");
		ptr=ptr->next;
    }
}

printAloclist(Aloc_node * headAloc)
{
	Aloc_node * ptr= headAloc;
	printf("-------------------------------------------------------------\n");
	printf("Allocated list:\n");
	while(ptr!=NULL)
	{
		printf("Name: ");
		puts(ptr->name);
		printf("Starting index of block: ");
		printf("%d\n",ptr->startIndex);
		printf("Amount of fragmented space: ");
		printf("%d bytes\n",ptr->fragSpace);
		printf("Total space taken: ");
		printf("%d bytes\n",ptr->totalSpace);
		printf("-----------------------------------------------------------\n");
		ptr=ptr->next;
	}
}

// This is a function to delete free list (used in case of compaction)
void deleteList(Free_node *headFree)
{
	Free_node *temp;
	Free_node *ptrFree;
	ptrFree=headFree;
	while(ptrFree!=NULL)
	{
		temp=ptrFree;
		ptrFree=ptrFree->next;
		free(temp);
	}
}
// ----------sorting alloc list based on start indices--------------------------------------------------
Aloc_node * Divide(Aloc_node *p)
{
    Aloc_node *q,*r;
    q=p;
    r=(p->next)->next;
    while(r!=NULL)
    {
        q=q->next;
        r=r->next;
        if(r!=NULL)
        {
            r=r->next;
        }
    }
    r=q;
    q=q->next;
    r->next=NULL;
    return q;
}

Aloc_node * Merge(Aloc_node *p, Aloc_node *q)
{
    Aloc_node * head, *r;
    if(p==NULL || q==NULL)
    {
        printf("SOMETHING IS WRONG\n");
    }
    else
    {
        if(p->startIndex<q->startIndex)
        {
            head=p;
            p=p->next;
        }
        else 
        {
            head=q;
            q=q->next;
        }
       
    }
    r=head;
    while(p!=NULL && q!=NULL)
    {
        if(p->startIndex<q->startIndex)
        {
            r=r->next=p;
            p=p->next;
        }
        else
        {
            r=r->next=q;
            q=q->next;
        }

    }
    if(p!=NULL)
    {
        r->next=p;
    }
    if(q!=NULL)
    {
        r->next=q;

    }
    return head;
}

Aloc_node * Sort(Aloc_node *p)
{
    Aloc_node *q, *newhead;
    newhead=p;
    if(p!=NULL && (p->next!=NULL))
    {
        q=Divide(p);
        p=Sort(p);
        q=Sort(q);
        newhead=Merge(p,q);
    }
    return newhead;
}
//----------------------------------------------------------------------------------------------------

/* compact function is used when no free chunk is sufficient enough to occupy the requested memory. 
It divides the heap in two parts: one totally occupied and one totally free.*/
Aloc_node *compact(Aloc_node *headAloc,Free_node **headFree)
{
	Aloc_node* ptrAloc=headAloc, *ptr, *temp1;
	Aloc_node* temp, *newhead;
	Free_node *nptr;
	newhead=(Aloc_node *)malloc(sizeof(Aloc_node));   // dummy node.
	newhead->next=NULL;
	ptr=newhead;
    int startIndex=1;
    
	while(ptrAloc!=NULL)                // creating copy of each node in allocate list and changing their indices accordingly
	{
		temp=(Aloc_node *)malloc(sizeof(Aloc_node));
		temp->startIndex=startIndex;
		strcpy(temp->name, ptrAloc->name);
		temp->fragSpace=ptrAloc->fragSpace;
		temp->totalSpace=ptrAloc->totalSpace;
		temp->next=NULL;
		startIndex+=(temp->totalSpace+temp->fragSpace)/gran;
		temp1=ptrAloc;
		ptrAloc=ptrAloc->next;
		free(temp1);
		ptr->next=temp;
		ptr=ptr->next;
	}
	
	temp=newhead;
	newhead=newhead->next;
	free(temp);
	
	deleteList(*headFree);
	nptr=(Free_node *)malloc(sizeof(Free_node)); // only one node is left in free list that represents the entire free chunk
	nptr->startIndex=startIndex;
	nptr->freeSpace=N-startIndex+1;
	nptr->next=NULL;
	*headFree=nptr;
	return newhead;
}


// allocate requested memory.
Aloc_node *mallocNew(Aloc_node *headAloc, Free_node **headFree, int size, char string[])
{
	Free_node *ptrFree,*temp,*nptr;
	Aloc_node *ptrAloc, *ptr;
	ptrFree=*headFree;
	ptrAloc=headAloc;
	int startIndex;
	int flag=1;
// following the FIRST FIT method;	
	while(ptrFree!=NULL && ptrFree->freeSpace<(int)ceil((double)size/gran))
	{
		ptrFree=ptrFree->next;
	}
	if(ptrAloc!=NULL)
	{
	
		while(ptrAloc->next!=NULL)
		{
			ptrAloc=ptrAloc->next;
		}
    }
    
	if(ptrFree==NULL)  // if no sufficient space is found to allocate the memory.
	{
		flag=0;
	}
	else
	{
		startIndex=ptrFree->startIndex;
	}
	
	if(flag==0)
	{
		headAloc=compact(headAloc,headFree);
		nptr=*headFree;
		startIndex=nptr->startIndex;
		ptrFree=*headFree;
		ptrAloc=headAloc;
		if(ptrAloc!=NULL)
		{
		
			while(ptrAloc->next!=NULL)
			{
				ptrAloc=ptrAloc->next;
			}
	    }
	}
	
	if((int)ceil((double)size/gran)+startIndex<=N+1) // if generated space is sufficient to fit in the entire requested memory
	{
		ptr=(Aloc_node *)malloc(sizeof(Aloc_node));
	
		ptr->startIndex=startIndex;
		strcpy(ptr->name, string);
		ptr->fragSpace=(gran-(size%gran))%gran;
		ptr->totalSpace=size;
		ptr->next=NULL;
		if(ptrAloc!=NULL)
		{
			ptrAloc->next=ptr;
		}
		else
		{
			headAloc=ptr;
		}
		// making according changed to free list
		if(ptrFree->freeSpace>((int)ceil((double)size/gran)))
		{
			ptrFree->freeSpace=ptrFree->freeSpace-(int)ceil((double)size/gran);
			ptrFree->startIndex+=(int)ceil((double)size/gran);
		}
		else if(ptrFree->freeSpace==((int)ceil((double)size/gran)))
		{
			temp=ptrFree;
			ptrFree=ptrFree->next;
			free(temp);
		}
		
	}
	else
	{
		printf("THE HEAP IS FULL\n");
	}
	headAloc=Sort(headAloc); // to maintain a sorted allocate list
	*headFree=ptrFree;
	return headAloc;
}

// to coalesce the adjacent free chunks called after every deletion
void mergeFreeList(Free_node * headFree)
{
	
	Free_node *temp,*prev,*ptr;
	
	if(headFree!=NULL)
	{
		ptr=headFree->next;
		prev=headFree;
		while(ptr!=NULL)
		{
			// all chunks that can be coalesced are combined in this while loop. 
			while(prev!=NULL && ptr!=NULL && prev->startIndex+prev->freeSpace==ptr->startIndex)
			{
				temp=ptr;
				prev->freeSpace+=ptr->freeSpace;
				ptr=ptr->next;
				prev->next=ptr;
				free(temp);
			}
			if(ptr!=NULL)  // we increment ptr only if ptr has not reached the end.
			{
			
				prev=ptr;
				ptr=ptr->next;
			}
		}
	}	
}

// to free particular allocated chunk.
void freeNew(Aloc_node **headAloc, Free_node **headFree, char string[])
{
	int flag=0;
	Aloc_node *ptrAloc, *prev;
	Free_node *ptrFree, *prevFree, *temp;
	ptrFree=prevFree=*headFree;
	ptrAloc=*headAloc;
	int startIndex,totalSpace;
	
	//the search is done based on given pointer name.
	if(ptrAloc!=NULL)
	{
		if(strcmp(ptrAloc->name, string)==0) // if the chunk to be deleted is first one.
		{
			*headAloc=ptrAloc->next;
			startIndex=ptrAloc->startIndex;  // if found then the start index and occupied space are saved.
			totalSpace=ptrAloc->totalSpace;
			free(ptrAloc);
			flag=1;
		}
		else
		{
			while(ptrAloc!=NULL && !flag)
			{
				if(strcmp(ptrAloc->name, string)==0)
				{
					flag=1;
				}
				else
				{
					prev=ptrAloc;
					ptrAloc=ptrAloc->next;
				}
			}
			if(flag==1)
			{
				prev->next=ptrAloc->next;
				startIndex=ptrAloc->startIndex;
				totalSpace=ptrAloc->totalSpace;
				free(ptrAloc);
			}
			else
			{
				printf("THE GIVEN ADDRESS DOES NOT EXIST\n");
			}
		}
	}

	if(flag==1)   // start index and occupied space are given to a node in free list that is then correctly inserted 
	{
		temp=(Free_node *)malloc(sizeof(Free_node));
		temp->startIndex=startIndex;
		temp->freeSpace=(int)ceil((double)totalSpace/gran);
		temp->next=NULL;
		if(ptrFree==NULL)
		{
			*headFree=temp;
		}
		else
		{
			while(ptrFree!=NULL && ptrFree->startIndex<startIndex)
			{
				prevFree=ptrFree;
				ptrFree=ptrFree->next;
			}
			
			if(prevFree==ptrFree)
			{
				*headFree=temp;
				temp->next=prevFree;
			}
			else
			{
				temp->next=ptrFree;
				prevFree->next=temp;
			}
			mergeFreeList(*headFree);
		}
	}
}

int main(int argc, char const *argv[])
{

	Free_node *headFree=NULL;
	Aloc_node *headAloc=NULL;
	int flag=1,n,x,s,size;
	char stringname[100];
	headFree=(Free_node *)malloc(sizeof(Free_node));
	headFree->startIndex=1;
	headFree->freeSpace=N;
	headFree->next=NULL;
	

    while(flag==1){
    	
    	printf("*************************************\n");
    	printf("1. Insertion\n");
    	printf("2. Deletion\n");
    	printf("3. Print\n");
    	printf("4. Quit\n");
    	printf("*************************************\n");
    	printf("Enter your choice\n");
    	scanf("%d",&n);
    	switch(n)
    	{
    		case 1:printf("1. insert an integer\n");
    			   printf("2. insert an character\n");
    			   printf("3. insert an float\n");
    			   printf("4. insert an double\n");
    			   printf("5. insert an array of integers\n");
    			   printf("6. insert an array of character\n");
    			   printf("7. insert an array of float\n");
    			   printf("8. insert an array of double\n");
    			   printf("9. insert user defined structure\n");
    		       
    		       printf("Enter your choice\n");
    		       scanf("%d",&x);
    		       switch(x)
    		       {
    		       	  case 1: printf("enter the name of the pointer: ");
    		       	  		  scanf("%s",stringname);
    		       	  	      s=sizeof(int);
    		       	  	      break;
    		       	  case 2: printf("enter the name of the pointer: ");
    		       	  		  scanf("%s",stringname);
    		       	  	      s=sizeof(char);
    		       	  	      break;
    		       	  case 3: printf("enter the name of the pointer: ");
    		       	  		  scanf("%s",stringname);
    		       	  	      s=sizeof(float);
    		       	  	      break;
    		       	  case 4: printf("enter the name of the pointer: ");
    		       	  		  scanf("%s",stringname);
    		       	  	      s=sizeof(double);
    		       	  	      break;
    		       	  case 5: printf("enter the name of the pointer: ");
    		       	  		  scanf("%s",stringname);
							  printf("enter size of array: ");
    		       	  		  scanf("%d",&size);
    		       	  		  s=sizeof(int)*size;
    		       	  		  break;
    		       	  case 6: printf("enter the name of the pointer: ");
    		       	  		  scanf("%s",stringname);
						      printf("enter size of array: ");
    		       	  		  scanf("%d",&size);
    		       	  		  s=sizeof(char)*size;
    		       	  		  break;
    		       	  case 7: printf("enter the name of the pointer: ");
    		       	  		 scanf("%s",stringname);
						      printf("enter size of array: ");
    		       	  		  scanf("%d",&size);
    		       	  		  s=sizeof(float)*size;
    		       	  		  break;
    		       	  case 8: printf("enter the name of the pointer: ");
    		       	  		 scanf("%s",stringname);
						      printf("enter size of array: ");
    		       	  		  scanf("%d",&size);
    		       	  		  s=sizeof(double)*size;
    		       	  		  break;
    		       	  case 9: printf("enter the name of the pointer: ");
    		       	  		  scanf("%s",stringname);
    		       	  		  printf("enter size: ");
						      scanf("%d",&s);
						      break;
    		       	  
    		       	  default: printf("Not a valid choice");
    		       	  		   flag=0;
						 	   break; 
    		       	  		  
				   }
				   headAloc=mallocNew(headAloc, &headFree, s, stringname);
				   break;
				   
			case 2: printf("Name of the pointer to be freed:  ");
					scanf("%s",stringname);
					freeNew(&headAloc,&headFree,stringname);
					break;
					
					
			case 3: printAloclist(headAloc);
			        printFreelist(headFree);
			        break;
			        
			case 4: printf("EXIT");
			        flag=0;
					break;        
			        
			default: printf("Not a valid choice");
					 flag=0;
					 break;        
		}
	}
    
return 0;
}
