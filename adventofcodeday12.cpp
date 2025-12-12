#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Represents a 2D polyomino shape
struct Grid {
  int rows;
  int cols;
  vector<string> data;

  bool operator<(const Grid &other) const {
    if (rows != other.rows)
      return rows < other.rows;
    if (cols != other.cols)
      return cols < other.cols;
    return data < other.data;
  }

  // Calculate area (number of #)
  int area() const {
    int a = 0;
    for (const string &r : data) {
      for (char c : r)
        if (c == '#')
          a++;
    }
    return a;
  }
};

// Shape ID -> List of unique symmetries
map<int, vector<Grid>> shape_symmetries;
// Precomputed areas for optimization
map<int, int> shape_areas;

Grid rotate(const Grid &g) {
  Grid res;
  res.rows = g.cols;
  res.cols = g.rows;
  res.data.resize(res.rows, string(res.cols, '.'));
  for (int r = 0; r < g.rows; ++r) {
    for (int c = 0; c < g.cols; ++c) {
      // (r, c) -> (c, rows - 1 - r)
      res.data[c][g.rows - 1 - r] = g.data[r][c];
    }
  }
  // Trim? No, problem description implies shape is strict.
  // Actually, shapes like:
  // ###
  // #..
  // ###
  // rotated are bounding box rotated.
  // We should assume compact representation?
  // The input format gives grid.
  // Let's assume the '#' are significant and '.' are transparent.
  // But for placement, we need relative coordinates.
  // Standardize: Trim empty rows/cols?
  // Given the input Example:
  // 4:
  // ###
  // #..
  // ###
  // This is 3x3. Rotated it is 3x3.
  // We strictly use the grid provided.
  return res;
}

Grid flip(const Grid &g) {
  Grid res = g;
  reverse(res.data.begin(), res.data.end());
  return res;
}

// Normalize grid (not strictly needed if input is already minimal bounding box,
// but good practice if safe) Actually simple rotation/flip is enough as DFS
// finds placement.

void precompute_symmetries(int id, Grid g) {
  set<Grid> unique_syms;
  Grid curr = g;
  for (int i = 0; i < 4; ++i) {
    unique_syms.insert(curr);
    unique_syms.insert(flip(curr));
    curr = rotate(curr);
  }
  shape_symmetries[id] = vector<Grid>(unique_syms.begin(), unique_syms.end());
  shape_areas[id] = g.area();
}

// Region target
struct Region {
  int w, h;
  map<int, int> required_counts; // shape ID -> count
};

// Solver State
struct SolverState {
  int region_w, region_h;
  vector<string> board; // '.' empty, '#' occupied (or other char)
  map<int, int> remaining_counts;

  SolverState(int w, int h, map<int, int> req)
      : region_w(w), region_h(h), remaining_counts(req) {
    board.resize(h, string(w, '.'));
  }

  int total_remaining_area() {
    int a = 0;
    for (auto const &[id, count] : remaining_counts) {
      a += count * shape_areas[id];
    }
    return a;
  }

  int board_free_area() {
    int a = 0;
    for (const string &r : board) {
      for (char c : r)
        if (c == '.')
          a++;
    }
    return a;
  }
};

bool can_place(const SolverState &state, const Grid &g, int r, int c,
               int &actual_r, int &actual_c) {
  // We try to place the grid g such that its first '#' lands on (r,c).
  // Find first hash in g
  int first_hash_r = -1, first_hash_c = -1;
  for (int i = 0; i < g.rows; ++i) {
    for (int j = 0; j < g.cols; ++j) {
      if (g.data[i][j] == '#') {
        first_hash_r = i;
        first_hash_c = j;
        goto found;
      }
    }
  }
found:;

  // Alignment: (first_hash_r, first_hash_c) maps to (r, c) on board
  // So top-left of g maps to (r - first_hash_r, c - first_hash_c)
  int board_offset_r = r - first_hash_r;
  int board_offset_c = c - first_hash_c;

  // Check bounds and collision
  for (int i = 0; i < g.rows; ++i) {
    for (int j = 0; j < g.cols; ++j) {
      if (g.data[i][j] == '#') {
        int br = board_offset_r + i;
        int bc = board_offset_c + j;
        if (br < 0 || br >= state.region_h || bc < 0 || bc >= state.region_w)
          return false;
        if (state.board[br][bc] != '.')
          return false;
      }
    }
  }

  // Return true if placement is valid
  actual_r = board_offset_r;
  actual_c = board_offset_c;
  return true;
}

void place(SolverState &state, const Grid &g, int offset_r, int offset_c) {
  for (int i = 0; i < g.rows; ++i) {
    for (int j = 0; j < g.cols; ++j) {
      if (g.data[i][j] == '#') {
        state.board[offset_r + i][offset_c + j] = '#';
      }
    }
  }
}

