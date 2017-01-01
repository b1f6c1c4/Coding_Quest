/*
 * CtrlUnit.h
 *
 *  Created on: 2016-11-26
 *      Author: Noah
 */

#ifndef CTRLUNIT_H_
#define CTRLUNIT_H_

#define DeltaPhase 536871   // 10us / 20ms * 2^30

void Processing(int16 newACcurrent, int16 newDCvoltage);

#endif /* CTRLUNIT_H_ */
