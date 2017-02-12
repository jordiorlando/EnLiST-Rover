// Text Field class. Creates a nice little text field for editable data display.
// Also customizable.
// TODO: finish comments
public class TextField {
  private boolean isPressed;
  private float xPos, yPos, _width = 20;
  private color strokeColor = color(48, 48, 48);
  private color textColor = color(48, 48, 48);
  private color hoverColor = color(64, 64, 64);

  TextField() {
  }

  TextField(color s) {
    strokeColor = s;
  }

  TextField(color s, color t) {
    strokeColor = s;
    textColor = t;
  }

  TextField(color s, color t, color h) {
    strokeColor = s;
    textColor = t;
    hoverColor = h;
  }

  void draw() {
    if (over() || isPressed) {
      cursor(TEXT);
      fill(255, 255, 255);
    } else {
      cursor(ARROW);
      fill(222, 222, 222);
    }

    noStroke();
    rectMode(CORNER);
    rect(xPos - 2, yPos - 6, 24, 16, 2);
  }

  void draw(String t) {
    _width = textWidth(t);

    if (over() || isPressed) {
      cursor(TEXT);
      fill(255, 255, 255);
    } else {
      cursor(ARROW);
      fill(222, 222, 222);
    }

    noStroke();
    rectMode(CORNER);
    rect(xPos - 2, yPos - 6, _width + 4, 16, 2);

    fill(textColor);
    text(t, xPos, yPos);
  }

  void draw(String t, float x, float y) {
    xPos = x;
    yPos = y;
    _width = textWidth(t);

    if (over() || isPressed) {
      cursor(TEXT);
      fill(255, 255, 255);
    } else {
      cursor(ARROW);
      fill(222, 222, 222);
    }

    noStroke();
    rectMode(CORNER);
    rect(xPos - 2, yPos - 6, _width + 4, 16, 2);

    fill(textColor);
    text(t, xPos, yPos);
  }

  void set(boolean state) {
    isPressed = state;
  }

  boolean over() {
    if ((mouseX > xPos - 2) && (mouseX < xPos + _width + 4) &&
        (mouseY > yPos - 6) && (mouseY < yPos + 10)) {
      return true;
    } else {
      return false;
    }
  }

  boolean pressed() {
    return isPressed;
  }
}
