#include<iostream>
#include<string>
#include<vector>
#include <chrono>
#include <thread>
#include<tuple>
#include<map>
#include<random>
#include<algorithm>
#include<set>
#include<cmath>
using namespace std;
using namespace chrono;

const double C = 1.414;  // UCB exploration parameter
const int simulations = 20;
const int total_candidates = 10;  // add candidates
const int board_size = 15;

const int score_five = 1000000;
const int score_live_four = 100000;
const int score_threat_four = 50000;
const int score_live_three = 10000;
const int score_threat_three = 5000;
const int score_live_two = 1000;
const int score_double_three = 80000;  

int current_player = 1;  // 1: black turn; -1: white turn;
vector<vector<int>> board;
int input_x, input_y;
int Min_x = 100, Min_y = 100, Max_x = -100, Max_y = -100;

// four directions
const int dx[] = {0, 1, 1, 1};
const int dy[] = {1, 0, 1, -1};

int switchPlayer(int player) { return -player; }

void printBoard(const vector<vector<int>>& _board) {
    cout << "   ";
    for(int i = 0; i < board_size; i++) {
        cout << (i <= 9 ? " " : "") << i << " ";
    }
    cout << "y\n";
    
    for(int i = 0; i < board_size; i++) {
        cout << (i <= 9 ? " " : "") << i << "  ";
        for(int j = 0; j < board_size; j++) {
            char c = (_board[i][j] == 0) ? '.' : (_board[i][j] == 1 ? 'X' : 'O');
            cout << c << "  ";
        }
        cout << "\n";
    }
    cout << "x\n";
}

void clearBoard() { system("cls"); }

bool isValidPos(int x, int y) {
    return x >= 0 && x < board_size && y >= 0 && y < board_size;
}

bool errorCheck(int x, int y, const string& error_source) {
    if(!isValidPos(x, y)) {
        cout << "Error: Invalid move... Error source: (" << error_source << ") Error type: (out of bound)\n";
        return true;
    }
    if(board[x][y] != 0) {
        cout << "Error: Invalid move... Error source: (" << error_source << ") Error type: (chess piece already exists)\n";
        return true;
    }
    return false;
}

struct LineInfo {
    int count;      
    bool left_open; 
    bool right_open;
    int left_space;  
    int right_space; 
};


LineInfo getAdvancedLineInfo(int x, int y, int dir, int player, const vector<vector<int>>& _board) {
    LineInfo info = {1, false, false, 0, 0};
    
    int nx = x + dx[dir], ny = y + dy[dir];
    int space_count = 0;
    while(isValidPos(nx, ny)) {
        if(_board[nx][ny] == player) {
            info.count++;
            space_count = 0; 
        } else if(_board[nx][ny] == 0) {
            space_count++;
            if(space_count > 1) break; 
        } else {
            break; 
        }
        nx += dx[dir];
        ny += dy[dir];
    }
    if(isValidPos(nx, ny) && _board[nx][ny] == 0) {
        info.right_open = true;
        info.right_space = space_count;
    }
    
    nx = x - dx[dir];
    ny = y - dy[dir];
    space_count = 0;
    while(isValidPos(nx, ny)) {
        if(_board[nx][ny] == player) {
            info.count++;
            space_count = 0;
        } else if(_board[nx][ny] == 0) {
            space_count++;
            if(space_count > 1) break;
        } else {
            break;
        }
        nx -= dx[dir];
        ny -= dy[dir];
    }
    if(isValidPos(nx, ny) && _board[nx][ny] == 0) {
        info.left_open = true;
        info.left_space = space_count;
    }
    
    return info;
}

LineInfo getLineInfo(int x, int y, int dir, int player, const vector<vector<int>>& _board) {
    LineInfo info = {1, false, false, 0, 0};
    
    int nx = x + dx[dir], ny = y + dy[dir];
    while(isValidPos(nx, ny) && _board[nx][ny] == player) {
        info.count++;
        nx += dx[dir];
        ny += dy[dir];
    }
    if(isValidPos(nx, ny) && _board[nx][ny] == 0) {
        info.right_open = true;
    }
    
    nx = x - dx[dir];
    ny = y - dy[dir];
    while(isValidPos(nx, ny) && _board[nx][ny] == player) {
        info.count++;
        nx -= dx[dir];
        ny -= dy[dir];
    }
    if(isValidPos(nx, ny) && _board[nx][ny] == 0) {
        info.left_open = true;
    }
    
    return info;
}

