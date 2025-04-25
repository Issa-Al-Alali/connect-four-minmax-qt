#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QMessageBox>
#include <vector>
#include <limits>

using namespace std;

// Define the players
const char PLAYER_X = 'X';
const char PLAYER_O = 'O';
const char EMPTY = ' ';

// Structure to represent a move
struct Move {
    int row, col;
};

// Function to check if there are any moves left on the board
bool isMovesLeft(const vector<vector<char>>& board) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == EMPTY) {
                return true;
            }
        }
    }
    return false;
}

// Function to evaluate the board state
// Returns +10 if PLAYER_X wins, -10 if PLAYER_O wins, 0 otherwise
int evaluate(const vector<vector<char>>& board) {
    // Check rows for win
    for (int row = 0; row < 3; ++row) {
        if (board[row][0] == board[row][1] && board[row][1] == board[row][2]) {
            if (board[row][0] == PLAYER_X) return +10;
            else if (board[row][0] == PLAYER_O) return -10;
        }
    }

    // Check columns for win
    for (int col = 0; col < 3; ++col) {
        if (board[0][col] == board[1][col] && board[1][col] == board[2][col]) {
            if (board[0][col] == PLAYER_X) return +10;
            else if (board[0][col] == PLAYER_O) return -10;
        }
    }

    // Check diagonals for win
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        if (board[0][0] == PLAYER_X) return +10;
        else if (board[0][0] == PLAYER_O) return -10;
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
        if (board[0][2] == PLAYER_X) return +10;
        else if (board[0][2] == PLAYER_O) return -10;
    }

    // If no winner, return 0
    return 0;
}

// The MiniMax algorithm function
// isMaximizingPlayer is true if it's the maximizing player's turn (AI - PLAYER_X)
int minimax(vector<vector<char>>& board, int depth, bool isMaximizingPlayer) {
    int score = evaluate(board);

    // If Maximizer has won the game return his evaluated score
    if (score == 10) return score;

    // If Minimizer has won the game return his evaluated score
    if (score == -10) return score;

    // If there are no more moves and no winner then it's a tie
    if (isMovesLeft(board) == false) return 0;

    // If this maximizer's move
    if (isMaximizingPlayer) {
        int best = numeric_limits<int>::min(); // Initialize best score to negative infinity

        // Traverse all cells
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                // Check if cell is empty
                if (board[i][j] == EMPTY) {
                    // Make the move
                    board[i][j] = PLAYER_X;

                    // Call minimax recursively and choose the maximum value
                    best = max(best, minimax(board, depth + 1, !isMaximizingPlayer));

                    // Undo the move
                    board[i][j] = EMPTY;
                }
            }
        }
        return best;
    }
    // If this minimizer's move
    else {
        int best = numeric_limits<int>::max(); // Initialize best score to positive infinity

        // Traverse all cells
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                // Check if cell is empty
                if (board[i][j] == EMPTY) {
                    // Make the move
                    board[i][j] = PLAYER_O;

                    // Call minimax recursively and choose the minimum value
                    best = min(best, minimax(board, depth + 1, !isMaximizingPlayer));

                    // Undo the move
                    board[i][j] = EMPTY;
                }
            }
        }
        return best;
    }
}

// This will return the best possible move for the player
Move findBestMove(vector<vector<char>>& board) {
    int bestVal = numeric_limits<int>::min(); // Initialize best value to negative infinity
    Move bestMove;
    bestMove.row = -1;
    bestMove.col = -1;

    // Traverse all cells, evaluate minimax function for all empty cells.
    // And return the cell with optimal value.
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            // Check if cell is empty
            if (board[i][j] == EMPTY) {
                // Make the move
                board[i][j] = PLAYER_X;

                // compute evaluation function for this move.
                int moveVal = minimax(board, 0, false);

                // Undo the move
                board[i][j] = EMPTY;

                // If the value of the current move is more than the best value, then update best
                if (moveVal > bestVal) {
                    bestMove.row = i;
                    bestMove.col = j;
                    bestVal = moveVal;
                }
            }
        }
    }

    return bestMove;
}

// --- Qt GUI Implementation ---

