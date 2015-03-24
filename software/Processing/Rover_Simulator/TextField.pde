// Text Field class. Creates a nice little text field for editable data display.
// Also customizable.
// TODO: finish comments
public class TextField {
	private boolean bPressed;
	private float fXPos, fYPos, fWidth;
	private color cStrokeColor = color(48, 48, 48);
	private color cFillColor = color(64, 64, 64);
	private color cHoverColor = color(48, 48, 48);

	TextField(float fXTemp, float fYTemp, float fWidthTemp) {
		fXPos = fXTemp;
		fYPos = fYTemp;
		fWidth = fWidthTemp;
	}

	TextField(float fXTemp, float fYTemp, float fWidthTemp,
	color cStrokeColorTemp) {
		fXPos = fXTemp;
		fYPos = fYTemp;
		fWidth = fWidthTemp;
		cStrokeColor = cStrokeColorTemp;
	}

	TextField(float fXTemp, float fYTemp, float fWidthTemp,
	color cStrokeColorTemp,
	color cFillColorTemp) {
		fXPos = fXTemp;
		fYPos = fYTemp;
		fWidth = fWidthTemp;
		cStrokeColor = cStrokeColorTemp;
		cFillColor = cFillColorTemp;
	}

	TextField(float fXTemp, float fYTemp, float fWidthTemp,
	color cStrokeColorTemp,
	color cFillColorTemp,
	color cHoverColorTemp) {
		fXPos = fXTemp;
		fYPos = fYTemp;
		fWidth = fWidthTemp;
		cStrokeColor = cStrokeColorTemp;
		cFillColor = cFillColorTemp;
		cHoverColor = cHoverColorTemp;
	}

	void draw() {
		if (bPressed) {
			stroke(cStrokeColor);
			strokeWeight(1);
		} else {
			noStroke();
		}

		fill(255, 255, 255);
		rectMode(CORNER);
		rect(fXPos - 2, fYPos - 10, fWidth + 4, 20);

		if (over()) {
			cursor(TEXT);
		} else {
			cursor(ARROW);
		}
	}

	void draw(String sText) {
		if (bPressed) {
			stroke(cStrokeColor);
			strokeWeight(1);
		} else {
			noStroke();
		}

		fill(255, 255, 255);
		rectMode(CORNER);
		rect(fXPos - 2, fYPos - 10, fWidth + 4, 20);

		if (over() || bPressed) {
			cursor(TEXT);
			fill(cHoverColor);
		} else {
			cursor(ARROW);
			fill(cFillColor);
		}

		text(sText, fXPos, fYPos);
	}

	void set(boolean bState) {
		bPressed = bState;
	}

	boolean over() {
		if ((mouseX > fXPos - 2) && (mouseX < fXPos + fWidth + 4) && (mouseY > fYPos - 10) && (mouseY < fYPos + 10)) {
			return true;
		} else {
			return false;
		}
	}

	boolean pressed() {
		return bPressed;
	}
}
