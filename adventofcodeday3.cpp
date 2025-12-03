#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

/*
int main(int argc, char* argv[]) {
	string filename = "input.txt";
	if (argc > 1) {
		filename = argv[1];
	}

	ifstream infile(filename);
	if (!infile) {
		cerr << "Error opening file: " << filename << endl;
		return 1;
	}

	string line;
	long long total_joltage = 0;

	while (getline(infile, line)) {
		if (line.empty())
			continue;

		int max_joltage = -1;
		for (size_t i = 0; i < line.length(); ++i) {
			for (size_t j = i + 1; j < line.length(); ++j) {
				int digit1 = line[i] - '0';
				int digit2 = line[j] - '0';
				int current_joltage = digit1 * 10 + digit2;
				if (current_joltage > max_joltage) {
					max_joltage = current_joltage;
				}
			}
		}
		if (max_joltage != -1) {
			total_joltage += max_joltage;
		}
	}

	cout << total_joltage << endl;
	return 0;
}
*/

int main(int argc, char* argv[]) {
	string filename = "input.txt";
	if (argc > 1) {
		filename = argv[1];
	}

	ifstream infile(filename);
	if (!infile) {
		cerr << "Error opening file: " << filename << endl;
		return 1;
	}

	string line;
	long long total_joltage = 0;

	while (getline(infile, line)) {
		if (line.empty())
			continue;

		string result = "";
		int current_index = -1;
		int len = line.length();
		int k = 12; // We need 12 digits

		for (int i = 0; i < 12; ++i) {
			char max_digit = -1;
			int max_idx = -1;
			// Search range: from current_index + 1 to len - (digits needed - 1) - 1
			// We need (12 - i) digits total, including this one.
			// So we need (12 - i - 1) digits AFTER this one.
			// Last possible index for this digit is len - 1 - (12 - i - 1) = len - 12
			// + i

			int search_limit = len - k;

			for (int j = current_index + 1; j <= search_limit; ++j) {
				if (line[j] > max_digit) {
					max_digit = line[j];
					max_idx = j;
				}
				if (max_digit == '9')
					break; // Optimization: can't get better than 9
			}

			result += max_digit;
			current_index = max_idx;
			k--;
		}

		total_joltage += stoll(result);
	}

	cout << total_joltage << endl;
	return 0;
}