#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QMessageBox>
#include <vector>
#include <limits>
#include <string>
#include <QDebug>
#include <QPainter> // Required for drawing
#include <QPixmap>  // Required for drawing on labels
#include <QColor>   // Required for colors

using namespace std;

// Define board dimensions
const int ROWS = 6;
const int COLS = 7;

// Define players
const char PLAYER_YELLOW = 'Y'; // Human player
const char PLAYER_RED = 'R';     // AI player
const char EMPTY = ' ';

// Structure to represent a move (just the column)
struct Move {
    int col;
};

// --- Connect Four Game Logic ---

// Basic board representation
using Connect4Board = vector<vector<char>>;

// Function to initialize an empty board
Connect4Board initializeBoard() {
    return Connect4Board(ROWS, vector<char>(COLS, EMPTY));
}

// Function to check if a column is valid and not full
bool isValidLocation(const Connect4Board& board, int col) {
    return col >= 0 && col < COLS && board[0][col] == EMPTY;
}

// Function to drop a piece into a column
// Returns the row where the piece landed, or -1 if the move was invalid
int dropPiece(Connect4Board& board, int col, char piece) {
    if (!isValidLocation(board, col)) {
        return -1; // Invalid move
    }
    for (int r = ROWS - 1; r >= 0; --r) {
        if (board[r][col] == EMPTY) {
            board[r][col] = piece;
            return r; // Return the row where the piece landed
        }
    }
    return -1; // Should not reach here if isValidLocation is checked
}

// Function to check for a win
bool checkWin(const Connect4Board& board, char piece) {
    // Check horizontal
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c <= COLS - 4; ++c) {
            if (board[r][c] == piece && board[r][c + 1] == piece && board[r][c + 2] == piece && board[r][c + 3] == piece) {
                return true;
            }
        }
    }

    // Check vertical
    for (int c = 0; c < COLS; ++c) {
        for (int r = 0; r <= ROWS - 4; ++r) {
            if (board[r][c] == piece && board[r + 1][c] == piece && board[r + 2][c] == piece && board[r + 3][c] == piece) {
                return true;
            }
        }
    }

    // Check positive diagonal
    for (int r = 0; r <= ROWS - 4; ++r) {
        for (int c = 0; c <= COLS - 4; ++c) {
            if (board[r][c] == piece && board[r + 1][c + 1] == piece && board[r + 2][c + 2] == piece && board[r + 3][c + 3] == piece) {
                return true;
            }
        }
    }

    // Check negative diagonal
    for (int r = 3; r < ROWS; ++r) {
        for (int c = 0; c <= COLS - 4; ++c) {
            if (board[r][c] == piece && board[r - 1][c + 1] == piece && board[r - 2][c + 2] == piece && board[r - 3][c + 3] == piece) {
                return true;
            }
        }
    }

    return false;
}

// Function to check for a tie
bool checkTie(const Connect4Board& board) {
    for (int c = 0; c < COLS; ++c) {
        if (board[0][c] == EMPTY) {
            return false; // Board is not full
        }
    }
    return true; // Board is full and no winner
}

