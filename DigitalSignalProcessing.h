/*
 * DigitalSignalProcessing.h
 *
 *  Created on: 2016-11-26
 *      Author: Noah
 */

#ifndef DIGITALSIGNALPROCESSING_H_
#define DIGITALSIGNALPROCESSING_H_

#define ACGain _IQ30(0.001953125)   // Number in the parenthesis take A/ACcurrent as units
#define ACOffset _IQ26(0.0) // Number in the parenthesis take Ampere as units
#define DCGain _IQ30(0.015625)      // Number in the parenthesis take V/DCvoltage as units
#define DCOffset _IQ22(0.0) // Number in the parenthesis take Voltage as units

#define DeltaPhase 536871   // 10us / 20ms * 2^30

#define a1_1st _IQ27(1.0)
#define a2_1st _IQ27(1.0)
#define a3_1st _IQ27(1.0)
#define b1_1st _IQ27(1.0)
#define b2_1st _IQ27(1.0)
#define b3_1st _IQ27(1.0)

#define a1_2nd _IQ27(1.0)
#define a2_2nd _IQ27(1.0)
#define a3_2nd _IQ27(1.0)
#define b1_2nd _IQ27(1.0)
#define b2_2nd _IQ27(1.0)
#define b3_2nd _IQ27(1.0)

#define a1_3rd _IQ27(1.0)
#define a2_3rd _IQ27(1.0)
#define a3_3rd _IQ27(1.0)
#define b1_3rd _IQ27(1.0)
#define b2_3rd _IQ27(1.0)
#define b3_3rd _IQ27(1.0)

#endif /* DIGITALSIGNALPROCESSING_H_ */
