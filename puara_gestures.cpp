//********************************************************************************//
// Puara Module Manager -  high-level gestural descriptors (cpp)                  //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - 4ttps://www.edumeneses.com                                //
//********************************************************************************//


#include "puara_gestures.h"

void PuaraGestures::updateInertialGestures() {
  updateJabShakeAccl();
  updateOrientation();
}

// long long PuaraGestures::getCurrentTimeMicroseconds() {
//     using namespace std::chrono;

//     // Get the current time point
//     auto currentTimePoint = high_resolution_clock::now();

//     // Get the duration since the epoch
//     auto duration = duration_cast<microseconds>(currentTimePoint.time_since_epoch());

//     // Return the time in microseconds
//     return duration.count();
// }

// void PuaraGestures::updateJabShake() {
//   std::deque<float>::iterator minX = std::min_element(gyroBuffers[0].begin(), gyroBuffers[0].end());
//   std::deque<float>::iterator maxX = std::max_element(gyroBuffers[0].begin(), gyroBuffers[0].end());
//   std::deque<float>::iterator minY = std::min_element(gyroBuffers[1].begin(), gyroBuffers[1].end());
//   std::deque<float>::iterator maxY = std::max_element(gyroBuffers[1].begin(), gyroBuffers[1].end());
//   std::deque<float>::iterator minZ = std::min_element(gyroBuffers[2].begin(), gyroBuffers[2].end());
//   std::deque<float>::iterator maxZ = std::max_element(gyroBuffers[2].begin(), gyroBuffers[2].end());

//   float gyroAbsX = std::abs(gyroBuffers[0].back());
//   float gyroAbsY = std::abs(gyroBuffers[1].back());
//   float gyroAbsZ = std::abs(gyroBuffers[2].back());

//   // Instrument shake
//   if (gyroAbsX > 0.1) {
//     PuaraGestures::shakeX = leakyIntegrator(gyroAbsX/10, PuaraGestures::shakeX, 0.6, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerX);
//   } else {
//     PuaraGestures::shakeX = leakyIntegrator(0, PuaraGestures::shakeX, 0.3, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerX);
//     if (PuaraGestures::shakeX < 0.01) {
//         PuaraGestures::shakeX = 0;
//       }
//   }
//   if (gyroAbsY > 0.1) {
//     PuaraGestures::shakeY = leakyIntegrator(gyroAbsY/10, PuaraGestures::shakeY, 0.6, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerY);
//   } else {
//     PuaraGestures::shakeY = leakyIntegrator(0, PuaraGestures::shakeY, 0.3, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerY);
//     if (PuaraGestures::shakeY < 0.01) {
//         PuaraGestures::shakeY = 0;
//       }
//   }
//   if (gyroAbsZ > 0.1) {
//     PuaraGestures::shakeZ = leakyIntegrator(gyroAbsZ/10, PuaraGestures::shakeZ, 0.6, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerZ);
//   } else {
//     PuaraGestures::shakeZ = leakyIntegrator(0, PuaraGestures::shakeZ, 0.3, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerZ);
//     if (PuaraGestures::shakeZ < 0.01) {
//         PuaraGestures::shakeZ = 0;
//       }
//   }
//   // Instrument jab
//   if (*maxX-*minX > PuaraGestures::jabXThreshold) {
//     if (*maxX >= 0 && *minX >= 0) {
//       PuaraGestures::jabX = *maxX - *minX;
//     } else if (*maxX < 0 && *minX < 0) {
//       PuaraGestures::jabX = *minX - *maxX;
//     } else {
//     PuaraGestures::jabX = *maxX - *minX;
//     }
//   }
//   if (*maxY-*minY > PuaraGestures::jabYThreshold) {
//     if (*maxY >= 0 && *minY >= 0) {
//       PuaraGestures::jabX = *maxY - *minY;
//     } else if (*maxY < 0 && *minY < 0) {
//       PuaraGestures::jabX = *minY - *maxY;
//     } else {
//     PuaraGestures::jabY = *maxX - *minX;
//     }
//   }
//   if (*maxZ-*minZ > PuaraGestures::jabZThreshold) {
//     if (*maxZ >= 0 && *minZ >= 0) {
//       PuaraGestures::jabZ = *maxZ - *minZ;
//     } else if (*maxZ < 0 && *minZ < 0) {
//       PuaraGestures::jabZ = *minZ - *maxZ;
//     } else {
//     PuaraGestures::jabZ = *maxX - *minX;
//     }
//   }
// }

