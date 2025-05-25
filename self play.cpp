#include<iostream>
#include<string>
#include<vector>
#include<chrono>
#include<thread>
#include<tuple>
#include<map>
#include<random>
#include<algorithm>
#include<set>
#include<climits>
#include<fstream>
#include<sstream>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;
int balence_coefficient;

class chessBoard{
private:
	int size;
	vector<vector<int>> board;				//-1: white(O); 0: empty(.); 1: black(X)
	int direction[4][2]={
		{1,0},
		{0,1},
		{1,1},
		{1,-1}
	};
public:
	int current_player;
	chessBoard(int s, int cp){				//board size, current player
		size=s;
		board.resize(s,vector<int>(s,0));
		current_player=cp;
	}
	bool checkBoundary(int x, int y){return (x<0||y<0||x>=size||y>=size)?false:true;}
	bool ifValid(int x, int y){return (board[x][y]==0)?true:false;}
	pair<int, int> takeInput(){
		int input_x, input_y;
		while(1){
			cout<<"Input x value: ";
			cin>>input_x;
			cout<<"Input y value: ";
			cin>>input_y;
			if(checkBoundary(input_x, input_y) && ifValid(input_x, input_y)){
				break;
			}else{
				cout<<"Invalid move, out of bound or position occupied"<<endl;
			}
		}
		return make_pair(input_x, input_y);
	}
	void printBoard(){
		//print number row
		cout<<"    ";
		for(int i=0;i<size;i++){
			if(i<=8)cout<<i<<"  ";
			else cout<<i<<" ";
		}
		cout<<'y'<<endl;
		for(int i=0;i<size;i++){
			if(i<=9) cout<<" "<<i<<"  ";
			else cout<<i<<"  ";
			for(int j=0;j<size;j++){
				if(board[i][j]==0){		//none
					cout<<'.';
				}else if(board[i][j]==1){	//black
					cout<<'X';
				}else if(board[i][j]==-1){	//white
					cout<<'O';
				}
				cout<<"  ";
			}
			cout<<endl;
		}
		cout<<'x'<<endl;
	}
	void printWinningMessage(int player){
		clearScreen();
		printBoard();
		string winning_player=(player==1)?"black":"white";
		cout<<endl<<winning_player<<" wins!"<<endl;
		//exit(0);																		//DON'T EXIT
	}
	void switchPlayer(){current_player*=-1;}
	void printParameters(){
		cout<<endl;
		cout<<"current player: "<<((current_player==1)?"black":"white")<<endl;
	}
	bool placeAMove(int x, int y){
		if(!checkBoundary(x, y)){
			cout<<endl<<"error: placeAMove, out of bound"<<endl;
			return false;
		}
		if(!ifValid(x, y)){
			cout<<endl<<"error: placeAMove, chess piece already exists"<<endl;
			return false;
		}
		board[x][y]=current_player;
		return true;
	}
	bool checkStatus(int x, int y){
		if(!checkBoundary(x, y)){
			cout<<endl<<"error: checkStatus, out of bound"<<endl;
			return false;
		}
		if(board[x][y]!=current_player){
			cout<<endl<<"error: checkStatus, players don't match"<<endl;
			return false;
		}
		for(int dir=0;dir<4;dir++){
			int count=1;
			
			int dx=direction[dir][0];
			int dy=direction[dir][1];
			int i=x+dx, j=y+dy;
			while(checkBoundary(i,j)&&board[i][j]==current_player){
				i+=dx;
				j+=dy;
				count++;
			}
			i=x-dx;
			j=y-dy;
			while(checkBoundary(i,j)&&board[i][j]==current_player){
				i-=dx;
				j-=dy;
				count++;
			}
			if(count>=5){
				printWinningMessage(current_player);
				return false;
			}
		}
		return true;							//no one wins
	}
	void clearScreen(){system("cls");}
	vector<vector<int>> getBoard() const{
		return board;
	}
	int getSize() const{
		return size;
	}
};

