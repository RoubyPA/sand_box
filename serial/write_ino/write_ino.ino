/* Example for spy.c
 * Date:     2017
 * Arduino:  Nano ATmega328
 *
 * Write data on serial port (USB) 
 */

void 
setup()
{
  Serial.begin(9600);
  while (!Serial);
    // Wait serial connection
}

void 
loop()
{
  for (int i = 0 ; i < 300 ; i++)
    {
      Serial.print("<");
      Serial.print(i);
      Serial.print(">\t");
      Serial.print("Serial log");
      Serial.print("\n");
      delay(150);
    }
  Serial.println("quit");
  delay(1000);
}
