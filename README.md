# Solar Monitor Simulator (Hệ thống Giả lập Giám sát Năng lượng Mặt trời)

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Dự án này là một ứng dụng giả lập Firmware (phần mềm nhúng) viết bằng **C++**, mô phỏng quá trình hoạt động thời gian thực của bộ biến tần (Inverter) và hệ thống quản lý pin (BMS) trong điện mặt trời.

Mục tiêu cốt lõi của dự án là áp dụng các kỹ thuật lập trình hệ thống nâng cao, xử lý đa luồng an toàn và quản lý bộ nhớ nghiêm ngặt mà không phụ thuộc vào giao diện đồ họa.

## 🌟 Tính năng kỹ thuật nổi bật (Technical Highlights)

Dự án triển khai các mảng kiến thức cốt lõi của Lập trình viên C/C++ Hệ thống:

- **Multithreading & Synchronization (Đa luồng & Đồng bộ hóa):** \* Áp dụng mô hình **Producer-Consumer**. Luồng Cảm biến (500ms) và Luồng Điều khiển (1000ms) hoạt động song song.
  - Sử dụng `std::mutex` và `std::lock_guard` để triệt tiêu hoàn toàn vấn đề tranh chấp dữ liệu (Race Condition).
- **Bitwise Operations (Thao tác Bit):** Quản lý trạng thái lỗi phần cứng (Quá áp, Quá nhiệt) thông qua thanh ghi 8-bit (`uint8_t status_register`), sử dụng các toán tử `|`, `&`, `~`, `<<` để tối ưu hóa bộ nhớ cấp thấp.
- **Design Patterns & I/O Process:** Triển khai **Singleton Pattern** cho module `Logger`, đảm bảo tính Thread-safe khi ghi nhật ký hoạt động ra tệp `system_log.csv` liên tục từ nhiều luồng.
- **Hardware Interrupts & Timer (Ngắt & Định thời):** \* Sử dụng thư viện `<csignal>` (`SIGINT`) và cờ `std::atomic<bool>` để bẻ lái luồng thực thi, xử lý tắt hệ thống an toàn (Graceful Shutdown) khi nhận tín hiệu khẩn cấp.
  - Tích hợp luồng **Watchdog Timer** giám sát sức khỏe hệ thống định kỳ.
- **Memory Management (Quản lý Bộ nhớ):** Ứng dụng **Smart Pointers** (`std::unique_ptr`) xuyên suốt dự án để tự động hóa việc dọn dẹp RAM, đảm bảo **Zero Memory Leak**.
- **Object-Oriented Programming (OOP):** Khai thác triệt để Tính Kế thừa, Tính Đóng gói và Tính Đa hình (Virtual functions) thông qua kiến trúc lớp `Device` trừu tượng.

## 📂 Cấu trúc dự án (Project Structure)

```text
SolarMonitor-Simulator/
│
├── include/                 # Header files (.h)
│   ├── Device.h             # Abstract Base Class
│   ├── SolarPanel.h         # Lớp giả lập Tấm pin
│   ├── LithiumBattery.h     # Lớp giả lập Pin lưu trữ
│   ├── Inverter.h           # Xử lý Bitwise & Điều khiển
│   ├── Logger.h             # Singleton Thread-safe Logger
│   └── SharedData.h         # Struct chia sẻ bộ nhớ giữa các luồng
│
├── src/                     # Source files (.cpp)
│   ├── Inverter.cpp         # Logic xử lý lỗi Inverter
│   ├── Logger.cpp           # Logic ghi file CSV
│   └── main.cpp             # Quản lý Đa luồng, Timer và Ngắt
│
├── system_log.csv           # File nhật ký sinh ra khi chạy (Auto-generated)
├── .gitignore
└── README.md
🚀 Hướng dẫn cài đặt và khởi chạy (Getting Started)
Yêu cầu hệ thống (Prerequisites)
Trình biên dịch hỗ trợ chuẩn C++17 trở lên (ví dụ: GCC/G++ qua MinGW hoặc MSYS2).

Visual Studio Code hoặc bất kỳ IDE C++ nào.

Biên dịch (Build)
Nếu sử dụng Terminal/Command Line, chạy lệnh sau tại thư mục gốc của dự án:

Bash
g++ -std=c++17 -I ./include ./src/*.cpp -o SolarMonitor
Chạy ứng dụng (Run)
Bash
./SolarMonitor
🎮 Hướng dẫn mô phỏng (Simulation Guide)
Theo dõi thời gian thực: Khi chạy, hệ thống sẽ in ra Terminal các thông số điện áp, công suất và nhiệt độ cập nhật liên tục. Trạng thái thanh ghi lỗi (8 bit) sẽ hiển thị 00000000.

Theo dõi Watchdog: Cứ mỗi 3 giây, Watchdog Timer sẽ báo cáo xác nhận hệ thống không bị treo.

Kích hoạt Ngắt phần cứng (E-Stop): Nhấn tổ hợp phím Ctrl + C trên bàn phím. Hệ thống sẽ bắt tín hiệu ngắt, dừng an toàn toàn bộ các luồng, ghi chú log cuối cùng và giải phóng RAM.

Kiểm tra Log: Mở file system_log.csv để xem lịch sử hoạt động được ghi lại bởi Singleton Logger.
✍️ Tác giả (Author)
Đặng Văn Toàn (ToànDV)

Sinh viên Chuyên ngành Kỹ thuật Phần mềm (Software Engineering) - Đại học FPT

Dự án Portfolio tập trung vào C/C++ System và Embedded Software.
```
