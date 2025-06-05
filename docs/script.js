const C = 1.414;  // UCB exploration parameter
const simulations = 100;
const total_candidates = 10;  // add candidates
const board_size = 15;

const score_five = 1000000;
const score_live_four = 100000;
const score_threat_four = 50000;
const score_live_three = 10000;
const score_threat_three = 1500;
const score_live_two = 1000;
const score_double_three = 80000;

// Game state variables
let current_player = 1;  // 1: black turn; -1: white turn;
let board = [];
let input_x, input_y;
let Min_x = 100, Min_y = 100, Max_x = -100, Max_y = -100;
let aiEnabled = true;
let gameRunning = true;
let stepCount = 0;
let lastMove = null;

// four directions
const dx = [0, 1, 1, 1];
const dy = [1, 0, 1, -1];

// DOM elements
let boardElement, statusElement, logElement, aiToggleButton;

// Initialize the game
function initGame() {
    boardElement = document.getElementById('board');
    statusElement = document.getElementById('status');
    logElement = document.getElementById('log');
    aiToggleButton = document.getElementById('ai-toggle');
    
    // Initialize board array
    board = Array(board_size).fill().map(() => Array(board_size).fill(0));
    
    createBoard();
    setupEventListeners();
    updateStatus();
    addLog("Game started! Black (X) goes first.");
}

function createBoard() {
    boardElement.innerHTML = '';
    for(let i = 0; i < board_size; i++) {
        for(let j = 0; j < board_size; j++) {
            const cell = document.createElement('div');
            cell.className = 'cell';
            cell.dataset.x = i;
            cell.dataset.y = j;
            cell.addEventListener('click', () => handleCellClick(i, j));
            boardElement.appendChild(cell);
        }
    }
}

function setupEventListeners() {
    document.getElementById('restart-btn').addEventListener('click', restartGame);
    aiToggleButton.addEventListener('click', toggleAI);
    
    // Mouse tracking for coordinates display
    boardElement.addEventListener('mouseover', (e) => {
        if(e.target.classList.contains('cell')) {
            const x = parseInt(e.target.dataset.x);
            const y = parseInt(e.target.dataset.y);
            document.getElementById('click-x').textContent = x;
            document.getElementById('click-y').textContent = y;
        }
    });
}

function switchPlayer(player) { 
    return -player; 
}

function isValidPos(x, y) {
    return x >= 0 && x < board_size && y >= 0 && y < board_size;
}

function errorCheck(x, y, error_source) {
    if(!isValidPos(x, y)) {
        addLog(`Error: Invalid move... Error source: (${error_source}) Error type: (out of bound)`);
        return true;
    }
    if(board[x][y] !== 0) {
        addLog(`Error: Invalid move... Error source: (${error_source}) Error type: (chess piece already exists)`);
        return true;
    }
    return false;
}

// LineInfo structure equivalent
class LineInfo {
    constructor() {
        this.count = 1;
        this.left_open = false;
        this.right_open = false;
        this.left_space = 0;
        this.right_space = 0;
    }
}

function getLineInfo(x, y, dir, player, _board) {
    const info = new LineInfo();
    
    // Check right direction
    let nx = x + dx[dir], ny = y + dy[dir];
    while(isValidPos(nx, ny) && _board[nx][ny] === player) {
        info.count++;
        nx += dx[dir];
        ny += dy[dir];
    }
    if(isValidPos(nx, ny) && _board[nx][ny] === 0) {
        info.right_open = true;
    }
    
    // Check left direction
    nx = x - dx[dir];
    ny = y - dy[dir];
    while(isValidPos(nx, ny) && _board[nx][ny] === player) {
        info.count++;
        nx -= dx[dir];
        ny -= dy[dir];
    }
    if(isValidPos(nx, ny) && _board[nx][ny] === 0) {
        info.left_open = true;
    }
    
    return info;
}

function isFiveInRow(x, y, player, _board) {
    for(let dir = 0; dir < 4; dir++) {
        const info = getLineInfo(x, y, dir, player, _board);
        if(info.count >= 5) return true;
    }
    return false;
}

