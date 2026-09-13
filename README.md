# Hệ thống giám sát và điều khiển nông nghiệp STM32

Project sử dụng STM32 để theo dõi nhiệt độ, độ ẩm không khí, độ ẩm đất và ánh sáng,
đồng thời điều khiển bơm, đèn và quạt. Firmware viết bằng C bare-metal, thao tác
trực tiếp thanh ghi, không dùng HAL hoặc RTOS.

## Chức năng

- Đọc dữ liệu cảm biến mỗi 10 giây và hiển thị trên LCD.
- Tự động điều khiển bơm, đèn, quạt theo ngưỡng cài đặt.
- Chuyển sang chế độ Manual để bật/tắt từng thiết bị bằng nút nhấn.
- Chỉnh ngưỡng, lưu cấu hình vào Flash và khôi phục ngưỡng mặc định.

## MCU, cảm biến và thiết bị

| Thành phần | Vai trò |
| --- | --- |
| STM32F103C8T6 — ARM Cortex-M3 | MCU điều khiển, clock 72 MHz từ thạch anh ngoài 8 MHz |
| DHT11 | Đo nhiệt độ và độ ẩm không khí |
| Cảm biến độ ẩm đất ngõ ra analog | Đọc tín hiệu đất qua ADC |
| Cảm biến ánh sáng ngõ ra analog | Đọc mức ánh sáng qua ADC |
| Hai LCD 16×2 giao tiếp I2C | Hiển thị số đo và ngưỡng cài đặt |
| Ba relay | Điều khiển bơm, đèn và quạt |
| Nút nhấn và LED | Thay đổi cài đặt, điều khiển thủ công và báo trạng thái |

Giá trị đất và ánh sáng hiển thị theo tỷ lệ toàn thang ADC, chưa hiệu chuẩn
thành độ ẩm đất hoặc độ rọi thực tế.

## Ngoại vi MCU sử dụng

- **RCC:** cấu hình clock hệ thống và cấp clock cho ngoại vi.
- **GPIO, AFIO, EXTI:** kết nối cảm biến, relay, LED và xử lý nút nhấn.
- **ADC1:** đọc cảm biến ánh sáng tại PA5 và cảm biến đất tại PA6.
- **I2C1:** giao tiếp hai LCD tại địa chỉ `0x27` và `0x26`, dùng PB6/PB7.
- **TIM2, TIM3, TIM4 và SysTick:** tạo thời gian chờ, lên lịch lấy mẫu, đo xung DHT11 và chống dội nút.
- **Flash nội:** lưu ngưỡng cài đặt để sử dụng sau khi khởi động lại.

## Cách build

Yêu cầu **Windows PowerShell**, **GNU Make** và **GNU Arm GCC**.
Chạy tại thư mục gốc của project:

```powershell
make build GCC_DIR=C:/Toolchains/arm-gnu-toolchain
```

Thay `GCC_DIR` bằng thư mục toolchain trên máy, chứa
`bin/arm-none-eabi-gcc` và `bin/arm-none-eabi-objcopy`; dùng đường dẫn không có
khoảng trắng. File firmware được tạo tại **`Output/makefile.hex`**.