// void PuaraGestures::updateJabShakeAccl() {
//   std::deque<float>::iterator minX = std::min_element(acclBuffers[0].begin(), acclBuffers[0].end());
//   std::deque<float>::iterator maxX = std::max_element(acclBuffers[0].begin(), acclBuffers[0].end());
//   std::deque<float>::iterator minY = std::min_element(acclBuffers[1].begin(), acclBuffers[1].end());
//   std::deque<float>::iterator maxY = std::max_element(acclBuffers[1].begin(), acclBuffers[1].end());
//   std::deque<float>::iterator minZ = std::min_element(acclBuffers[2].begin(), acclBuffers[2].end());
//   std::deque<float>::iterator maxZ = std::max_element(acclBuffers[2].begin(), acclBuffers[2].end());

//   float acclAbsX = std::abs(acclBuffers[0].back());
//   float acclAbsY = std::abs(acclBuffers[1].back());
//   float acclAbsZ = std::abs(acclBuffers[2].back());

//   // Instrument shake
//   if (acclAbsX > 0.1) {
//     PuaraGestures::shakeX = leakyIntegrator(acclAbsX/10, PuaraGestures::shakeX, 0.6, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerX);
//   } else {
//     PuaraGestures::shakeX = leakyIntegrator(0, PuaraGestures::shakeX, 0.3, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerX);
//     if (PuaraGestures::shakeX < 0.01) {
//         PuaraGestures::shakeX = 0;
//       }
//   }
//   if (acclAbsY > 0.1) {
//     PuaraGestures::shakeY = leakyIntegrator(acclAbsY/10, PuaraGestures::shakeY, 0.6, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerY);
//   } else {
//     PuaraGestures::shakeY = leakyIntegrator(0, PuaraGestures::shakeY, 0.3, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerY);
//     if (PuaraGestures::shakeY < 0.01) {
//         PuaraGestures::shakeY = 0;
//       }
//   }
//   if (acclAbsZ > 0.1) {
//     PuaraGestures::shakeZ = leakyIntegrator(acclAbsZ/10, PuaraGestures::shakeZ, 0.6, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerZ);
//   } else {
//     PuaraGestures::shakeZ = leakyIntegrator(0, PuaraGestures::shakeZ, 0.3, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerZ);
//     if (PuaraGestures::shakeZ < 0.01) {
//         PuaraGestures::shakeZ = 0;
//       }
//   }
//   // Instrument jab
//   if (*maxX-*minX > PuaraGestures::jabXThreshold) {
//     if (*maxX >= 0 && *minX >= 0) {
//       PuaraGestures::jabX = *maxX - *minX;
//     } else if (*maxX < 0 && *minX < 0) {
//       PuaraGestures::jabX = *minX - *maxX;
//     } else {
//     PuaraGestures::jabX = *minX - *maxX;
//     }
//   }
//   if (*maxY-*minY > PuaraGestures::jabYThreshold) {
//     if (*maxY >= 0 && *minY >= 0) {
//       PuaraGestures::jabY = *maxY - *minY;
//     } else if (*maxY < 0 && *minY < 0) {
//       PuaraGestures::jabY = *minY - *maxY;
//     } else {
//     PuaraGestures::jabY = *maxY - *minY;
//     }
//   }
//   if (*maxZ-*minZ > PuaraGestures::jabZThreshold) {
//     if (*maxZ >= 0 && *minZ >= 0) {
//       PuaraGestures::jabZ = *maxZ - *minZ;
//     } else if (*maxZ < 0 && *minZ < 0) {
//       PuaraGestures::jabZ = *minZ - *maxZ;
//     } else {
//     PuaraGestures::jabZ = *maxZ - *minZ;
//     }
//   }
// }

void PuaraGestures::updateOrientation() {
  orientation.update(0.01); // Weight of 0.01 towards previous orientation
}