function countLiveFour(x, y, player, _board) {
    let count = 0;
    for(let dir = 0; dir < 4; dir++) {
        const info = getLineInfo(x, y, dir, player, _board);
        if(info.count === 4 && info.left_open && info.right_open) {
            count++;
        }
    }
    return count;
}

function countThreatFour(x, y, player, _board) {
    let count = 0;
    for(let dir = 0; dir < 4; dir++) {
        const info = getLineInfo(x, y, dir, player, _board);
        if(info.count === 4 && (info.left_open || info.right_open) && !(info.left_open && info.right_open)) {
            count++;
        }
    }
    return count;
}

function countLiveThree(x, y, player, _board) {
    let count = 0;
    for(let dir = 0; dir < 4; dir++) {
        const info = getLineInfo(x, y, dir, player, _board);
        if(info.count === 3 && info.left_open && info.right_open) {
            count++;
        }
    }
    return count;
}

function countThreatThree(x, y, player, _board) {
    let count = 0;
    for(let dir = 0; dir < 4; dir++) {
        const info = getLineInfo(x, y, dir, player, _board);
        if(info.count === 3 && (info.left_open || info.right_open) && !(info.left_open && info.right_open)) {
            count++;
        }
    }
    return count;
}

function countLiveTwo(x, y, player, _board) {
    let count = 0;
    for(let dir = 0; dir < 4; dir++) {
        const info = getLineInfo(x, y, dir, player, _board);
        if(info.count === 2 && info.left_open && info.right_open) {
            count++;
        }
    }
    return count;
}

function hasDoubleThreeThreat(x, y, player, _board) {
    const temp_board = _board.map(row => [...row]);
    temp_board[x][y] = player;
    
    let live_three_count = 0;
    for(let dir = 0; dir < 4; dir++) {
        const info = getLineInfo(x, y, dir, player, temp_board);
        if(info.count === 3 && info.left_open && info.right_open) {
            live_three_count++;
        }
    }
    
    return live_three_count >= 2;
}

function evaluatePosition(x, y, player, _board) {
    let score = 0;
    
    score += score_five * (isFiveInRow(x, y, player, _board) ? 1 : 0);
    score += score_live_four * 1.1 * countLiveFour(x, y, player, _board);
    score += score_threat_four * countThreatFour(x, y, player, _board);
    score += score_live_three * 1.1 * countLiveThree(x, y, player, _board);
    score += score_threat_three * countThreatThree(x, y, player, _board);
    score += score_live_two * countLiveTwo(x, y, player, _board);
    
    if(hasDoubleThreeThreat(x, y, player, _board)) {
        score += score_double_three;
    }
    
    const opponent = switchPlayer(player);
    score += score_five * 1.1 * (isFiveInRow(x, y, opponent, _board) ? 1 : 0);
    score += score_live_four * 1.1 * countLiveFour(x, y, opponent, _board);
    score += score_threat_four * 1.0 * countThreatFour(x, y, opponent, _board);
    score += score_live_three * 0.9 * countLiveThree(x, y, opponent, _board);
    score += score_threat_three * 0.8 * countThreatThree(x, y, opponent, _board);
    score += score_live_two * 0.5 * countLiveTwo(x, y, opponent, _board);
    
    if(hasDoubleThreeThreat(x, y, opponent, _board)) {
        score += score_double_three * 0.95;
    }
    
    return score;
}

function checkGameStatus(_board) {
    for(let i = 0; i < board_size; i++) {
        for(let j = 0; j < board_size; j++) {
            if(_board[i][j] !== 0) {
                if(isFiveInRow(i, j, _board[i][j], _board)) {
                    return _board[i][j];
                }
            }
        }
    }
    
    let full = true;
    for(let i = 0; i < board_size; i++) {
        for(let j = 0; j < board_size; j++) {
            if(_board[i][j] === 0) {
                full = false;
                break;
            }
        }
        if(!full) break;
    }
    
    return full ? -2 : 0;  // -2 tie, 0 continue
}

