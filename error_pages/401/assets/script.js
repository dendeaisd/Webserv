document.addEventListener("DOMContentLoaded", () => {
  const loaders = document.querySelectorAll(".loader, .loader1");
  const safeZone = {
      top: 30,
      bottom: 70,
      left: 20,
      right: 80
  };+

  loaders.forEach(loader => {
      let randomX, randomY;

      do {
          randomX = Math.random() * 100;
          randomY = Math.random() * 100;
      } while (
          randomX >= safeZone.left && randomX <= safeZone.right &&
          randomY >= safeZone.top && randomY <= safeZone.bottom
      );

      loader.style.position = 'absolute';
      loader.style.top = `${randomY}%`;
      loader.style.left = `${randomX}%`;
  });
});