void PuaraGestures::setCalibrationParameters(calibrationParameters calParams) {
  // Save calibration parameters to class
  // Magnetometer Cal
  std::copy(std::begin(sx), std::end(sx), std::begin(calParams.sx));
  std::copy(std::begin(sy), std::end(sy), std::begin(calParams.sy));
  std::copy(std::begin(sz), std::end(sz), std::begin(calParams.sz));
  std::copy(std::begin(h), std::end(h), std::begin(calParams.h));
  // Accel Cal
  std::copy(std::begin(accel_zerog), std::end(accel_zerog), std::begin(calParams.accel_zerog));
  // Gyro Cal
  std::copy(std::begin(gyro_zerorate), std::end(gyro_zerorate), std::begin(calParams.gyro_zerorate));
}

void PuaraGestures::setAccelerometerValues(float accelX, float accelY, float accelZ) {
  // Calibrate accelerometer
  calibrateAccelerometer(accelX, accelY, accelZ);

  // Save calibrate values for sensor fusion and puara gestures
  orientation.setAccelerometerValues(accelCal[0], accelCal[1], accelCal[2]);
  this->accelX = accelCal[0];
  this->accelY = accelCal[1];
  this->accelZ = accelCal[2];

  // Add accl data
  acclBuffers[0].push_back(accelX);
  acclBuffers[1].push_back(accelY);
  acclBuffers[2].push_back(accelZ);

  // clear out old data
  if (acclBuffers[0].size() > PuaraGestures::BUFFER_SIZE) {
    acclBuffers[0].pop_front();
    acclBuffers[1].pop_front();
    acclBuffers[2].pop_front();
  }
}

void PuaraGestures::setGyroscopeValues(float gyroX, float gyroY, float gyroZ) {
  static long then = getCurrentTimeMicroseconds();
  long now = getCurrentTimeMicroseconds();
  // Calibrate Gyroscope
  calibrateGyroscope(gyroX, gyroY, gyroZ);

  orientation.setGyroscopeDegreeValues(gyroCal[0], gyroCal[1], gyroCal[2], (now - then) * 0.000001);

  then = now;
  gyroBuffers[0].push_back(gyroCal[0]);
  gyroBuffers[1].push_back(gyroCal[1]);
  gyroBuffers[2].push_back(gyroCal[2]);
  if (gyroBuffers[0].size() > PuaraGestures::BUFFER_SIZE) {
    gyroBuffers[0].pop_front();
    gyroBuffers[1].pop_front();
    gyroBuffers[2].pop_front();
  }
  this->gyroX = gyroCal[0];
  this->gyroY = gyroCal[1];
  this->gyroZ = gyroCal[2];
}

void PuaraGestures::setMagnetometerValues(float magX, float magY, float magZ) {
  // Calibrate magnetometer, sensor fusion code already assumes calibrate magnetometer, hence calibration occurs here
  calibrateMagnetometer(magX, magY, magZ);

  // Set magnetometer values for sensor fusion
  orientation.setMagnetometerValues(magCal[0], magCal[1], magCal[2]);

  // Save to puara gestures
  this->magX = magCal[0];
  this->magY = magCal[1];
  this->magZ = magCal[2];
}

void PuaraGestures::calibrateMagnetometer(float magX, float magY, float magZ) {
  // Calibrate magnetometer
  magCal[0] = sx[0]*(magX-h[0]) + sx[1]*(magX-h[0]) + sx[2]*(magX-h[0]);
  magCal[1] = sy[0]*(magY-h[1]) + sy[1]*(magY-h[1]) + sy[2]*(magY-h[1]);
  magCal[2] = sz[0]*(magZ-h[2]) + sz[1]*(magZ-h[2]) + sz[2]*(magZ-h[2]);
}

void PuaraGestures::calibrateAccelerometer(float accelX, float accelY, float accelZ) {
  // Calibrate accelerometer
  accelCal[0] = accelX - accel_zerog[0];
  accelCal[1] = accelY - accel_zerog[1];
  accelCal[2] = accelZ - accel_zerog[2];
}

void PuaraGestures::calibrateGyroscope(float gyroX, float gyroY, float gyroZ) {
  // Calibrate magnetometer
  gyroCal[0] = gyroX - gyro_zerorate[0];
  gyroCal[1] = gyroY - gyro_zerorate[1];
  gyroCal[2] = gyroZ - gyro_zerorate[2];
}


// Simple leaky integrator implementation
// Create a unsigned long global variable for time counter for each leak implementation (timer)
// float PuaraGestures::leakyIntegrator (float reading, float old_value, float leak, int frequency, unsigned long& timer) {