void remove(SolverState &state, const Grid &g, int offset_r, int offset_c) {
  for (int i = 0; i < g.rows; ++i) {
    for (int j = 0; j < g.cols; ++j) {
      if (g.data[i][j] == '#') {
        state.board[offset_r + i][offset_c + j] = '.';
      }
    }
  }
}

// BFS for Flood Fill to count size of connected empty component
int measure_component(const vector<string> &board, int r, int c,
                      vector<vector<bool>> &visited) {
  int size = 0;
  int h = board.size();
  int w = board[0].size();
  vector<pair<int, int>> q;
  q.push_back({r, c});
  visited[r][c] = true;
  size++;

  int head = 0;
  while (head < q.size()) {
    pair<int, int> curr = q[head++];
    int cr = curr.first;
    int cc = curr.second;

    int dr[] = {0, 0, 1, -1};
    int dc[] = {1, -1, 0, 0};

    for (int i = 0; i < 4; ++i) {
      int nr = cr + dr[i];
      int nc = cc + dc[i];
      if (nr >= 0 && nr < h && nc >= 0 && nc < w && board[nr][nc] == '.' &&
          !visited[nr][nc]) {
        visited[nr][nc] = true;
        size++;
        q.push_back({nr, nc});
      }
    }
  }
  return size;
}

bool solve(SolverState &state, const vector<int> &pieces, int idx,
           int min_piece_area) {
  if (idx == pieces.size())
    return true;

  // Pruning: Flood Fill
  // If any isolated empty region is smaller than min_piece_area (and we have
  // pieces left), and that space cannot be filled? Actually, simply checking if
  // "total empty area" is enough is weak. Checking "is there any component <
  // min_piece_area" is stronger. Optimisation: Only run this check occasionally
  // or if board is partitioned? Running full flood fill every step is
  // expensive. Heuristic: Run only if idx is small? Or every step? Given the
  // constraints (likely small grids), let's try running it.

  /*
  vector<vector<bool>> visited(state.region_h, vector<bool>(state.region_w,
  false)); int wasted_area = 0; for(int r=0; r<state.region_h; ++r) { for(int
  c=0; c<state.region_w; ++c) { if(state.board[r][c] == '.' && !visited[r][c]) {
              int comp_size = measure_component(state.board, r, c, visited);
              if (comp_size < min_piece_area) {
                  wasted_area += comp_size;
              }
          }
      }
  }

  // Remaining needed area
  int needed = 0;
  for(int i=idx; i<pieces.size(); ++i) needed += shape_areas[pieces[i]];

  if (state.board_free_area() - wasted_area < needed) return false;
  */

  // Strategy: Place pieces[idx].
  // Try all valid positions?
  // "First Empty Cell" strategy forces a specific filling order which prevents
  // permutations. BUT we are iterating pieces in fixed order. We can't use
  // First Empty Cell easily because pieces[idx] might not fit in the first
  // empty cell. If we skip the first empty cell, who fills it? Later pieces? If
  // pieces are distinct, we just place pieces[idx]. If pieces are identical, we
  // should enforce ordering (e.g. piece i+1 pos > piece i pos).

  // Approach 2: Place pieces[idx] in ANY valid position.
  // To minimize branching: Try positions in scanning order (r, c).

  // OPTIMIZATION: Identical pieces handling.
  // If pieces[idx] == pieces[idx-1], we restrict placement to be after
  // pieces[idx-1]. We need to pass `last_pos` to `solve`? Let's add that if
  // needed. For now, assume distinct or full search.

  int piece_id = pieces[idx];

  // Try all symmetries
  for (const Grid &sym : shape_symmetries[piece_id]) {
    // Try all positions (r, c)
    // Optimization: Checking every (r,c) is slow?
    // Yes. But with pruning it might pass.
    // Better: Iterate (r, c)
    for (int r = 0; r < state.region_h; ++r) {
      for (int c = 0; c < state.region_w; ++c) {
        if (state.board[r][c] == '.') { // Potential top-left or anchored?
          // Note: 'place' logic uses relative offsets.
          // We must ensure the 'can_place' logic works for top-left of BOUNDING
          // BOX at (r,c)? My previous `can_place` mapped "First Hash" to (r,c).
          // That was for "First Empty Cell" strategy.
          // Here we just want to place the grid `sym` such that its (0,0) is at
          // (r,c). Or standard "First Hash" anchored? Let's stick to: "Try to
          // place `sym` such that its (0,0) is at (r,c)".

          // Check bounds/collision
          bool fits = true;
          for (int i = 0; i < sym.rows; ++i) {
            for (int j = 0; j < sym.cols; ++j) {
              if (sym.data[i][j] == '#') {
                int br = r + i;
                int bc = c + j;
                if (br >= state.region_h || bc >= state.region_w ||
                    state.board[br][bc] != '.') {
                  fits = false;
                  goto break_check;
                }
              }
            }
          }
        break_check:;

          if (fits) {
            // Place
            for (int i = 0; i < sym.rows; ++i) {
              for (int j = 0; j < sym.cols; ++j) {
                if (sym.data[i][j] == '#')
                  state.board[r + i][c + j] = '#';
              }
            }

            if (solve(state, pieces, idx + 1, min_piece_area))
              return true;

            // Backtrack
            for (int i = 0; i < sym.rows; ++i) {
              for (int j = 0; j < sym.cols; ++j) {
                if (sym.data[i][j] == '#')
                  state.board[r + i][c + j] = '.';
              }
            }
          }
        }
      }
    }
  }
  return false;
}