class Node{
public:
	int current_score=0;
	vector<vector<int>> board;
	int current_player;
	int size=15;
	int direction[4][2]={
		{1,0},
		{0,1},
		{1,1},
		{1,-1}
	};
	bool checkBoundary(int x, int y){return (x<0||y<0||x>=size||y>=size)?false:true;}
	
	Node(vector<vector<int>> b, int cp){
		current_player=cp;
		board=b;
	}
	
	bool GameOver(int cp){
		for(int p=0;p<size;p++){
			for(int q=0;q<size;q++){
				if(board[p][q]!=cp) continue;
				int x=p, y=q;
				for(int dir=0;dir<4;dir++){
					int count=1;
					int dx=direction[dir][0];
					int dy=direction[dir][1];
					int i=x+dx, j=y+dy;
					while(checkBoundary(i,j)&&board[i][j]==cp){
						i+=dx;
						j+=dy;
						count++;
					}
					i=x-dx;
					j=y-dy;
					while(checkBoundary(i,j)&&board[i][j]==cp){
						i-=dx;
						j-=dy;
						count++;
					}
					if(count>=5){
						return true;
					}
				}
			}
		}
		return false;
	}
	
	// check if can win immadiately
	pair<int,int> findWinningMove(int cp){
		for(int i=0;i<size;i++){
			for(int j=0;j<size;j++){
				if(board[i][j]==0){
					board[i][j]=cp;
					if(GameOver(cp)){
						board[i][j]=0;
						return make_pair(i,j);
					}
					board[i][j]=0;
				}
			}
		}
		return make_pair(-1,-1);
	}
	
	// if need defence
	pair<int,int> findBlockingMove(int cp){
		int enemy = -cp;
		for(int i=0;i<size;i++){
			for(int j=0;j<size;j++){
				if(board[i][j]==0){
					board[i][j]=enemy;
					if(GameOver(enemy)){
						board[i][j]=0;
						return make_pair(i,j);
					}
					board[i][j]=0;
				}
			}
		}
		return make_pair(-1,-1);
	}
	
	vector<pair<int,int>> getValidMoves(){
		vector<pair<int,int>> moves;
		set<pair<int,int>> candidate_moves;
		
		// prioritize positions around existing pieces
		for(int i=0;i<size;i++){
			for(int j=0;j<size;j++){
				if(board[i][j]!=0){
					for(int di=-2;di<=2;di++){
						for(int dj=-2;dj<=2;dj++){
							int ni=i+di, nj=j+dj;
							if(checkBoundary(ni,nj) && board[ni][nj]==0){
								candidate_moves.insert({ni,nj});
							}
						}
					}
				}
			}
		}
		
		// if no pieces exist, start from center
		if(candidate_moves.empty()){
			candidate_moves.insert({size/2, size/2});
		}
		
		for(auto move : candidate_moves){
			moves.push_back(move);
		}
		
		return moves;
	}
	
	int evaluateBoard(int evaluate_player){
		if(GameOver(evaluate_player)){
			return 1000000;
		}
		if(GameOver(-evaluate_player)){
			return -1000000;
		}
		
		int enemy_player=evaluate_player*-1;
		int evaluate_player_score=0;
		int enemy_player_score=0;
		
		for(int i=0;i<size;i++){
			for(int j=0;j<size;j++){
				int cp=board[i][j];
				if(cp==0) continue;
				
				for(int dir=0;dir<4;dir++){
					int dx=direction[dir][0];
					int dy=direction[dir][1];
					int prev_x=i-dx;
					int prev_y=j-dy;
					
					// avoid recalculating the same line
					if(checkBoundary(prev_x,prev_y) && board[prev_x][prev_y]==cp){
						continue;
					}
	
					if(board[i][j]==evaluate_player){
						evaluate_player_score+=isFiveInRow(i,j,dir);
						evaluate_player_score+=isFour(i,j,dir);  
						evaluate_player_score+=isThree(i,j,dir); 
						evaluate_player_score+=isTwo(i,j,dir);  
					}else if(board[i][j]==enemy_player){
						enemy_player_score+=isFiveInRow(i,j,dir);
						enemy_player_score+=isFour(i,j,dir);
						enemy_player_score+=isThree(i,j,dir);
						enemy_player_score+=isTwo(i,j,dir);
					}
				}
			}
		}
		
		// defense is more important than offense
		int total_score=evaluate_player_score - enemy_player_score * balence_coefficient;
		current_score=total_score;
		return total_score;
	}
	
