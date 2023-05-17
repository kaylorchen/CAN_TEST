//
// Created by Kaylor on 22-11-21.
//

#ifndef MOTORDRIVER_DRIVER_DRIVERCAN_H_
#define MOTORDRIVER_DRIVER_DRIVERCAN_H_

#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include <linux/can.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/can/raw.h>
#include "linux/can/error.h"
#include <unistd.h>
#include "thread"
#include "queue"
#include "mutex"


#define _DEBUG
#include "_debug.h"

// #define CAN_DEBUG

struct CanFrame{
  uint8_t data[8];
  uint8_t size;
  uint32_t can_id;
};


class DriverCan {
 public:
  DriverCan(const char *str);

  void CanFiltersConfig(const void *rfilter, int size);

  bool CanWrite(uint32_t can_id, const uint8_t *data, uint8_t size);

  void CanRead(uint32_t *can_id, uint8_t *data, uint8_t *size);

  bool GetCanFrame(struct CanFrame &can_frame);

  bool DropOneCanFrame(void);

  void ClearReadQueue(void);

  uint16_t GetReadQueueSize(void);

 private:
  int can_fd_;
  char device_string_[32];
  std::queue<struct CanFrame> read_queue_;
  std::mutex mutex_;

  static void Callback(void *ptr);
};

#endif //MOTORDRIVER_DRIVER_DRIVERCAN_H_
