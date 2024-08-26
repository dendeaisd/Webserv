document
  .getElementById("code-input")
  .addEventListener("keydown", function (event) {
    if (event.key === "Enter") {
      event.preventDefault();
      document.getElementById("submit-button").click();
    }
  });

document.getElementById("submit-button").addEventListener("click", function () {
  const codeInput = document.getElementById("code-input").value;
  const errorMessage = document.getElementById("error-message");

  let message = "";

  switch (codeInput) {
    case "0000":
      message = "Oh, you're trying to outsmart me with all zeros? Not today!";
      break;
    case "1234":
      message = "1234? Classic move! But this door won't budge.";
      break;
    case "1337":
      message = "Leet skills, huh? But this vault remains uncracked.";
      break;
    case "8008":
      message = "Haha, very funny. But no entry!";
      break;
    case "0403": // The correct code
      unlockVault();
      break;
    default:
      message = "Oops! That's not the right code. Try again!";
  }

  errorMessage.textContent = message;
  errorMessage.classList.remove("hidden");

  setTimeout(() => {
    errorMessage.classList.add("hidden");
  }, 4000);
});

function unlockVault() {
  alert(
    "You entered the correct code! However..."
  );
  window.location.href = "../../403_success/success.html"
}