// Simplified evaluation function for Connect Four
// Evaluates the board state based on potential winning lines
int evaluateBoard(const Connect4Board& board) {
    int score = 0;

    // Simple evaluation: count 2-in-a-rows and 3-in-a-rows for each player
    // More sophisticated evaluation would consider blocking, threats, etc.

    // Evaluate for AI (Red)
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c <= COLS - 4; ++c) {
            int redCount = 0;
            int emptyCount = 0;
            for (int i = 0; i < 4; ++i) {
                if (board[r][c + i] == PLAYER_RED) redCount++;
                else if (board[r][c + i] == EMPTY) emptyCount++;
            }
            if (redCount == 3 && emptyCount == 1) score += 5;
            else if (redCount == 2 && emptyCount == 2) score += 2;
        }
    }

    for (int c = 0; c < COLS; ++c) {
        for (int r = 0; r <= ROWS - 4; ++r) {
            int redCount = 0;
            int emptyCount = 0;
            for (int i = 0; i < 4; ++i) {
                if (board[r + i][c] == PLAYER_RED) redCount++;
                else if (board[r + i][c] == EMPTY) emptyCount++;
            }
            if (redCount == 3 && emptyCount == 1) score += 5;
            else if (redCount == 2 && emptyCount == 2) score += 2;
        }
    }

    for (int r = 0; r <= ROWS - 4; ++r) {
        for (int c = 0; c <= COLS - 4; ++c) {
            int redCount = 0;
            int emptyCount = 0;
            for (int i = 0; i < 4; ++i) {
                if (board[r + i][c + i] == PLAYER_RED) redCount++;
                else if (board[r + i][c + i] == EMPTY) emptyCount++;
            }
            if (redCount == 3 && emptyCount == 1) score += 5;
            else if (redCount == 2 && emptyCount == 2) score += 2;
        }
    }

    for (int r = 3; r < ROWS; ++r) {
        for (int c = 0; c <= COLS - 4; ++c) {
            int redCount = 0;
            int emptyCount = 0;
            for (int i = 0; i < 4; ++i) {
                if (board[r - i][c + i] == PLAYER_RED) redCount++;
                else if (board[r - i][c + i] == EMPTY) emptyCount++;
            }
            if (redCount == 3 && emptyCount == 1) score += 5;
            else if (redCount == 2 && emptyCount == 2) score += 2;
        }
    }


    // Evaluate for Human (Yellow) - Penalize AI for human opportunities
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c <= COLS - 4; ++c) {
            int yellowCount = 0;
            int emptyCount = 0;
            for (int i = 0; i < 4; ++i) {
                if (board[r][c + i] == PLAYER_YELLOW) yellowCount++;
                else if (board[r][c + i] == EMPTY) emptyCount++;
            }
            if (yellowCount == 3 && emptyCount == 1) score -= 4; // Block human win
            else if (yellowCount == 2 && emptyCount == 2) score -= 1;
        }
    }

    for (int c = 0; c < COLS; ++c) {
        for (int r = 0; r <= ROWS - 4; ++r) {
            int yellowCount = 0;
            int emptyCount = 0;
            for (int i = 0; i < 4; ++i) {
                if (board[r + i][c] == PLAYER_YELLOW) yellowCount++;
                else if (board[r + i][c] == EMPTY) emptyCount++;
            }
            if (yellowCount == 3 && emptyCount == 1) score -= 4; // Block human win
            else if (yellowCount == 2 && emptyCount == 2) score -= 1;
        }
    }

    for (int r = 0; r <= ROWS - 4; ++r) {
        for (int c = 0; c <= COLS - 4; ++c) {
            int yellowCount = 0;
            int emptyCount = 0;
            for (int i = 0; i < 4; ++i) {
                if (board[r + i][c + i] == PLAYER_YELLOW) yellowCount++;
                else if (board[r + i][c + i] == EMPTY) emptyCount++;
            }
            if (yellowCount == 3 && emptyCount == 1) score -= 4; // Block human win
            else if (yellowCount == 2 && emptyCount == 2) score -= 1;
        }
    }

    for (int r = 3; r < ROWS; ++r) {
        for (int c = 0; c <= COLS - 4; ++c) {
            int yellowCount = 0;
            int emptyCount = 0;
            for (int i = 0; i < 4; ++i) {
                if (board[r - i][c + i] == PLAYER_YELLOW) yellowCount++;
                else if (board[r - i][c + i] == EMPTY) emptyCount++;
            }
            if (yellowCount == 3 && emptyCount == 1) score -= 4; // Block human win
            else if (yellowCount == 2 && emptyCount == 2) score -= 1;
        }
    }


    return score;
}


// The MiniMax algorithm function
// isMaximizingPlayer is true if it's the maximizing player's turn (AI - Red)
int minimax(Connect4Board& board, int depth, int maxDepth, bool isMaximizingPlayer) {
    // Check for terminal states
    if (checkWin(board, PLAYER_RED)) return 1000 - depth; // AI wins (higher score for shorter game)
    if (checkWin(board, PLAYER_YELLOW)) return -1000 + depth; // Human wins (lower score for shorter game)
    if (checkTie(board)) return 0; // Tie

    // Base case: If max depth is reached
    if (depth == maxDepth) {
        return evaluateBoard(board);
    }

    if (isMaximizingPlayer) { // AI's turn (Red)
        int best = numeric_limits<int>::min();

        // Iterate over possible columns to drop a piece
        for (int col = 0; col < COLS; ++col) {
            if (isValidLocation(board, col)) {
                // Make the move
                Connect4Board tempBoard = board; // Create a temporary board
                dropPiece(tempBoard, col, PLAYER_RED);

                // Recurse
                best = max(best, minimax(tempBoard, depth + 1, maxDepth, false));
            }
        }
        return best;
    }
    else { // Human's turn (Yellow)
        int best = numeric_limits<int>::max();

        // Iterate over possible columns to drop a piece
        for (int col = 0; col < COLS; ++col) {
            if (isValidLocation(board, col)) {
                // Make the move
                Connect4Board tempBoard = board; // Create a temporary board
                dropPiece(tempBoard, col, PLAYER_YELLOW);

                // Recurse
                best = min(best, minimax(tempBoard, depth + 1, maxDepth, true));
            }
        }
        return best;
    }
}

