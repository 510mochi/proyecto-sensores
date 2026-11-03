
int pirsensor = 0;

void setup()
{
  pinMode(13, OUTPUT); // Control del módulo relé
  pinMode(2, INPUT);   // Sensor PIR
}

void loop()
{
  pirsensor = digitalRead(2);

  if (pirsensor == HIGH)
  {
    digitalWrite(13, HIGH); // Encender foco
  }
  else
  {
    digitalWrite(13, LOW);  // Apagar foco
  }

  delay(10);
}
