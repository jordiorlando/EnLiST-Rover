// Radio Button class. Creates a nice round radio button as well as an optional
// text label. Also customizable.
// TODO: finish comments
public class RadioButton {
  private boolean isPressed;
  private float xPos, yPos, radius;

  private color strokeColor = color(48, 48, 48);
  private color fillColor   = color(64, 64, 64);
  private color hoverColor  = color(128, 128, 128);

  RadioButton(float x, float y, float r) {
    xPos = x;
    yPos = y;
    radius = r;
  }

  RadioButton(float x, float y, float r,
  color s) {
    xPos = x;
    yPos = y;
    radius = r;
    strokeColor = s;
  }

  RadioButton(float x, float y, float r,
  color s,
  color f) {
    xPos = x;
    yPos = y;
    radius = r;
    strokeColor = s;
    fillColor = f;
  }

  RadioButton(float x, float y, float r,
  color s,
  color f,
  color h) {
    xPos = x;
    yPos = y;
    radius = r;
    strokeColor = s;
    fillColor = f;
    hoverColor = h;
  }

  void draw() {
    if (over()) {
      fill(hoverColor);
    } else if (isPressed) {
      fill(fillColor);
    } else {
      noFill();
    }
    strokeWeight(1);
    stroke(strokeColor);
    ellipseMode(RADIUS);
    ellipse(xPos, yPos, radius, radius);
  }

  void draw(String t) {
    if (over()) {
      fill(hoverColor);
    } else if (isPressed) {
      fill(fillColor);
    } else {
      noFill();
    }
    strokeWeight(1);
    stroke(strokeColor);
    ellipseMode(RADIUS);
    ellipse(xPos, yPos, radius, radius);

    fill(fillColor);
    textSize(radius * 3);
    text(t, xPos + radius + 10, yPos - 2);
  }

  void set(boolean state) {
    isPressed = state;
  }

  void toggle() {
    isPressed = !isPressed;
  }

  boolean over() {
    if (sqrt(sq(mouseX - xPos) + sq(mouseY - yPos)) > radius + 2) {
      return false;
    } else {
      return true;
    }
  }

  boolean pressed() {
    return isPressed;
  }
}
