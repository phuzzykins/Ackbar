#ifndef __ACKBAR_MECHANISM_H__
#define __ACKBAR_MECHANISM_H__

#include "AckbarComponent.h"

class AckbarMechanism : public AckbarComponent
{
  public:
    virtual void activate() = 0;
    virtual void reset() = 0;
};


#endif // __ACKBAR_Mechanism_H__
