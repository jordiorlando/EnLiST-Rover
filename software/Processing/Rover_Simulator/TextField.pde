// Text Field class. Creates a nice little text field for editable data display.
// Also customizable.
// TODO: finish comments
public class TextField {
	private boolean bPressed;
	private float fXPos, fYPos, fWidth = 20;
	private color cStrokeColor = color(48, 48, 48);
	private color cTextColor = color(48, 48, 48);
	private color cHoverColor = color(64, 64, 64);

	TextField() {
	}

	TextField(color cStrokeColorTemp) {
		cStrokeColor = cStrokeColorTemp;
	}

	TextField(color cStrokeColorTemp, color cTextColorTemp) {
		cStrokeColor = cStrokeColorTemp;
		cTextColor = cTextColorTemp;
	}

	TextField(color cStrokeColorTemp, color cTextColorTemp, color cHoverColorTemp) {
		cStrokeColor = cStrokeColorTemp;
		cTextColor = cTextColorTemp;
		cHoverColor = cHoverColorTemp;
	}

	void draw() {
		if (over() || bPressed) {
			cursor(TEXT);
			fill(255, 255, 255);
		} else {
			cursor(ARROW);
			fill(222, 222, 222);
		}

		noStroke();
		rectMode(CORNER);
		rect(fXPos - 2, fYPos - 6, 24, 16, 2);
	}

	void draw(String sText) {
		fWidth = textWidth(sText);

		if (over() || bPressed) {
			cursor(TEXT);
			fill(255, 255, 255);
		} else {
			cursor(ARROW);
			fill(222, 222, 222);
		}

		noStroke();
		rectMode(CORNER);
		rect(fXPos - 2, fYPos - 6, fWidth + 4, 16, 2);

		fill(cTextColor);
		text(sText, fXPos, fYPos);
	}

	void draw(String sText, float fXTemp, float fYTemp) {
		fXPos = fXTemp;
		fYPos = fYTemp;
		fWidth = textWidth(sText);

		if (over() || bPressed) {
			cursor(TEXT);
			fill(255, 255, 255);
		} else {
			cursor(ARROW);
			fill(222, 222, 222);
		}

		noStroke();
		rectMode(CORNER);
		rect(fXPos - 2, fYPos - 6, fWidth + 4, 16, 2);

		fill(cTextColor);
		text(sText, fXPos, fYPos);
	}

	void set(boolean bState) {
		bPressed = bState;
	}

	boolean over() {
		if ((mouseX > fXPos - 2) && (mouseX < fXPos + fWidth + 4) && (mouseY > fYPos - 6) && (mouseY < fYPos + 10)) {
			return true;
		} else {
			return false;
		}
	}

	boolean pressed() {
		return bPressed;
	}
}
