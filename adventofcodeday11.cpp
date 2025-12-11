#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>


using namespace std;

map<string, vector<string>> adj;
map<pair<string, string>, long long> memo_map;

long long count_paths(string u, string target) {
	if (u == target)
		return 1;
	if (memo_map.count({ u, target }))
		return memo_map[{u, target}];

	long long paths = 0;
	if (adj.count(u)) {
		for (const string& v : adj[u]) {
			paths += count_paths(v, target);
		}
	}
	return memo_map[{u, target}] = paths;
}

void solve_part1() {
	memo_map.clear();
	cout << count_paths("you", "out") << endl;
}

void solve_part2() {
	memo_map.clear();
	// Path 1: svr -> dac -> fft -> out
	long long p1 = count_paths("svr", "dac") * count_paths("dac", "fft") *
		count_paths("fft", "out");

	// Path 2: svr -> fft -> dac -> out
	long long p2 = count_paths("svr", "fft") * count_paths("fft", "dac") *
		count_paths("dac", "out");

	cout << p1 + p2 << endl;
}

int main(int argc, char* argv[]) {
	string filename = "input.txt";
	if (argc > 1) {
		filename = argv[1];
	}

	ifstream infile(filename);
	istream* input_stream = &cin;
	if (filename != "input.txt" || argc > 1) {
		if (!infile) {
			cerr << "Error opening file: " << filename << endl;
			return 1;
		}
		input_stream = &infile;
	}

	// Read graph from file always, strictly speaking we should follow standard
	// input logic But since part selection is needed, we assume file arg is
	// always the graph file.
	string line;
	// We strictly use the file stream for graph setup to avoid eating cin
	ifstream graph_in(filename);
	if (!graph_in) {
		cerr << "Error opening graph file: " << filename << endl;
		return 1;
	}

	while (getline(graph_in, line)) {
		if (line.empty())
			continue;
		size_t colon_pos = line.find(':');
		string src = line.substr(0, colon_pos);
		string dests_str = line.substr(colon_pos + 1);
		stringstream ss(dests_str);
		string dest;
		while (ss >> dest) {
			adj[src].push_back(dest);
		}
	}

	int part;
	cout << "Select part (1 or 2): ";
	if (!(cin >> part)) {
		cerr << "Invalid part selection" << endl;
		return 1;
	}

	if (part == 1) {
		solve_part1();
	}
	else if (part == 2) {
		solve_part2();
	}
	else {
		cerr << "Invalid part" << endl;
	}

	return 0;
}
