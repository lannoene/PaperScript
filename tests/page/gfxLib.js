const ctx = document.getElementById("canvas1").getContext('2d');
const CANVAS_WIDTH = 1280;
const CANVAS_HEIGHT = 720;
const sunImg = new Image();
sunImg.src = "sun.webp";

const imgs = [new Image(), new Image(), new Image()];
imgs[0].src = "sun.webp";
imgs[1].src = "big_stand.png";
imgs[2].src = "tree.jpeg";

function g() {

}

let graphics = {
	draw: (id, x, y) => {
		ctx.drawImage(imgs[id], x, y);
	},
	reqNextFrame: () => {
		requestAnimationFrame(g);
	},
	clearScreen: () => {
		ctx.clearRect(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);
	}
}