//****************************************************************************//
// Puara Module Manager -  high-level gestural descriptors (cpp)              //
// Metalab - Société des Arts Technologiques (SAT)                            //
// Input Devices and Music Interaction Laboratory (IDMIL), McGill University  //
// Edu Meneses (2022) - https://www.edumeneses.com                            //
//****************************************************************************//


#include "puara_gestures.h"

void PuaraGestures::updateInertialGestures() {
  updateJabShake();
  updateOrientation();
}

void PuaraGestures::updateJabShake() {
  std::deque<float>::iterator minX = std::min_element(gyroBuffers[0].begin(), gyroBuffers[0].end());
  std::deque<float>::iterator maxX = std::max_element(gyroBuffers[0].begin(), gyroBuffers[0].end());
  std::deque<float>::iterator minY = std::min_element(gyroBuffers[1].begin(), gyroBuffers[1].end());
  std::deque<float>::iterator maxY = std::max_element(gyroBuffers[1].begin(), gyroBuffers[1].end());
  std::deque<float>::iterator minZ = std::min_element(gyroBuffers[2].begin(), gyroBuffers[2].end());
  std::deque<float>::iterator maxZ = std::max_element(gyroBuffers[2].begin(), gyroBuffers[2].end());

  float gyroAbsX = std::abs(gyroBuffers[0].back());
  float gyroAbsY = std::abs(gyroBuffers[1].back());
  float gyroAbsZ = std::abs(gyroBuffers[2].back());
    
  // Instrument shake
  if (gyroAbsX > 0.1) {
    PuaraGestures::shakeX = leakyIntegrator(gyroAbsX/10, PuaraGestures::shakeX, 0.6, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerX);
  } else {
    PuaraGestures::shakeX = leakyIntegrator(0, PuaraGestures::shakeX, 0.3, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerX);
    if (PuaraGestures::shakeX < 0.01) {
        PuaraGestures::shakeX = 0;
      }
  }
  if (gyroAbsY > 0.1) {
    PuaraGestures::shakeY = leakyIntegrator(gyroAbsY/10, PuaraGestures::shakeY, 0.6, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerY);
  } else {
    PuaraGestures::shakeY = leakyIntegrator(0, PuaraGestures::shakeY, 0.3, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerY);
    if (PuaraGestures::shakeY < 0.01) {
        PuaraGestures::shakeY = 0;
      }
  }
  if (gyroAbsZ > 0.1) {
    PuaraGestures::shakeZ = leakyIntegrator(gyroAbsZ/10, PuaraGestures::shakeZ, 0.6, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerZ);
  } else {
    PuaraGestures::shakeZ = leakyIntegrator(0, PuaraGestures::shakeZ, 0.3, PuaraGestures::leakyShakeFreq, PuaraGestures::leakyShakeTimerZ);
    if (PuaraGestures::shakeZ < 0.01) {
        PuaraGestures::shakeZ = 0;
      }
  }
  // Instrument jab
  if (*maxX-*minX > PuaraGestures::jabThreshold) {
    if (*maxX >= 0 && *minX >= 0) {
      PuaraGestures::jabX = *maxX - *minX;
    } else if (*maxX < 0 && *minX < 0) {
      PuaraGestures::jabX = *minX - *maxX;
    } else {
    PuaraGestures::jabX = 0;
    }
  }
  if (*maxY-*minY > 10) {
    PuaraGestures::jabY = *maxY - *minY;
  } else {
    PuaraGestures::jabY = 0;
  }
  if (*maxZ-*minZ > 10) {
    PuaraGestures::jabZ = *maxZ - *minZ;
  } else {
    PuaraGestures::jabZ = 0;
  }
}

void PuaraGestures::updateOrientation() {
  orientation = ImuOrientation::getOrientation(orientation, accelAxes, gyroAxes, magAxes, 1.0);
}