function getCandidateMoves(_board, range = 2) {
    const candidates = new Set();
    
    for(let i = 0; i < board_size; i++) {
        for(let j = 0; j < board_size; j++) {
            if(_board[i][j] !== 0) {
                for(let di = -range; di <= range; di++) {
                    for(let dj = -range; dj <= range; dj++) {
                        const ni = i + di, nj = j + dj;
                        if(isValidPos(ni, nj) && _board[ni][nj] === 0) {
                            candidates.add(`${ni},${nj}`);
                        }
                    }
                }
            }
        }
    }
    
    if(candidates.size === 0) {
        const center = Math.floor(board_size / 2);
        for(let i = center - 1; i <= center + 1; i++) {
            for(let j = center - 1; j <= center + 1; j++) {
                if(isValidPos(i, j)) {
                    candidates.add(`${i},${j}`);
                }
            }
        }
    }
    
    return Array.from(candidates).map(pos => {
        const [x, y] = pos.split(',').map(Number);
        return [x, y];
    });
}

function getUrgentMove(player, _board) {
    const candidates = getCandidateMoves(_board);
    
    // Check for winning moves
    for(const [x, y] of candidates) {
        if(isFiveInRow(x, y, player, _board)) {
            return [x, y];
        }
    }
    
    // Block opponent's winning moves
    const opponent = switchPlayer(player);
    for(const [x, y] of candidates) {
        if(isFiveInRow(x, y, opponent, _board)) {
            return [x, y];
        }
    }
    
    // Create live four
    for(const [x, y] of candidates) {
        if(countLiveFour(x, y, player, _board) > 0) {
            return [x, y];
        }
    }
    
    // Block opponent's live four
    for(const [x, y] of candidates) {
        if(countLiveFour(x, y, opponent, _board) > 0) {
            return [x, y];
        }
    }
    
    // Create double three
    for(const [x, y] of candidates) {
        if(hasDoubleThreeThreat(x, y, player, _board)) {
            return [x, y];
        }
    }
    
    // Block opponent's double three
    for(const [x, y] of candidates) {
        if(hasDoubleThreeThreat(x, y, opponent, _board)) {
            return [x, y];
        }
    }
    
    // Create threat four
    for(const [x, y] of candidates) {
        if(countThreatFour(x, y, player, _board) > 0) {
            return [x, y];
        }
    }
    
    // // Block opponent's threat four this is actually threat three
    // for(const [x, y] of candidates) {
    //     if(countThreatFour(x, y, opponent, _board) > 0) {
    //         return [x, y];
    //     }
    // }
    
    // Create live three
    for(const [x, y] of candidates) {
        if(countLiveThree(x, y, player, _board) > 0) {
            return [x, y];
        }
    }
    // //oponent's live three
    // for(const [x, y] of candidates) {
    //     if(countLiveThree(x, y, opponent, _board) > 0) {
    //         return [x, y];
    //     }
    // }
    //bug below?
    // Block opponent's live three (prioritized)
    // const threat_moves = [];
    // for(const [x, y] of candidates) {
    //     const live_three_count = countLiveThree(x, y, opponent, _board);
    //     if(live_three_count > 0) {
    //         threat_moves.push([[x, y], live_three_count]);
    //     }
    // }
    
    // if(threat_moves.length > 0) {
    //     threat_moves.sort((a, b) => b[1] - a[1]);
    //     return threat_moves[0][0];
    // }
    
    return [-1, -1];
}

function simulateGame(x, y, player, _board) {
    const sim_board = _board.map(row => [...row]);
    sim_board[x][y] = player;
    let sim_player = switchPlayer(player);
    
    let moves = 0;
    const MAX_MOVES = 80;
    
    while(moves < MAX_MOVES) {
        const status = checkGameStatus(sim_board);
        if(status !== 0) {
            return status === player ? 1 : (status === switchPlayer(player) ? -1 : 0);
        }
        
        const candidates = getCandidateMoves(sim_board, 1);
        if(candidates.length === 0) break;
        
        const urgent = getUrgentMove(sim_player, sim_board);
        if(urgent[0] !== -1) {
            sim_board[urgent[0]][urgent[1]] = sim_player;
        } else {
            const randomIdx = Math.floor(Math.random() * candidates.length);
            const [nx, ny] = candidates[randomIdx];
            sim_board[nx][ny] = sim_player;
        }
        
        sim_player = switchPlayer(sim_player);
        moves++;
    }
    
    return 0;
}