int main(int argc, char *argv[]) {
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
  // Parsing Shapes: "ID:" followed by grid blocks, empty line separator?
  // Input format example:
  // 0:
  // ###
  // ...
  // <empty line>
  // Region specs at end? "4x4: ..."
  // We need to detect when shapes end and regions start.
  // Regions start with digit, then 'x', then digit.
  // Shapes start with digit, then ':'.

  // Actually we can distinguish by line content.

  int current_shape_id = -1;
  Grid current_grid;
  current_grid.rows = 0;
  current_grid.cols = 0;

  vector<Region> regions;

  while (getline(infile, line)) {
    if (line.empty()) {
      if (current_shape_id != -1 && current_grid.rows > 0) {
        current_grid.cols = current_grid.data[0].size();
        precompute_symmetries(current_shape_id, current_grid);
        current_shape_id = -1;
        current_grid = Grid();
        current_grid.rows = 0;
      }
      continue;
    }

    // Check if Region line: "12x5: ..."
    if (line.find('x') != string::npos && line.find(':') != string::npos) {
      // Finish last shape if pending (no empty line case)
      if (current_shape_id != -1 && current_grid.rows > 0) {
        current_grid.cols = current_grid.data[0].size();
        precompute_symmetries(current_shape_id, current_grid);
        current_shape_id = -1;
        current_grid = Grid();
        current_grid.rows = 0;
      }

      // Parse region
      Region reg;
      size_t x_pos = line.find('x');
      size_t colon_pos = line.find(':');
      reg.w = stoi(line.substr(0, x_pos));
      reg.h = stoi(line.substr(x_pos + 1, colon_pos - x_pos - 1));

      stringstream ss(line.substr(colon_pos + 1));
      int shape_idx = 0;
      int count;
      while (ss >> count) {
        if (count > 0) {
          reg.required_counts[shape_idx] = count;
        }
        shape_idx++;
      }
      regions.push_back(reg);
    }
    // Check if Shape header: "0:"
    else if (line.find(':') != string::npos) {
      // Finish last shape if pending
      if (current_shape_id != -1 && current_grid.rows > 0) {
        current_grid.cols = current_grid.data[0].size();
        precompute_symmetries(current_shape_id, current_grid);
        current_shape_id = -1;
        current_grid = Grid();
        current_grid.rows = 0;
      }

      current_shape_id = stoi(line.substr(0, line.find(':')));
    }
    // Use Grid Data
    else {
      if (current_shape_id != -1) {
        current_grid.data.push_back(line);
        current_grid.rows++;
      }
    }
  }
  // Finish very last shape
  if (current_shape_id != -1 && current_grid.rows > 0) {
    current_grid.cols = current_grid.data[0].size();
    precompute_symmetries(current_shape_id, current_grid);
  }

  int possible_regions = 0;
  for (const Region &r : regions) {
    SolverState state(r.w, r.h, r.required_counts);

    // Initial Pruning: Area Check
    // Initial Pruning: Area Check
    if (state.total_remaining_area() > state.region_w * state.region_h) {
      continue;
    }

    // Flatten pieces for fixed ordering (Largest First)
    vector<int> pieces;
    for (auto const &[id, count] : r.required_counts) {
      for (int k = 0; k < count; ++k)
        pieces.push_back(id);
    }

    // Sort pieces descending by area
    sort(pieces.begin(), pieces.end(),
         [](int a, int b) { return shape_areas[a] > shape_areas[b]; });

    // Smallest piece area for pruning
    int min_piece_area = 1e9;
    for (int id : pieces)
      min_piece_area = min(min_piece_area, shape_areas[id]);
    if (pieces.empty())
      min_piece_area = 0;

    if (solve(state, pieces, 0, min_piece_area)) {
      possible_regions++;
    }
  }

  cout << possible_regions << endl;

  return 0;
}
