/*
 * MIT License
 * Copyright (c) 2019, 2018 - present OMRON Corporation
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/* includes */
#include <Wire.h>

/* defines */
#define D6T_ADDR 0x0A  // for I2C 7bit address
#define D6T_CMD 0x4C  // for D6T-44L-06/06H, D6T-8L-09/09H, for D6T-1A-01/02

#define N_ROW 16
#define N_PIXEL (4 * 4)
#define N_READ ((N_PIXEL + 1) * 2 + 1)

#define SAMPLE_TIME_0045MS	45
#define SAMPLE_TIME_0090MS	90
#define SAMPLE_TIME_0130MS	130
#define SAMPLE_TIME_0175MS	175
#define SAMPLE_TIME_0215MS	215
#define SAMPLE_TIME_0260MS	260
#define SAMPLE_TIME_0300MS	300
#define SAMPLE_TIME_0345MS	345
#define SAMPLE_TIME_0390MS	390
#define SAMPLE_TIME_0430MS	430
#define SAMPLE_TIME_0475MS	475
#define SAMPLE_TIME_0515MS	515
#define SAMPLE_TIME_0560MS	560
#define SAMPLE_TIME_0600MS	600
#define SAMPLE_TIME_0645MS	645
#define SAMPLE_TIME_0690MS	690
#define SAMPLE_TIME_0730MS	730
#define SAMPLE_TIME_0775MS	775
#define SAMPLE_TIME_0815MS	815
#define SAMPLE_TIME_0860MS	860
#define SAMPLE_TIME_0900MS	900
#define SAMPLE_TIME_0945MS	945
#define SAMPLE_TIME_0990MS	990
#define SAMPLE_TIME_1030MS	1030
#define SAMPLE_TIME_1070MS	1070
#define SAMPLE_TIME_1115MS	1115
#define SAMPLE_TIME_1160MS	1160
#define SAMPLE_TIME_1200MS	1200
#define SAMPLE_TIME_1245MS	1245
#define SAMPLE_TIME_1290MS	1290
#define SAMPLE_TIME_1330MS	1330
#define SAMPLE_TIME_1370MS	1370

#define PARA_0045MS	((uint8_t)0x01)
#define PARA_0090MS	((uint8_t)0x02)
#define PARA_0130MS	((uint8_t)0x03)
#define PARA_0175MS	((uint8_t)0x04)
#define PARA_0215MS	((uint8_t)0x05)
#define PARA_0260MS	((uint8_t)0x06)
#define PARA_0300MS	((uint8_t)0x07)
#define PARA_0345MS	((uint8_t)0x08)
#define PARA_0390MS	((uint8_t)0x09)
#define PARA_0430MS	((uint8_t)0x0A)
#define PARA_0475MS	((uint8_t)0x0B)
#define PARA_0515MS	((uint8_t)0x0C)
#define PARA_0560MS	((uint8_t)0x0D)
#define PARA_0600MS	((uint8_t)0x0E)
#define PARA_0645MS	((uint8_t)0x0F)
#define PARA_0690MS	((uint8_t)0x10)
#define PARA_0730MS	((uint8_t)0x11)
#define PARA_0775MS	((uint8_t)0x12)
#define PARA_0815MS	((uint8_t)0x13)
#define PARA_0860MS	((uint8_t)0x14)
#define PARA_0900MS	((uint8_t)0x15)
#define PARA_0945MS	((uint8_t)0x16)
#define PARA_0990MS	((uint8_t)0x17)
#define PARA_1030MS	((uint8_t)0x18)
#define PARA_1070MS	((uint8_t)0x19)
#define PARA_1115MS	((uint8_t)0x1A)
#define PARA_1160MS	((uint8_t)0x1B)
#define PARA_1200MS	((uint8_t)0x1C)
#define PARA_1245MS	((uint8_t)0x1D)
#define PARA_1290MS	((uint8_t)0x1E)
#define PARA_1330MS	((uint8_t)0x1F)
#define PARA_1370MS	((uint8_t)0x20)

/***** Setting Parameter *****/
#define comparingNumInc 5  // x300 ms   (range: 1 to 39)   (example) 5 -> 1.5 sec
#define comparingNumDec 5  // x300 ms   (range: 1 to 39)   (example) 5 -> 1.5 sec
#define threshHoldInc 10 //  /10 degC   (example) 10 -> 1.0 degC
#define threshHoldDec 10 //  /10 degC   (example) 10 -> 1.0 degC
bool  enablePix[16] = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true};
/****************************/

/***** Setting Parameter 2 *****/
#define samplingTime SAMPLE_TIME_0300MS //ms (Can select only, 45ms, 90ms, 130ms, 175ms, 215ms, 260ms, 300ms, 435ms, 390ms, 430ms, 475ms, 515ms, 560ms, 600ms, 645ms, 690ms, 730ms, 775ms, 815ms, 860ms, 900ms, 945ms, 990ms, 1030ms, 1070ms, 1115ms, 1160ms, 1200ms, 1245ms, 1290ms, 1330ms, 1370ms)
/****************************/

