#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>


using namespace std;

long long solve_part1(const vector<pair<long long, long long>>& ranges,
	istream& input) {
	string line;
	long long fresh_count = 0;
	while (getline(input, line)) {
		if (line.empty())
			continue;
		long long id = stoll(line);
		bool is_fresh = false;
		for (const auto& range : ranges) {
			if (id >= range.first && id <= range.second) {
				is_fresh = true;
				break;
			}
		}
		if (is_fresh) {
			fresh_count++;
		}
	}
	return fresh_count;
}

long long solve_part2(vector<pair<long long, long long>> ranges) {
	if (ranges.empty())
		return 0;
	sort(ranges.begin(), ranges.end());

	vector<pair<long long, long long>> merged;
	for (const auto& range : ranges) {
		if (merged.empty() || range.first > merged.back().second + 1) {
			merged.push_back(range);
		}
		else {
			merged.back().second = max(merged.back().second, range.second);
		}
	}

	long long total_fresh = 0;
	for (const auto& range : merged) {
		total_fresh += (range.second - range.first + 1);
	}
	return total_fresh;
}

int main(int argc, char* argv[]) {
	string filename = "input.txt";
	if (argc > 1) {
		filename = argv[1];
	}

	ifstream infile(filename);
	istream* input_ptr = &cin;
	if (filename != "input.txt" || argc > 1) {
		if (!infile) {
			cerr << "Error opening file: " << filename << endl;
			return 1;
		}
		input_ptr = &infile;
	}

	vector<pair<long long, long long>> ranges;
	string line;

	// Read ranges
	while (getline(*input_ptr, line)) {
		if (line.empty()) {
			break;
		}

		size_t dash_pos = line.find('-');
		if (dash_pos != string::npos) {
			long long start = stoll(line.substr(0, dash_pos));
			long long end = stoll(line.substr(dash_pos + 1));
			ranges.push_back({ start, end });
		}
	}

	int part;
	if (input_ptr == &cin) {
		cin.clear();
	}

	cout << "Select part (1 or 2): ";
	if (!(cin >> part)) {
		cerr << "Invalid input for part selection." << endl;
		return 1;
	}

	if (part == 1) {
		cout << solve_part1(ranges, *input_ptr) << endl;
	}
	else if (part == 2) {
		cout << solve_part2(ranges) << endl;
	}
	else {
		cerr << "Invalid part selected." << endl;
		return 1;
	}

	return 0;
}