async function mctsBestMove() {
    addLog("AI thinking with MCTS...");
    statusElement.textContent = "AI is thinking...";
    statusElement.className = "thinking";
    
    const urgent = getUrgentMove(current_player, board);
    if(urgent[0] !== -1) {
        addLog("Found urgent move!");
        return urgent;
    }
    
    const candidates = getCandidateMoves(board);
    const evaluations = [];
    
    for(const [x, y] of candidates) {
        const score = evaluatePosition(x, y, current_player, board);
        evaluations.push([x, y, score, 0]);
    }
    
    evaluations.sort((a, b) => b[2] - a[2]);
    
    const num_candidates = Math.min(total_candidates, evaluations.length);
    const results = Array(num_candidates).fill().map(() => [0, 0, 0]);
    
    addLog(`Evaluating ${num_candidates} candidates...`);
    
    for(let i = 0; i < num_candidates; i++) {
        const [x, y, score, _] = evaluations[i];
        
        for(let sim = 0; sim < simulations; sim++) {
            const result = simulateGame(x, y, current_player, board);
            if(result === 1) results[i][0]++;
            else if(result === -1) results[i][1]++;
            else results[i][2]++;
        }
        
        // Add small delay for UI responsiveness
        if(i % 3 === 0) {
            await new Promise(resolve => setTimeout(resolve, 1));
        }
    }
    
    let best_ucb = -1e9;
    let best_idx = 0;
    const total_sims = num_candidates * simulations;
    
    for(let i = 0; i < num_candidates; i++) {
        const win_rate = (results[i][0] + 0.5 * results[i][2]) / simulations;
        const ucb = win_rate + C * Math.sqrt(Math.log(total_sims) / simulations);
        
        if(ucb > best_ucb) {
            best_ucb = ucb;
            best_idx = i;
        }
    }
    
    addLog(`Selected move with UCB: ${best_ucb.toFixed(4)}`);
    return [evaluations[best_idx][0], evaluations[best_idx][1]];
}

function updateBounds(x, y) {
    Min_x = Math.min(x - 2, Min_x);
    Max_x = Math.max(x + 2, Max_x);
    Min_y = Math.min(y - 2, Min_y);
    Max_y = Math.max(y + 2, Max_y);
    
    Min_x = Math.max(0, Min_x);
    Max_x = Math.min(board_size - 1, Max_x);
    Min_y = Math.max(0, Min_y);
    Max_y = Math.min(board_size - 1, Max_y);
}

function placeMove(x, y, player) {
    if(errorCheck(x, y, "placeMove")) return false;
    
    updateBounds(x, y);
    board[x][y] = player;
    
    // Update visual board
    const cellIndex = x * board_size + y;
    const cell = boardElement.children[cellIndex];
    
    // Remove last move highlight
    if(lastMove) {
        const lastCellIndex = lastMove[0] * board_size + lastMove[1];
        boardElement.children[lastCellIndex].classList.remove('last-move');
    }
    
    // Add piece and highlight
    cell.className = `cell ${player === 1 ? 'black' : 'white'} last-move`;
    lastMove = [x, y];
    
    const status = checkGameStatus(board);
    if(status !== 0) {
        gameRunning = false;
        if(status === 1) {
            addLog("Black (X) wins!");
            statusElement.innerHTML = '<span class="winner">Black (X) wins!</span>';
        } else if(status === -1) {
            addLog("White (O) wins!");
            statusElement.innerHTML = '<span class="winner">White (O) wins!</span>';
        } else {
            addLog("Draw!");
            statusElement.innerHTML = '<span class="winner">Draw!</span>';
        }
        return true;
    }
    
    return true;
}

