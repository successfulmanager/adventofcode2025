#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;


/*
// Function to check if an ID is invalid (repeated sequence of digits)
bool is_invalid(long long n) {
  string s = to_string(n);
  if (s.length() % 2 != 0) {
    return false;
  }
  int half = s.length() / 2;
  string first = s.substr(0, half);
  string second = s.substr(half);
  return first == second;
}

int main() {
  string line;
  if (!getline(cin, line))
    return 0;

  stringstream ss(line);
  string segment;
  long long total_sum = 0;

  while (getline(ss, segment, ',')) {
    size_t dash_pos = segment.find('-');
    if (dash_pos == string::npos)
      continue;

    long long start = stoll(segment.substr(0, dash_pos));
    long long end = stoll(segment.substr(dash_pos + 1));

    for (long long i = start; i <= end; ++i) {
      if (is_invalid(i)) {
        total_sum += i;
      }
    }
  }

  cout << total_sum << endl;
  return 0;
}
*/

// Function to check if an ID is invalid (repeated sequence of digits at least
// twice)
bool is_invalid(long long n) {
	string s = to_string(n);
	return (s + s).find(s, 1) < s.length();
}

int main() {
	string line;
	if (!getline(cin, line))
		return 0;

	stringstream ss(line);
	string segment;
	long long total_sum = 0;

	while (getline(ss, segment, ',')) {
		size_t dash_pos = segment.find('-');
		if (dash_pos == string::npos)
			continue;

		long long start = stoll(segment.substr(0, dash_pos));
		long long end = stoll(segment.substr(dash_pos + 1));

		for (long long i = start; i <= end; ++i) {
			if (is_invalid(i)) {
				total_sum += i;
			}
		}
	}

	cout << total_sum << endl;
	return 0;
}