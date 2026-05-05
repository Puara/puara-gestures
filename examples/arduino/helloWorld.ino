// simple script to test CI setup 

void setup() {
  // Initialize serial communication at 9600 bits per second
  Serial.begin(9600); 
}

void loop() {
  // Print "Hello World!" to the Serial Monitor
  Serial.println("Hello World!"); 
  // Wait for 1 second (1000 milliseconds) before repeating
  delay(1000); 
}