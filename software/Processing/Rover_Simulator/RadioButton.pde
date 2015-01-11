// Radio Button class. Creates a nice round radio button as well as an optional
// text label. Also customizable.
// TODO: finish comments
public class RadioButton {
	private boolean bPressed;
	private float fXPos, fYPos, fRadius;
	private color cStrokeColor = color(48, 48, 48);
	private color cFillColor = color(64, 64, 64);
	private color cHoverColor = color(128, 128, 128);

	RadioButton(float fXTemp, float fYTemp, float fRadiusTemp) {
		fXPos = fXTemp;
		fYPos = fYTemp;
		fRadius = fRadiusTemp;
	}

	RadioButton(float fXTemp, float fYTemp, float fRadiusTemp,
	color cStrokeColorTemp) {
		fXPos = fXTemp;
		fYPos = fYTemp;
		fRadius = fRadiusTemp;
		cStrokeColor = cStrokeColorTemp;
	}

	RadioButton(float fXTemp, float fYTemp, float fRadiusTemp,
	color cStrokeColorTemp,
	color cFillColorTemp) {
		fXPos = fXTemp;
		fYPos = fYTemp;
		fRadius = fRadiusTemp;
		cStrokeColor = cStrokeColorTemp;
		cFillColor = cFillColorTemp;
	}

	RadioButton(float fXTemp, float fYTemp, float fRadiusTemp,
	color cStrokeColorTemp,
	color cFillColorTemp,
	color cHoverColorTemp) {
		fXPos = fXTemp;
		fYPos = fYTemp;
		fRadius = fRadiusTemp;
		cStrokeColor = cStrokeColorTemp;
		cFillColor = cFillColorTemp;
		cHoverColor = cHoverColorTemp;
	}

	void draw() {
		if (over()) {
			fill(cHoverColor);
		} else if (bPressed){
			fill(cFillColor);
		} else {
			noFill();
		}
		strokeWeight(1);
		stroke(cStrokeColor);
		ellipseMode(RADIUS);
		ellipse(fXPos, fYPos, fRadius, fRadius);
	}

	void draw(String sText) {
		if (over()) {
			fill(cHoverColor);
		} else if (bPressed){
			fill(cFillColor);
		} else {
			noFill();
		}
		strokeWeight(1);
		stroke(cStrokeColor);
		ellipseMode(RADIUS);
		ellipse(fXPos, fYPos, fRadius, fRadius);

		fill(cFillColor);
		text(sText, fXPos + fRadius + 10, fYPos - 2);
	}

	void set(boolean bState) {
		bPressed = bState;
	}

	void toggle() {
		bPressed = !bPressed;
	}

	boolean over() {
		if (sqrt(sq(mouseX - fXPos) + sq(mouseY - fYPos)) > fRadius) {
			return false;
		} else {
			return true;
		}
	}

	boolean pressed() {
		return bPressed;
	}
}
