/*
 * CtrlUnit.h
 *
 *  Created on: 2016-11-26
 *      Author: Noah
 */

#ifndef CTRLUNIT_H_
#define CTRLUNIT_H_

#define ACGain _IQ30(0.001953125)   // Number in the parenthesis take A/ACcurrent as units
#define ACOffset _IQ26(0.0) // Number in the parenthesis take Ampere as units
#define DCGain _IQ30(0.015625)      // Number in the parenthesis take V/DCvoltage as units
#define DCOffset _IQ22(0.0) // Number in the parenthesis take Voltage as units

#define DeltaPhase 536871   // 10us / 20ms * 2^30

void Processing(int16 newACcurrent, int16 newDCvoltage);

#endif /* CTRLUNIT_H_ */
