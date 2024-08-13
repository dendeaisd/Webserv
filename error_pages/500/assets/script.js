
function ensureNoScroll() {
	document.body.style.overflow = 'hidden';
  }
  
  window.addEventListener('resize', ensureNoScroll);
  window.addEventListener('load', ensureNoScroll);
  
  