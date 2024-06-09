
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <fstream>

using namespace std;

const short WIDTH = 2400;
const short HEIGHT = 1500;
const short R = 15;
const short FONTSIZE = 25;

static int getIntSqrt(int x) 
{
	long long l = 0, r = x, m;
	while (l < r) {
		m = (l + r) / 2;
		if (m * m < x)
			l = m + 1;
		else
			r = m;
	}
	return l;
}

static int checkCollision(int x, int y, vector<sf::Vector2i>& points) 
{
	for (int i = 0; i < points.size(); ++i) 
	{
		int dx = points[i].x - x;
		int dy = points[i].y - y;
		int distSquare = dx * dx + dy * dy;
		if (distSquare <= 9 * R * R) {
			return i;
		}
	}
	return -1;
}

static void generatePoints(int n, vector<sf::Vector2i>& points)
{
	for (int i = 0; i < n; ++i) 
	{
		short x, y;
		do {
			x = rand() % (WIDTH - 2 * R);
			y = rand() % (HEIGHT - 2 * R);
		} while (checkCollision(x, y, points) != -1);

		points.push_back(sf::Vector2i(x, y));
	}
}

static void readPointsFromFile(vector<sf::Vector2i>& points) 
{
	ifstream infile("points-data.txt");
	int x, y;
	while (infile >> x >> y)
	{
		points.push_back(sf::Vector2i(x, y));
	}
	infile.close();
}

static void makeLink
(
	vector<sf::Vector2i>& points, 
	vector<vector<pair<int, int>>>& graph, 
	int start, 
	int end
) 
{
	int dx = (points[start].x - points[end].x);
	int dy = (points[start].y - points[end].y);
	int dist = getIntSqrt(dx * dx + dy * dy);
	graph[start].push_back(pair<int, int>(end, dist));
	graph[end].push_back(pair<int, int>(start, dist));
}

static void generateGraph
(
	vector<sf::Vector2i>& points,
	vector<vector<pair<int, int>>>& graph
) 
{
	int n = points.size();
	int maxConnections = 1;

	for (int i = 0; i < n - 1; ++i) 
	{
		int limit = rand() % maxConnections + 1;
		
		for (int now = 0; now < limit; ++now) 
		{
			int ind = rand() % (n - i - 1) + i + 1;
			makeLink(points, graph, i, ind);
		}
	}
}

static void readGraphFromFile
(
	vector<sf::Vector2i>& points,
	vector<vector<pair<int, int>>>& graph
) 
{
	ifstream infile("graph-data.txt");
	int start, end;
	while (infile >> start >> end)
	{
		makeLink(points, graph, start, end);
	}
	infile.close();
}

static vector<int> dijkstra
(
	vector<vector<pair<int, int>>>& graph, 
	vector<int>& fromTo, 
	int start
) 
{
	vector<int> distances(graph.size(), INT_MAX);
	vector<bool> used(graph.size(), false);
	int min_node = start;
	int min_dist = 0;

	fromTo = vector<int>(graph.size(), -1);
	distances[min_node] = 0;

	while (min_dist < INT_MAX) 
	{
		int i = min_node;
		used[i] = true;

		for (auto& node_data : graph[i]) 
		{
			int neighbour = node_data.first;
			int weight = node_data.second;
			int distance = min_dist + weight;
			if (distance < distances[neighbour]) 
			{
				distances[neighbour] = distance;
				fromTo[neighbour] = i;
			}
		}

		min_dist = INT_MAX;
		for (int i = 0; i < graph.size(); ++i) 
		{
			if (!used[i] && distances[i] < min_dist) 
			{
				min_dist = distances[i];
				min_node = i;
			}
		}
	}

	return distances;
}

