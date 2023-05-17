#include <iostream>
#include "driver_can.h"

int main(int argc, char **argv) {
  DriverCan driver_can("can1");
  uint32_t can_id;
  std::cout << "Pls input can_id = ";
  std::cin >> std::hex >> can_id;
  uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8};
  while (true) {
    sleep(1);
    driver_can.CanWrite(can_id, data, 8);
    data[0]++;
  }
  return 0;
}
