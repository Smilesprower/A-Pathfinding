#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <queue>

using namespace std;

template <class NodeType, class ArcType> class GraphArc;
template <class NodeType, class ArcType> class GraphNode;

// ----------------------------------------------------------------
//  Name:           Graph
//  Description:    This is the graph class, it contains all the
//                  nodes.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
class Graph {
private:

    // typedef the classes to make our lives easier.
    typedef GraphArc<NodeType, ArcType> Arc;
    typedef GraphNode<NodeType, ArcType> Node;

// ----------------------------------------------------------------
//  Description:    An array of all the nodes in the graph.
// ----------------------------------------------------------------
    Node** m_pNodes;

// ----------------------------------------------------------------
//  Description:    The maximum number of nodes in the graph.
// ----------------------------------------------------------------
    int m_maxNodes;


// ----------------------------------------------------------------
//  Description:    The actual number of nodes in the graph.
// ----------------------------------------------------------------
    int m_count;


public:           
    // Constructor and destructor functions
    Graph( int size );
    ~Graph();

    // Accessors
    Node** nodeArray() const {
       return m_pNodes;
    }

    // Public member functions.
    bool addNode( NodeType data, int index , sf::Vector2f p_pos, sf::Font p_font);
    void removeNode( int index );
    bool addArc( int from, int to, ArcType weight);
    void removeArc( int from, int to );
    Arc* getArc( int from, int to );        
    void clearMarks();
    void depthFirst( Node* pNode, void (*pProcess)(Node*) );
    void breadthFirst( Node* pNode, void (*pProcess)(Node*) );
	void adaptedBreadthFirst( Node* pCurrent, Node* pGoal );	
	void aStar(Node* pStart, Node* pDest, void(*pProcess)(Node*), std::vector<Node *>& path);

	// Updated Functor - Includes heuristics in search
	//////////////////////////
	class NodeSearchCostComparer {
	public:
		bool operator()(Node * n1, Node * n2) {
			tuple<string, int, int> p1 = n1->data();
			tuple<string, int, int> p2 = n2->data();
			return get<1>(p1) +get<2>(p1) > get<1>(p2) +get<2>(p2);
		}
	};
};

// ----------------------------------------------------------------
//  Name:           Graph
//  Description:    Constructor, this constructs an empty graph
//  Arguments:      The maximum number of nodes.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
Graph<NodeType, ArcType>::Graph( int size ) : m_maxNodes( size ) {
   int i;
   m_pNodes = new Node * [m_maxNodes];
   // go through every index and clear it to null (0)
   for( i = 0; i < m_maxNodes; i++ ) {
        m_pNodes[i] = 0;
   }

   // set the node count to 0.
   m_count = 0;
}

// ----------------------------------------------------------------
//  Name:           ~Graph
//  Description:    destructor, This deletes every node
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
Graph<NodeType, ArcType>::~Graph() {
   int index;
   for( index = 0; index < m_maxNodes; index++ ) {
        if( m_pNodes[index] != 0 ) {
            delete m_pNodes[index];
        }
   }
   // Delete the actual array
   delete m_pNodes;
}

// ----------------------------------------------------------------
//  Name:           addNode
//  Description:    This adds a node at a given index in the graph.
//  Arguments:      The first parameter is the data to store in the node.
//                  The second parameter is the index to store the node.
//  Return Value:   true if successful
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
bool Graph<NodeType, ArcType>::addNode( NodeType data, int index, sf::Vector2f p_pos, sf::Font p_font ) {
   bool nodeNotPresent = false;
   // find out if a node does not exist at that index.
   if ( m_pNodes[index] == 0) {
      nodeNotPresent = true;
      // create a new node, put the data in it, and unmark it.
      m_pNodes[index] = new Node;
      m_pNodes[index]->setData(data);
      m_pNodes[index]->setMarked(false);
	  m_pNodes[index]->SetPosition(p_pos);
	  m_pNodes[index]->SetUpNode(p_font);
      // increase the count and return success.
      m_count++;
    }
        
    return nodeNotPresent;
}