bool isFiveInRow(int x, int y, int player, const vector<vector<int>>& _board) {
    for(int dir = 0; dir < 4; dir++) {
        LineInfo info = getLineInfo(x, y, dir, player, _board);
        if(info.count >= 5) return true;
    }
    return false;
}

int countLiveFour(int x, int y, int player, const vector<vector<int>>& _board) {
    int count = 0;
    for(int dir = 0; dir < 4; dir++) {
        LineInfo info = getLineInfo(x, y, dir, player, _board);
        if(info.count == 4 && info.left_open && info.right_open) {
            count++;
        }
    }
    return count;
}

int countThreatFour(int x, int y, int player, const vector<vector<int>>& _board) {
    int count = 0;
    for(int dir = 0; dir < 4; dir++) {
        LineInfo info = getLineInfo(x, y, dir, player, _board);
        if(info.count == 4 && (info.left_open || info.right_open) && !(info.left_open && info.right_open)) {
            count++;
        }
    }
    return count;
}

int countLiveThree(int x, int y, int player, const vector<vector<int>>& _board) {
    int count = 0;
    for(int dir = 0; dir < 4; dir++) {
        LineInfo info = getLineInfo(x, y, dir, player, _board);
        if(info.count == 3 && info.left_open && info.right_open) {
            count++;
        }
    }
    return count;
}

int countJumpLiveThree(int x, int y, int player, const vector<vector<int>>& _board) {
    int count = 0;
    for(int dir = 0; dir < 4; dir++) {
        // .X.XX. 
        vector<int> pattern;
        for(int i = -3; i <= 3; i++) {
            int nx = x + i * dx[dir];
            int ny = y + i * dy[dir];
            if(isValidPos(nx, ny)) {
                pattern.push_back(_board[nx][ny]);
            } else {
                pattern.push_back(-2); 
            }
        }
        
        
        if(pattern.size() >= 7) {
        
            if(pattern[0] == 0 && pattern[1] == player && pattern[2] == player && 
               pattern[3] == player && pattern[4] == 0 && pattern[5] == player && pattern[6] == 0) {
                count++;
            }
            
            if(pattern[0] == 0 && pattern[1] == player && pattern[2] == 0 && 
               pattern[3] == player && pattern[4] == player && pattern[5] == player && pattern[6] == 0) {
                count++;
            }
        }
    }
    return count;
}

int countThreatThree(int x, int y, int player, const vector<vector<int>>& _board) {
    int count = 0;
    for(int dir = 0; dir < 4; dir++) {
        LineInfo info = getLineInfo(x, y, dir, player, _board);
        if(info.count == 3 && (info.left_open || info.right_open) && !(info.left_open && info.right_open)) {
            count++;
        }
    }
    return count;
}

int countLiveTwo(int x, int y, int player, const vector<vector<int>>& _board) {
    int count = 0;
    for(int dir = 0; dir < 4; dir++) {
        LineInfo info = getLineInfo(x, y, dir, player, _board);
        if(info.count == 2 && info.left_open && info.right_open) {
            count++;
        }
    }
    return count;
}

bool hasDoubleThreeThreat(int x, int y, int player, const vector<vector<int>>& _board) {
    vector<vector<int>> temp_board = _board;
    temp_board[x][y] = player;
    
    int live_three_count = 0;
    for(int dir = 0; dir < 4; dir++) {
        LineInfo info = getLineInfo(x, y, dir, player, temp_board);
        if(info.count == 3 && info.left_open && info.right_open) {
            live_three_count++;
        }
    }
    
    return live_three_count >= 2;
}

int evaluatePosition(int x, int y, int player, const vector<vector<int>>& _board) {
    int score = 0;
    
    score += score_five * (isFiveInRow(x, y, player, _board) ? 1 : 0);
    score += score_live_four * countLiveFour(x, y, player, _board);
    score += score_threat_four * countThreatFour(x, y, player, _board);
    score += score_live_three * countLiveThree(x, y, player, _board);
    score += score_threat_three * countThreatThree(x, y, player, _board);
    score += score_live_two * countLiveTwo(x, y, player, _board);
    
    if(hasDoubleThreeThreat(x, y, player, _board)) {
        score += score_double_three;
    }
    
    int opponent = switchPlayer(player);
    score += score_five * 1.1 * (isFiveInRow(x, y, opponent, _board) ? 1 : 0);
    score += score_live_four * 1.1 * countLiveFour(x, y, opponent, _board);
    score += score_threat_four * 1.0 * countThreatFour(x, y, opponent, _board);
    score += score_live_three * 0.9 * countLiveThree(x, y, opponent, _board);
    score += score_threat_three * 0.8 * countThreatThree(x, y, opponent, _board);
    score += score_live_two * 0.6 * countLiveTwo(x, y, opponent, _board);
    

    if(hasDoubleThreeThreat(x, y, opponent, _board)) {
        score += score_double_three * 0.95;
    }
    
    return score;
}

