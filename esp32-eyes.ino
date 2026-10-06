#include "Face.h"

Face *face;

const int BUTTON_PIN = 10;

int currentExpression = 0;

// Etat du bouton pour l'anti-rebond.
bool lastReading = HIGH;
bool stableButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 30;

void SetExpression(int index)
{
  switch (index)
  {
    case 0:  face->Expression.GoTo_Normal();      break;
    case 1:  face->Expression.GoTo_Angry();       break;
    case 2:  face->Expression.GoTo_Glee();        break;
    case 3:  face->Expression.GoTo_Happy();       break;
    case 4:  face->Expression.GoTo_Sad();         break;
    case 5:  face->Expression.GoTo_Worried();     break;
    case 6:  face->Expression.GoTo_Focused();     break;
    case 7:  face->Expression.GoTo_Annoyed();     break;
    case 8:  face->Expression.GoTo_Surprised();   break;
    case 9:  face->Expression.GoTo_Skeptic();     break;
    case 10: face->Expression.GoTo_Frustrated();  break;
    case 11: face->Expression.GoTo_Unimpressed(); break;
    case 12: face->Expression.GoTo_Sleepy();      break;
    case 13: face->Expression.GoTo_Suspicious();  break;
    case 14: face->Expression.GoTo_Squint();      break;
    case 15: face->Expression.GoTo_Furious();     break;
    case 16: face->Expression.GoTo_Scared();      break;
    case 17: face->Expression.GoTo_Awe();         break;
  }

  Serial.print("Expression = ");
  Serial.println(index);
}

void CheckButton()
{
  bool reading = digitalRead(BUTTON_PIN);

  // Le signal vient de changer : on redémarre le délai anti-rebond.
  if (reading != lastReading)
  {
    lastDebounceTime = millis();
    lastReading = reading;
  }

  // Si le signal est resté stable assez longtemps, on valide le changement.
  if ((millis() - lastDebounceTime) >= debounceDelay)
  {
    if (reading != stableButtonState)
    {
      stableButtonState = reading;

      // Avec INPUT_PULLUP : LOW = bouton appuyé.
      if (stableButtonState == LOW)
      {
        currentExpression++;

        if (currentExpression >= 18)
          currentExpression = 0;

        Serial.println("BOUTON GPIO10 !");
        SetExpression(currentExpression);
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);
  delay(100);

  // Bouton entre GPIO 10 et GND.
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lastReading = digitalRead(BUTTON_PIN);
  stableButtonState = lastReading;

  face = new Face(128, 128, 50);

  // Les expressions sont maintenant choisies par le bouton.
  // On garde les mouvements de regard et les clignements automatiques.
  face->RandomBehavior = false;
  face->RandomLook = true;
  face->RandomBlink = true;

  SetExpression(currentExpression);
}

void loop()
{
  CheckButton();

  face->Update();
  delay(5);
}
