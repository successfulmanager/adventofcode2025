#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Robust Fraction Struct
struct Fraction {
	long long num;
	long long den;

	Fraction(long long n = 0, long long d = 1) {
		if (d == 0) {
			// Divide by zero panic handling or assume infinite?
			// For this problem, we shouldn't hit it in normal math if careful.
			d = 1;
			n = 0; // Error state
		}
		if (d < 0) {
			n = -n;
			d = -d;
		}
		long long common = std::gcd(abs(n), d);
		num = n / common;
		den = d / common;
	}

	Fraction operator+(const Fraction& other) const {
		// a/b + c/d = (ad + bc) / bd
		// Optimize to use LCM?
		long long common_den = std::lcm(den, other.den);
		long long n =
			num * (common_den / den) + other.num * (common_den / other.den);
		return Fraction(n, common_den);
	}

	Fraction operator-(const Fraction& other) const {
		long long common_den = std::lcm(den, other.den);
		long long n =
			num * (common_den / den) - other.num * (common_den / other.den);
		return Fraction(n, common_den);
	}

	Fraction operator*(const Fraction& other) const {
		return Fraction(num * other.num, den * other.den);
	}

	Fraction operator/(const Fraction& other) const {
		return Fraction(num * other.den, den * other.num);
	}

	bool operator<(const Fraction& other) const {
		return num * other.den < other.num * den;
	}

	bool operator>(const Fraction& other) const {
		return num * other.den > other.num * den;
	}

	bool operator<=(const Fraction& other) const { return !(*this > other); }

	bool operator>=(const Fraction& other) const { return !(*this < other); }

	bool operator==(const Fraction& other) const {
		return num == other.num && den == other.den;
	}

	bool is_integer() const { return den == 1; }

	// Check if integer (within epsilon is removed)
	long long to_long() const { return num / den; }

	double to_double() const { return (double)num / den; }
};

struct Machine {
	vector<int> target_pattern;
	vector<vector<int>> buttons;
	vector<int> joltage_requirements;
};

Machine parse_machine(string line) {
	Machine m;
	size_t bracket_start = line.find('[');
	size_t bracket_end = line.find(']');
	string pattern_str =
		line.substr(bracket_start + 1, bracket_end - bracket_start - 1);

	for (char c : pattern_str) {
		if (c == '#')
			m.target_pattern.push_back(1);
		else
			m.target_pattern.push_back(0);
	}

	size_t pos = bracket_end + 1;
	while ((pos = line.find('(', pos)) != string::npos) {
		size_t end_paren = line.find(')', pos);
		string content = line.substr(pos + 1, end_paren - pos - 1);
		vector<int> button_indices;
		stringstream ss(content);
		string segment;
		while (getline(ss, segment, ',')) {
			button_indices.push_back(stoi(segment));
		}
		m.buttons.push_back(button_indices);
		pos = end_paren + 1;
	}

	size_t brace_start = line.find('{');
	if (brace_start != string::npos) {
		size_t brace_end = line.find('}');
		string content = line.substr(brace_start + 1, brace_end - brace_start - 1);
		stringstream ss(content);
		string segment;
		while (getline(ss, segment, ',')) {
			m.joltage_requirements.push_back(stoi(segment));
		}
	}
	return m;
}

int min_presses_p1 = -1;

void solve_part1_recursive(const Machine& m, int button_idx,
	vector<int>& current_state, int current_presses) {
	if (button_idx == m.buttons.size()) {
		bool match = true;
		for (size_t i = 0; i < m.target_pattern.size(); ++i) {
			if (current_state[i] % 2 != m.target_pattern[i]) {
				match = false;
				break;
			}
		}
		if (match) {
			if (min_presses_p1 == -1 || current_presses < min_presses_p1) {
				min_presses_p1 = current_presses;
			}
		}
		return;
	}

	if (min_presses_p1 != -1 && current_presses >= min_presses_p1)
		return;

	solve_part1_recursive(m, button_idx + 1, current_state, current_presses);

	for (int idx : m.buttons[button_idx]) {
		if (idx < current_state.size()) {
			current_state[idx]++;
		}
	}
	solve_part1_recursive(m, button_idx + 1, current_state, current_presses + 1);
	for (int idx : m.buttons[button_idx]) {
		if (idx < current_state.size()) {
			current_state[idx]--;
		}
	}
}