	int isFiveInRow(int x, int y, int dir){
		int cp=board[x][y];
		int count=1;
		int i=x+direction[dir][0];
		int j=y+direction[dir][1];
		while(checkBoundary(i,j)&&board[i][j]==cp){
			count++;
			i+=direction[dir][0];
			j+=direction[dir][1];
		}
		if(count>=5){
			return 1000000;
		}
		return 0;
	}
	

	int isFour(int x, int y, int dir){
		int cp=board[x][y];
		int count=1;
		

		int i=x+direction[dir][0];
		int j=y+direction[dir][1];
		while(checkBoundary(i,j)&&board[i][j]==cp){
			count++;
			i+=direction[dir][0];
			j+=direction[dir][1];
		}
		
		if(count==4){
			
			bool front_empty = false, back_empty = false;
			
			
			int front_x = x-direction[dir][0];
			int front_y = y-direction[dir][1];
			if(checkBoundary(front_x,front_y) && board[front_x][front_y]==0){
				front_empty = true;
			}
			
			
			if(checkBoundary(i,j) && board[i][j]==0){
				back_empty = true;
			}
			
			if(front_empty && back_empty){
				return 100000;//live four
			}else if(front_empty || back_empty){
				return 70000;  // threat four
			}
		}
		return 0;
	}
	
	int isThree(int x, int y, int dir){
		int cp=board[x][y];
		int count=1;
		
		int i=x+direction[dir][0];
		int j=y+direction[dir][1];
		while(checkBoundary(i,j)&&board[i][j]==cp){
			count++;
			i+=direction[dir][0];
			j+=direction[dir][1];
		}
		
		if(count==3){
			bool front_empty = false, back_empty = false;
			
			int front_x = x-direction[dir][0];
			int front_y = y-direction[dir][1];
			if(checkBoundary(front_x,front_y) && board[front_x][front_y]==0){
				front_empty = true;
			}
			
			if(checkBoundary(i,j) && board[i][j]==0){
				back_empty = true;
			}
			
			if(front_empty && back_empty){
				return 5000;
			}else if(front_empty || back_empty){
				return 1000; //threat three
			}
		}
		return 0;
	}
	

	int isTwo(int x, int y, int dir){
		int cp=board[x][y];
		int count=1;
		
		int i=x+direction[dir][0];
		int j=y+direction[dir][1];
		while(checkBoundary(i,j)&&board[i][j]==cp){
			count++;
			i+=direction[dir][0];
			j+=direction[dir][1];
		}
		
		if(count==2){
			bool front_empty = false, back_empty = false;
			
			int front_x = x-direction[dir][0];
			int front_y = y-direction[dir][1];
			if(checkBoundary(front_x,front_y) && board[front_x][front_y]==0){
				front_empty = true;
			}
			
			if(checkBoundary(i,j) && board[i][j]==0){
				back_empty = true;
			}
			
			if(front_empty && back_empty){
				return 500; // live two
			}else if(front_empty || back_empty){
				return 100; // threat two
			}
		}
		return 0;
	}
	
};

