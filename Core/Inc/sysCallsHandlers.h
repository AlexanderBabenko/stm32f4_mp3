/*
 * sysCalls.h
 *
 *  Created on: 14 ��� 2014 �.
 *      Author: Sanya
 */

#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include <stdint.h>

//------------------------------------------------------------------------------
/*!
 * \brief  Cortex-M3 exceptions initialization
 * \param  none
 * \return none
 */
int32_t SysCallsHandlersInit(void);


#endif /* SYSCALLS_H_ */