void PuaraGestures::updateAccel(float accelX, float accelY, float accelZ) {
  accelBuffers[0].push_back(accelX);
  accelBuffers[1].push_back(accelY);
  accelBuffers[2].push_back(accelZ);
  if (accelBuffers[0].size() > PuaraGestures::BUFFER_SIZE) {
    accelBuffers[0].pop_front();
    accelBuffers[1].pop_front();
    accelBuffers[2].pop_front();
  }
  accelAxes.x = accelX;
  accelAxes.y = accelY;
  accelAxes.z = accelZ;
}

void PuaraGestures::updateGyro(float gyroX, float gyroY, float gyroZ) {        
  gyroBuffers[0].push_back(gyroX);
  gyroBuffers[1].push_back(gyroY);
  gyroBuffers[2].push_back(gyroZ);
  if (gyroBuffers[0].size() > PuaraGestures::BUFFER_SIZE) {
    gyroBuffers[0].pop_front();
    gyroBuffers[1].pop_front();
    gyroBuffers[2].pop_front();
  }
  gyroAxes.x = gyroX;
  gyroAxes.y = gyroY;
  gyroAxes.z = gyroZ;
}

void PuaraGestures::updateMag(float magX, float magY, float magZ) {
  magBuffers[0].push_back(magX);
  magBuffers[1].push_back(magY);
  magBuffers[2].push_back(magZ);
  if (magBuffers[0].size() > PuaraGestures::BUFFER_SIZE) {
    magBuffers[0].pop_front();
    magBuffers[1].pop_front();
    magBuffers[2].pop_front();
  }
  magAxes.x = magX;
  magAxes.y = magY;
  magAxes.z = magZ;
}

// Simple leaky integrator implementation
// Create a unsigned long global variable for time counter for each leak implementation (timer)
float PuaraGestures::leakyIntegrator (float reading, float old_value, float leak, int frequency, unsigned long& timer) {
  
  float new_value;
  if (frequency <= 0) {
    new_value = reading + (old_value * leak);
  } else if ((esp_timer_get_time()/1000LL)  - (1000 / frequency) < timer) {  
    new_value = reading + old_value;
  } else {
    new_value = reading + (old_value * leak);
    timer = (esp_timer_get_time()/1000LL);
  }
  return new_value;
}

float PuaraGestures::getAccelX() {
  return accelAxes.x;
};

float PuaraGestures::getAccelY() {
  return accelAxes.y;
};

float PuaraGestures::getAccelZ() {
  return accelAxes.z;
};

float PuaraGestures::getGyroX() {
  return gyroAxes.x;
};

float PuaraGestures::getGyroY() {
  return gyroAxes.y;
};

float PuaraGestures::getGyroZ() {
  return gyroAxes.z;
};

float PuaraGestures::getMagX() {
  return magAxes.x;
};

float PuaraGestures::getMagY() {
  return magAxes.y;
};

float PuaraGestures::getMagZ() {
  return magAxes.z;
};

float PuaraGestures::getYaw() {
  ImuOrientation::Quaternion o = orientation;
  float yaw = atan2(2.0f * (o.w * o.x + o.y * o.z), o.w * o.w - o.x * o.x - o.y * o.y + o.z * o.z);
  yaw *= 180.0f / M_PI;
  return yaw;
}

float PuaraGestures::getPitch() {
  ImuOrientation::Quaternion o = orientation;
  float pitch = -asin(2.0f * (o.x * o.z - o.w * o.y));
  pitch *= 180.0f / M_PI;
  return pitch;
}

float PuaraGestures::getRoll() {
  ImuOrientation::Quaternion o = orientation;
  float roll = atan2(2.0f * (o.x * o.y + o.w * o.z), o.w * o.w + o.x * o.x - o.y * o.y - o.z * o.z);
  roll *= 180.0f / M_PI;
  roll += DECLINATION;
  return roll;
}

float PuaraGestures::getShakeX() {
  return shakeX;
};