int solve_part1(const Machine& m) {
	min_presses_p1 = -1;
	vector<int> current_state(m.target_pattern.size(), 0);
	solve_part1_recursive(m, 0, current_state, 0);
	return min_presses_p1;
}

// Optimization for Part 2: Gauss-Jordan Elimination
long long min_total_presses = -1;

void solve_system_recursive(const vector<vector<Fraction>>& reduced_matrix,
	const vector<int>& free_vars,
	const vector<int>& pivot_vars,
	const vector<long long>& global_bounds,
	vector<long long>& current_solution,
	int free_var_idx) {

	// Pruning: Calculate current cost from already assigned free variables
	long long current_cost = 0;
	for (size_t f = 0; f < free_var_idx; ++f) {
		current_cost += current_solution[free_vars[f]];
	}

	if (min_total_presses != -1 && current_cost >= min_total_presses)
		return;

	if (free_var_idx == free_vars.size()) {
		// Calculate basic vars
		bool valid = true;
		long long temp_cost = current_cost;

		for (size_t i = 0; i < pivot_vars.size(); ++i) {
			int p_idx = pivot_vars[i];
			Fraction val = reduced_matrix[i].back(); // RHS
			for (size_t f = 0; f < free_vars.size(); ++f) {
				int f_idx = free_vars[f];
				val = val -
					(reduced_matrix[i][f_idx] * Fraction(current_solution[f_idx]));
			}

			if (!val.is_integer()) {
				valid = false;
				break;
			}
			long long int_val = val.to_long();
			if (int_val < 0) {
				valid = false;
				break;
			}
			if (int_val > global_bounds[p_idx]) {
				valid = false;
				break;
			} // Check global upper bound

			current_solution[p_idx] = int_val;
			temp_cost += int_val;
		}

		if (valid) {
			if (min_total_presses == -1 || temp_cost < min_total_presses) {
				min_total_presses = temp_cost;
			}
		}
		// Reset basic vars for next iteration (not strictly needed if overwritten,
		// but good practice)
		for (int p_idx : pivot_vars)
			current_solution[p_idx] = 0;
		return;
	}

	int current_free_var_idx = free_vars[free_var_idx];

	// Determine strict range [low, high]
	// 1. Global Bound
	long long g_bound = global_bounds[current_free_var_idx];

	long long lower_bound = 0;
	long long upper_bound = g_bound;

	// 2. Constraints from basic vars
	// X_pivot = RHS' - Coeff * X_f - Sum(Coeff_future * X_future)
	// We require 0 <= X_pivot <= Global_Bound_Pivot

	for (size_t i = 0; i < pivot_vars.size(); ++i) {
		Fraction coeff = reduced_matrix[i][current_free_var_idx];
		if (coeff.num == 0)
			continue; // This free var doesn't affect this pivot var

		Fraction rhs_rem = reduced_matrix[i].back();
		for (size_t f = 0; f < free_var_idx; ++f) {
			rhs_rem = rhs_rem - (reduced_matrix[i][free_vars[f]] *
				Fraction(current_solution[free_vars[f]]));
		}

		// Constraint 1: X_pivot >= 0
		// X_pivot = rhs_rem - coeff * X_f - Sum_future_terms >= 0
		// coeff * X_f <= rhs_rem - Sum_future_terms
		// To get the tightest upper bound for X_f, we need to maximize (rhs_rem -
		// Sum_future_terms). This means minimizing Sum_future_terms.
		// Sum_future_terms = sum(reduced_matrix[i][free_vars[k]] * X_k) for k >
		// free_var_idx Min Sum_future_terms occurs when X_k = 0 for positive
		// coeffs, and X_k = global_bounds[X_k] for negative coeffs.

		Fraction future_min_sum(0);
		for (size_t k = free_var_idx + 1; k < free_vars.size(); ++k) {
			Fraction fc = reduced_matrix[i][free_vars[k]];
			if (fc < Fraction(0)) { // If coefficient is negative, X_k should be max
				// to minimize the sum
				future_min_sum =
					future_min_sum + (fc * Fraction(global_bounds[free_vars[k]]));
			}
		}

		Fraction max_rhs_for_xf = rhs_rem - future_min_sum;

		if (coeff > Fraction(0)) {
			// X_f <= max_rhs_for_xf / coeff
			if (max_rhs_for_xf < Fraction(0)) { // If RHS is negative, X_f must be
				// negative, which is impossible
				upper_bound = -1;                 // Force break
			}
			else {
				Fraction lim = max_rhs_for_xf / coeff;
				long long l = lim.to_long(); // Truncate for upper bound
				if (l < upper_bound)
					upper_bound = l;
			}
		}
		else { // coeff < Fraction(0)
			// X_f >= max_rhs_for_xf / coeff (inequality flips)
			Fraction lim = max_rhs_for_xf / coeff;
			long long l = (long long)ceil(lim.to_double()); // Ceil for lower bound
			if (l > lower_bound)
				lower_bound = l;
		}

		// Constraint 2: X_pivot <= GlobalPivotBound
		// rhs_rem - coeff * X_f - Sum_future_terms <= GlobalPivotBound
		// coeff * X_f >= rhs_rem - Sum_future_terms - GlobalPivotBound
		// To get the tightest lower bound for X_f, we need to minimize (rhs_rem -
		// Sum_future_terms - GlobalPivotBound). This means maximizing
		// Sum_future_terms. Max Sum_future_terms occurs when X_k =
		// global_bounds[X_k] for positive coeffs, and X_k = 0 for negative coeffs.

		Fraction future_max_sum(0);
		for (size_t k = free_var_idx + 1; k < free_vars.size(); ++k) {
			Fraction fc = reduced_matrix[i][free_vars[k]];
			if (fc > Fraction(0)) { // If coefficient is positive, X_k should be max
				// to maximize the sum
				future_max_sum =
					future_max_sum + (fc * Fraction(global_bounds[free_vars[k]]));
			}
		}

		Fraction min_rhs_for_xf =
			rhs_rem - future_max_sum - Fraction(global_bounds[pivot_vars[i]]);

		if (coeff > Fraction(0)) {
			// X_f >= min_rhs_for_xf / coeff
			Fraction lim = min_rhs_for_xf / coeff;
			long long l = (long long)ceil(lim.to_double()); // Ceil for lower bound
			if (l > lower_bound)
				lower_bound = l;
		}
		else { // Coeff < Fraction(0)
			// X_f <= min_rhs_for_xf / coeff (inequality flips)
			Fraction lim = min_rhs_for_xf / coeff;
			long long l = (long long)floor(lim.to_double()); // Floor for upper bound
			if (l < upper_bound)
				upper_bound = l;
		}
	}

	if (lower_bound < 0)
		lower_bound = 0; // X_f must be non-negative
	if (upper_bound > g_bound)
		upper_bound = g_bound; // X_f cannot exceed its global bound

	if (upper_bound < lower_bound)
		return; // Impossible range

	for (long long val = lower_bound; val <= upper_bound; ++val) {
		current_solution[current_free_var_idx] = val;

		// Pruning: If current partial cost already exceeds or equals
		// min_total_presses, no need to continue down this path
		if (min_total_presses != -1 && (current_cost + val) >= min_total_presses) {
			// Since we iterate from lower_bound upwards, and cost increases, we can
			// break. No need to reset current_solution[current_free_var_idx] here, as
			// it will be reset after the loop.
			break;
		}

		solve_system_recursive(reduced_matrix, free_vars, pivot_vars, global_bounds,
			current_solution, free_var_idx + 1);
	}
	current_solution[current_free_var_idx] = 0; // Reset for backtracking
}