// ----------------------------------------------------------------
//  Name:           removeNode
//  Description:    This removes a node from the graph
//  Arguments:      The index of the node to return.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::removeNode( int index ) {
     // Only proceed if node does exist.
     if( m_pNodes[index] != 0 ) {
         // now find every arc that points to the node that
         // is being removed and remove it.
         int node;
         Arc* arc;

         // loop through every node
         for( node = 0; node < m_maxNodes; node++ ) {
              // if the node is valid...
              if( m_pNodes[node] != 0 ) {
                  // see if the node has an arc pointing to the current node.
                  arc = m_pNodes[node]->getArc( m_pNodes[index] );
              }
              // if it has an arc pointing to the current node, then
              // remove the arc.
              if( arc != 0 ) {
                  removeArc( node, index );
              }
         }
        

        // now that every arc pointing to the current node has been removed,
        // the node can be deleted.
        delete m_pNodes[index];
        m_pNodes[index] = 0;
        m_count--;
    }
}

// ----------------------------------------------------------------
//  Name:           addArd
//  Description:    Adds an arc from the first index to the 
//                  second index with the specified weight.
//  Arguments:      The first argument is the originating node index
//                  The second argument is the ending node index
//                  The third argument is the weight of the arc
//  Return Value:   true on success.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
bool Graph<NodeType, ArcType>::addArc( int from, int to, ArcType weight ) {
     bool proceed = true; 
     // make sure both nodes exist.
     if( m_pNodes[from] == 0 || m_pNodes[to] == 0 ) {
         proceed = false;
     }
        
     // if an arc already exists we should not proceed
     if( m_pNodes[from]->getArc( m_pNodes[to] ) != 0 ) {
         proceed = false;
     }

     if (proceed == true) {
        // add the arc to the "from" node.
        m_pNodes[from]->addArc( m_pNodes[to], weight );
     }
        
     return proceed;
}

// ----------------------------------------------------------------
//  Name:           removeArc
//  Description:    This removes the arc from the first index to the second index
//  Arguments:      The first parameter is the originating node index.
//                  The second parameter is the ending node index.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::removeArc( int from, int to ) {
     // Make sure that the node exists before trying to remove
     // an arc from it.
     bool nodeExists = true;
     if( m_pNodes[from] == 0 || m_pNodes[to] == 0 ) {
         nodeExists = false;
     }

     if (nodeExists == true) {
        // remove the arc.
        m_pNodes[from]->removeArc( m_pNodes[to] );
     }
}


// ----------------------------------------------------------------
//  Name:           getArc
//  Description:    Gets a pointer to an arc from the first index
//                  to the second index.
//  Arguments:      The first parameter is the originating node index.
//                  The second parameter is the ending node index.
//  Return Value:   pointer to the arc, or 0 if it doesn't exist.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
// Dev-CPP doesn't like Arc* as the (typedef'd) return type?
GraphArc<NodeType, ArcType>* Graph<NodeType, ArcType>::getArc( int from, int to ) {
     Arc* pArc = 0;
     // make sure the to and from nodes exist
     if( m_pNodes[from] != 0 && m_pNodes[to] != 0 ) {
         pArc = m_pNodes[from]->getArc( m_pNodes[to] );
     }
                
     return pArc;
}


// ----------------------------------------------------------------
//  Name:           clearMarks
//  Description:    This clears every mark on every node.
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::clearMarks() {
     int index;
     for( index = 0; index < m_maxNodes; index++ ) {
          if( m_pNodes[index] != 0 ) {
              m_pNodes[index]->setMarked(false);
          }
     }
}


// ----------------------------------------------------------------
//  Name:           depthFirst
//  Description:    Performs a depth-first traversal on the specified 
//                  node.
//  Arguments:      The first argument is the starting node
//                  The second argument is the processing function.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::depthFirst( Node* pNode, void (*pProcess)(Node*) ) {
     if( pNode != 0 ) {
           // process the current node and mark it
           pProcess( pNode );
           pNode->setMarked(true);

           // go through each connecting node
           list<Arc>::iterator iter = pNode->arcList().begin();
           list<Arc>::iterator endIter = pNode->arcList().end();
        
		   for( ; iter != endIter; ++iter) {
			    // process the linked node if it isn't already marked.
                if ( (*iter).node()->marked() == false ) {
                   depthFirst( (*iter).node(), pProcess);
                }            
           }
     }
}


