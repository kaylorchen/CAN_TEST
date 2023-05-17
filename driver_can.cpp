//
// Created by Kaylor on 22-11-21.
//

#include "driver_can.h"

void DriverCan::Callback(void *ptr) {
  DriverCan *driver_can = (DriverCan *)ptr;
  while (true) {
    struct CanFrame can_frame;
    driver_can->CanRead(&can_frame.can_id, &can_frame.data[0], &can_frame.size);
    // for (size_t i = 0; i < 4; i++)
    // {
    //   printf("%02X ", can_frame.data[i]);
    // }
    // printf("\n");

    driver_can->mutex_.lock();
    driver_can->read_queue_.push(can_frame);
    driver_can->mutex_.unlock();
    //        printf("can id = %X\r\n", can_frame.can_id);
  }
}

DriverCan::DriverCan(const char *str) {
  memset(device_string_, 0, sizeof(device_string_));
  strcpy(device_string_, str);
  _debug("device is %s\r\n", device_string_);
  can_fd_ = socket(AF_CAN, SOCK_RAW, CAN_RAW);
  if (can_fd_ < 0) {
    perror("socket can create error!\n");
    return;
  }
  struct ifreq ifr;
  strcpy(ifr.ifr_name, str);
  ioctl(can_fd_, SIOCGIFINDEX, &ifr);

  struct sockaddr_can addr;
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  /*关闭回环模式*/
  int loopback = 0; /* 0 = disabled, 1 = enabled (default) */
  setsockopt(can_fd_, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback,
             sizeof(loopback));

  /*关闭自收自发*/
  int recv_own_msgs = 0; /* 0 = disabled (default), 1 = enabled */
  setsockopt(can_fd_, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &recv_own_msgs,
             sizeof(recv_own_msgs));

  // 将套接字与 can_fd_ 绑定
  int bind_res = bind(can_fd_, (struct sockaddr *)&addr, sizeof(addr));
  if (bind_res < 0) {
    perror("bind error!");
    return;
  }
  std::thread t(DriverCan::Callback, this);
  t.detach();
}

void DriverCan::CanFiltersConfig(const void *rfilter, int size) {
  setsockopt(can_fd_, SOL_CAN_RAW, CAN_RAW_FILTER, rfilter, size);
}

bool DriverCan::CanWrite(uint32_t can_id, const uint8_t *data, uint8_t size) {
  struct can_frame frame;
  memcpy(frame.data, data, size);
  frame.can_dlc = size;
  frame.can_id = can_id;
  int nbytes = write(can_fd_, &frame, sizeof(struct can_frame));
  if (nbytes != sizeof(frame)) {
    _debug("write error, nbytes = %d\n", nbytes);
    return false;
  } else {
#ifdef CAN_DEBUG
    printf("write successful, id = %X [%d] ", can_id, size);
    for (int i = 0; i < size; ++i) {
      printf("%02X ", *(data + i));
    }
    printf("\n");
#endif
    return true;
  }
}

void DriverCan::CanRead(uint32_t *can_id, uint8_t *data, uint8_t *size) {
  struct can_frame can_frame;
  int nbytes = read(can_fd_, &can_frame, sizeof(struct can_frame));
  if (nbytes < 0) {
    perror("can raw socket read");
    close(can_fd_);
    _exit(-1);
  }

  if (nbytes < sizeof(struct can_frame)) {
    _debug("read: incomplete CAN frame\n");
    close(can_fd_);
    _exit(-1);
  }
  *can_id = can_frame.can_id;
  *size = can_frame.can_dlc;
  memcpy(data, can_frame.data, can_frame.can_dlc);
#ifdef CAN_DEBUG
  printf("read successful id = %X [%d] ", *can_id, *size);
  for (int i = 0; i < *size; ++i) {
    printf(" %02X", *(data + i));
  }
  printf("\n");
#endif
}

bool DriverCan::GetCanFrame(struct CanFrame &can_frame) {
  bool ret = false;
  mutex_.lock();
  if (read_queue_.empty() != true) {
    can_frame = read_queue_.front();
    read_queue_.pop();
    ret = true;
  }
  mutex_.unlock();
  return ret;
}

void DriverCan::ClearReadQueue() {
  std::queue<struct CanFrame> empty;
  mutex_.lock();
  std::swap(empty, read_queue_);
  mutex_.unlock();
}

uint16_t DriverCan::GetReadQueueSize(void) {
  mutex_.lock();
  uint16_t tmp = read_queue_.size();
  mutex_.unlock();
  return read_queue_.size();
}

bool DriverCan::DropOneCanFrame(void) {
  bool ret = false;
  mutex_.lock();
  if (read_queue_.empty() != true) {
    read_queue_.pop();
    ret = true;
  }
  mutex_.unlock();
  return ret;
}