// MinMax algorithm + Alpha-Beta pruning
int alphaBetaMinMax(Node node, int depth, int alpha, int beta, bool maximizing_player, int ai_player){
	// termination conditions
	if(depth==0 || node.GameOver(ai_player) || node.GameOver(-ai_player)){
		return node.evaluateBoard(ai_player);
	}
	
	vector<pair<int,int>> valid_moves = node.getValidMoves();
	
	if(maximizing_player){
		int max_eval = INT_MIN;
		for(auto move : valid_moves){
			int x = move.first, y = move.second;
			// Simulate move
			node.board[x][y] = node.current_player;
			Node child_node(node.board, -node.current_player);
			
			int eval = alphaBetaMinMax(child_node, depth-1, alpha, beta, false, ai_player);
			
			// Undo move
			node.board[x][y] = 0;
			
			max_eval = max(max_eval, eval);
			alpha = max(alpha, eval);
			
			// Alpha-Beta pruning
			if(beta <= alpha){
				break;
			}
		}
		return max_eval;
	}else{
		int min_eval = INT_MAX;
		for(auto move : valid_moves){
			int x = move.first, y = move.second;
			// Simulate move
			node.board[x][y] = node.current_player;
			Node child_node(node.board, -node.current_player);
			
			int eval = alphaBetaMinMax(child_node, depth-1, alpha, beta, true, ai_player);
			
			// Undo move
			node.board[x][y] = 0;
			
			min_eval = min(min_eval, eval);
			beta = min(beta, eval);
			
			// Alpha-Beta pruning
			if(beta <= alpha){
				break;
			}
		}
		return min_eval;
	}
}

pair<int, int> AIBestMove(vector<vector<int>> board, int current_player, int search_depth){
	Node node(board, current_player);
	
	
	pair<int,int> winning_move = node.findWinningMove(current_player);
	if(winning_move.first != -1){
		cout << "AI: (" << winning_move.first << "," << winning_move.second << ")" << endl;
		return winning_move;
	}
	

	pair<int,int> blocking_move = node.findBlockingMove(current_player);
	if(blocking_move.first != -1){
		cout << "AI: (" << blocking_move.first << "," << blocking_move.second << ")" << endl;
		return blocking_move;
	}
	
	vector<pair<int,int>> valid_moves = node.getValidMoves();
	vector<tuple<int,int,int>> best_move_score;
	int select_groups=3; 
	
	for(auto move : valid_moves){
		int x = move.first, y = move.second;
		// Simulate move
		node.board[x][y] = current_player;
		Node child_node(node.board, -current_player);
		
		// Use MinMax algorithm with Alpha-Beta pruning
		int score = alphaBetaMinMax(child_node, search_depth-1, INT_MIN, INT_MAX, false, current_player);
		
		// Undo move
		node.board[x][y] = 0;
		best_move_score.push_back({move.first,move.second, score});
	}
	
	sort(best_move_score.begin(), best_move_score.end(), [](const auto& a, const auto& b) {
        return get<2>(a) > get<2>(b); 
    });
    

    if(best_move_score.size() > 1 && get<2>(best_move_score[0]) - get<2>(best_move_score[1]) >= 10000){
    	cout << "AI best move: " << get<2>(best_move_score[0]) - get<2>(best_move_score[1]) << endl;
    	return make_pair(get<0>(best_move_score[0]),get<1>(best_move_score[0]));
	}
	
    select_groups=min(select_groups,int(best_move_score.size()));
    random_device rd;
	mt19937 gen(rd());  
    uniform_int_distribution<> dis(0,select_groups-1); 
    int index=dis(gen);
	return make_pair(get<0>(best_move_score[index]),get<1>(best_move_score[index]));
}

struct FlatGameRecord {
    vector<int> flat_board;// 255 size
    int player;             // 1 / -1
    int move_x, move_y;   
    int result;          //1 win; -1 lose; 0 tie
    int steps;
};

