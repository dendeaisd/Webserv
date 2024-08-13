function typeWriterEffect(element, text, typingSpeed = 30, initialDelay = 0) {
  let i = 0;
  element.innerHTML = "";

  setTimeout(() => {
    const intervalId = setInterval(() => {
      i++;
      element.innerHTML = text.slice(0, i) + "|";

      if (i === text.length) {
        clearInterval(intervalId);
        element.innerHTML = text;
      }
    }, typingSpeed);
  }, initialDelay);
}

const targetElement = document.getElementsByTagName("div")[0];
const textToType = targetElement.innerHTML.toString();

typeWriterEffect(targetElement, textToType);