uint8_t rbuf[N_READ];
int16_t pix_data[16] = {0};
int16_t seqData[16][40] = {0};
bool  occuPix[16] = {0};
bool  occuPixFlag = false;
uint8_t  resultOccupancy = 0;
uint16_t  totalCount = 0;

/** JUDGE_occupancy: judge occupancy*/
bool judge_seatOccupancy(void) { 
  int i = 0;
  int j = 0; 
  for (i = 0; i < 16; i++){
    for (j = 0; j < 39; j++){
      seqData[i][39 - j] = seqData[i][38 - j];
    }
    seqData[i][0] = pix_data[i];            
  }
  if (totalCount <= comparingNumInc){
    totalCount++;
  }
  if (totalCount > comparingNumInc){
    for (i = 0; i < 16; i++){
      if (enablePix[i] == true){
        if (occuPix[i] == false){
          if ((int16_t)(seqData[i][0] - seqData[i][comparingNumInc]) >= (int16_t)threshHoldInc){
            occuPix[i] = true;
          }
        }
        else{   
          if ((int16_t)(seqData[i][comparingNumDec] - seqData[i][0]) >= (int16_t)threshHoldDec){
            occuPix[i] = false;
          }
        }
      }
    }
    if (resultOccupancy == 0) {
      for (i = 0; i < 16; i++){                   
        if(occuPix[i] == true){
          resultOccupancy = 1;
          break;
        }
      }
    }
    else{  //resultOccupancy == true
      occuPixFlag = false;
      for (i = 0; i < 16; i++){
        if (occuPix[i] == true){
          occuPixFlag = true;
          break;
        }
        else{                            
        }
      }
      if (occuPixFlag == false){
        resultOccupancy = 0;
      }
    }
  }
  return true;
}

uint8_t calc_crc(uint8_t data) {
    int index;
    uint8_t temp;
    for (index = 0; index < 8; index++) {
        temp = data;
        data <<= 1;
        if (temp & 0x80) {data ^= 0x07;}
    }
    return data;
}

/** <!-- D6T_checkPEC {{{ 1--> D6T PEC(Packet Error Check) calculation.
 * calculate the data sequence,
 * from an I2C Read client address (8bit) to thermal data end.
 */
bool D6T_checkPEC(uint8_t buf[], int n) {
    int i;
    uint8_t crc = calc_crc((D6T_ADDR << 1) | 1);  // I2C Read address (8bit)
    for (i = 0; i < n; i++) {
        crc = calc_crc(buf[i] ^ crc);
    }
    bool ret = crc != buf[n];
    if (ret) {
        Serial.print("PEC check failed:");
        Serial.print(crc, HEX);
        Serial.print("(cal) vs ");
        Serial.print(buf[n], HEX);
        Serial.println("(get)");
    }
    return ret;
}


/** <!-- conv8us_s16_le {{{1 --> convert a 16bit data from the byte stream.
 */
int16_t conv8us_s16_le(uint8_t* buf, int n) {
    int ret;
    ret = buf[n];
    ret += buf[n + 1] << 8;
    return (int16_t)ret;   // and convert negative.
}


/** <!-- setup {{{1 -->
 * 1. initialize a Serial port for output.
 * 2. initialize an I2C peripheral.
 */
