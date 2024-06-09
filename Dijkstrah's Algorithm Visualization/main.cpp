
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <random>

using namespace std;

const short WIDTH = 2400;
const short HEIGHT = 1500;
const short R = 15;

static int getIntSqrt(int x) {
	int l = 0, r = x;
	while (l < r) {
		long long m = (long long)(l + r) / 2;
		if (m * m < x)
			l = m + 1;
		else
			r = m;
	}
	return l;
}

static bool hasCollision(short x, short y, vector<pair<short, short>>& points) {
	for (int i = 0; i < points.size(); ++i) {
		short dx = points[i].first - x;
		short dy = points[i].second - y;
		int distSquare = dx * dx + dy * dy;
		if (distSquare <= 9 * R * R)
			return true;
	}
	return false;
}

static void generatePoints(int n, vector<pair<short, short>>& points) {
	for (int i = 0; i < n; ++i) {
		short x, y;
		do {
			x = rand() % (WIDTH - 2 * R);
			y = rand() % (HEIGHT - 2 * R);
		} while (hasCollision(x, y, points));
		points.push_back(pair<short, short>(x, y));
	}
}

static void generatePointsByHand(vector<pair<short, short>>& points) {
	int k = 4;
	points.push_back(pair<short, short>(100 * k, 100 * k));
	points.push_back(pair<short, short>(200 * k, 100 * k));
	points.push_back(pair<short, short>(300 * k, 100 * k));
	points.push_back(pair<short, short>(100 * k, 200 * k));
	points.push_back(pair<short, short>(100 * k, 300 * k));
	points.push_back(pair<short, short>(300 * k, 300 * k));
	points.push_back(pair<short, short>(200 * k, 160 * k));
	points.push_back(pair<short, short>(140 * k, 160 * k));
	points.push_back(pair<short, short>(140 * k, 120 * k));
	points.push_back(pair<short, short>(180 * k, 120 * k));
	points.push_back(pair<short, short>(180 * k, 110 * k));
	points.push_back(pair<short, short>(110 * k, 110 * k));
}

static void makeLink
(
	vector<pair<short, short>>& points, 
	vector<vector<pair<int, int>>>& graph, 
	int start, int end
) {
	int dx = (points[start].first - points[end].first);
	int dy = (points[start].second - points[end].second);
	int dist = getIntSqrt(dx * dx + dy * dy);
	graph[start].push_back(pair<int, int>(end, dist));
	graph[end].push_back(pair<int, int>(start, dist));
}

static void generateGraph
(
	vector<pair<short, short>>& points,
	vector<vector<pair<int, int>>>& graph
) {

	int n = points.size();
	int maxConnections = 1;

	for (int i = 0; i < n - 1; ++i) {

		int limit = rand() % maxConnections + 1;
		for (int now = 0; now < limit; ++now) {

			int ind = rand() % (n - i - 1) + i + 1;
			makeLink(points, graph, i, ind);
		}
	}
}

static void generateGraphByHand
(
	vector<pair<short, short>>& points,
	vector<vector<pair<int, int>>>& graph
) {
	makeLink(points, graph, 0, 3);
	makeLink(points, graph, 0, 1);
	makeLink(points, graph, 1, 0);
	//makeLink(points, graph, 1, 6);
	makeLink(points, graph, 1, 2);
	makeLink(points, graph, 2, 1);
	makeLink(points, graph, 2, 5);
	makeLink(points, graph, 2, 6);
	//makeLink(points, graph, 3, 4);
	//makeLink(points, graph, 3, 0);
	//makeLink(points, graph, 4, 3);
	makeLink(points, graph, 4, 5);
	makeLink(points, graph, 4, 6);
	makeLink(points, graph, 5, 2);
	makeLink(points, graph, 5, 4);
	//makeLink(points, graph, 6, 1);
	makeLink(points, graph, 6, 7);
	makeLink(points, graph, 6, 4);
	makeLink(points, graph, 7, 6);
	makeLink(points, graph, 7, 8);
	makeLink(points, graph, 8, 7);
	makeLink(points, graph, 8, 9);
	makeLink(points, graph, 9, 8);
	makeLink(points, graph, 9, 10);
	makeLink(points, graph, 10, 9);
	makeLink(points, graph, 10, 11);
}

static vector<int> dijkstra
(
	vector<vector<pair<int, int>>>& graph, 
	map<short, short>& fromTo, 
	int start
) {

	vector<int> distances(graph.size(), INT_MAX);
	vector<bool> used(graph.size(), false);
	int min_node = start;
	int min_dist = 0;

	distances[min_node] = 0;
	while (min_dist < INT_MAX) {

		int i = min_node;
		used[i] = true;
		for (auto& node_data : graph[i]) {
			int neighbour = node_data.first;
			int weight = node_data.second;
			int distance = min_dist + weight;
			if (distance < distances[neighbour]) {
				distances[neighbour] = distance;
				fromTo[neighbour] = i;
			}
		}

		min_dist = INT_MAX;
		for (int i = 0; i < graph.size(); ++i) {
			if (!used[i] && distances[i] < min_dist) {
				min_dist = distances[i];
				min_node = i;
			}
		}
	}

	return distances;
}