class TicTacToeWindow : public QMainWindow {
    Q_OBJECT

public:
    TicTacToeWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        // Set up the main window
        setWindowTitle("Tic-Tac-Toe with MiniMax");
        setFixedSize(300, 350); // Set a fixed size for simplicity

        // Create the central widget and layout
        QWidget* centralWidget = new QWidget(this);
        QGridLayout* gridLayout = new QGridLayout(centralWidget);
        centralWidget->setLayout(gridLayout);
        setCentralWidget(centralWidget);

        // Create the game board buttons
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                QPushButton* button = new QPushButton("", centralWidget);
                button->setFixedSize(80, 80); // Fixed size for buttons
                button->setFont(QFont("Arial", 30)); // Set font size for X/O
                gridLayout->addWidget(button, i, j);
                boardButtons[i][j] = button;

                // Connect button click to the handleButtonClick slot
                connect(button, &QPushButton::clicked, this, [=]() {
                    handleButtonClick(i, j);
                    });

                // Initialize the board state
                gameBoard[i][j] = EMPTY;
            }
        }

        // Create a status label
        statusLabel = new QLabel("Your turn (O)", centralWidget);
        statusLabel->setAlignment(Qt::AlignCenter);
        statusLabel->setFont(QFont("Arial", 14));
        gridLayout->addWidget(statusLabel, 3, 0, 1, 3); // Span across all columns

        // Player O starts
        currentPlayer = PLAYER_O;
    }

private slots:
    // Slot to handle button clicks
    void handleButtonClick(int row, int col) {
        // Only process if the cell is empty and it's the player's turn
        if (gameBoard[row][col] == EMPTY && currentPlayer == PLAYER_O) {
            // Make the player's move
            gameBoard[row][col] = PLAYER_O;
            boardButtons[row][col]->setText(QString(PLAYER_O));
            boardButtons[row][col]->setEnabled(false); // Disable the button after move

            // Check for game end
            if (checkGameEnd()) {
                return; // Game is over
            }

            // Switch to AI's turn
            currentPlayer = PLAYER_X;
            statusLabel->setText("AI's turn (X)");

            // Trigger AI's move
            QApplication::processEvents(); // Process events to update GUI before AI move
            makeAIMove();

            // Check for game end after AI's move
            checkGameEnd();

            // Switch back to player's turn if game is not over
            if (evaluate(gameBoard) == 0 && isMovesLeft(gameBoard)) {
                currentPlayer = PLAYER_O;
                statusLabel->setText("Your turn (O)");
            }
        }
    }

    // Function to make the AI's move
    void makeAIMove() {
        Move aiMove = findBestMove(gameBoard);

        // Make the AI's move
        gameBoard[aiMove.row][aiMove.col] = PLAYER_X;
        boardButtons[aiMove.row][aiMove.col]->setText(QString(PLAYER_X));
        boardButtons[aiMove.row][aiMove.col]->setEnabled(false); // Disable the button
    }

    // Function to check if the game has ended
    bool checkGameEnd() {
        int score = evaluate(gameBoard);
        if (score == 10) {
            statusLabel->setText("AI Wins!");
            disableAllButtons();
            QMessageBox::information(this, "Game Over", "AI Wins!");
            return true;
        }
        else if (score == -10) {
            statusLabel->setText("You Win!");
            disableAllButtons();
            QMessageBox::information(this, "Game Over", "You Win!");
            return true;
        }
        else if (!isMovesLeft(gameBoard)) {
            statusLabel->setText("It's a Tie!");
            disableAllButtons();
            QMessageBox::information(this, "Game Over", "It's a Tie!");
            return true;
        }
        return false;
    }

    // Function to disable all buttons after the game ends
    void disableAllButtons() {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                boardButtons[i][j]->setEnabled(false);
            }
        }
    }


private:
    QPushButton* boardButtons[3][3]; // 2D array of buttons
    vector<vector<char>> gameBoard = vector<vector<char>>(3, vector<char>(3, EMPTY)); // Game board state
    char currentPlayer; // 'X' or 'O'
    QLabel* statusLabel; // Label to display game status
};

#include "main.moc" // Include the generated moc file

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    TicTacToeWindow w;
    w.show();
    return a.exec();
}
