//////////////////////////////////////////////////////////// 
// Headers 
//////////////////////////////////////////////////////////// 
#include "stdafx.h" 

//sfml & openGL Includes
#ifdef _DEBUG 
#pragma comment(lib,"sfml-graphics-d.lib") 
#pragma comment(lib,"sfml-audio-d.lib") 
#pragma comment(lib,"sfml-system-d.lib") 
#pragma comment(lib,"sfml-window-d.lib") 
#pragma comment(lib,"sfml-network-d.lib") 
#else 
#pragma comment(lib,"sfml-graphics.lib") 
#pragma comment(lib,"sfml-audio.lib") 
#pragma comment(lib,"sfml-system.lib") 
#pragma comment(lib,"sfml-window.lib") 
#pragma comment(lib,"sfml-network.lib") 
#endif 
#pragma comment(lib,"opengl32.lib") 
#pragma comment(lib,"glu32.lib") 

#include "SFML/Graphics.hpp" 
#include "SFML/OpenGL.hpp" 
#include "SFML/System/String.hpp" 


//standard c++ includes
#include <iostream> 
#include <string>
#include <fstream>
#include "Graph.h"

using namespace std;

typedef GraphArc<tuple<string, int, int>, int >ArcType;
typedef GraphNode<tuple<string, int, int>, int >Node;
std::vector<sf::Text> pathTaking;
sf::Font font;
void pProcess(Node * pNode) {
	cout << "Visiting: " << std::get<0>(pNode->data()) << "Weight: " << std::get<1>(pNode->data()) << "Heuristic: " << std::get<2>(pNode->data()) << endl;
	sf::Text temp;
	temp.setCharacterSize(15);
	temp.setFont(font);
	temp.setStyle(sf::Text::Bold);
	temp.setString("Name: " + std::get<0>(pNode->data()) + " Cost: " + std::to_string(std::get<1>(pNode->data())) + " Huer: " + std::to_string(std::get<2>(pNode->data())));
	pathTaking.push_back(temp);
}
//////////////////////////////////////////////////////////// 
/// Entry point of application 
//////////////////////////////////////////////////////////// 
int main(int argc, char *argv[])
{
	// Create the main window 
	sf::RenderWindow App(sf::VideoMode(800, 700, 32), "SFML OpenGL");
	font.loadFromFile("C:\\Windows\\Fonts\\GARA.TTF");

	std::vector<sf::Text> display;
	std::vector<sf::IntRect> nodeRect;
	sf::Vector2i nodeStart, nodeEnd;
	sf::Text text, startText, resetText, titleText;
	sf::RectangleShape startRectShape, resetRectShape;

	text.setPosition(sf::Vector2f(600, 300));
	text.setFont(font);
	text.setStyle(sf::Text::Bold);
	text.setColor(sf::Color(0, 255, 0));
	text.setCharacterSize(15);

	int startNodeNum = -1;
	int endNodeNum = -1;
	const int NUMOFNODES = 30;

	bool startNode = false;
	bool endNode = false;
	bool aStar = false;

	Graph<tuple<string, int, int>, int > myGraph(NUMOFNODES);

	string c;
	int i = 0;
	int x = 0;
	int y = 0;

	// Create a Node and read in Node Position and Name from file
	///////////////////////////
	ifstream myfile;
	myfile.open("nodes.txt");

	while (myfile >> c >> x >> y)
		myGraph.addNode(make_tuple(c, std::numeric_limits<int>::max() * 0.00001, std::numeric_limits<int>::max() * 0.00001), i++, sf::Vector2f(x + 50, y + 100), font);
	myfile.close();


	// Add Arcs from file and Draw weight values between Arcs
	///////////////////////////
	myfile.open("arcs.txt");
	int from, to, weight;
	while (myfile >> from >> to >> weight) 
	{
		myGraph.addArc(from, to, weight);
		sf::Vector2f pos = ((myGraph.nodeArray()[from]->getPosition() - myGraph.nodeArray()[to]->getPosition()) / 2.0f) + myGraph.nodeArray()[to]->getPosition();
		text.setPosition(pos.x - 10, pos.y - 10);
		text.setString(std::to_string(weight));
		display.push_back(text);

	}
	myfile.close();

	// Set Up Collision for Nodes with Mouse Click
	///////////////////////////
	for (int i = 0; i < NUMOFNODES; i++)
	{
		sf::Vector2f nodePos = myGraph.nodeArray()[i]->getPosition();
		sf::IntRect r(nodePos.x - 25, nodePos.y - 25, myGraph.nodeArray()[i]->GetSize() * 2, myGraph.nodeArray()[i]->GetSize() * 2);
		nodeRect.push_back(r);
	}


	// Extra stuff - Buttons etc
	///////////////////////////
	startText.setCharacterSize(50);
	startText.setPosition(sf::Vector2f(65, 542));
	startText.setFont(font);
	startText.setString("Start");

	resetText = startText;
	resetText.setPosition(sf::Vector2f(410, 542));
	resetText.setString("Reset");

	titleText = startText;
	titleText.setPosition(sf::Vector2f(600, 70));
	titleText.setString("Path Taking");
	titleText.setCharacterSize(30);

	startRectShape.setFillColor(sf::Color(0, 0, 0, 0));
	startRectShape.setOutlineColor(sf::Color(255, 255,255));
	startRectShape.setOutlineThickness(2);
	startRectShape.setPosition(sf::Vector2f(50, 550));
	startRectShape.setSize(sf::Vector2f(125, 50));

	resetRectShape = startRectShape;
	resetRectShape.setPosition(sf::Vector2f(400, 550));

	// Start game loop 
	while (App.isOpen())
	{
		// Process events 
		sf::Event Event;
		while (App.pollEvent(Event))
		{
			// Close window : exit 
			if (Event.type == sf::Event::Closed)
				App.close();

			if (Event.type == sf::Event::KeyPressed)
			{ //process keyboard events

				// Escape key : exit 
				if (Event.key.code == sf::Keyboard::Escape)
					App.close();
			}
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)|| sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				sf::IntRect mouseRect(sf::Mouse::getPosition(App).x, sf::Mouse::getPosition(App).y, 1, 1);

				// Reset Function
				////////////////////////////
				if (aStar)
				{
					sf::IntRect resetRect((int)resetRectShape.getPosition().x, (int)resetRectShape.getPosition().y, (int)resetRectShape.getSize().x, (int)resetRectShape.getSize().y);
					if (mouseRect.intersects(resetRect))
					{
						startNode = false;
						endNode = false;
						aStar = false;
						pathTaking.clear();

						for (int i = 0; i < NUMOFNODES; i++)
						{
							myGraph.nodeArray()[i]->Reset();
							myGraph.nodeArray()[i]->setData(make_tuple(get<0>(myGraph.nodeArray()[i]->data()), std::numeric_limits<int>::max() * 0.00001, std::numeric_limits<int>::max() * 0.00001));
						}
					}
				}
				// A Star Function
				////////////////////////////
				else if (endNode && startNode)
				{
				
					sf::IntRect startRect((int)startRectShape.getPosition().x, (int)startRectShape.getPosition().y, (int)startRectShape.getSize().x, (int)startRectShape.getSize().y);

					if (mouseRect.intersects(startRect))
					{
						std::vector<Node *> thePath;
						myGraph.aStar(myGraph.nodeArray()[startNodeNum], myGraph.nodeArray()[endNodeNum], pProcess, thePath);
						aStar = true;
					}
				}

			}
			// Select Start Node
			////////////////////////////
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !startNode)
			{
				sf::IntRect mouseRect(sf::Mouse::getPosition(App).x, sf::Mouse::getPosition(App).y, 1, 1);
				for (int i = 0; i < NUMOFNODES; i++)
				{
					if (mouseRect.intersects((nodeRect.at(i))))
					{
						myGraph.nodeArray()[i]->SetColor(sf::Color(255, 255, 0));
						startNodeNum = i;
						startNode = true;
					}
				}
			}
			// Select Start Node
			////////////////////////////
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !endNode)
			{
				sf::IntRect mouseRect(sf::Mouse::getPosition(App).x, sf::Mouse::getPosition(App).y, 1, 1);
				for (int i = 0; i < NUMOFNODES; i++)
				{
					if (mouseRect.intersects((nodeRect.at(i))))
					{
						myGraph.nodeArray()[i]->SetColor(sf::Color(150, 0, 150));
						endNodeNum = i;
						endNode = true;

						sf::Vector2f endPos = myGraph.nodeArray()[endNodeNum]->getPosition();
						for (int i = 0; i < NUMOFNODES; i++)
						{
							sf::Vector2f startPos = myGraph.nodeArray()[i]->getPosition();
							float distance = sqrt(((endPos.x - startPos.x)*(endPos.x - startPos.x) + (endPos.y - startPos.y)*(endPos.y - startPos.y)));

							std::string temp = ("    " + (get<0>(myGraph.nodeArray()[i]->data())) + "\n" + " C-??? " + "\n" + " H-" + std::to_string((int)distance));
							myGraph.nodeArray()[i]->SetText(temp);
							myGraph.nodeArray()[i]->setData(make_tuple(get<0>(myGraph.nodeArray()[i]->data()), get<1>(myGraph.nodeArray()[i]->data()), distance));
						}
					}
				}
			}

		}
		//prepare frame
		App.clear();

		App.draw(startRectShape);
		App.draw(resetRectShape);
		App.draw(startText);
		App.draw(resetText);
		App.draw(titleText);

		for (int i = 0; i < pathTaking.size(); i++)
		{
			pathTaking.at(i).setPosition(sf::Vector2f(580, 120 + 30 * i));
			App.draw(pathTaking.at(i));
		}

		for (int i = 0; i < NUMOFNODES; i++)
			myGraph.nodeArray()[i]->DrawArcs(App);
		for (int i = 0; i < NUMOFNODES; i++)
			myGraph.nodeArray()[i]->Draw(App);

		for (int i = 0; i < display.size(); i++)
			App.draw(display.at(i));

		// Finally, display rendered frame on screen 
		App.display();
	}

	return EXIT_SUCCESS;
}