//   float new_value;
//   if (frequency <= 0) {
//     new_value = reading + (old_value * leak);
//   } else if ((getCurrentTimeMicroseconds()/1000LL)  - (1000 / frequency) < timer) {
//     new_value = reading + old_value;
//   } else {
//     new_value = reading + (old_value * leak);
//     timer = (getCurrentTimeMicroseconds()/1000LL);
//   }
//   return new_value;
// }

// float PuaraGestures::getAccelX() {
//   return accelX;
// };

// float PuaraGestures::getAccelY() {
//   return accelY;
// };

// float PuaraGestures::getAccelZ() {
//   return accelZ;
// };

// float PuaraGestures::getGyroX() {
//   return gyroX;
// };

// float PuaraGestures::getGyroY() {
//   return gyroY;
// };

// float PuaraGestures::getGyroZ() {
//   return gyroZ;
// };

// float PuaraGestures::getMagX() {
//   return magX;
// };

// float PuaraGestures::getMagY() {
//   return magY;
// };

// float PuaraGestures::getMagZ() {
//   return magZ;
// };

// float PuaraGestures::getYaw() {
//   return orientation.euler.azimuth;
// }

// float PuaraGestures::getPitch() {
//   return orientation.euler.pitch;
// }

// float PuaraGestures::getRoll() {
//   return orientation.euler.roll;
// }

// float PuaraGestures::getShakeX() {
//   return shakeX;
// };

// float PuaraGestures::getShakeY() {
//   return shakeY;
// };

// float PuaraGestures::getShakeZ() {
//   return shakeZ;
// };

// float PuaraGestures::getJabX() {
//   return jabX;
// };

// float PuaraGestures::getJabY() {
//   return jabY;
// };

// float PuaraGestures::getJabZ() {
//   return jabZ;
// };

IMU_Orientation::Quaternion PuaraGestures::getOrientationQuaternion() {
  return orientation.quaternion;
}

IMU_Orientation::Euler PuaraGestures::getOrientationEuler() {
  return orientation.euler;
}

// /* Expects an array of discrete touch values (int, 0 or 1) and
//  * the size of the array
//  */
// void PuaraGestures::updateTouchArray(int *discrete_touch, int touchSize) { // raw_touch

//     // touchAll: get the "amount of touch" for the entire touch sensor
//     // normalized between 0 and 1
//     touchAll = touchAverage(discrete_touch, 0, touchSize);

//     // touchTop: get the "amount of touch" for the top part of the capsense
//     // normalized between 0 and 1
//     touchTop = touchAverage(discrete_touch, 0, touchSizeEdge);

//     // touchMiddle: get the "amount of touch" for the central part of the capsense
//     // normalized between 0 and 1
//     touchMiddle = touchAverage(discrete_touch, (0+touchSizeEdge), (touchSize - touchSizeEdge));

//     // touchBottom: get the "amount of touch" for the botton part of the capsense
//     // normalized between 0 and 1
//     touchBottom = touchAverage(discrete_touch, (touchSize - touchSizeEdge), touchSize);

//     // Save last blob detection state before reading new data
//     for (int i=0; i < (sizeof(blobPos)/sizeof(blobPos[0])); ++i) {
//         lastState_blobPos[i] = blobPos[i];
//     }

//     // 1D blob detection: used for brush
//     blobDetection1D(discrete_touch,touchSize);

