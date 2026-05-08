// Puara Gestures - includeEigen example
// Shows the cross-platform Eigen include helper and a basic matrix operation.

#include <Arduino.h>
#include <puara/utils/includeEigen.h>

void setup() {
  Serial.begin(115200);

  Eigen::Matrix3f mat = Eigen::Matrix3f::Identity();

  Serial.println("3x3 Identity matrix (via puara/utils/includeEigen.h):");
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) {
      Serial.print(mat(r, c));
      Serial.print(" ");
    }
    Serial.println();
  }

  // Simple dot product
  Eigen::Vector3f a(1.0f, 0.0f, 0.0f);
  Eigen::Vector3f b(0.0f, 1.0f, 0.0f);
  Serial.print("Dot product of (1,0,0) and (0,1,0): ");
  Serial.println(a.dot(b));
}

void loop() {}