long long solve_part2(const Machine& m) {
	min_total_presses = -1;

	int num_vars = m.buttons.size();
	int num_eqs = m.joltage_requirements.size();

	// Calculate Global Bounds for each variable (button press count)
	// A button press adds 1 to certain joltage requirements.
	// So, for any button j, if it affects requirement i, then x_j <=
	// requirement_i. The global bound for x_j is the minimum of all requirements
	// it affects.
	vector<long long> global_bounds(num_vars);
	for (int j = 0; j < num_vars; ++j) {
		long long min_b = -1; // Represents unbounded initially
		bool affects_any_req = false;
		for (int row_idx : m.buttons[j]) {
			if (row_idx < m.joltage_requirements.size()) {
				affects_any_req = true;
				long long req = m.joltage_requirements[row_idx];
				if (min_b == -1 || req < min_b) {
					min_b = req;
				}
			}
		}
		if (!affects_any_req) {
			// If a button affects no requirements, it's effectively a free variable
			// that doesn't contribute to the target. Its optimal value is 0.
			// However, for the general solver, we need an upper bound.
			// A very large number or the sum of all requirements could be used.
			// For this problem, 2000000 is a heuristic max.
			global_bounds[j] = 2000000;
		}
		else {
			global_bounds[j] = min_b;
		}
	}

	vector<vector<Fraction>> matrix(num_eqs, vector<Fraction>(num_vars + 1));

	for (int j = 0; j < num_vars; ++j) {
		for (int row_idx : m.buttons[j]) {
			if (row_idx < num_eqs) {
				matrix[row_idx][j] = Fraction(1);
			}
		}
	}
	for (int i = 0; i < num_eqs; ++i) {
		matrix[i][num_vars] = Fraction(m.joltage_requirements[i]);
	}

	int pivot_row = 0;
	vector<int> pivot_cols;

	for (int col = 0; col < num_vars && pivot_row < num_eqs; ++col) {
		int sel = -1;
		for (int row = pivot_row; row < num_eqs; ++row) {
			if (matrix[row][col].num != 0) {
				sel = row;
				break;
			}
		}

		if (sel == -1)
			continue;

		swap(matrix[pivot_row], matrix[sel]);

		Fraction p_val = matrix[pivot_row][col];
		for (int j = col; j <= num_vars; ++j) {
			matrix[pivot_row][j] = matrix[pivot_row][j] / p_val;
		}

		for (int i = 0; i < num_eqs; ++i) {
			if (i != pivot_row) {
				Fraction factor = matrix[i][col];
				for (int j = col; j <= num_vars; ++j) {
					matrix[i][j] = matrix[i][j] - (factor * matrix[pivot_row][j]);
				}
			}
		}

		pivot_cols.push_back(col);
		pivot_row++;
	}

	for (int i = pivot_row; i < num_eqs; ++i) {
		if (matrix[i][num_vars].num != 0) {
			return -1; // Inconsistent system
		}
	}

	vector<int> free_vars;
	vector<bool> is_pivot(num_vars, false);
	for (int col : pivot_cols)
		is_pivot[col] = true;
	for (int j = 0; j < num_vars; ++j) {
		if (!is_pivot[j])
			free_vars.push_back(j);
	}

	matrix.resize(pivot_row); // Only keep the non-zero rows

	vector<long long> current_solution(num_vars, 0);
	solve_system_recursive(matrix, free_vars, pivot_cols, global_bounds,
		current_solution, 0);

	return min_total_presses;
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

	vector<string> lines;
	string line;
	while (getline(*input, line)) {
		lines.push_back(line);
	}

	if (input == &cin)
		cin.clear();
	int part;
	cout << "Select part (1 or 2): ";
	if (!(cin >> part)) {
		cerr << "Invalid input for part selection." << endl;
		return 1;
	}

	long long total_presses = 0;
	for (const string& l : lines) {
		if (l.empty())
			continue;
		Machine m = parse_machine(l);
		long long presses = -1;
		if (part == 1) {
			presses = solve_part1(m);
		}
		else if (part == 2) {
			presses = solve_part2(m);
		}

		if (presses != -1) {
			total_presses += presses;
		}
		else {
			// Safe to ignore for valid puzzles usually
		}
	}

	cout << total_presses << endl;

	return 0;
}
