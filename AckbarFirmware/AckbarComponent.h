#ifndef __ACKBAR_COMPONENT_H__
#define __ACKBAR_COMPONENT_H__

#include "AckbarConfiguration.h"

class AckbarComponent
{
  public:
    virtual void begin() = 0;
    virtual void calibrate() = 0;

    void setConfiguration(AckbarConfiguration * c)
    {
      configuration = c;
    };

  protected:
    AckbarConfiguration * configuration;
};


#endif // __ACKBAR_COMPONENT_H__
