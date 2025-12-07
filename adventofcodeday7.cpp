#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>


using namespace std;

long long solve_part1(const vector<string>& grid, int start_row,
	int start_col) {
	int rows = grid.size();
	int cols = grid[0].size();
	set<int> active_cols;
	active_cols.insert(start_col);

	long long split_count = 0;

	for (int r = start_row + 1; r < rows; ++r) {
		set<int> next_active_cols;
		if (active_cols.empty())
			break;

		for (int c : active_cols) {
			if (c < 0 || c >= cols)
				continue;

			char cell = grid[r][c];
			if (cell == '^') {
				split_count++;
				if (c - 1 >= 0)
					next_active_cols.insert(c - 1);
				if (c + 1 < cols)
					next_active_cols.insert(c + 1);
			}
			else {
				next_active_cols.insert(c);
			}
		}
		active_cols = next_active_cols;
	}
	return split_count;
}

long long solve_part2(const vector<string>& grid, int start_row,
	int start_col) {
	int rows = grid.size();
	int cols = grid[0].size();
	map<int, long long> active_counts;
	active_counts[start_col] = 1;

	for (int r = start_row + 1; r < rows; ++r) {
		map<int, long long> next_active_counts;
		if (active_counts.empty())
			break;

		for (auto const& [c, count] : active_counts) {
			if (c < 0 || c >= cols)
				continue;

			char cell = grid[r][c];
			if (cell == '^') {
				if (c - 1 >= 0)
					next_active_counts[c - 1] += count;
				if (c + 1 < cols)
					next_active_counts[c + 1] += count;
			}
			else {
				next_active_counts[c] += count;
			}
		}
		active_counts = next_active_counts;
	}

	long long total_timelines = 0;
	for (auto const& [c, count] : active_counts) {
		total_timelines += count;
	}
	return total_timelines;
}

int main(int argc, char* argv[]) {
	string filename = "input.txt";
	if (argc > 1) {
		filename = argv[1];
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

	vector<string> grid;
	string line;
	while (getline(*input, line)) {
		grid.push_back(line);
	}

	int rows = grid.size();
	if (rows == 0)
		return 0;

	int start_row = -1, start_col = -1;
	for (int r = 0; r < rows; ++r) {
		size_t found = grid[r].find('S');
		if (found != string::npos) {
			start_row = r;
			start_col = found;
			break;
		}
	}

	if (start_row == -1) {
		cerr << "Start point S not found." << endl;
		return 0;
	}

	if (input == &cin)
		cin.clear();
	int part;
	cout << "Select part (1 or 2): ";
	if (!(cin >> part)) {
		cerr << "Invalid input for part selection." << endl;
		return 1;
	}

	if (part == 1) {
		cout << solve_part1(grid, start_row, start_col) << endl;
	}
	else if (part == 2) {
		cout << solve_part2(grid, start_row, start_col) << endl;
	}
	else {
		cerr << "Invalid part selected." << endl;
		return 1;
	}

	return 0;
}