void setup() {
	uint8_t para = 0;
	switch(samplingTime){
		case SAMPLE_TIME_0045MS:
			para = PARA_0045MS;
			break;
		case SAMPLE_TIME_0090MS:
			para = PARA_0090MS;
			break;
		case SAMPLE_TIME_0130MS:
			para = PARA_0130MS;
			break;
		case SAMPLE_TIME_0175MS:
			para = PARA_0175MS;
			break;
		case SAMPLE_TIME_0215MS:
			para = PARA_0215MS;
			break;
		case SAMPLE_TIME_0260MS:
			para = PARA_0260MS;
			break;
		case SAMPLE_TIME_0300MS:
			para = PARA_0300MS;
			break;
		case SAMPLE_TIME_0345MS:
			para = PARA_0345MS;
			break;
		case SAMPLE_TIME_0390MS:
			para = PARA_0390MS;
			break;
		case SAMPLE_TIME_0430MS:
			para = PARA_0430MS;
			break;
		case SAMPLE_TIME_0475MS:
			para = PARA_0475MS;
			break;
		case SAMPLE_TIME_0515MS:
			para = PARA_0515MS;
			break;
		case SAMPLE_TIME_0560MS:
			para = PARA_0560MS;
			break;
		case SAMPLE_TIME_0600MS:
			para = PARA_0600MS;
			break;
		case SAMPLE_TIME_0645MS:
			para = PARA_0645MS;
			break;
		case SAMPLE_TIME_0690MS:
			para = PARA_0690MS;
			break;
		case SAMPLE_TIME_0730MS:
			para = PARA_0730MS;
			break;
		case SAMPLE_TIME_0775MS:
			para = PARA_0775MS;
			break;
		case SAMPLE_TIME_0815MS:
			para = PARA_0815MS;
			break;
		case SAMPLE_TIME_0860MS:
			para = PARA_0860MS;
			break;
		case SAMPLE_TIME_0900MS:
			para = PARA_0900MS;
			break;
		case SAMPLE_TIME_0945MS:
			para = PARA_0945MS;
			break;
		case SAMPLE_TIME_0990MS:
			para = PARA_0990MS;
			break;
		case SAMPLE_TIME_1030MS:
			para = PARA_1030MS;
			break;
		case SAMPLE_TIME_1070MS:
			para = PARA_1070MS;
			break;
		case SAMPLE_TIME_1115MS:
			para = PARA_1115MS;
			break;
		case SAMPLE_TIME_1160MS:
			para = PARA_1160MS;
			break;
		case SAMPLE_TIME_1200MS:
			para = PARA_1200MS;
			break;
		case SAMPLE_TIME_1245MS:
			para = PARA_0945MS;
			break;
		case SAMPLE_TIME_1290MS:
			para = PARA_1290MS;
			break;
		case SAMPLE_TIME_1330MS:
			para = PARA_1330MS;
			break;
		case SAMPLE_TIME_1370MS:
			para = PARA_1370MS;
		default:
			para = PARA_0300MS;
			break;
	}	
	
    Serial.begin(115200);  // Serial baudrate = 115200bps
    Wire.begin();  // i2c master
	
    Wire.beginTransmission(D6T_ADDR);  // I2C client address
    Wire.write(0x50);                  // D6T register
    Wire.write(0x52);                  // D6T register
    Wire.write(0x45);                  // D6T register
    Wire.write(0x4C);                  // D6T register
    Wire.write(0x45);                  // D6T register
    Wire.write(0x41);                  // D6T register
    Wire.write(0x53);                  // D6T register
    Wire.write(0x45);                  // D6T register
    Wire.write(0x00);                  // D6T register
    Wire.endTransmission();            // I2C repeated start for read
    Wire.beginTransmission(D6T_ADDR);  // I2C client address
    Wire.write(0x42);                  // D6T register
    Wire.write(para);                  // D6T register
    Wire.write(0x00);                  // D6T register
    Wire.endTransmission();            // I2C repeated start for read
    Wire.beginTransmission(D6T_ADDR);  // I2C client address
    Wire.write(0x50);                  // D6T register
    Wire.write(0x00);                  // D6T register
    Wire.write(0x00);                  // D6T register
    Wire.write(0x00);                  // D6T register
    Wire.write(0x00);                  // D6T register
    Wire.write(0x00);                  // D6T register
    Wire.write(0x00);                  // D6T register
    Wire.write(0x00);                  // D6T register
    Wire.write(0x00);                  // D6T register
    Wire.endTransmission();            // I2C repeated start for read
}


/** <!-- loop - Thermal sensor {{{1 -->
 * 1. read sensor.
 * 2. output results, format is: [degC]
 */
void loop() {
    int i, j;
  int jj;

    memset(rbuf, 0, N_READ);
    // Wire buffers are enough to read D6T-16L data (33bytes) with
    // MKR-WiFi1010 and Feather ESP32,
    // these have 256 and 128 buffers in their libraries.
    Wire.beginTransmission(D6T_ADDR);  // I2C client address
    Wire.write(D6T_CMD);               // D6T register
    Wire.endTransmission();            // I2C repeated start for read
    delay(1);
    Wire.requestFrom(D6T_ADDR, N_READ);
    i = 0;
    while (Wire.available()) {
        rbuf[i++] = Wire.read();
    }

    if (D6T_checkPEC(rbuf, N_READ - 1)) {
        return;
    }

    // 1st data is PTAT measurement (: Proportional To Absolute Temperature)
    int16_t itemp = conv8us_s16_le(rbuf, 0);
    Serial.print("PTAT:");
    Serial.print(itemp / 10.0, 1);
    Serial.print(", Temperature:");

    // loop temperature pixels of each thrmopiles measurements
    for (i = 0, j = 2; i < N_PIXEL; i++, j += 2) {
        itemp = conv8us_s16_le(rbuf, j);
        pix_data[i] = itemp;
        Serial.print(itemp / 10.0, 1);  // print PTAT & Temperature
    /**********debug ***********************/
    //Serial.print(","); 
    //if(occuPix[i] == false){
    //  jj = 0;
    //}
    //else{
    //  jj = 1;
    //}
    //Serial.print(jj, 1); 
        /*************************************/
    if ((i % N_ROW) == N_ROW - 1) {
            Serial.print(" [degC]");  // wrap text at ROW end.
        } else {
            Serial.print(", ");   // print delimiter
        }
    }
    judge_seatOccupancy(); //add
    Serial.print(", Occupancy:");
    Serial.println(resultOccupancy, 1);
    delay(samplingTime);
}
// vi: ft=arduino:fdm=marker:et:sw=4:tw=80