static void renderPoints
(
	vector<pair<short, short>>& points, 
	vector<int>& distances, 
	sf::RenderWindow& window, 
	sf::Text text, 
	int start, int end
) {
	sf::CircleShape shape(R);
	shape.setFillColor(sf::Color::Green);
	for (int i = 0; i < points.size(); ++i) {
		auto& point = points[i];
		shape.setPosition(sf::Vector2f(point.first, point.second));

		text.setString(to_string(i));
		text.setPosition(sf::Vector2f(points[i].first + R / 2, points[i].second + R + 20));

		window.draw(shape);
		window.draw(text);
	}
	shape.setPosition(sf::Vector2f(points[start].first, points[start].second));
	shape.setFillColor(sf::Color::Yellow);

	text.setString("Start");
	text.setPosition(sf::Vector2f(points[start].first, points[start].second - R - 5));

	window.draw(shape);
	window.draw(text);

	shape.setPosition(sf::Vector2f(points[end].first, points[end].second));
	shape.setFillColor(sf::Color::Blue);

	text.setString(to_string(distances[end]) + " km");
	text.setPosition(sf::Vector2f(points[end].first, points[end].second - R - 5));

	window.draw(shape);
	window.draw(text);
}

static void renderLines
(
	vector<vector<pair<int, int>>>& graph, 
	vector<pair<short, short>>& points, 
	map<short, short>& fromTo, int end, 
	sf::RenderWindow& window, 
	sf::Text& text
) {
	int from = end;
	while (fromTo.count(from)) {
		int to = fromTo[from];

		sf::Vertex line[] = {
			sf::Vertex(sf::Vector2f(points[from].first + R, points[from].second + R), sf::Color::Red),
			sf::Vertex(sf::Vector2f(points[to].first + R, points[to].second + R), sf::Color::Red)
		};

		int midX = (points[from].first + points[to].first) / 2;
		int midY = (points[from].second + points[to].second) / 2;
		text.setPosition(sf::Vector2f(midX, midY));

		int dx = (points[from].first - points[to].first);
		int dy = (points[from].second - points[to].second);
		int dist = getIntSqrt(dx * dx + dy * dy);
		text.setFillColor(sf::Color::White);
		text.setString(to_string(dist));

		window.draw(line, 2, sf::Lines);
		window.draw(text);

		from = to;
	}
}

static void renderAllLines
(
	vector<vector<pair<int, int>>>& graph,
	vector<pair<short, short>>& points,
	sf::RenderWindow& window,
	sf::Text& text
) {
	for (int from = 0; from < points.size(); ++from) {
		for (auto& node : graph[from]) {

			int to = node.first;

			sf::Color color = sf::Color::White;
			color.a = 50;
			sf::Vertex line[] = {
				sf::Vertex(sf::Vector2f(points[from].first + R, points[from].second + R), color),
				sf::Vertex(sf::Vector2f(points[to].first + R, points[to].second + R), color)
			};

			int midX = (points[from].first + points[to].first) / 2;
			int midY = (points[from].second + points[to].second) / 2;
			text.setPosition(sf::Vector2f(midX, midY));

			int dx = (points[from].first - points[to].first);
			int dy = (points[from].second - points[to].second);
			int dist = getIntSqrt(dx * dx + dy * dy);
			text.setString(to_string(dist));
			text.setFillColor(color);

			window.draw(line, 2, sf::Lines);
			window.draw(text);
		}
	}
}

static void renderVisualization 
(
	vector<vector<pair<int, int>>>& graph, 
	vector<pair<short, short>>& points, 
	map<short, short>& fromTo, 
	vector<int>& distances, 
	int start, int end
) {

	sf::Font font;
	if (!font.loadFromFile("Roboto-Regular.ttf")) {
		cout << "Font not found" << endl;
	}
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(25);
	text.setOutlineColor(sf::Color::Black);

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Dijktra's Algorithm Visualization");
	sf::View view(sf::FloatRect(0, 0, WIDTH, HEIGHT));

	window.setView(view);

	sf::Vector2f anchor;
	sf::Vector2f viewSize = view.getSize();
	float powerOfZoom = 0.05f;
	bool isPressed = false;

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
			if (event.type == sf::Event::MouseButtonPressed)
			{
				isPressed = true;
				anchor.x = event.mouseButton.x;
				anchor.y = event.mouseButton.y;
			}
			if (event.type == sf::Event::MouseMoved and isPressed)
			{
				int x, y;
				float dx, dy, scale;

				x = event.mouseMove.x;
				y = event.mouseMove.y;
				
				scale = viewSize.x / WIDTH;
				dx = (anchor.x - x) * scale;
				dy = (anchor.y - y) * scale;

				view.move(sf::Vector2f(dx, dy));
				cout << x << " " << y << endl;

				anchor.x = x;
				anchor.y = y;
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				isPressed = false;
			}
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		renderPoints(points, distances, window, text, start, end);
		renderAllLines(graph, points, window, text);
		renderLines(graph, points, fromTo, end, window, text);
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

	vector<pair<short, short>> points;

	if (isRandom)
	{
		int n;
		cout << "Enter count of points: "; cin >> n; cout << endl;
		generatePoints(n, points);
	}
	else
	{
		generatePointsByHand(points);
	}
	
	vector<vector<pair<int, int>>> graph(points.size(), vector<pair<int, int>>(0));

	if (isRandom) 
	{
		generateGraph(points, graph);
	}
	else 
	{
		generateGraphByHand(points, graph);
	}

	int start = 0;
	int end = points.size() - 1;
	map<short, short> fromTo;
	vector<int> distances = dijkstra(graph, fromTo, start);

	renderVisualization(graph, points, fromTo, distances, start, end);

	return 0;
}