float PuaraGestures::getShakeY() {
  return shakeY;
};

float PuaraGestures::getShakeZ() {
  return shakeZ;
};

float PuaraGestures::getJabX() {
  return jabX;
};

float PuaraGestures::getJabY() {
  return jabY;
};

float PuaraGestures::getJabZ() {
  return jabZ;
};

double PuaraGestures::getOrientationW() {
  return orientation.w;
}

double PuaraGestures::getOrientationX() {
  return orientation.x;
}

double PuaraGestures::getOrientationY() {
  return orientation.y;
}

double PuaraGestures::getOrientationZ() {
  return orientation.z;
}

/* Expects an array of discrete touch values (int, 0 or 1) and
 * the size of the array
 */ 
void PuaraGestures::updateTouchArray(int *discrete_touch, int touchSize) { // raw_touch
    
    // touchAll: get the "amount of touch" for the entire touch sensor
    // normalized between 0 and 1
    touchAll = touchAverage(discrete_touch, 0, touchSize);

    // touchTop: get the "amount of touch" for the top part of the capsense
    // normalized between 0 and 1
    touchTop = touchAverage(discrete_touch, 0, touchSizeEdge);

    // touchMiddle: get the "amount of touch" for the central part of the capsense
    // normalized between 0 and 1
    touchMiddle = touchAverage(discrete_touch, (0+touchSizeEdge), (touchSize - touchSizeEdge));

    // touchBottom: get the "amount of touch" for the botton part of the capsense
    // normalized between 0 and 1
    touchBottom = touchAverage(discrete_touch, (touchSize - touchSizeEdge), touchSize);

    // Save last blob detection state before reading new data
    for (int i=0; i < (sizeof(blobPos)/sizeof(blobPos[0])); ++i) {
        lastState_blobPos[i] = blobPos[i];
    }

    // 1D blob detection: used for brush
    blobDetection1D(discrete_touch,touchSize);

    // brush: direction and intensity of capsense brush motion
    // rub: intensity of rub motion
    // in ~cm/s (distance between stripes = ~1.5cm)
    for (int i=0; i < (sizeof(blobPos)/sizeof(blobPos[0])); ++i) { 
        float movement = blobPos[i] - lastState_blobPos[i]; 
        if ( blobPos[i] == -1 ) {
            multiBrush[i] = 0;
            multiRub[i] = 0;
            brushCounter[i] = 0;
        } else if (movement == 0) {
            if (brushCounter[i] < 10) {
                brushCounter[i]++;
                // wait some time before dropping the rub/brush values
            } else if (multiBrush[i] < 0.001) {
                multiBrush[i] = 0;
                multiRub[i] = 0;
            } else {
                multiBrush[i] = leakyIntegrator(movement*0.15, multiBrush[i], 0.7, leakyBrushFreq, leakyBrushTimer);
                multiRub[i] = leakyIntegrator(abs(movement*0.15), multiRub[i], 0.7, leakyRubFreq, leakyRubTimer);
            }
        } else if ( abs(movement) > 1 ) {
            multiBrush[i] = leakyIntegrator(0, multiBrush[i], 0.6, leakyBrushFreq, leakyBrushTimer);
        } else {
            multiBrush[i] = leakyIntegrator(movement*0.15, multiBrush[i], 0.8, leakyBrushFreq, leakyBrushTimer);
            multiRub[i] = leakyIntegrator(abs(movement)*0.15, multiRub[i], 0.99, leakyRubFreq, leakyRubTimer);
            brushCounter[i] = 0;
        }
    }
    brush =  arrayAverageZero(multiBrush,4);
    rub = arrayAverageZero(multiRub,4);
}

float PuaraGestures::touchAverage (float * touchArrayStrips, int firstStrip, int lastStrip) {
    int sum = 0;
    for (int i = firstStrip; i < lastStrip-1; ++i)
      sum += touchArrayStrips[i];
      
    return  ((float) sum) / (lastStrip - firstStrip);
}