static void renderPoints
(
	vector<sf::Vector2i>& points, 
	vector<int>& distances, 
	sf::RenderWindow& window, 
	sf::Text text, 
	int start, int end
) 
{
	sf::CircleShape shape(R);

	shape.setFillColor(sf::Color::Green);

	for (int i = 0; i < points.size(); ++i) 
	{
		auto& point = points[i];
		shape.setPosition(sf::Vector2f(point.x, point.y));

		text.setString(to_string(i));
		text.setPosition(sf::Vector2f(points[i].x + R / 2, points[i].y + R + 20));

		window.draw(shape);
		window.draw(text);
	}

	string strDistance = to_string(distances[end]) + " km";
	if (distances[end] == INT_MAX)
	{
		strDistance = "Can't be reached";
	}

	shape.setPosition(sf::Vector2f(points[start].x, points[start].y));
	shape.setFillColor(sf::Color::Yellow);
	text.setString("Start");
	text.setPosition(sf::Vector2f(points[start].x, points[start].y - R - 5));
	window.draw(shape);
	window.draw(text);

	shape.setPosition(sf::Vector2f(points[end].x, points[end].y));
	shape.setFillColor(sf::Color::Blue);
	text.setString(strDistance);
	text.setPosition(sf::Vector2f(points[end].x, points[end].y - R - 5));
	window.draw(shape);
	window.draw(text);
}

static void renderLine
(
	sf::RenderWindow& window,
	sf::Text& text,
	sf::Color color,
	vector<sf::Vector2i>& points,
	int from,
	int to
) 
{
	sf::Vertex line[] = {
		sf::Vertex(sf::Vector2f(points[from].x + R, points[from].y + R), color),
		sf::Vertex(sf::Vector2f(points[to].x + R, points[to].y + R), color)
	};

	int midX = (points[from].x + points[to].x) / 2;
	int midY = (points[from].y + points[to].y) / 2;

	int dx = (points[from].x - points[to].x);
	int dy = (points[from].y - points[to].y);
	int dist = getIntSqrt(dx * dx + dy * dy);

	text.setPosition(sf::Vector2f(midX, midY));
	text.setFillColor(sf::Color::White);
	text.setString(to_string(dist));

	window.draw(line, 2, sf::Lines);
	window.draw(text);
}

static void renderRedLines
(
	vector<vector<pair<int, int>>>& graph, 
	vector<sf::Vector2i>& points, 
	vector<int>& fromTo,
	int start,
	int end, 
	sf::RenderWindow& window, 
	sf::Text& text
) 
{
	int from = end;
	while (from != start)
	{
		int to = fromTo[from];
		if (to == -1)
		{
			break;
		}
		sf::Color color = sf::Color::Red;

		renderLine(window, text, color, points, from, to);
		from = to;
	}
}

static void renderAllLines
(
	vector<vector<pair<int, int>>>& graph,
	vector<sf::Vector2i>& points,
	sf::RenderWindow& window,
	sf::Text& text
) 
{
	for (int from = 0; from < points.size(); ++from) 
	{
		for (auto& node : graph[from]) 
		{
			int to = node.first;
			sf::Color color = sf::Color::White;
			
			color.a = 50;
			renderLine(window, text, color, points, from, to);
		}
	}
}

