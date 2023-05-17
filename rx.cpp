//
// Created by kaylor on 5/17/23.
//
#include "driver_can.h"
#include "iostream"
#include "stdio.h"

int main(int argc, char **argv) {
  DriverCan driver_can("can0");
  uint32_t filter_id;
  std::cout << "Pls input filter id = ";
  std::cin >> std::hex >> filter_id;
  struct can_filter can_filter[1];
  can_filter[0].can_id = filter_id;
  can_filter[0].can_mask = CAN_SFF_MASK;
  driver_can.CanFiltersConfig(can_filter, sizeof(can_filter));
  driver_can.ClearReadQueue();
  while (true) {
    struct CanFrame frame;
    if (driver_can.GetCanFrame(frame)) {
      printf("can id is %X, and data[0] = %X\n", frame.can_id, frame.data[0]);
//      std::cout << "can id is " <<  frame.can_id << std::endl;
//      printf("can id is %X, and data[0] = %d", frame.can_id, frame.data[0]);
    }
//    printf("test\n");
    sleep(1);
  }
  return 0;
}