void saveToJson(const vector<FlatGameRecord>& data, const string& filename) {
    json j_array = json::array();
    for (const auto& rec : data) {
        json j;
        j["board"] = rec.flat_board;
        j["player"] = rec.player;
        j["move"] = {rec.move_x, rec.move_y};
        j["result"] = rec.result;
        j["steps"] = rec.steps;
        j_array.push_back(j);
    }
    ofstream out(filename);
    out << j_array.dump(2); // pretty print
}

int main(){
	balence_coefficient = 1.5;
	const int board_size = 15;
	const int search_depth = 3; 
	

	
	for(int i=0;i<10;i++){							//number of games played
		int current_player = 1;  
		chessBoard board(board_size, current_player);
		board.printBoard();
		pair<int,int> input;
		int input_x, input_y;
		int steps = 0;
		
		vector<FlatGameRecord> game_history;
		
		while(1){
			FlatGameRecord record;
			record.flat_board.clear();
			steps++;
			vector<vector<int>> b = board.getBoard();  // current board
			for (const auto& row : b) {
				for (int cell : row) {
					record.flat_board.push_back(cell); // flatten board
				}
			}
			if(board.current_player == 1){  // AI 1 
				
				if(steps==1){
					board.placeAMove(7, 7);
					/*----------*/
					record.player = board.current_player;
					record.move_x = 7;
					record.move_y = 7;
					record.result = 0; 
					record.steps = steps;
					game_history.push_back(record);
					/*---------*/
				}else{
					pair<int, int> AI_move = AIBestMove(board.getBoard(), board.current_player, search_depth);
					board.placeAMove(AI_move.first, AI_move.second);
					/*-----------*/
					
					record.player = board.current_player;
					record.move_x = AI_move.first;
					record.move_y = AI_move.second;
					record.result = 0; 
					record.steps = steps;
					game_history.push_back(record);
					/*-----------*/
					if(!board.checkStatus(AI_move.first, AI_move.second)){
						/*--------*/
						int winner = board.current_player;
						for (auto& r : game_history) {
						    if (winner == 0) {
						        r.result = 0;  // tie
						    } else {
						        r.result = (r.player == winner) ? 1 : -1;
						    }
						}
						/*-------*/	
						saveToJson(game_history, "selfplay_game_"+to_string(i)+".json");
						/*-------*/					
						break;
					}
				}
			}else{  // AI 2
				if(steps==2){
					random_device rd;
					mt19937 gen(rd());  
				    uniform_int_distribution<> dis(-1,1); 
					int x=dis(gen);
					int y=dis(gen);
					while(x==0&&y==0){
						y=dis(gen);
					}
					board.placeAMove(7+x,7+y);
					/*------------*/
					
					record.player = board.current_player;
					record.move_x = 7+x;
					record.move_y = 7+y;
					record.result = 0; 
					record.steps = steps;
					game_history.push_back(record);
					/*---------------*/
				}else{
					pair<int, int> AI_move = AIBestMove(board.getBoard(), board.current_player, search_depth);
					board.placeAMove(AI_move.first, AI_move.second);
					/*------------*/
					
					record.player = board.current_player;
					record.move_x = AI_move.first;
					record.move_y = AI_move.second;
					record.result = 0; 
					record.steps = steps;
					game_history.push_back(record);
					/*-------------*/
					if(!board.checkStatus(AI_move.first, AI_move.second)){
						/*--------*/
						int winner = board.current_player;
						for (auto& r : game_history) {
						    if (winner == 0) {
						        r.result = 0;  // tie
						    } else {
						        r.result = (r.player == winner) ? 1 : -1;
						    }
						}
						/*------*/
						saveToJson(game_history, "selfplay_game_"+to_string(i)+".json");
						/*-------*/
						break;
					}					
				}

			}
			board.clearScreen();
			board.printBoard();
			board.printParameters();
			board.switchPlayer();
		}
		cout << "steps: " << steps << endl << endl;
	}
	
	return 0;
}
