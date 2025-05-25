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
using namespace std;
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
		exit(0);
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
	
	vector<pair<int,int>> getValidMoves(){
		vector<pair<int,int>> moves;
		set<pair<int,int>> candidate_moves;
		
		// Prioritize positions around existing pieces
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
		
		// If no pieces exist, start from center
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
			return 100000;
		}
		if(GameOver(-evaluate_player)){
			return -100000;
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
					
					// Avoid recalculating the same line
					if(checkBoundary(prev_x,prev_y) && board[prev_x][prev_y]==cp){
						continue;
					}
	
					if(board[i][j]==evaluate_player){
						evaluate_player_score+=isFiveInRow(i,j,dir);
						evaluate_player_score+=isLiveFour(i,j,dir);
						evaluate_player_score+=isThreatFour(i,j,dir);
						evaluate_player_score+=isLiveThree(i,j,dir);
						evaluate_player_score+=isThreatThree(i,j,dir);
						evaluate_player_score+=isLiveTwo(i,j,dir);
						evaluate_player_score+=isThreatTwo(i,j,dir);
					}else if(board[i][j]==enemy_player){
						enemy_player_score+=isFiveInRow(i,j,dir);
						enemy_player_score+=isLiveFour(i,j,dir);
						enemy_player_score+=isThreatFour(i,j,dir);
						enemy_player_score+=isLiveThree(i,j,dir);
						enemy_player_score+=isThreatThree(i,j,dir);
						enemy_player_score+=isLiveTwo(i,j,dir);
						enemy_player_score+=isThreatTwo(i,j,dir);
					}
				}
			}
		}
		
		// Defense is more important than offense
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
			return 100000;
		}
		return 0;
	}
	
	int isLiveFour(int x, int y, int dir){
		int i=x-direction[dir][0],j=y-direction[dir][1];
		if(checkBoundary(i,j)&&board[i][j]!=0){
			return 0;
		}
		int cp=board[x][y];
		int count=1;
		i=x+direction[dir][0];
		j=y+direction[dir][1];
		while(checkBoundary(i,j)&&board[i][j]==cp){
			i+=direction[dir][0];
			j+=direction[dir][1];
			count++;
		}
		if(count==4&&checkBoundary(i,j)&&board[i][j]==0){
			return 50000;
		}
		return 0;
	}
	
	int isThreatFour(int x, int y, int dir){
		bool front=false, tail=false;
		int i=x-direction[dir][0],j=y-direction[dir][1];
		if(checkBoundary(i,j)&&board[i][j]==0){
			front=true;
		}
		int cp=board[x][y];
		int count=1;
		i=x+direction[dir][0];
		j=y+direction[dir][1];
		while(checkBoundary(i,j)&&board[i][j]==cp){
			i+=direction[dir][0];
			j+=direction[dir][1];
			count++;
		}
		if(checkBoundary(i,j)&&board[i][j]==0){
			tail=true;
		}
		if(count==4&&((front&&!tail)||(!front&&tail))){
			return 10000;
		}
		return 0;
	}
	
	int isLiveThree(int x, int y, int dir){
		int i=x-direction[dir][0],j=y-direction[dir][1];
		if(checkBoundary(i,j)&&board[i][j]!=0){
			return 0;
		}
		int cp=board[x][y];
		int count=1;
		i=x+direction[dir][0];
		j=y+direction[dir][1];
		while(checkBoundary(i,j)&&board[i][j]==cp){
			i+=direction[dir][0];
			j+=direction[dir][1];
			count++;
		}
		if(count==3&&checkBoundary(i,j)&&board[i][j]==0){
			return 5000;
		}
		return 0;
	}
	
	int isThreatThree(int x, int y, int dir){
		bool front=false, tail=false;
		int i=x-direction[dir][0],j=y-direction[dir][1];
		if(checkBoundary(i,j)&&board[i][j]==0){
			front=true;
		}
		int cp=board[x][y];
		int count=1;
		i=x+direction[dir][0];
		j=y+direction[dir][1];
		while(checkBoundary(i,j)&&board[i][j]==cp){
			i+=direction[dir][0];
			j+=direction[dir][1];
			count++;
		}
		if(checkBoundary(i,j)&&board[i][j]==0){
			tail=true;
		}
		if(count==3&&((front&&!tail)||(!front&&tail))){
			return 1000;
		}
		return 0;
	}
	
	int isLiveTwo(int x, int y, int dir){
		int i=x-direction[dir][0],j=y-direction[dir][1];
		if(checkBoundary(i,j)&&board[i][j]!=0){
			return 0;
		}
		int cp=board[x][y];
		int count=1;
		i=x+direction[dir][0];
		j=y+direction[dir][1];
		while(checkBoundary(i,j)&&board[i][j]==cp){
			i+=direction[dir][0];
			j+=direction[dir][1];
			count++;
		}
		if(count==2&&checkBoundary(i,j)&&board[i][j]==0){
			return 500;
		}
		return 0;
	}
	
	int isThreatTwo(int x, int y, int dir){
		bool front=false, tail=false;
		int i=x-direction[dir][0],j=y-direction[dir][1];
		if(checkBoundary(i,j)&&board[i][j]==0){
			front=true;
		}
		int cp=board[x][y];
		int count=1;
		i=x+direction[dir][0];
		j=y+direction[dir][1];
		while(checkBoundary(i,j)&&board[i][j]==cp){
			i+=direction[dir][0];
			j+=direction[dir][1];
			count++;
		}
		if(checkBoundary(i,j)&&board[i][j]==0){
			tail=true;
		}
		if(count==2&&((front&&!tail)||(!front&&tail))){
			return 100;
		}
		return 0;
	}
};

