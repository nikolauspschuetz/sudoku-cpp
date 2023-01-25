#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <iterator>
#include <map>
#include <vector>
#include <unistd.h>

using namespace std;


// const std::string DEFAULT_PUZZLE = "puzzles/ppg-20221205.txt";
const std::string DEFAULT_PUZZLE = "puzzles/rps-20221103.txt";
int depth = 0;


std::string getenv(const std::string& key, const std::string& d) {
    const char* value = getenv(key.c_str());
    return value ? value : d;
}

void print_board(vector < vector < int > > *board) {
    for ( int i = 0; i < 9; i++ ) {
        for ( int j = 0; j < 9; j ++ ) {
            cout << (*board)[i][j] << (j < 8 ? "," : "");
        }
        cout << "\n";
    }
}

void backtrack(vector < vector < int > > *board);
void solve(vector < vector < int > > *board);

vector < vector < int > > Board(const std::string& fname) {
    vector < vector < int > > content;
    vector < int > row;
    string line, word;
    fstream file (fname, ios::in);
    if (file.is_open()) {
        while (getline(file, line)) {
            if ((sizeof(line) > 0) && (line[0] != '#')) {
                row.clear();
                stringstream str(line);
                while (getline(str, word, ',')) {
                    int n;
                    n = std::stoi(word);
                    row.push_back(n);
                }
                content.push_back(row);
            }
        }
    } else {
        cout << "Could not open the file\n";
    }
    file.close();
    return content;
}

vector < int > options(vector < vector < int > > *board, const int r, const int c) {
    map < int, bool > opts = {{1, true}, {2, true}, {3, true}, {4, true}, {5, true}, {6, true}, {7, true}, {8, true}, {9, true},};
    // for (int i = 1; i <= 9; i ++) { 
    //     opts.insert({i, true}); 
    // }
    int row[9] = {}, col[9] = {};
    const int r3 = 3 * (r / 3), c3 = 3 * (c / 3);
    for (int i = 0; i < 9; i++) {
        // cout << "inserting r=" << r << ", c=" << c << " value " << board[r][i] << " as false\n";
        if ((*board)[r][i] > 0) {
            opts[(*board)[r][i]] = false;
        }
        if ((*board)[i][c] > 0) {
            opts[(*board)[i][c]] = false;
        }
        // cout << "opt " << board[r][i] << " = " << opts[board[r][i]] << "\n";
        // opts.insert({board[i][c], false});
        row[i] = (*board)[r][i];
        col[i] = (*board)[i][c];
    }
    int box[9] = {};
    int b = 0;
    for (int i = r3; i < r3 + 3; i++) {
        for (int j = c3; j < c3 + 3; j++) {
            if ((*board)[i][j] > 0) {
                opts[(*board)[i][j]] = false;
            }
            box[b] = (*board)[i][j];
            b ++;
        }
    }
    // for (int i = 0; i < 9; i ++) { cout << "row " << i << ": " << row[i] << "\n";}
    // for (int i = 0; i < 9; i ++) { cout << "col " << i << ": " << col[i] << "\n";}
    // for (int i = 0; i < 9; i ++) { cout << "box " << i << ": " << box[i] << "\n";}
    
    vector < int > out = {};
    for (int i = 1; i <= 9; i ++) {
        if (opts[i]) {
            // cout << "option " << i << " is not taken\n";
            out.push_back(i);
        // } else {
        //     cout << "option " << i << " is taken\n";
        }
    }
    return out;
}

struct Cell
{
  int row;
  int col;
  vector<int> opts;
};

struct Answer
{
  int row;
  int col;
  int answer;
};

Answer solve_one(vector < vector < int > > *board) {
    bool solved = true;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if ((*board)[i][j] < 1) {
                solved = false;
                vector < int > opt = options(board, i, j);
                if (opt.size() == 0) throw 0;
                if (opt.size() == 1) return {i, j, opt[0]};                
            }
        }
    }
    if (!solved) {
        cout << "no solution for board (depth=" << depth << ")\n";
    }
    return {0, 0, 0};
}

bool done(vector < vector < int > > *board) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if ((*board)[i][j] < 1) {
                return false;
            }
        }
    }
    return true;
}


bool sortCells(Cell c1, Cell c2) {
    return (c1.opts.size() < c2.opts.size());
}

void solve(vector < vector < int > > *board) {
    while (!done(board)) {
        Answer a = solve_one(board);
        if (a.answer > 0) {
            cout << "solved cell row=" << a.row << ", col=" << a.col << ", answer=" << a.answer <<"\n";
            (*board)[a.row][a.col] = a.answer;
        } else {
            cout << "backtracking (depth=" << depth << ") in solve\n";
            backtrack(board);
        }
    }
}

void backtrack(vector < vector < int > > *board) {
    int backup[9][9];
    // make space
    vector < Cell > space;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            backup[i][j] = int((*board)[i][j]);
            if ((*board)[i][j] < 1) {
                const vector < int > o = options(board, i, j);
                const Cell c = {i, j, o};
                space.push_back(c);
            }
        }
    }
    // get first of sorted space
    sort(space.begin(), space.end(), sortCells);
    const Cell s = space[0];
    for (int o : s.opts) {
        try {
            depth++;
            cout << "backtracking (depth=" << depth << ") with " << s.row << ", " << s.col << " option " << o << "\n";
            (*board)[s.row][s.col] = o;
            solve(board);
            return;
        }
        catch (int e) {
            depth--;
            cout << "Exception " << e << " occurred.\n";
            print_board(board);
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    (*board)[i][j] = int(backup[i][j]);
                }
            }
            cout << "Reverted board\n";
            print_board(board);
        }
    }
    throw 1;
}


int main() {
    const std::string fname = getenv("PUZZLE", DEFAULT_PUZZLE);
    vector < vector < int > > board = Board(fname);
    cout << "starting board\n";
    print_board(&board);
    solve(&board);
    int out;
    if (done(&board)) {
        cout << "solved board\n";
        out = 0;
    } else {
        cout << "failed to solve board\n";
        out = 1;
    }
    print_board(&board);
    return out;
}