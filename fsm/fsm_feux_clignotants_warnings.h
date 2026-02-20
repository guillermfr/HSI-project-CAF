/**
 * \file        fifo.h
 * \author      Alexis Daley
 * \version     1.1
 * \date        02 february 2021
 * \brief       This files allow a user to create and manage a fifo buffer (circular buffer).
 * \details     This file gives the interfaces to be able to create and manage the fifo buffer.
 *              Concerning threads, the fifo is designed for one producer one consumer.
 *
 *              It implements following functions :
 *                  init : to init the fifo buffer
 *                  push : insert in the buffer
 *                  read : get fist data from the buffer
 *                  next : go to the next value, and read the new value
 */

#ifndef FSM_CW_H_
#define FSM_CW_H_

#include "../lib/output/types.h"
#include "../lib/output/donnees.h"

static int callback_initialisation(void);

static int callback_enter_actives_allumes(void);

static int callback_enter_actives_eteints(void);

static int callback_enter_acquittes_allume(void);

static int callback_enter_acquittes_eteint(void);

static int callback_eteindre_feux(void);

static int callback_erreur(void);

#endif /* FIFO_H_ */