static void renderVisualization 
(
	vector<vector<pair<int, int>>>& graph, 
	vector<sf::Vector2i>& points, 
	vector<int>& fromTo, 
	vector<int>& distances, 
	int start, 
	int end
) {

	sf::Font font;
	if (!font.loadFromFile("Roboto-Regular.ttf")) {
		cout << "Font not found" << endl;
	}
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(FONTSIZE);
	text.setOutlineColor(sf::Color::Black);

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Dijktra's Algorithm Visualization");
	sf::View view(sf::FloatRect(0, 0, WIDTH, HEIGHT));
	sf::Vector2f mainAnchor(0, 0);
	sf::Vector2f anchor;
	sf::Vector2f viewSize = view.getSize();
	float powerOfZoom = 0.05f;
	bool isPressed = false;
	int firstNode = -1;

	while (window.isOpen()) 
	{
		sf::Event event;

		while (window.pollEvent(event)) 
		{
			if (event.type == sf::Event::MouseWheelScrolled)
			{
				view.zoom(1.0f - powerOfZoom * event.mouseWheelScroll.delta);
				viewSize = view.getSize();
			}

			if (event.type == sf::Event::KeyPressed
				and event.key.scancode == sf::Keyboard::Scan::F)
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				int node = checkCollision(mousePos.x - mainAnchor.x, mousePos.y - mainAnchor.y, points);
				if (node == -1)
				{
					break;
				}

				if (firstNode == -1)
				{
					firstNode = node;
				}
				else
				{
					makeLink(points, graph, firstNode, node);
					distances = dijkstra(graph, fromTo, start);
					firstNode = -1;
				}
			}

			if (event.type == sf::Event::KeyPressed
				and event.key.scancode == sf::Keyboard::Scan::A)
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				int node = checkCollision(mousePos.x - mainAnchor.x, mousePos.y - mainAnchor.y, points);
				if (node != -1)
				{
					break;
				}

				points.push_back(sf::Vector2i(mousePos.x, mousePos.y));
				graph.push_back(vector<pair<int, int>>(0));
				distances.push_back(INT_MAX);
				fromTo.push_back(-1);
			}

			if (event.type == sf::Event::MouseButtonPressed
				and (event.mouseButton.button == sf::Mouse::Left
					or event.mouseButton.button == sf::Mouse::Right)
				)
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				int node = checkCollision(mousePos.x - mainAnchor.x, mousePos.y - mainAnchor.y, points);
				if (node == -1)
				{
					break;
				}

				if (event.mouseButton.button == sf::Mouse::Left
					and start != node)
				{
					start = node;
					distances = dijkstra(graph, fromTo, start);
				}
				else
				{
					end = node;
				}
			}

			if (event.type == sf::Event::MouseButtonPressed
				and event.mouseButton.button == sf::Mouse::Middle)
			{
				isPressed = true;
				anchor.x = event.mouseButton.x;
				anchor.y = event.mouseButton.y;
			}

			if (event.type == sf::Event::MouseMoved and isPressed)
			{
				float x, y;
				float dx, dy, scale;

				x = event.mouseMove.x;
				y = event.mouseMove.y;
				
				scale = viewSize.x / WIDTH;
				dx = (anchor.x - x) * scale;
				dy = (anchor.y - y) * scale;

				view.move(sf::Vector2f(dx, dy));
				mainAnchor.x -= dx;
				mainAnchor.y -= dy;

				anchor.x = x;
				anchor.y = y;
			}

			if (event.type == sf::Event::MouseButtonReleased
				and event.mouseButton.button == sf::Mouse::Middle)
			{
				isPressed = false;
			}

			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}
		window.clear();
		renderPoints(points, distances, window, text, start, end);
		renderAllLines(graph, points, window, text);
		renderRedLines(graph, points, fromTo, start, end, window, text);
		window.setView(view);
		window.display();
	}
}

int main() 
{
	string s = "";
	bool isRandom;

	cout << "Hello! It's the Dijkstrah's Algorithm Visualization Program!)" << endl;
	cout << "Do you want to set points from file or by random?" << endl;
	cout << "print R to set by random and F to set from File" << endl;

	while (s != "R" and s != "F" and s != "r" and s != "f") 
	{
		cout << "Proceed (R/f): "; 
		cin >> s;
	}

	isRandom = (s == "R" or s == "r") ? true : false;

	vector<sf::Vector2i> points;

	if (isRandom)
	{
		int n;
		cout << "Enter count of points: "; cin >> n; cout << endl;
		generatePoints(n, points);
	}
	else
	{
		readPointsFromFile(points);
	}
	
	vector<vector<pair<int, int>>> graph(points.size(), vector<pair<int, int>>(0));

	if (isRandom) 
	{
		generateGraph(points, graph);
	}
	else 
	{
		readGraphFromFile(points, graph);
	}

	int start = 0;
	int end = points.size() - 1;
	vector<int> fromTo(points.size(), -1);
	vector<int> distances = dijkstra(graph, fromTo, start);

	renderVisualization(graph, points, fromTo, distances, start, end);

	return 0;
}