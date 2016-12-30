/*
 * DigitalSignalProcessing.h
 *
 *  Created on: 2016-11-26
 *      Author: Noah
 */

#ifndef DIGITALSIGNALPROCESSING_H_
#define DIGITALSIGNALPROCESSING_H_

#define ACGain _IQ(1.0)
#define ACOffset _IQ(0.0)
#define DCGain _IQ(1.0)
#define DCOffset _IQ(0.0)
#define DeltaPhase _IQmpy(_IQ(0.05),_IQ(3.1415926535))// 10us / 20ms
#define ADCGND _IQ(1.5)

#define a1_1st _IQ(1.0)
#define a2_1st _IQ(1.0)
#define a3_1st _IQ(1.0)
#define b1_1st _IQ(1.0)
#define b2_1st _IQ(1.0)
#define b3_1st _IQ(1.0)

#define a1_2nd _IQ(1.0)
#define a2_2nd _IQ(1.0)
#define a3_2nd _IQ(1.0)
#define b1_2nd _IQ(1.0)
#define b2_2nd _IQ(1.0)
#define b3_2nd _IQ(1.0)

#define a1_3rd _IQ(1.0)
#define a2_3rd _IQ(1.0)
#define a3_3rd _IQ(1.0)
#define b1_3rd _IQ(1.0)
#define b2_3rd _IQ(1.0)
#define b3_3rd _IQ(1.0)

#endif /* DIGITALSIGNALPROCESSING_H_ */