int checkGameStatus(const vector<vector<int>>& _board) {
    for(int i = 0; i < board_size; i++) {
        for(int j = 0; j < board_size; j++) {
            if(_board[i][j] != 0) {
                if(isFiveInRow(i, j, _board[i][j], _board)) {
                    return _board[i][j];  
                }
            }
        }
    }
    
    bool full = true;
    for(int i = 0; i < board_size; i++) {
        for(int j = 0; j < board_size; j++) {
            if(_board[i][j] == 0) {
                full = false;
                break;
            }
        }
        if(!full) break;
    }
    
    return full ? -2 : 0;  // -2 tie 0 continue
}

vector<pair<int, int>> getCandidateMoves(const vector<vector<int>>& _board, int range = 2) {
    set<pair<int, int>> candidates;
    
    for(int i = 0; i < board_size; i++) {
        for(int j = 0; j < board_size; j++) {
            if(_board[i][j] != 0) {
                for(int di = -range; di <= range; di++) {
                    for(int dj = -range; dj <= range; dj++) {
                        int ni = i + di, nj = j + dj;
                        if(isValidPos(ni, nj) && _board[ni][nj] == 0) {
                            candidates.insert({ni, nj});
                        }
                    }
                }
            }
        }
    }
    
    if(candidates.empty()) {
        int center = board_size / 2;
        for(int i = center - 1; i <= center + 1; i++) {
            for(int j = center - 1; j <= center + 1; j++) {
                if(isValidPos(i, j)) {
                    candidates.insert({i, j});
                }
            }
        }
    }
    
    return vector<pair<int, int>>(candidates.begin(), candidates.end());
}

pair<int, int> getUrgentMove(int player, const vector<vector<int>>& _board) {
    vector<pair<int, int>> candidates = getCandidateMoves(_board);
    
    for(auto [x, y] : candidates) {
        if(isFiveInRow(x, y, player, _board)) {
            return {x, y};
        }
    }
    
    int opponent = switchPlayer(player);
    for(auto [x, y] : candidates) {
        if(isFiveInRow(x, y, opponent, _board)) {
            return {x, y};
        }
    }
    
    for(auto [x, y] : candidates) {
        if(countLiveFour(x, y, player, _board) > 0) {
            return {x, y};
        }
    }
    
    for(auto [x, y] : candidates) {
        if(countLiveFour(x, y, opponent, _board) > 0) {
            return {x, y};
        }
    }
    
    for(auto [x, y] : candidates) {
        if(hasDoubleThreeThreat(x, y, player, _board)) {
            return {x, y};
        }
    }
    
    for(auto [x, y] : candidates) {
        if(hasDoubleThreeThreat(x, y, opponent, _board)) {
            return {x, y};
        }
    }
    for(auto [x, y] : candidates) {
        if(countThreatFour(x, y, player, _board) > 0) {
            return {x, y};
        }
    }
    
    for(auto [x, y] : candidates) {
        if(countThreatFour(x, y, opponent, _board) > 0) {
            return {x, y};
        }
    }

    return {-1, -1};  
}

int simulateGame(int x, int y, int player, const vector<vector<int>>& _board) {
    vector<vector<int>> sim_board = _board;
    sim_board[x][y] = player;
    int sim_player = switchPlayer(player);
    
    static random_device rd;
    static mt19937 gen(rd());
    
    int moves = 0;
    const int MAX_MOVES = 50;  
    
    while(moves < MAX_MOVES) {
        int status = checkGameStatus(sim_board);
        if(status != 0) {
            return status == player ? 1 : (status == switchPlayer(player) ? -1 : 0);
        }
        
        vector<pair<int, int>> candidates = getCandidateMoves(sim_board, 1);
        if(candidates.empty()) break;
        
        pair<int, int> urgent = getUrgentMove(sim_player, sim_board);
        if(urgent.first != -1) {
            sim_board[urgent.first][urgent.second] = sim_player;
        } else {
            uniform_int_distribution<> dis(0, candidates.size() - 1);
            auto [nx, ny] = candidates[dis(gen)];
            sim_board[nx][ny] = sim_player;
        }
        
        sim_player = switchPlayer(sim_player);
        moves++;
    }
    
    return 0; 
}