float PuaraGestures::touchAverage (int * touchArrayStrips, int firstStrip, int lastStrip) {
    int sum = 0;
    for (int i = firstStrip; i < lastStrip; i++)
      sum += (float)touchArrayStrips[i];
      
    return  ((float) sum) / (lastStrip - firstStrip);
}

void PuaraGestures::blobDetection1D (int *discrete_touch, int touchSize) {
    blobAmount = 0;
    int sizeCounter = 0;
    int stripe = 0;
    for (int i=0; i<4; i++) {
        blobCenter[i] = 0;
        blobPos[i] = 0;
        blobSize[i] = 0;
    }

    for ( ; stripe<touchSize; stripe++) {
        if (blobAmount < maxBlobs) {
            if (discrete_touch[stripe] == 1) { // check for beggining of blob...
                sizeCounter = 1;
                blobPos[blobAmount] = stripe;
                while (discrete_touch[stripe+sizeCounter] == 1) { // then keep checking for end
                    sizeCounter++;
                }
                blobSize[blobAmount] = sizeCounter;
                blobCenter[blobAmount] = stripe + (sizeCounter / 2);
                stripe += sizeCounter + 1; // skip stripes already read
                blobAmount++;
            }
        }
    }
}

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

float PuaraGestures::arrayAverageZero (float * Array, int ArraySize) {
    float sum = 0;
    int count = 0;
    float output = 0;
    for (int i = 0; i < ArraySize; ++i) {
      if (Array[i] != 0) {
        sum += Array[i];
        count++;
      }
    }
    if (count > 0) {
    output = sum / count; 
    }
      
    return output;
}

void PuaraGestures::bitShiftArrayL (int * origArray, int * shiftedArray, int arraySize, int shift) {

  for (int i=0; i < arraySize; ++i) {
      shiftedArray[i] = origArray[i];
  }

  for (int k=0; k < shift; ++k) {
      for (int i=0; i < arraySize; ++i) {
          if ( i == (arraySize-1)) {
              shiftedArray[i] = (shiftedArray[i] << 1);
          }
          else {
              shiftedArray[i] = (shiftedArray[i] << 1) | (shiftedArray[i+1] >> 7);
          }
      }
  }
}

void PuaraGestures::updateButton(int buttonValue) {
  long currentTime = esp_timer_get_time()/1000LL;
    PuaraGestures::buttonValue = buttonValue;
    if (buttonValue < PuaraGestures::buttonThreshold) {
        if (!PuaraGestures::buttonPress) {
            PuaraGestures::buttonPress = true;
            PuaraGestures::buttonTimer = currentTime;
        }
        if (currentTime - PuaraGestures::buttonTimer > PuaraGestures::buttonHoldInterval) {
            PuaraGestures::buttonHold = true;
        }
    }
    else if (PuaraGestures::buttonHold) {
        PuaraGestures::buttonHold = false;
        PuaraGestures::buttonPress = false;
        PuaraGestures::buttonCount = 0;
    }
    else {
        if (PuaraGestures::buttonPress) {
            PuaraGestures::buttonPress = false;
            PuaraGestures::buttonPressTime = currentTime - PuaraGestures::buttonTimer;
            PuaraGestures::buttonTimer = currentTime;
            PuaraGestures::buttonCount++;
        }
    }
    if (!PuaraGestures::buttonPress && (currentTime - PuaraGestures::buttonTimer > PuaraGestures::buttonCountInterval)) {
        switch (PuaraGestures::buttonCount) {
            case 0:
                PuaraGestures::buttonTap = 0;
                PuaraGestures::buttonDtap = 0;
                PuaraGestures::buttonTtap = 0;
                break;
            case 1: 
                PuaraGestures::buttonTap = 1;
                PuaraGestures::buttonDtap = 0;
                PuaraGestures::buttonTtap = 0;
                break;
            case 2:
                PuaraGestures::buttonTap = 0;
                PuaraGestures::buttonDtap = 1;
                PuaraGestures::buttonTtap = 0;
                break;
            case 3:
                PuaraGestures::buttonTap = 0;
                PuaraGestures::buttonDtap = 0;
                PuaraGestures::buttonTtap = 1;
                break;
        }
        PuaraGestures::buttonCount = 0;
    }
}