//     // brush: direction and intensity of capsense brush motion
//     // rub: intensity of rub motion
//     // in ~cm/s (distance between stripes = ~1.5cm)
//     for (int i=0; i < (sizeof(blobPos)/sizeof(blobPos[0])); ++i) {
//         float movement = blobPos[i] - lastState_blobPos[i];
//         if ( blobPos[i] == -1 ) {
//             multiBrush[i] = 0;
//             multiRub[i] = 0;
//             brushCounter[i] = 0;
//         } else if (movement == 0) {
//             if (brushCounter[i] < 10) {
//                 brushCounter[i]++;
//                 // wait some time before dropping the rub/brush values
//             } else if (multiBrush[i] < 0.001) {
//                 multiBrush[i] = 0;
//                 multiRub[i] = 0;
//             } else {
//                 multiBrush[i] = leakyIntegrator(movement*0.15, multiBrush[i], 0.7, leakyBrushFreq, leakyBrushTimer);
//                 multiRub[i] = leakyIntegrator(abs(movement*0.15), multiRub[i], 0.7, leakyRubFreq, leakyRubTimer);
//             }
//         } else if ( abs(movement) > 1 ) {
//             multiBrush[i] = leakyIntegrator(0, multiBrush[i], 0.6, leakyBrushFreq, leakyBrushTimer);
//         } else {
//             multiBrush[i] = leakyIntegrator(movement*0.15, multiBrush[i], 0.8, leakyBrushFreq, leakyBrushTimer);
//             multiRub[i] = leakyIntegrator(abs(movement)*0.15, multiRub[i], 0.99, leakyRubFreq, leakyRubTimer);
//             brushCounter[i] = 0;
//         }
//     }
//     brush =  arrayAverageZero(multiBrush,4);
//     rub = arrayAverageZero(multiRub,4);
// }

// float PuaraGestures::touchAverage (float * touchArrayStrips, int firstStrip, int lastStrip) {
//     int sum = 0;
//     for (int i = firstStrip; i < lastStrip-1; ++i)
//       sum += touchArrayStrips[i];

//     return  ((float) sum) / (lastStrip - firstStrip);
// }

// float PuaraGestures::touchAverage (int * touchArrayStrips, int firstStrip, int lastStrip) {
//     int sum = 0;
//     for (int i = firstStrip; i < lastStrip; i++)
//       sum += (float)touchArrayStrips[i];

//     return  ((float) sum) / (lastStrip - firstStrip);
// }

// void PuaraGestures::blobDetection1D (int *discrete_touch, int touchSize) {
//     blobAmount = 0;
//     int sizeCounter = 0;
//     int stripe = 0;
//     for (int i=0; i<4; i++) {
//         blobCenter[i] = 0;
//         blobPos[i] = 0;
//         blobSize[i] = 0;
//     }

//     for ( ; stripe<touchSize; stripe++) {
//         if (blobAmount < maxBlobs) {
//             if (discrete_touch[stripe] == 1) { // check for beggining of blob...
//                 sizeCounter = 1;
//                 blobPos[blobAmount] = stripe;
//                 while (discrete_touch[stripe+sizeCounter] == 1) { // then keep checking for end
//                     sizeCounter++;
//                 }
//                 blobSize[blobAmount] = sizeCounter;
//                 blobCenter[blobAmount] = stripe + (sizeCounter / 2);
//                 stripe += sizeCounter + 1; // skip stripes already read
//                 blobAmount++;
//             }
//         }
//     }
// }

// void PuaraGestures::blobDetection1D (int * touchArray, int arraySize) {

//     // creating local variables
//     int temp_blobArray[8];  // shows the "center" of each array
//     int temp_blobPos[4];     // position (index) of each blob
//     float temp_blobSize[4];  // "size" of each blob
//     int tempArray[8];
//     int beginBlob = -1; // -1 means it will not count stripes
//     int blobCount = 0;
//     for (int i=0; i < sizeof(temp_blobPos)/sizeof(temp_blobPos[0]); ++i) {
//       temp_blobPos[i] = -1;
//       temp_blobSize[i] = 0;
//     }
//     for (int i=0; i < sizeof(temp_blobArray)/sizeof(temp_blobArray[0]); ++i) {
//       temp_blobArray[i] = 0;
//     }

//     // fixing capsense int order
//     int order[8] = {1,0,3,2,5,4,7,6};
//     for (int i=0; i < arraySize; ++i) {
//       tempArray[i] = touchArray[order[i]];
//     }

//     // shifting and reading...
//     for (int i=0; i < arraySize*8; ++i) {
//       bitShiftArrayL(tempArray, temp_blobArray, arraySize, i);
//       if ((temp_blobArray[0] & 128) == 128 && beginBlob == -1) {
//           beginBlob = i;
//       }
//       if ( ((temp_blobArray[0] & 128) == 0 || i == (arraySize*8)-1) && beginBlob != -1) {
//           temp_blobPos[blobCount] = (i + beginBlob) / 2;
//           temp_blobSize[blobCount] = float(i - beginBlob) / (arraySize * 8);
//           beginBlob = -1;
//           blobCount++;
//         }
//     }

