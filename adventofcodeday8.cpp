#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>


using namespace std;

struct Point {
	int x, y, z;
	int id;
};

struct Edge {
	int u, v;
	long long distSq;
};

struct DSU {
	vector<int> parent;
	vector<int> size;
	int num_components;

	DSU(int n) {
		parent.resize(n);
		iota(parent.begin(), parent.end(), 0);
		size.assign(n, 1);
		num_components = n;
	}

	int find(int i) {
		if (parent[i] == i)
			return i;
		return parent[i] = find(parent[i]);
	}

	bool unite(int i, int j) {
		int root_i = find(i);
		int root_j = find(j);
		if (root_i != root_j) {
			if (size[root_i] < size[root_j])
				swap(root_i, root_j);
			parent[root_j] = root_i;
			size[root_i] += size[root_j];
			num_components--;
			return true;
		}
		return false;
	}
};

bool compareEdges(const Edge& a, const Edge& b) { return a.distSq < b.distSq; }

long long solve_part1(int N, const vector<Edge>& edges, int num_points) {
	DSU dsu(num_points);
	int connections_made = 0;
	for (const auto& edge : edges) {
		if (connections_made >= N)
			break;
		// Part 1 logic: just unite, counting standard connections regardless of
		// redundancy? "Because these two junction boxes were already in the same
		// circuit, nothing happens!" But the example counts "ten shortest
		// connections" as iterations of the process. Wait, the problem says "After
		// making the ten shortest connections...". And "connect together the 1000
		// pairs of junction boxes which are closest together." This implies we
		// process the top N edges. If they are already connected, "nothing
		// happens", but it still counts as one of the N connections attempted? Or
		// does it mean we make N *successful* connections (that merge components)?
		// Re-reading: "The next two junction boxes are 431,825,988 and 425,690,689.
		// Because these two junction boxes were already in the same circuit,
		// nothing happens!" "After making the ten shortest connections, there are
		// 11 circuits...". The list of connections in the example seems to iterate
		// through the closest pairs. "connect together the 1000 pairs of junction
		// boxes which are closest together." I will assume this means processing
		// the first 1000 edges in the sorted list.

		dsu.unite(edge.u, edge.v);
		connections_made++;
	}

	vector<int> sizes;
	vector<bool> visited(num_points, false);
	for (int i = 0; i < num_points; ++i) {
		int root = dsu.find(i);
		if (!visited[root]) {
			sizes.push_back(dsu.size[root]);
			visited[root] = true;
		}
	}

	sort(sizes.rbegin(), sizes.rend());

	long long result = 1;
	for (int i = 0; i < min((int)sizes.size(), 3); ++i) {
		result *= sizes[i];
	}
	return result;
}

long long solve_part2(const vector<Edge>& edges, int num_points,
	const vector<Point>& points) {
	DSU dsu(num_points);
	int last_u = -1, last_v = -1;

	for (const auto& edge : edges) {
		if (dsu.unite(edge.u, edge.v)) {
			last_u = edge.u;
			last_v = edge.v;
			if (dsu.num_components == 1)
				break;
		}
	}

	if (last_u != -1 && last_v != -1) {
		return (long long)points[last_u].x * points[last_v].x;
	}
	return 0;
}

int main(int argc, char* argv[]) {
	string filename = "input.txt";
	int N = 1000;

	if (argc > 1) {
		filename = argv[1];
	}
	if (argc > 2) {
		try {
			N = stoi(argv[2]);
		}
		catch (...) {
			// Ignore if not a number (could be pipe issue or just no arg)
		}
	}

	ifstream infile(filename);
	istream* input = &cin;
	if (filename != "input.txt" || argc > 1) {
		if (!infile) {
			cerr << "Error opening file: " << filename << endl;
			return 1;
		}
		input = &infile;
	}

	vector<Point> points;
	string line;
	int id_counter = 0;
	while (getline(*input, line)) {
		if (line.empty())
			continue;
		stringstream ss(line);
		string segment;
		vector<int> coords;
		while (getline(ss, segment, ',')) {
			coords.push_back(stoi(segment));
		}
		if (coords.size() == 3) {
			points.push_back({ coords[0], coords[1], coords[2], id_counter++ });
		}
	}

	vector<Edge> edges;
	for (int i = 0; i < points.size(); ++i) {
		for (int j = i + 1; j < points.size(); ++j) {
			long long dx = points[i].x - points[j].x;
			long long dy = points[i].y - points[j].y;
			long long dz = points[i].z - points[j].z;
			long long distSq = dx * dx + dy * dy + dz * dz;
			edges.push_back({ i, j, distSq });
		}
	}

	sort(edges.begin(), edges.end(), compareEdges);

	if (input == &cin)
		cin.clear();
	int part;
	cout << "Select part (1 or 2): ";
	if (!(cin >> part)) {
		cerr << "Invalid input for part selection." << endl;
		return 1;
	}

	if (part == 1) {
		cout << solve_part1(N, edges, points.size()) << endl;
	}
	else if (part == 2) {
		cout << solve_part2(edges, points.size(), points) << endl;
	}
	else {
		cerr << "Invalid part selected." << endl;
		return 1;
	}

	return 0;
}
