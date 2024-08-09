document.addEventListener("DOMContentLoaded", () => {
  const loaders = document.querySelectorAll(".loader, .loader1");
  const safeZone = {
    top: 30,
    bottom: 70,
    left: 20,
    right: 80,
  };

  const eyeSize = 10;
  const placedLoaders = [];

  loaders.forEach((loader) => {
    let randomX, randomY;
    let overlap;

    do {
      overlap = false;
      randomX = Math.random() * 100;
      randomY = Math.random() * 100;

      if (
        randomX >= safeZone.left &&
        randomX <= safeZone.right &&
        randomY >= safeZone.top &&
        randomY <= safeZone.bottom
      ) {
        overlap = true;
      }

      placedLoaders.forEach((placed) => {
        const dx = randomX - placed.x;
        const dy = randomY - placed.y;

        const distance = Math.sqrt(dx * dx + dy * dy);

        if (distance < eyeSize) {
          overlap = true;
        }
      });
    } while (overlap);

    loader.style.position = "absolute";
    loader.style.top = `${randomY}%`;
    loader.style.left = `${randomX}%`;
    placedLoaders.push({ x: randomX, y: randomY });
  });
});