// MinMax algorithm + Alpha-Beta pruning
int alphaBetaMinMax(Node node, int depth, int alpha, int beta, bool maximizing_player, int ai_player){
	// Termination conditions
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
	vector<pair<int,int>> valid_moves = node.getValidMoves();
	
	int best_score = INT_MIN;
	pair<int, int> best_move = {-1, -1};
	
	for(auto move : valid_moves){
		int x = move.first, y = move.second;
		// Simulate move
		node.board[x][y] = current_player;
		Node child_node(node.board, -current_player);
		
		// Use MinMax algorithm with Alpha-Beta pruning
		int score = alphaBetaMinMax(child_node, search_depth-1, INT_MIN, INT_MAX, false, current_player);
		
		// Undo move
		node.board[x][y] = 0;
		
		if(score > best_score){
			best_score = score;
			best_move = move;
		}
	}
	
	cout<<"AI best move score: "<<best_score<<endl;
	return best_move;
}

int main(){
	balence_coefficient = 1;	//smaller than 1, more offensive tendency; Larger than 1, more defensice tendency
	const int board_size = 15;
	const int search_depth = 2;  // Search depth, adjustable based on performance
	
	int current_player = 1;      // 1: black(X), -1: white(O)
	chessBoard board(board_size, current_player);

	board.printBoard();
	for(int i=0;i<1000;i++){
		pair<int,int> input;
		int input_x, input_y;
		int steps = 0;
		
		while(1){
			steps++;
			if(board.current_player == 1){  // Player's turn
				if(steps==1){
					board.placeAMove(7, 7);
				}else{
					pair<int, int> AI_move = AIBestMove(board.getBoard(), board.current_player, 1);//depth
					board.placeAMove(AI_move.first, AI_move.second);
					if(!board.checkStatus(AI_move.first, AI_move.second)){
						break;
					}
				}
				
			}else{  // AI's turn
				pair<int, int> AI_move = AIBestMove(board.getBoard(), board.current_player, 2);
				board.placeAMove(AI_move.first, AI_move.second);
				if(!board.checkStatus(AI_move.first, AI_move.second)){
					break;
				}
			}
			board.clearScreen();
			board.printBoard();
			board.printParameters();
			board.switchPlayer();
		}
	}
	
	
	return 0;
}