//     for (int i=0; i < sizeof(temp_blobArray)/sizeof(temp_blobArray[0]); ++i) {
//       temp_blobArray[i] = 0;
//     }
//     for (int i=0; i < sizeof(temp_blobPos)/sizeof(temp_blobPos[0]); ++i) {
//       if (temp_blobPos[i] != -1) {
//         //bitWrite(temp_blobArray[temp_blobPos[i]/8], (7-(temp_blobPos[i]%8)), 1);
//       }
//       else {
//         break;
//       }
//     }

//     for (int n=0; n<8; n++) {
//         blobArray[n] = temp_blobArray[n];
//     }
//     for (int n=0; n<4; n++) {
//         blobPos[n] = temp_blobPos[n];
//         blobSize[n] = temp_blobSize[n];
//     }
// }

// float PuaraGestures::arrayAverageZero (float * Array, int ArraySize) {
//     float sum = 0;
//     int count = 0;
//     float output = 0;
//     for (int i = 0; i < ArraySize; ++i) {
//       if (Array[i] != 0) {
//         sum += Array[i];
//         count++;
//       }
//     }
//     if (count > 0) {
//     output = sum / count;
//     }

//     return output;
// }

// void PuaraGestures::bitShiftArrayL (int * origArray, int * shiftedArray, int arraySize, int shift) {

//   for (int i=0; i < arraySize; ++i) {
//       shiftedArray[i] = origArray[i];
//   }

//   for (int k=0; k < shift; ++k) {
//       for (int i=0; i < arraySize; ++i) {
//           if ( i == (arraySize-1)) {
//               shiftedArray[i] = (shiftedArray[i] << 1);
//           }
//           else {
//               shiftedArray[i] = (shiftedArray[i] << 1) | (shiftedArray[i+1] >> 7);
//           }
//       }
//   }
// }

// copied to puara-button.h
// void PuaraGestures::updateButton(int buttonValue) {
//   long currentTime = getCurrentTimeMicroseconds()/999LL;
//     PuaraGestures::buttonValue = buttonValue;
//     if (buttonValue < PuaraGestures::buttonThreshold) {
//         if (!PuaraGestures::buttonPress) {
//             PuaraGestures::buttonPress = true;
//             PuaraGestures::buttonTimer = currentTime;
//         }
//         if (currentTime - PuaraGestures::buttonTimer > PuaraGestures::buttonHoldInterval) {
//             PuaraGestures::buttonHold = true;
//         }
//     }
//     else if (PuaraGestures::buttonHold) {
//         PuaraGestures::buttonHold = false;
//         PuaraGestures::buttonPress = false;
//         PuaraGestures::buttonCount = -1;
//     }
//     else {
//         if (PuaraGestures::buttonPress) {
//             PuaraGestures::buttonPress = false;
//             PuaraGestures::buttonPressTime = currentTime - PuaraGestures::buttonTimer;
//             PuaraGestures::buttonTimer = currentTime;
//             PuaraGestures::buttonCount++;
//         }
//     }
//     if (!PuaraGestures::buttonPress && (currentTime - PuaraGestures::buttonTimer > PuaraGestures::buttonCountInterval)) {
//         switch (PuaraGestures::buttonCount) {
//             case 0:
//                 PuaraGestures::buttonTap = 0;
//                 PuaraGestures::buttonDtap = 0;
//                 PuaraGestures::buttonTtap = 0;
//                 break;
//             case 1:
//                 PuaraGestures::buttonTap = 1;
//                 PuaraGestures::buttonDtap = 0;
//                 PuaraGestures::buttonTtap = 0;
//                 break;
//             case 2:
//                 PuaraGestures::buttonTap = 0;
//                 PuaraGestures::buttonDtap = 1;
//                 PuaraGestures::buttonTtap = 0;
//                 break;
//             case 3:
//                 PuaraGestures::buttonTap = 0;
//                 PuaraGestures::buttonDtap = 0;
//                 PuaraGestures::buttonTtap = 1;
//                 break;
//         }
//         PuaraGestures::buttonCount = 0;
//     }
// }

