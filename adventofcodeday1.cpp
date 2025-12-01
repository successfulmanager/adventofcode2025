#include <fstream>
#include <iostream>
#include <string>
#include <vector>


int main(int argc, char* argv[]) {
	std::string filename = "input.txt";
	if (argc > 1) {
		filename = argv[1];
	}
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error: " << filename << " not found." << std::endl;
		return 1;
	}

	int dial_position = 50;
	long long zero_count = 0; // Use long long just in case
	std::string line;

	while (std::getline(file, line)) {
		if (line.empty())
			continue;

		// Trim whitespace
		size_t first = line.find_first_not_of(" \t\r\n");
		if (first == std::string::npos)
			continue; // Blank line
		size_t last = line.find_last_not_of(" \t\r\n");
		line = line.substr(first, (last - first + 1));

		char direction = line[0];
		int distance = 0;
		try {
			distance = std::stoi(line.substr(1));
		}
		catch (...) {
			std::cerr << "Invalid line format: " << line << std::endl;
			continue;
		}

		if (direction == 'L') {
			// Left means decreasing.
			// Distance to 0 (moving left) is simply current position.
			// If current is 0, distance to next 0 is 100.
			int dist_to_zero = (dial_position == 0) ? 100 : dial_position;

			if (distance >= dist_to_zero) {
				zero_count++; // Counts the first 0 crossing
				int remaining_dist = distance - dist_to_zero;
				zero_count += remaining_dist / 100; // Counts subsequent 0 crossings
			}

			dial_position = (dial_position - distance) % 100;
			if (dial_position < 0)
				dial_position += 100;
		}
		else if (direction == 'R') {
			// Right means increasing.
			// Distance to 0 (moving right) is 100 - current position.
			// If current is 0, distance to next 0 is 100.
			int dist_to_zero = (dial_position == 0) ? 100 : (100 - dial_position);

			if (distance >= dist_to_zero) {
				zero_count++; // Counts the first 0 crossing
				int remaining_dist = distance - dist_to_zero;
				zero_count += remaining_dist / 100; // Counts subsequent 0 crossings
			}

			dial_position = (dial_position + distance) % 100;
		}
		else {
			std::cerr << "Unknown direction: " << direction << std::endl;
			continue;
		}
	}

	std::cout << "Password: " << zero_count << std::endl;

	return 0;
}