void PuaraGestures::updateTrigButton(int buttonValue) {
  long currentTime = esp_timer_get_time()/1000LL;
    PuaraGestures::buttonValue = buttonValue;
    if (buttonValue >= PuaraGestures::buttonThreshold) {
        if (!PuaraGestures::buttonPress) {
            PuaraGestures::buttonPress = true;
            PuaraGestures::buttonTimer = currentTime;
        }
        if (currentTime - PuaraGestures::buttonTimer > PuaraGestures::buttonHoldInterval) {
            PuaraGestures::buttonHold = true;
        }
    }
    else if (PuaraGestures::buttonHold) {
        PuaraGestures::buttonHold = false;
        PuaraGestures::buttonPress = false;
        PuaraGestures::buttonCount = 0;
    }
    else {
        if (PuaraGestures::buttonPress) {
            PuaraGestures::buttonPress = false;
            PuaraGestures::buttonPressTime = currentTime - PuaraGestures::buttonTimer;
            PuaraGestures::buttonTimer = currentTime;
            PuaraGestures::buttonCount++;
        }
    }
    if (!PuaraGestures::buttonPress && (currentTime - PuaraGestures::buttonTimer > PuaraGestures::buttonCountInterval)) {
        switch (PuaraGestures::buttonCount) {
            case 0:
                PuaraGestures::buttonTap = 0;
                PuaraGestures::buttonDtap = 0;
                PuaraGestures::buttonTtap = 0;
                break;
            case 1: 
                PuaraGestures::buttonTap = 1;
                PuaraGestures::buttonDtap = 0;
                PuaraGestures::buttonTtap = 0;
                break;
            case 2:
                PuaraGestures::buttonTap = 0;
                PuaraGestures::buttonDtap = 1;
                PuaraGestures::buttonTtap = 0;
                break;
            case 3:
                PuaraGestures::buttonTap = 0;
                PuaraGestures::buttonDtap = 0;
                PuaraGestures::buttonTtap = 1;
                break;
        }
        PuaraGestures::buttonCount = 0;
    }
}

unsigned int PuaraGestures::getButtonCount() {
    return PuaraGestures::buttonCount;
}

bool PuaraGestures::getButtonTouch() {
    return PuaraGestures::buttonPress;
}

unsigned int PuaraGestures::getButtonValue() {
    return PuaraGestures::buttonValue;
}

unsigned int PuaraGestures::getButtonTap() {
    return PuaraGestures::buttonTap;
}

unsigned int PuaraGestures::getButtonDTap() {
    return PuaraGestures::buttonDtap;
}

unsigned int PuaraGestures::getButtonTTap() {
    return PuaraGestures::buttonTtap;
}

unsigned int PuaraGestures::getButtonThreshold() {
    return PuaraGestures::buttonValue;
}

unsigned int PuaraGestures::setButtonThreshold(int value) {
    PuaraGestures::buttonThreshold = value;
    return 1;
}

unsigned int PuaraGestures::getButtonPressTime() {
    return PuaraGestures::buttonPressTime;
}

bool PuaraGestures::getButtonHold() {
    return PuaraGestures::buttonHold;
}

unsigned int PuaraGestures::getButtonHoldInterval() {
    return PuaraGestures::buttonHoldInterval;
}

unsigned int PuaraGestures::setButtonHoldInterval(int value) {
    PuaraGestures::buttonHoldInterval = value;
    return 1;
}