// Function to find the best move for the AI using MiniMax
Move findBestMove(Connect4Board& board, int maxDepth) {
    int bestVal = numeric_limits<int>::min();
    Move bestMove = { -1 }; // Indicate no valid move found yet

    // Iterate over possible columns
    for (int col = 0; col < COLS; ++col) {
        if (isValidLocation(board, col)) {
            // Make the move on a temporary board
            Connect4Board tempBoard = board;
            dropPiece(tempBoard, col, PLAYER_RED);

            // Compute evaluation function for this move.
            // After AI moves, it's Human's turn (minimizing)
            int moveVal = minimax(tempBoard, 0, maxDepth, false);

            // If the value of the current move is more than the best value, then update best
            if (moveVal > bestVal) {
                bestVal = moveVal;
                bestMove = { col };
            }
        }
    }
    qDebug() << "Best AI move column:" << bestMove.col << "with value" << bestVal;
    return bestMove;
}


// --- Qt GUI Implementation ---

class Connect4Window : public QMainWindow {
    Q_OBJECT // Add this macro

public:
    Connect4Window(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Connect Four with MiniMax");
        setFixedSize(COLS * 80, ROWS * 80 + 50); // Adjust size for 6x7 board + status label

        QWidget* centralWidget = new QWidget(this);
        QGridLayout* gridLayout = new QGridLayout(centralWidget);
        centralWidget->setLayout(gridLayout);
        setCentralWidget(centralWidget);

        // Create the board buttons (representing columns to drop into)
        for (int c = 0; c < COLS; ++c) {
            QPushButton* button = new QPushButton("Drop", centralWidget);
            button->setFixedSize(80, 50); // Button size for dropping
            button->setFont(QFont("Arial", 12));
            gridLayout->addWidget(button, 0, c); // Place drop buttons in the top row

            // Connect button click to the handleColumnClick slot
            connect(button, &QPushButton::clicked, this, [=]() {
                handleColumnClick(c);
                });
            columnButtons[c] = button; // Store column buttons
        }

        // Create labels to represent the board squares
        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                QLabel* label = new QLabel("", centralWidget);
                label->setFixedSize(80, 80); // Size for board squares
                label->setStyleSheet("background-color: lightblue; border: 1px solid black;"); // Basic styling
                label->setAlignment(Qt::AlignCenter);
                // No font size needed as we will draw circles
                gridLayout->addWidget(label, r + 1, c); // Place board labels below drop buttons
                boardLabels[r][c] = label; // Store board labels
            }
        }


        // Status label
        statusLabel = new QLabel("Your turn (Yellow)", centralWidget); // Human starts as Yellow
        statusLabel->setAlignment(Qt::AlignCenter);
        statusLabel->setFont(QFont("Arial", 16));
        gridLayout->addWidget(statusLabel, ROWS + 1, 0, 1, COLS); // Span across all columns

        // Initialize game state
        gameBoard = initializeBoard();
        currentPlayer = PLAYER_YELLOW; // Human (Yellow) starts
        aiSearchDepth = 5; // Set a shallow search depth for simplicity/speed

        updateBoardUI(); // Update the UI to show the initial board
    }

