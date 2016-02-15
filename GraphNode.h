#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <list>

// Forward references
template <typename NodeType, typename ArcType> class GraphArc;

// -------------------------------------------------------
// Name:        GraphNode
// Description: This is the node class. The node class 
//              contains data, and has a linked list of 
//              arcs.
// -------------------------------------------------------
template<class NodeType, class ArcType>
class GraphNode {
private:    
// typedef the classes to make our lives easier.
    typedef GraphArc<NodeType, ArcType> Arc;
    typedef GraphNode<NodeType, ArcType> Node;
// -------------------------------------------------------
// Description: data inside the node
// -------------------------------------------------------
    NodeType m_data;

// -------------------------------------------------------
// Description: list of arcs that the node has.
// -------------------------------------------------------
    list<Arc> m_arcList;

// -------------------------------------------------------
// Description: This remembers if the node is marked.
// -------------------------------------------------------
    bool m_marked;

// -------------------------------------------------------
// Description: pointer to previous node
// -------------------------------------------------------
	Node *m_previous;
	sf::Vertex line[2];
	sf::Vector2f m_position;
	sf::CircleShape m_shape;
	const int m_size = 30;
	sf::Color m_color;
	sf::Text m_text;
	sf::Font font;
	
public:
	// Constructor function
	GraphNode( Node * previous = 0 ) : m_previous( previous ) {}

    // Accessor functions
    list<Arc> const & arcList() const {
        return m_arcList;              
    }

    bool marked() const {
        return m_marked;
    }

    NodeType const & data() const {
        return m_data;
    }

	Node * previous() const {
		return m_previous;
	}

    // Manipulator functions
    void setData(NodeType data) {
        m_data = data;
    }
    
    void setMarked(bool mark) {
        m_marked = mark;
    }
           
	void setPrevious(Node *previous) {
		m_previous = previous;
	}



    Arc* getArc( Node* pNode );    
    void addArc( Node* pNode, ArcType pWeight );
	void removeArc( Node* pNode );

	int GetSize(){ return m_size; }
	void SetPosition(sf::Vector2f p_pos){m_position = p_pos; }
	void SetColor(sf::Color p_color){m_shape.setFillColor(p_color);}
	void SetText(std::string p_string){ m_text.setString(p_string);}
	sf::Vector2f getPosition(){ return m_position; }

	void Reset()
	{
		m_marked = false;
		m_previous = NULL;
		m_shape.setFillColor(sf::Color(255, 255, 255));
		m_text.setString("    " + (get<0>(m_data)) + "\n" + " C-??? " + "\n" + " H-??? ");
	}

	void SetUpNode(sf::Font p_font)
	{
		font = p_font;
		m_shape.setOrigin(m_size, m_size);
		m_shape.setPosition(m_position);
		m_shape.setRadius(m_size);
		m_text.setPosition(sf::Vector2f(m_position.x - 25, m_position.y - 30));
		m_text.setString("    " + (get<0>(m_data)) + "\n" + " C-??? " + "\n" + " H-??? ");
		m_text.setCharacterSize(15);
		m_text.setColor(sf::Color(0, 0, 0));
		m_text.setFont(font);
		m_text.setStyle(sf::Text::Bold);
	}

	void DrawArcs(sf::RenderWindow &p_window)
	{
		list<Arc>::iterator iter = m_arcList.begin();
		list<Arc>::iterator endIter = m_arcList.end();
		for (; iter != endIter; ++iter)
			(*iter).Draw(p_window);
	}
	void Draw(sf::RenderWindow &p_window)
	{
		p_window.draw(m_shape);
		p_window.draw(m_text);
	}
};

// ----------------------------------------------------------------
//  Name:           getArc
//  Description:    This finds the arc in the current node that
//                  points to the node in the parameter.
//  Arguments:      The node that the arc connects to.
//  Return Value:   A pointer to the arc, or 0 if an arc doesn't
//                  exist from this to the specified input node.
// ----------------------------------------------------------------
template<typename NodeType, typename ArcType>
GraphArc<NodeType, ArcType>* GraphNode<NodeType, ArcType>::getArc( Node* pNode ) {

     list<Arc>::iterator iter = m_arcList.begin();
     list<Arc>::iterator endIter = m_arcList.end();
     Arc* pArc = 0;
     
     // find the arc that matches the node
     for( ; iter != endIter && pArc == 0; ++iter ) {         
          if ( (*iter).node() == pNode) {
               pArc = &( (*iter) );
          }
     }

     // returns null if not found
     return pArc;
}


// ----------------------------------------------------------------
//  Name:           addArc
//  Description:    This adds an arc from the current node pointing
//                  to the first parameter, with the second parameter 
//                  as the weight.
//  Arguments:      First argument is the node to connect the arc to.
//                  Second argument is the weight of the arc.
//  Return Value:   None.
// ----------------------------------------------------------------
template<typename NodeType, typename ArcType>
void GraphNode<NodeType, ArcType>::addArc( Node* pNode, ArcType weight ) {
   // Create a new arc.
   Arc a;
   a.setNode(pNode);
   a.setWeight(weight);
   a.setLine(m_shape.getPosition(), pNode->getPosition());
   /*
   a.setLines(m_shape.getPos, pNode->getPosition);
   a.lines[0] = m_SHAPE.GETPOS;
   a.lines[1] = pNode->getPostion();
   */

   // Add it to the arc list.
   m_arcList.push_back( a );
}


// ----------------------------------------------------------------
//  Name:           removeArc
//  Description:    This finds an arc from this node to input node 
//                  and removes it.
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<typename NodeType, typename ArcType>
void GraphNode<NodeType, ArcType>::removeArc( Node* pNode ) {
     list<Arc>::iterator iter = m_arcList.begin();
     list<Arc>::iterator endIter = m_arcList.end();

     int size = m_arcList.size();
     // find the arc that matches the node
     for( ; iter != endIter && m_arcList.size() == size; ++iter ) {
          if ( (*iter).node() == pNode) {
             m_arcList.remove( (*iter) );
          }                           
     }
}

#include "GraphArc.h"

#endif