pair<int, int> mctsBestMove() {
    cout << "\nAI thinking with MCTS...\n";
    
    pair<int, int> urgent = getUrgentMove(current_player, board);
    if(urgent.first != -1) {
        cout << "Found urgent move!\n";
        return urgent;
    }
    
    vector<pair<int, int>> candidates = getCandidateMoves(board);
    vector<tuple<int, int, int, int>> evaluations;  
    
    for(auto [x, y] : candidates) {
        int score = evaluatePosition(x, y, current_player, board);
        evaluations.push_back({x, y, score, 0});
    }
    
    sort(evaluations.begin(), evaluations.end(), 
         [](const auto& a, const auto& b) { return get<2>(a) > get<2>(b); });
    
    int num_candidates = min(total_candidates, (int)evaluations.size());
    vector<vector<int>> results(num_candidates, vector<int>(3, 0)); 
    
    cout << "Evaluating " << num_candidates << " candidates...\n";
    
    for(int i = 0; i < num_candidates; i++) {
        auto [x, y, score, _] = evaluations[i];
        
        for(int sim = 0; sim < simulations; sim++) {
            int result = simulateGame(x, y, current_player, board);
            if(result == 1) results[i][0]++;
            else if(result == -1) results[i][1]++;
            else results[i][2]++;
        }
        
        cout << "\rProgress: " << (i + 1) << "/" << num_candidates;
        cout.flush();
    }
    
    double best_ucb = -1e9;
    int best_idx = 0;
    int total_sims = num_candidates * simulations;
    
    for(int i = 0; i < num_candidates; i++) {
        double win_rate = (double)(results[i][0] + 0.5 * results[i][2]) / simulations;
        double ucb = win_rate + C * sqrt(log(total_sims) / simulations);
        
        if(ucb > best_ucb) {
            best_ucb = ucb;
            best_idx = i;
        }
    }
    
    cout << "\nSelected move with UCB: " << best_ucb << "\n";
    return {get<0>(evaluations[best_idx]), get<1>(evaluations[best_idx])};
}

void updateBounds(int x, int y) {
    Min_x = min(x - 2, Min_x);
    Max_x = max(x + 2, Max_x);
    Min_y = min(y - 2, Min_y);
    Max_y = max(y + 2, Max_y);
    
    Min_x = max(0, Min_x);
    Max_x = min(board_size - 1, Max_x);
    Min_y = max(0, Min_y);
    Max_y = min(board_size - 1, Max_y);
}

void placeMove(int x, int y, int player) {
    if(errorCheck(x, y, "placeMove")) return;
    
    updateBounds(x, y);
    board[x][y] = player;
    
    int status = checkGameStatus(board);
    if(status != 0) {
        printBoard(board);
        if(status == 1) cout << "Black (X) wins!\n";
        else if(status == -1) cout << "White (O) wins!\n";
        else cout << "Draw!\n";
        exit(0);
    }
}

void getPlayerInput() {
    while(true) {
        cout << "Input x value: ";
        cin >> input_x;
        cout << "Input y value: ";
        cin >> input_y;
        if(!errorCheck(input_x, input_y, "mainInput")) {
            break;
        }
    }
}

int main() {
    board.resize(board_size, vector<int>(board_size, 0));
    current_player = 1;
    
    printBoard(board);
    int steps = 0;
    
    while(true) {
        steps++;
        
        if(current_player == 1) {
            getPlayerInput();
            placeMove(input_x, input_y, current_player);
            clearBoard();
            printBoard(board);
            
            if(steps >= 3) {
                int score = evaluatePosition(input_x, input_y, current_player, board);
                cout << "Position (" << input_x << "," << input_y << ") score: " << score << "\n";
            }
        } else {  
            cout << "AI is thinking...\n";
            auto start = high_resolution_clock::now();
            
            pair<int, int> best_move;
            if(steps == 2) {
                vector<int> offsets = {-1, 0, 1};
                int dx = offsets[rand() % 3];
                int dy = offsets[rand() % 3];
                best_move = {input_x + dx, input_y + dy};
                
                if(!isValidPos(best_move.first, best_move.second) || 
                   board[best_move.first][best_move.second] != 0) {
                    best_move = {board_size/2, board_size/2};
                }
            } else {
                best_move = mctsBestMove();
            }
            
            placeMove(best_move.first, best_move.second, current_player);
            
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(end - start);
            
            clearBoard();
            printBoard(board);
            cout << "AI move: (" << best_move.first << "," << best_move.second << ")\n";
            cout << "Thinking time: " << duration.count() << "ms\n";
        }
        
        current_player = switchPlayer(current_player);
    }
    
    return 0;
}