// ----------------------------------------------------------------
//  Name:           breadthFirst
//  Description:    Performs a depth-first traversal the starting node
//                  specified as an input parameter.
//  Arguments:      The first parameter is the starting node
//                  The second parameter is the processing function.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::breadthFirst( Node* pNode, void (*pProcess)(Node*) ) {
   if( pNode != 0 ) {
	  queue<Node*> nodeQueue;        
	  // place the first node on the queue, and mark it.
      nodeQueue.push( pNode );
      pNode->setMarked(true);

      // loop through the queue while there are nodes in it.
      while( nodeQueue.size() != 0 ) {
         // process the node at the front of the queue.
         pProcess( nodeQueue.front() );

         // add all of the child nodes that have not been 
         // marked into the queue
         list<Arc>::const_iterator iter = nodeQueue.front()->arcList().begin();
         list<Arc>::const_iterator endIter = nodeQueue.front()->arcList().end();
         
		 for( ; iter != endIter; iter++ ) {
              if ( (*iter).node()->marked() == false) {
				 // mark the node and add it to the queue.
                 (*iter).node()->setMarked(true);
                 nodeQueue.push( (*iter).node() );
              }
         }

         // dequeue the current node.
         nodeQueue.pop();
      }
   }  
}


// ----------------------------------------------------------------
//  Name:           adaptedBreadthFirst
//  Description:    Performs a breadth-first traversal the starting node
//                  specified as an input parameter, terminating at the goal.
//  Arguments:      The first parameter is the starting node
//                  The second parameter is the processing function.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::adaptedBreadthFirst( Node* pCurrent, Node *pGoal ) {
   if( pCurrent != 0 && pGoal != 0 ) {
	  bool goalReached = false;
	  queue<Node*> nodeQueue;        
	  // place the first node on the queue, and mark it.
      nodeQueue.push( pCurrent );
      pCurrent->setMarked(true);

      // loop through the queue while there are nodes in it.
      while( nodeQueue.size() != 0 && goalReached == false) {
         // process the node at the front of the queue.
         // pProcess( nodeQueue.front() );

         // add all of the child nodes that have not been 
         // marked into the queue
         list<Arc>::const_iterator iter = nodeQueue.front()->arcList().begin();
         list<Arc>::const_iterator endIter = nodeQueue.front()->arcList().end();
         
		 for( ; iter != endIter && goalReached == false; iter++ ) {
			  // Check if current child node == goal
			  if ( (*iter).node() == pGoal ) {
				goalReached = true;
				pGoal->setPrevious( (*iter).node() );
			  }
              if ( (*iter).node()->marked() == false) {
				 // Set previous pointer of child to current.
				 (*iter).node()->setPrevious( nodeQueue.front() );
				 // mark the node and add it to the queue.
                 (*iter).node()->setMarked(true);
                 nodeQueue.push( (*iter).node() );
              }
         }

         // dequeue the current node.
         nodeQueue.pop();
      }
   }  
}
// A* Assignment
////////////////////////////////
/*
 A* is a combination of Dijkstra's algorithm and a heuristic pathfinder.
 A heuristic is a process for evaluating each node based on distance.
 To calculate the heuristic we used the distance formula from every node to the end node.

 Similar to UCS, except we add an extra cost to determine the shortest path.

 f(n) = h(n) + g(n)
 Where
	h(n) - distance from the current node to the goal node.
	g(n) - cost so far to reach n "Weights + costs added up".
	f(n) - total cost of the path though n to the end node.


 Again similar to UCS, Parent node expands its children and sets the next
 highest priority node based on the smallest f(n).

 However A* does not search every node nor search in every direction.
 It selects the node that is closer to the goal and searches
 the best path in the direction to the goal.

 If h(n) = 0, A* becomes Dijkstra's.
 If h(n) > cost of moving - Find the correct path but is slow.
 If h(n) < cost of moving - Not guaranteed to find the fastest path but it can run faster.
*/

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::aStar(Node* pStart, Node* pDest, void(*pProcess)(Node*), std::vector<Node *>& path)
{
	// Set up Priority Queue
	priority_queue<Node *, vector<Node *>, NodeSearchCostComparer> pq;

	// Add the start node to the queue
	pq.push(pStart);

	// Set the start nodes cost data to 0
	pStart->setData(NodeType(get<0>(pStart->data()), 0, get<2>(pStart->data())));
	
	// Mark the first node
	pStart->setMarked(true);
	
	// EXTRA - Set a string for the data to be printed on the node
	pStart->SetText("    " + (get<0>(pStart->data())) + "\n" + " C-" + std::to_string((get<1>(pStart->data()))) + "\n" + " H-" + std::to_string((get<2>(pStart->data()))));


	// While the priority queue is not empty and top node of the pq is not equal to the end node
	while (pq.size() != 0 && pq.top() != pDest)
	{
		// Iterator
		list<Arc>::const_iterator iter = pq.top()->arcList().begin();
		list<Arc>::const_iterator endIter = pq.top()->arcList().end();

		// Loop through all the connecting nodes in the arc list
		for (; iter != endIter; iter++)
		{
			if ((*iter).node() != pq.top()->previous())
			{
				// Heuristic Cost
				int hCost = get<2>(pq.top()->data());

				// Total Weight Cost
				int gCost = get<1>(pq.top()->data()) + (*iter).weight();

				// Total Cost
				int distC = gCost + hCost;

				// Childs Total Cost
				int currF = get<1>((*iter).node()->data()) + get<2>((*iter).node()->data());

				// if the pq.top nodes total cost, is less than the connecting nodes total cost
				if (distC < currF)
				{
					// Update the connecting nodes total cost
					(*iter).node()->setData(NodeType(get<0>((*iter).node()->data()), gCost, get<2>((*iter).node()->data())));

					// Set the connecting nodes previous to the top of the priority queue
					(*iter).node()->setPrevious(pq.top());

					// EXTRA - setting values to be seen on the node
					(*iter).node()->SetText("    " + (get<0>((*iter).node()->data())) + "\n" + " C-" + std::to_string(gCost) + "\n" + " H-" + std::to_string((get<2>((*iter).node()->data()))));
				}
				// If the connecting node is not marked
				if (!(*iter).node()->marked())
				{
					// Push it to the priority queue
					pq.push((*iter).node());

					// Mark the node
					(*iter).node()->setMarked(true);

					// EXTRA - Change color of the node as to show it has been marked
					(*iter).node()->SetColor(sf::Color(0, 0, 255));
				}
			}
		}
		// Pop from the prioriy queue
		pq.pop();
	}
	// Printy out function
	//////////////////////////
	if (pq.size() != 0 && pq.top() == pDest)
	{
		// Looping backwards while the pDest != 0
		//////////////////////////
		for (Node *a = pDest; a->previous() != 0; a = a->previous())
		{
			// Push back the path
			//////////////////////////
			path.push_back(a);
			a->SetColor(sf::Color(255, 0, 0));

			if (a == pDest)
				a->SetColor(sf::Color(150, 0, 150));

		}
		// Get the start Path
		//////////////////////////
		path.push_back(pStart);

		// Reverse the path " Didn't think of this the last time"
		//////////////////////////
		std::reverse(path.begin(), path.end());

		// Print the path
		//////////////////////////
		for (int i = 0; i < path.size(); i++)
			pProcess(path.at(i));
	}
}
/*
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::ucs(Node pStart, Node pDest, void(*pVisit)(Node), std::vector<Node >& path)
{
	priority_queue<Node, vector<Node >, NodeSearchCostComparer> pq;
	pq.push(pStart);

	pStart->setData(NodeType(get<0>(pStart->data()), 0, 0));
	pStart->setMarked(true);

	// while the queue is not empty AND pq.top != goal
	while (pq.size() != 0 && pq.top() != 0)
	{
		list<Arc>::const_iterator iter = pq.top()->arcList().begin();
		list<Arc>::const_iterator endIter = pq.top()->arcList().end();
		for (; iter != endIter; iter++)
		{
			if ((*iter).node() != pq.top()->previous())
			{
				int dist = (*iter).weight() + get<1>(pq.top()->data());
				int curDist = get<1>((*iter).node()->data());

				// if distance is less than child distance
				if (dist < curDist)
				{
					// This is where we calculate the 90% of the heuristic cost
					(iter).node()->setData(NodeType(get<0>((*iter).node()->data()), dist, dist * 0.9f));
					(*iter).node()->setPrevious(pq.top());
				}

				// if not marked
				if ((*iter).node()->marked() == false)
				{
					pq.push((*iter).node()); // add the child to the pq
					(*iter).node()->setMarked(true); // and mark it
				}
				// end of iteration
			}

		}
		pq.pop();
	}

	// print out the elements
	Node* print = pDest;

	while (print->previous() != 0)
	{
		path.push_back(print);
		print = print->previous();
	}
	path.push_back(pStart);

	// otherwise prints last to first element in path
	std::reverse(path.begin(), path.end());

	for (int i = 0; i < path.size(); i++)
	{
		pVisit(path.at(i)); // output for each element in path
	}
}
*/

#include "GraphNode.h"
#include "GraphArc.h"


#endif
