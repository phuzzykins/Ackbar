#ifndef __ACKBAR_STATE_H__
#define __ACKBAR_STATE_H__

enum AckbarState
{
  RESET,
  ERROR,
  STARTUP,
  CALIBRATING,
  ARMING,
  ARMED,
  ACTIVE
};
  
#endif // __ACKBAR_STATE_H__