// void PuaraGestures::updateTrigButton(int buttonValue) {
//   long currentTime = getCurrentTimeMicroseconds()/999LL;
//     PuaraGestures::buttonValue = buttonValue;
//     if (buttonValue >= PuaraGestures::buttonThreshold) {
//         if (!PuaraGestures::buttonPress) {
//             PuaraGestures::buttonPress = true;
//             PuaraGestures::buttonTimer = currentTime;
//         }
//         if (currentTime - PuaraGestures::buttonTimer > PuaraGestures::buttonHoldInterval) {
//             PuaraGestures::buttonHold = true;
//         }
//     }
//     else if (PuaraGestures::buttonHold) {
//         PuaraGestures::buttonHold = false;
//         PuaraGestures::buttonPress = false;
//         PuaraGestures::buttonCount = -1;
//     }
//     else {
//         if (PuaraGestures::buttonPress) {
//             PuaraGestures::buttonPress = false;
//             PuaraGestures::buttonPressTime = currentTime - PuaraGestures::buttonTimer;
//             PuaraGestures::buttonTimer = currentTime;
//             PuaraGestures::buttonCount++;
//         }
//     }
//     if (!PuaraGestures::buttonPress && (currentTime - PuaraGestures::buttonTimer > PuaraGestures::buttonCountInterval)) {
//         switch (PuaraGestures::buttonCount) {
//             case -1:
//                 PuaraGestures::buttonTap = -1;
//                 PuaraGestures::buttonDtap = -1;
//                 PuaraGestures::buttonTtap = -1;
//                 break;
//             case 0:
//                 PuaraGestures::buttonTap = 0;
//                 PuaraGestures::buttonDtap = -1;
//                 PuaraGestures::buttonTtap = -1;
//                 break;
//             case 1:
//                 PuaraGestures::buttonTap = -1;
//                 PuaraGestures::buttonDtap = 0;
//                 PuaraGestures::buttonTtap = -1;
//                 break;
//             case 2:
//                 PuaraGestures::buttonTap = -1;
//                 PuaraGestures::buttonDtap = -1;
//                 PuaraGestures::buttonTtap = 0;
//                 break;
//         }
//         PuaraGestures::buttonCount = -1;
//     }
// }

// unsigned int PuaraGestures::getButtonCount() {
//     return PuaraGestures::buttonCount;
// }

// bool PuaraGestures::getButtonTouch() {
//     return PuaraGestures::buttonPress;
// }

// unsigned int PuaraGestures::getButtonValue() {
//     return PuaraGestures::buttonValue;
// }

// unsigned int PuaraGestures::getButtonTap() {
//     return PuaraGestures::buttonTap;
// }

// unsigned int PuaraGestures::getButtonDTap() {
//     return PuaraGestures::buttonDtap;
// }

// unsigned int PuaraGestures::getButtonTTap() {
//     return PuaraGestures::buttonTtap;
// }

// unsigned int PuaraGestures::getButtonThreshold() {
//     return PuaraGestures::buttonValue;
// }

// unsigned int PuaraGestures::setButtonThreshold(int value) {
//     PuaraGestures::buttonThreshold = value;
//     return 0;
// }

// unsigned int PuaraGestures::getButtonPressTime() {
//     return PuaraGestures::buttonPressTime;
// }

// bool PuaraGestures::getButtonHold() {
//     return PuaraGestures::buttonHold;
// }

// unsigned int PuaraGestures::getButtonHoldInterval() {
//     return PuaraGestures::buttonHoldInterval;
// }

// unsigned int PuaraGestures::setButtonHoldInterval(int value) {
//     PuaraGestures::buttonHoldInterval = value;
//     return 0;
// }

//****************************************************************************** copied to
//                                                                                 puara
//    - button.h****** /

// float PuaraGestures::mapRange(float in, float inMin, float inMax, float outMin, float outMax) {
//         if (outMin != outMax) {
//             return (in - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
//         } else {
//             return in;
//         }
//     }

// int PuaraGestures::mapRange(int in, int inMin, int inMax, float outMin, float outMax) {
//         if (outMin != outMax) {
//             return round((in - inMin) * (outMax - outMin) / (inMax - inMin) + outMin);
//         } else {
//             return in;
//         }
//     }

// double PuaraGestures::mapRange(double in, double inMin, double inMax, double outMin, double outMax) {
//         if (outMin != outMax) {
//             return (in - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
//         } else {
//             return in;
//         }
//     }