async function handleCellClick(x, y) {
    if(!gameRunning || (aiEnabled && current_player === -1)) return;
    if(stepCount === 0){
        input_x = x;
        input_y = y;
    }
    if(placeMove(x, y, current_player)) {
        stepCount++;
        
        if(current_player === 1) {
            addLog(`Player move: (${x},${y})`, 'player-move');
            if(stepCount >= 3) {
                const score = evaluatePosition(x, y, current_player, board);
                document.getElementById('move-score').textContent = score;
                addLog(`Position (${x},${y}) score: ${score}`);
            }
        }
        
        if(gameRunning) {
            current_player = switchPlayer(current_player);
            updateStatus();
            
            if(aiEnabled && current_player === -1 && gameRunning) {
                setTimeout(handleAIMove, 100);
            }
        }
    }
}

async function handleAIMove() {
    if(!gameRunning) return;
    
    const startTime = performance.now();
    
    let best_move;
    if(stepCount === 1) {
        // Simple opening strategy
        addLog("start simple strategy");
        const offsets = [-1, 0, 1];
        let dx = offsets[Math.floor(Math.random() * 3)];
        let dy = offsets[Math.floor(Math.random() * 3)];
        while(isValidPos(input_x+dx, input_y+dy)||(dx===0&&dy===0)){
            dx = offsets[Math.floor(Math.random() * 3)];
            dy = offsets[Math.floor(Math.random() * 3)];
        }
        best_move = [input_x + dx, input_y + dy];
        
        // if(!isValidPos(best_move[0], best_move[1]) || 
        //    board[best_move[0]][best_move[1]] !== 0) {
        //     best_move = [Math.floor(board_size/2), Math.floor(board_size/2)];
        // }
    } else {
        best_move = await mctsBestMove();
    }
    
    const endTime = performance.now();
    const thinkingTime = Math.round(endTime - startTime);
    
    if(placeMove(best_move[0], best_move[1], current_player)) {
        stepCount++;
        
        document.getElementById('ai-time').textContent = thinkingTime;
        addLog(`AI move: (${best_move[0]},${best_move[1]})`, 'ai-move');
        addLog(`Thinking time: ${thinkingTime}ms`);
        
        if(gameRunning) {
            current_player = switchPlayer(current_player);
            updateStatus();
        }
    }
}

function updateStatus() {
    if(gameRunning) {
        const playerText = current_player === 1 ? "Black's turn (X)" : "White's turn (O)";
        statusElement.textContent = playerText;
        statusElement.className = "";
    }
}

function addLog(message, className = '') {
    const logDiv = document.createElement('div');
    logDiv.textContent = message;
    if(className) logDiv.className = className;
    logElement.appendChild(logDiv);
    logElement.scrollTop = logElement.scrollHeight;
}

function restartGame() {
    // Reset game state
    board = Array(board_size).fill().map(() => Array(board_size).fill(0));
    current_player = 1;
    gameRunning = true;
    stepCount = 0;
    lastMove = null;
    Min_x = 100; Min_y = 100; Max_x = -100; Max_y = -100;
    
    // Reset UI
    createBoard();
    updateStatus();
    document.getElementById('move-score').textContent = '-';
    document.getElementById('ai-time').textContent = '-';
    document.getElementById('click-x').textContent = '-';
    document.getElementById('click-y').textContent = '-';
    
    // Clear log
    logElement.innerHTML = '';
    addLog("Game restarted! Black (X) goes first.");
}

function toggleAI() {
    aiEnabled = !aiEnabled;
    aiToggleButton.textContent = `AI: ${aiEnabled ? 'ON' : 'OFF'}`;
    
    if(aiEnabled) {
        addLog("AI enabled");
        if(current_player === -1 && gameRunning) {
            setTimeout(handleAIMove, 100);
        }
    } else {
        addLog("AI disabled - Two player mode");
    }
}
/*
// Store player input coordinates for AI opening strategy
function updatePlayerInput(x, y) {
    input_x = x;
    input_y = y;
}

// Override handleCellClick to store player input
const originalHandleCellClick = handleCellClick;
handleCellClick = async function(x, y) {
    if(current_player === 1) {
        updatePlayerInput(x, y);
    }
    return originalHandleCellClick.call(this, x, y);
};

// Initialize game when DOM is loaded
document.addEventListener('DOMContentLoaded', initGame);
*/
