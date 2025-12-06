#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


using namespace std;

// Helper to extract strips
vector<vector<string>> extract_strip_data(const vector<string>& lines) {
	vector<vector<string>> strips;
	size_t rows = lines.size();
	if (rows == 0)
		return strips;
	size_t cols = lines[0].size();

	size_t col = 0;
	while (col < cols) {
		// Skip empty columns
		while (col < cols) {
			bool empty = true;
			for (const string& l : lines) {
				if (col < l.length() && l[col] != ' ') {
					empty = false;
					break;
				}
			}
			if (!empty)
				break;
			col++;
		}

		if (col >= cols)
			break;

		// Find end of strip
		size_t start_col = col;
		size_t end_col = col;
		while (end_col < cols) {
			bool empty = true;
			for (const string& l : lines) {
				if (end_col < l.length() && l[end_col] != ' ') {
					empty = false;
					break;
				}
			}
			if (empty)
				break;
			end_col++;
		}

		vector<string> strip;
		for (const string& l : lines) {
			if (start_col < l.length()) {
				size_t len = min((size_t)(end_col - start_col), l.length() - start_col);
				strip.push_back(l.substr(start_col, len));
			}
			else {
				strip.push_back("");
			}
		}
		strips.push_back(strip);

		col = end_col;
	}
	return strips;
}

long long evaluate_strip_p1(const vector<string>& strip) {
	if (strip.empty())
		return 0;

	vector<long long> numbers;
	char op = ' ';

	for (const string& line : strip) {
		size_t first = line.find_first_not_of(" ");
		if (first == string::npos)
			continue;

		string trimmed = line.substr(first, line.find_last_not_of(" ") - first + 1);
		if (trimmed.empty())
			continue;

		if (isdigit(trimmed[0]) || (trimmed.size() > 1 && trimmed[0] == '-')) {
			numbers.push_back(stoll(trimmed));
		}
		else if (trimmed.size() == 1 &&
			(trimmed[0] == '+' || trimmed[0] == '*')) {
			op = trimmed[0];
		}
	}

	if (op == '+') {
		long long sum = 0;
		for (long long num : numbers)
			sum += num;
		return sum;
	}
	else if (op == '*') {
		long long prod = 1;
		for (long long num : numbers)
			prod *= num;
		return prod;
	}
	return 0;
}

long long evaluate_strip_p2(const vector<string>& strip) {
	if (strip.empty())
		return 0;

	vector<long long> numbers;
	char op = ' ';
	size_t width = 0;
	for (const auto& s : strip)
		width = max(width, s.length());

	// Iterate columns
	for (size_t c = 0; c < width; ++c) {
		string num_str = "";

		for (const string& line : strip) {
			if (c < line.length() && line[c] != ' ') {
				if (isdigit(line[c])) {
					num_str += line[c];
				}
				else if (line[c] == '+' || line[c] == '*') {
					op = line[c]; // Found operator
				}
			}
		}

		if (!num_str.empty()) {
			numbers.push_back(stoll(num_str));
		}
	}

	if (op == '+') {
		long long sum = 0;
		for (long long num : numbers)
			sum += num;
		return sum;
	}
	else if (op == '*') {
		long long prod = 1;
		for (long long num : numbers)
			prod *= num;
		return prod;
	}
	return 0;
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

	vector<string> lines;
	string line;
	size_t max_len = 0;
	while (getline(*input_ptr, line)) {
		lines.push_back(line);
		max_len = max(max_len, line.length());
	}

	// Pad lines globally
	for (string& l : lines) {
		if (l.length() < max_len) {
			l.resize(max_len, ' ');
		}
	}

	vector<vector<string>> strips = extract_strip_data(lines);

	int part;
	if (input_ptr == &cin)
		cin.clear();
	cout << "Select part (1 or 2): ";
	if (!(cin >> part)) {
		cerr << "Invalid input for part selection." << endl;
		return 1;
	}

	long long grand_total = 0;
	for (const auto& strip : strips) {
		if (part == 1) {
			grand_total += evaluate_strip_p1(strip);
		}
		else if (part == 2) {
			grand_total += evaluate_strip_p2(strip);
		}
	}

	cout << grand_total << endl;

	return 0;
}
