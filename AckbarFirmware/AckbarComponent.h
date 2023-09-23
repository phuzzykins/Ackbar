#ifndef __ACKBAR_COMPONENT_H__
#define __ACKBAR_COMPONENT_H__

#include "AckbarConfiguration.h"

class AckbarComponent
{
  public:
    virtual void begin();
    virtual void calibrate();
    void setConfiguration(AckbarConfiguration * c)
    {
      configuration = c;
    };

  protected:
    AckbarConfiguration * configuration;
};


#endif // __ACKBAR_COMPONENT_H__
