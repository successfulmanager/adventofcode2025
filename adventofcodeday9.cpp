#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


using namespace std;

struct Point {
	long long x, y;
};

// Check if a point is strictly inside a rectangle
bool is_point_in_rect_strict(const Point& p, long long x1, long long x2,
	long long y1, long long y2) {
	long long min_x = min(x1, x2);
	long long max_x = max(x1, x2);
	long long min_y = min(y1, y2);
	long long max_y = max(y1, y2);
	return p.x > min_x && p.x < max_x && p.y > min_y && p.y < max_y;
}

// Ray casting algorithm for point in polygon
bool is_point_in_polygon(double x, double y, const vector<Point>& poly) {
	int n = poly.size();
	bool inside = false;
	for (int i = 0, j = n - 1; i < n; j = i++) {
		// Check if ray cast to the left intersects the edge
		bool y_check = (poly[i].y > y) != (poly[j].y > y);
		if (y_check) {
			double intersect_x = (poly[j].x - poly[i].x) * (y - poly[i].y) /
				(double)(poly[j].y - poly[i].y) +
				poly[i].x;
			if (x < intersect_x) {
				inside = !inside;
			}
		}
	}
	return inside;
}

// Orientation: 0 colinear, 1 clockwise, 2 counterclockwise
int orientation(Point p, Point q, Point r) {
	long long val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
	if (val == 0)
		return 0;
	return (val > 0) ? 1 : 2;
}

// Check if segment p1q1 strictly intersects segment p2q2
// strictly means they cross at a point that is interior to both segments.
bool segments_intersect_strict(Point p1, Point q1, Point p2, Point q2) {
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// Strict crossing requires orientations to be different AND non-zero.
	// Specifically, p1 and q1 must be on opposite sides of p2-q2, AND p2 and q2
	// must be on opposite sides of p1-q1. (1, 2) or (2, 1). 0 indicates on-line
	// (touch), which is not strict crossing.
	bool straddle1 = (o1 == 1 && o2 == 2) || (o1 == 2 && o2 == 1);
	bool straddle2 = (o3 == 1 && o4 == 2) || (o3 == 2 && o4 == 1);

	return straddle1 && straddle2;
}

bool edge_crosses_rect_boundary(Point u, Point v, long long min_x,
	long long max_x, long long min_y,
	long long max_y) {
	Point r1 = { min_x, min_y };
	Point r2 = { max_x, min_y };
	Point r3 = { max_x, max_y };
	Point r4 = { min_x, max_y };

	// Check intersection with each of 4 rect edges.
	if (segments_intersect_strict(u, v, r1, r2))
		return true;
	if (segments_intersect_strict(u, v, r2, r3))
		return true;
	if (segments_intersect_strict(u, v, r3, r4))
		return true;
	if (segments_intersect_strict(u, v, r4, r1))
		return true;
	return false;
}

long long solve_part1(const vector<Point>& points) {
	long long max_area = 0;
	for (size_t i = 0; i < points.size(); ++i) {
		for (size_t j = i + 1; j < points.size(); ++j) {
			long long width = abs(points[i].x - points[j].x) + 1;
			long long height = abs(points[i].y - points[j].y) + 1;
			long long area = width * height;
			if (area > max_area) {
				max_area = area;
			}
		}
	}
	return max_area;
}

long long solve_part2(const vector<Point>& points) {
	long long max_area = 0;

	for (size_t i = 0; i < points.size(); ++i) {
		for (size_t j = i + 1; j < points.size(); ++j) {
			Point p1 = points[i];
			Point p2 = points[j];

			long long min_x = min(p1.x, p2.x);
			long long max_x = max(p1.x, p2.x);
			long long min_y = min(p1.y, p2.y);
			long long max_y = max(p1.y, p2.y);

			long long width = max_x - min_x + 1;
			long long height = max_y - min_y + 1;
			long long area = width * height;

			if (area <= max_area)
				continue;

			// Validation
			// 1. Center inside
			double cx = (min_x + max_x) / 2.0;
			double cy = (min_y + max_y) / 2.0;
			if (!is_point_in_polygon(cx, cy, points))
				continue;

			// 2. Vertices of polygon strictly inside rect
			bool valid = true;
			for (const auto& p : points) {
				if (is_point_in_rect_strict(p, min_x, max_x, min_y, max_y)) {
					valid = false;
					break;
				}
			}
			if (!valid)
				continue;

			// 3. Edges crossing
			for (size_t k = 0; k < points.size(); ++k) {
				Point u = points[k];
				Point v = points[(k + 1) % points.size()];
				if (edge_crosses_rect_boundary(u, v, min_x, max_x, min_y, max_y)) {
					valid = false;
					break;
				}
			}

			if (valid) {
				max_area = area;
			}
		}
	}
	return max_area;
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

	vector<Point> points;
	string line;
	while (getline(*input, line)) {
		if (line.empty())
			continue;
		stringstream ss(line);
		string segment;
		vector<long long> coords;
		while (getline(ss, segment, ',')) {
			coords.push_back(stoll(segment));
		}
		if (coords.size() == 2) {
			points.push_back({ coords[0], coords[1] });
		}
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
		cout << solve_part1(points) << endl;
	}
	else if (part == 2) {
		cout << solve_part2(points) << endl;
	}
	else {
		cerr << "Invalid part selected." << endl;
		return 1;
	}

	return 0;
}
