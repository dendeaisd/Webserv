const gameContainer = document.getElementById("game");
const resetButton = document.getElementById("reset-btn");

let currentPlayer = "X";
let gameBoard = ["", "", "", "", "", "", "", "", ""];
let gameActive = true;
function handleClick(event) {
  const cellIndex = event.target.dataset.index;

  if (gameBoard[cellIndex] !== "" || !gameActive) {
    return;
  }

  gameBoard[cellIndex] = currentPlayer;
  event.target.innerText = currentPlayer;

  checkWinner();
  if (gameActive) {
    currentPlayer = currentPlayer === "X" ? "O" : "X";
  }
}

function checkWinner() {
  const winningConditions = [
    [0, 1, 2],
    [3, 4, 5],
    [6, 7, 8],
    [0, 3, 6],
    [1, 4, 7],
    [2, 5, 8],
    [0, 4, 8],
    [2, 4, 6],
  ];

  let roundWon = false;
  for (let i = 0; i < winningConditions.length; i++) {
    const [a, b, c] = winningConditions[i];
    if (
      gameBoard[a] &&
      gameBoard[a] === gameBoard[b] &&
      gameBoard[a] === gameBoard[c]
    ) {
      roundWon = true;
      break;
    }
  }

  if (roundWon) {
    setTimeout(() => alert(`Player ${currentPlayer} wins!`), 100);
    gameActive = false;
    return;
  }

  if (!gameBoard.includes("")) {
    setTimeout(() => alert("It's a draw!"), 100);
    gameActive = false;
  }
}

function createBoard() {
  gameContainer.innerHTML = "";
  gameBoard = ["", "", "", "", "", "", "", "", ""];
  gameActive = true;
  currentPlayer = "X";

  for (let i = 0; i < 9; i++) {
    const button = document.createElement("button");
    button.dataset.index = i;
    button.addEventListener("click", handleClick);
    gameContainer.appendChild(button);
  }
}

function resetGame() {
  createBoard();
}

createBoard();
resetButton.addEventListener("click", resetGame);
