#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>


using namespace std;

/*
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
	else {
		// Fallback to checking if input.txt exists if no arg provided, otherwise
		// maybe cin? The plan said "Read the grid from standard input (or file)".
		// Let's stick to the pattern used in previous days: try file arg, else
		// default to input.txt if it exists, else cin? Actually, previous solution
		// used input.txt by default. Let's stick to that for consistency with my
		// previous code structure if possible, but the prompt didn't explicitly ask
		// for file input this time, though I added it in Day 3. Let's support file
		// input via command line arg, defaulting to cin if no arg, or maybe just
		// cin as per plan "Read the grid from standard input (or file)". To be safe
		// and consistent with Day 3's final state: Default to "input.txt" if no
		// arg, but if that fails, maybe just read from cin? Let's just implement
		// reading from the filename provided in argv[1] or "input.txt".
	}

	// Re-evaluating input strategy to be simple and robust:
	// If argc > 1, read from argv[1].
	// Else, read from cin.
	// Wait, the previous day I hardcoded "input.txt" as default.
	// Let's stick to: if argc > 1 use argv[1], else use cin. This is standard
	// competitive programming style. BUT, my previous code for Day 3 used
	// "input.txt" default. Let's use the explicit file approach from Day 3 for
	// consistency if the user expects that. However, the plan says "Read the grid
	// from standard input (or file)". I'll implement: read from argv[1] if
	// present, otherwise read from cin.

	vector<string> grid;
	string line;

	if (argc > 1) {
		ifstream file(argv[1]);
		if (!file) {
			cerr << "Error opening file: " << argv[1] << endl;
			return 1;
		}
		while (getline(file, line)) {
			grid.push_back(line);
		}
	}
	else {
		while (getline(cin, line)) {
			grid.push_back(line);
		}
	}

	int rows = grid.size();
	if (rows == 0)
		return 0;
	int cols = grid[0].size();
	int accessible_count = 0;

	int dr[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
	int dc[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

	for (int r = 0; r < rows; ++r) {
		for (int c = 0; c < cols; ++c) {
			if (grid[r][c] == '@') {
				int neighbor_count = 0;
				for (int i = 0; i < 8; ++i) {
					int nr = r + dr[i];
					int nc = c + dc[i];

					if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
						if (grid[nr][nc] == '@') {
							neighbor_count++;
						}
					}
				}

				if (neighbor_count < 4) {
					accessible_count++;
				}
			}
		}
	}

	cout << accessible_count << endl;

	return 0;
}
*/

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

	if (argc > 1) {
		ifstream file(argv[1]);
		if (!file) {
			cerr << "Error opening file: " << argv[1] << endl;
			return 1;
		}
		while (getline(file, line)) {
			grid.push_back(line);
		}
	}
	else {
		while (getline(cin, line)) {
			grid.push_back(line);
		}
	}

	int rows = grid.size();
	if (rows == 0)
		return 0;
	int cols = grid[0].size();
	int dr[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
	int dc[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

	int total_removed = 0;

	while (true) {
		vector<pair<int, int>> to_remove;
		for (int r = 0; r < rows; ++r) {
			for (int c = 0; c < cols; ++c) {
				if (grid[r][c] == '@') {
					int neighbor_count = 0;
					for (int i = 0; i < 8; ++i) {
						int nr = r + dr[i];
						int nc = c + dc[i];

						if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
							if (grid[nr][nc] == '@') {
								neighbor_count++;
							}
						}
					}

					if (neighbor_count < 4) {
						to_remove.push_back({ r, c });
					}
				}
			}
		}

		if (to_remove.empty()) {
			break;
		}

		total_removed += to_remove.size();
		for (const auto& p : to_remove) {
			grid[p.first][p.second] = '.';
		}
	}

	cout << total_removed << endl;

	return 0;
}