private slots:
    // Slot to handle column button clicks
    void handleColumnClick(int col) {
        // Only process if it's the human player's turn (Yellow) and the column is valid
        if (currentPlayer != PLAYER_YELLOW || !isValidLocation(gameBoard, col)) {
            QMessageBox::warning(this, "Invalid Move", "Cannot drop here.");
            return;
        }

        // Drop the human player's piece
        int row = dropPiece(gameBoard, col, PLAYER_YELLOW);
        updateBoardUI(); // Update the UI

        // Check for game end after human move
        if (checkGameEnd()) {
            return; // Game is over
        }

        // Switch to AI's turn
        currentPlayer = PLAYER_RED;
        statusLabel->setText("AI's turn (Red)");

        // Trigger AI's move
        QApplication::processEvents(); // Update UI before AI thinks
        makeAIMove();
        updateBoardUI();

        // Check for game end after AI move
        if (!checkGameEnd()) {
            // Switch back to human's turn if game is not over
            currentPlayer = PLAYER_YELLOW;
            statusLabel->setText("Your turn (Yellow)");
        }
    }

    // Function to update the board UI based on the gameBoard state
    void updateBoardUI() {
        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                QPixmap pixmap(boardLabels[r][c]->size());
                pixmap.fill(QColor(173, 216, 230)); // Light blue background for the board square

                QPainter painter(&pixmap);
                painter.setRenderHint(QPainter::Antialiasing); // For smoother circles

                if (gameBoard[r][c] == PLAYER_YELLOW) {
                    painter.setBrush(QBrush(Qt::yellow));
                    painter.setPen(Qt::NoPen); // No border for the circle
                    // Draw a circle slightly smaller than the label to have some padding
                    int circleSize = min(boardLabels[r][c]->width(), boardLabels[r][c]->height()) * 0.8;
                    int x = (boardLabels[r][c]->width() - circleSize) / 2;
                    int y = (boardLabels[r][c]->height() - circleSize) / 2;
                    painter.drawEllipse(x, y, circleSize, circleSize);
                }
                else if (gameBoard[r][c] == PLAYER_RED) {
                    painter.setBrush(QBrush(Qt::red));
                    painter.setPen(Qt::NoPen); // No border for the circle
                    int circleSize = min(boardLabels[r][c]->width(), boardLabels[r][c]->height()) * 0.8;
                    int x = (boardLabels[r][c]->width() - circleSize) / 2;
                    int y = (boardLabels[r][c]->height() - circleSize) / 2;
                    painter.drawEllipse(x, y, circleSize, circleSize);
                }

                painter.end(); // End painting

                boardLabels[r][c]->setPixmap(pixmap); // Set the pixmap to the label
                boardLabels[r][c]->setText(""); // Ensure no text is displayed
            }
        }
        // Update the state of column buttons (disable if full)
        for (int c = 0; c < COLS; ++c) {
            columnButtons[c]->setEnabled(isValidLocation(gameBoard, c));
        }
    }

    // Function to make the AI's move using MiniMax
    void makeAIMove() {
        statusLabel->setText("AI's turn (Red) - Thinking...");
        QApplication::processEvents(); // Update status label
        qDebug() << "AI thinking...";

        Move aiMove = findBestMove(gameBoard, aiSearchDepth);

        if (aiMove.col != -1) { // Check if a valid move was found
            dropPiece(gameBoard, aiMove.col, PLAYER_RED);
            qDebug() << "AI made move in column:" << aiMove.col;
        }
        else {
            // No valid move found for AI (shouldn't happen in a normal game unless game over)
            statusLabel->setText("AI has no legal moves.");
            qDebug() << "AI found no legal moves.";
        }
    }

    // Function to check if the game has ended
    bool checkGameEnd() {
        if (checkWin(gameBoard, PLAYER_YELLOW)) {
            statusLabel->setText("You Win!");
            disableAllButtons();
            QMessageBox::information(this, "Game Over", "You Win!");
            currentPlayer = EMPTY; // Indicate game over
            qDebug() << "Game Over: Human Wins!";
            return true;
        }
        else if (checkWin(gameBoard, PLAYER_RED)) {
            statusLabel->setText("AI Wins!");
            disableAllButtons();
            QMessageBox::information(this, "Game Over", "AI Wins!");
            currentPlayer = EMPTY; // Indicate game over
            qDebug() << "Game Over: AI Wins!";
            return true;
        }
        else if (checkTie(gameBoard)) {
            statusLabel->setText("It's a Tie!");
            disableAllButtons();
            QMessageBox::information(this, "Game Over", "It's a Tie!");
            currentPlayer = EMPTY; // Indicate game over
            qDebug() << "Game Over: Tie!";
            return true;
        }
        return false;
    }

    // Function to disable all column buttons after the game ends
    void disableAllButtons() {
        for (int c = 0; c < COLS; ++c) {
            columnButtons[c]->setEnabled(false);
        }
    }


private:
    QPushButton* columnButtons[COLS]; // Buttons for dropping pieces
    QLabel* boardLabels[ROWS][COLS]; // Labels to represent board squares
    Connect4Board gameBoard; // Game board state
    char currentPlayer; // 'Y' or 'R'
    int aiSearchDepth; // Depth for MiniMax search
    QLabel* statusLabel; // Label to display game status
};

#include "main.moc" // Include the generated moc file

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    Connect4Window w;
    w.show();
    return a.exec();
}
