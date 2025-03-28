#pragma once

#include <database.hpp>
#include <device.hpp>

#include "bsp_gpio.h"
#include "comp_pid.hpp"
#include "dev_ahrs.hpp"

namespace Device {
class ICM42688 {
 public:
  typedef struct {
    Component::Type::Vector3 gyro_offset; /* 陀螺仪偏置 */
  } Calibration;                          /* 校准数据 */

  typedef struct {
    /* 旋转矩阵 */
    float rot_mat[3][3];
  } Rotation;

  typedef enum {
    DATA_RATE_UNKNOW,
    DATA_RATE_32KHZ,
    DATA_RATE_16KHZ,
    DATA_RATE_8KHZ,
    DATA_RATE_4KHZ,
    DATA_RATE_2KHZ,
    DATA_RATE_1KHZ,
    DATA_RATE_200HZ,
    DATA_RATE_100HZ,
    DATA_RATE_50HZ,
    DATA_RATE_25HZ,
    DATA_RATE_12_5HZ,
    DATA_RATE_500HZ = 0XF,
  } DataRate;

  ICM42688(ICM42688::Rotation &rot, DataRate date_rate = DATA_RATE_1KHZ);

  bool Init(DataRate date_rate);

  void Prase();

  bool StartRecv();

  void Select() { bsp_gpio_write_pin(BSP_GPIO_IMU_CS, false); }

  void Unselect() { bsp_gpio_write_pin(BSP_GPIO_IMU_CS, true); }

  void WriteSingle(uint8_t reg, uint8_t data);

  uint8_t ReadSingle(uint8_t reg);

  void Read(uint8_t reg, uint8_t *data, uint8_t len);

  static int CaliCMD(ICM42688 *icm42688, int argc, char **argv);

  void Off() { WriteSingle(0X4E, 0x00); }
  void On() { WriteSingle(0X4E, 0x0f); }

  System::Database::Key<Calibration> cali_;
  Rotation &rot_;

  DataRate datarate_;

  System::Semaphore raw_;
  System::Semaphore new_;

  float temp_ = 0.0f; /* 温度 */

  System::Thread thread_accl_, thread_gyro_;

  Message::Topic<Component::Type::Vector3> accl_tp_;
  Message::Topic<Component::Type::Vector3> gyro_tp_;

  Component::Type::Vector3 accl_{};
  Component::Type::Vector3 gyro_{};

  System::Term::Command<ICM42688 *> cmd_;

  Component::PID imu_temp_ctrl_pid_;
  int16_t raw_data_[6];

  double cali_x_, cali_y_, cali_z_;
  uint32_t cali_count_;
  bool in_cali_ = false;
};
}  // namespace Device
