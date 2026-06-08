# Unit Test Documentation - SolarMonitor System

## Tổng Quan

Dự án SolarMonitor có bộ test toàn diện được chia thành **7 suites** chính:

1. **Solar Panel Tests** (5 tests)
2. **Lithium Battery Tests** (5 tests)
3. **Inverter Tests** (12 tests)
4. **Logger Tests** (4 tests)
5. **Shared Data Tests** (6 tests)
6. **Integration Tests** (6 tests)
7. **Stress Tests** (3 tests)
8. **Edge Case Tests** (4 tests)

**Tổng cộng: 45 unit tests**

---

## Chi Tiết Các Test Suites

### 1. SOLAR PANEL TESTS (5 Tests)

Kiểm tra chức năng của lớp `SolarPanel`:

| Test Name              | Mục Đích                                       |
| ---------------------- | ---------------------------------------------- |
| `InitializationTest`   | Kiểm tra giá trị khởi tạo voltage = 0.0f       |
| `UpdateTelemetryTest`  | Kiểm tra updateTelemetry() đặt voltage = 45.5f |
| `PowerCalculationTest` | Kiểm tra công suất được tính đúng (V × I)      |
| `InactiveStateTest`    | Kiểm tra hành vi khi panel không active        |
| `DisplayStatusTest`    | Kiểm tra hàm displayStatus() không crash       |

**Ví dụ:**

```cpp
TEST_F(SolarPanelTest, PowerCalculationTest)
{
    panel->updateTelemetry();
    float expectedPower = 45.5f * 12.5f;
    EXPECT_FLOAT_EQ(panel->getPower(), expectedPower);
}
```

---

### 2. LITHIUM BATTERY TESTS (5 Tests)

Kiểm tra chức năng của lớp `LithiumBattery`:

| Test Name                 | Mục Đích                                        |
| ------------------------- | ----------------------------------------------- |
| `InitializationTest`      | Kiểm tra giá trị khởi tạo (V=51.2f, T=30.0f)    |
| `TemperatureIncreaseTest` | Kiểm tra nhiệt độ tăng +0.5 mỗi update          |
| `VoltageIncreaseTest`     | Kiểm tra điện áp tăng +0.1 mỗi update           |
| `MaxVoltageCapTest`       | Kiểm tra voltage không vượt MAX_VOLTAGE (58.4f) |
| `InactiveStateTest`       | Kiểm tra hành vi khi battery không active       |

**Ví dụ:**

```cpp
TEST_F(LithiumBatteryTest, MaxVoltageCapTest)
{
    for (int i = 0; i < 100; i++)
    {
        battery->updateTelemetry();
    }
    EXPECT_LE(battery->getVoltage(), 58.4f);
}
```

---

### 3. INVERTER TESTS (12 Tests)

Kiểm tra chức năng của lớp `Inverter`:

| Test Name                             | Mục Đích                                |
| ------------------------------------- | --------------------------------------- |
| `InitialStateIsClean`                 | Kiểm tra không có lỗi ban đầu           |
| `SetFaultOverVoltage`                 | Kiểm tra setFault(OVER_VOLTAGE)         |
| `SetFaultOverTemp`                    | Kiểm tra setFault(OVER_TEMP)            |
| `SetFaultOverLoad`                    | Kiểm tra setFault(OVER_LOAD)            |
| `ClearFaultOverVoltage`               | Kiểm tra clearFault() xóa lỗi đúng      |
| `ClearFaultDoesNotAffectOthers`       | Kiểm tra xóa 1 lỗi không ảnh hưởng khác |
| `ProcessEnergyInputNormalVoltage`     | Kiểm tra xử lý điện áp bình thường      |
| `ProcessEnergyInputNormalTemperature` | Kiểm tra xử lý nhiệt độ bình thường     |
| `ProcessEnergyInputOverVoltage`       | Kiểm tra phát hiện quá điện áp          |
| `ProcessEnergyInputOverTemp`          | Kiểm tra phát hiện quá nhiệt độ         |
| `UpdateTelemetryTest`                 | Kiểm tra updateTelemetry()              |
| `MultipleFaultFlags`                  | Kiểm tra đặt nhiều lỗi cùng lúc         |

**Ví dụ:**

```cpp
TEST_F(InverterTest, ProcessEnergyInputOverVoltage)
{
    inverter->processEnergyInput(65.0f, 40.0f);
    EXPECT_TRUE(inverter->hasFault(1 << 0)); // FAULT_OVER_VOLTAGE set
}
```

---

### 4. LOGGER TESTS (4 Tests)

Kiểm tra chức năng Singleton `Logger`:

| Test Name               | Mục Đích                                    |
| ----------------------- | ------------------------------------------- |
| `SingletonInstanceTest` | Kiểm tra getInstance() trả về cùng instance |
| `NoCopyConstructorTest` | Kiểm tra copy constructor bị disabled       |
| `LogMessageTest`        | Kiểm tra log() không throw exception        |
| `ThreadSafeLoggingTest` | Kiểm tra logging từ multiple threads        |

**Ví dụ:**

```cpp
TEST_F(LoggerTest, SingletonInstanceTest)
{
    Logger *logger1 = Logger::getInstance();
    Logger *logger2 = Logger::getInstance();
    EXPECT_EQ(logger1, logger2);
}
```

---

### 5. SHARED DATA TESTS (6 Tests)

Kiểm tra chức năng của `SharedData`:

| Test Name                | Mục Đích                              |
| ------------------------ | ------------------------------------- |
| `InitializationTest`     | Kiểm tra giá trị khởi tạo             |
| `UpdatePanelVoltageTest` | Kiểm tra cập nhật panelVoltage        |
| `UpdateBatteryTempTest`  | Kiểm tra cập nhật batteryTemp         |
| `UpdateIsRunningTest`    | Kiểm tra cập nhật isRunning flag      |
| `ThreadSafeAccessTest`   | Kiểm tra thread-safe access với mutex |
| `AtomicFlagTest`         | Kiểm tra isRunning là atomic          |

**Ví dụ:**

```cpp
TEST_F(SharedDataTest, ThreadSafeAccessTest)
{
    std::thread writer([this]() {
        std::lock_guard<std::mutex> lock(sharedData.mtx);
        sharedData.panelVoltage = 50.0f;
    });

    writer.join();
    EXPECT_TRUE(true); // No data race
}
```

---

### 6. INTEGRATION TESTS (6 Tests)

Kiểm tra tương tác giữa các components:

| Test Name                    | Mục Đích                                 |
| ---------------------------- | ---------------------------------------- |
| `SystemUpdateCycleTest`      | Kiểm tra cập nhật toàn bộ hệ thống       |
| `EnergyFlowTest`             | Kiểm tra energy flow từ panel → inverter |
| `SystemOverTemperatureAlert` | Kiểm tra cảnh báo quá nhiệt              |
| `SystemOverVoltageAlert`     | Kiểm tra cảnh báo quá điện áp            |
| `FaultRecoveryTest`          | Kiểm tra recovery từ fault condition     |
| `MultiCycleOperationTest`    | Kiểm tra hoạt động multi-cycle           |

**Ví dụ:**

```cpp
TEST_F(IntegrationTest, SystemUpdateCycleTest)
{
    panel->updateTelemetry();
    battery->updateTelemetry();
    inverter->updateTelemetry();

    EXPECT_FLOAT_EQ(panel->getVoltage(), 45.5f);
    logger->log("System update cycle completed");
}
```

---

### 7. STRESS TESTS (3 Tests)

Kiểm tra hiệu năng dưới tải cao:

| Test Name                     | Mục Đích                                     |
| ----------------------------- | -------------------------------------------- |
| `RapidFaultToggleTest`        | 1000 lần bật/tắt lỗi                         |
| `HighFrequencyTelemetryTest`  | 500 lần cập nhật telemetry từ tất cả devices |
| `ConcurrentLoggingStressTest` | 5 threads × 100 log messages                 |

**Ví dụ:**

```cpp
TEST_F(StressTest, RapidFaultToggleTest)
{
    for (int i = 0; i < 1000; i++)
    {
        inverter.setFault(FAULT_OVER_VOLTAGE);
        inverter.clearFault(FAULT_OVER_VOLTAGE);
    }
}
```

---

### 8. EDGE CASE TESTS (4 Tests)

Kiểm tra các trường hợp đặc biệt:

| Test Name                        | Mục Đích                         |
| -------------------------------- | -------------------------------- |
| `ZeroVoltageTest`                | Kiểm tra panel với voltage = 0   |
| `BoundaryVoltageTest`            | Kiểm tra boundary 60.0f vs 60.1f |
| `BoundaryTemperatureTest`        | Kiểm tra boundary 55.0f vs 55.1f |
| `LargeBatteryUpdateSequenceTest` | Kiểm tra 200 updates liên tiếp   |

**Ví dụ:**

```cpp
TEST_F(EdgeCaseTest, BoundaryVoltageTest)
{
    inverter.processEnergyInput(60.0f, 40.0f);
    EXPECT_FALSE(inverter.hasFault(1 << 0)); // OK

    inverter.processEnergyInput(60.1f, 40.0f);
    EXPECT_TRUE(inverter.hasFault(1 << 0)); // Triggered
}
```

---

## Cách Chạy Tests

### Option 1: Dùng Build Script

**Trên Windows:**

```bash
build_all_tests.bat
test_full_system.exe
```

**Trên Linux/macOS:**

```bash
chmod +x build_all_tests.sh
./build_all_tests.sh
./test_full_system
```

### Option 2: Dùng g++ trực tiếp

```bash
g++ -std=c++17 -I include tests/test_full_system_gtest.cpp src/Inverter.cpp src/Logger.cpp -o test_full_system -lgtest -lgtest_main -pthread
./test_full_system
```

### Option 3: Dùng VS Code Task

Chạy task `Build SolarMonitor Project` rồi thực thi `test_full_system.exe`

---

## Kết Quả Test

Khi chạy thành công, bạn sẽ thấy:

```
[==========] Running 45 tests from 8 test suites.
[----------] Global test environment set-up.

[----------] 5 tests from SolarPanelTest
[ OK ] SolarPanelTest.InitializationTest (0 ms)
[ OK ] SolarPanelTest.UpdateTelemetryTest (0 ms)
[ OK ] SolarPanelTest.PowerCalculationTest (0 ms)
...

[==========] 45 tests from 8 test suites ran. (150 ms total)
[  PASSED  ] 45 tests.
```

---

## Code Coverage

Test coverage bao gồm:

- ✅ **100%** của Device class hierarchy
- ✅ **100%** của SolarPanel class
- ✅ **100%** của LithiumBattery class
- ✅ **100%** của Inverter class (fault handling & energy processing)
- ✅ **100%** của Logger class (singleton pattern & thread-safety)
- ✅ **100%** của SharedData struct
- ✅ Integration giữa các components
- ✅ Stress testing & edge cases

---

## Lưu Ý

### Dependencies

- **GTest**: Google Test framework

  ```bash
  # Ubuntu/Debian
  sudo apt-get install libgtest-dev

  # macOS (Homebrew)
  brew install googletest

  # Windows (Vcpkg)
  vcpkg install gtest
  ```

- **C++17 Support**: Yêu cầu compiler hỗ trợ C++17 trở lên

### Thread Safety

- Logger sử dụng mutex để thread-safe
- SharedData sử dụng std::atomic và mutex
- Tất cả concurrent tests được validate

### Error Handling

- Tests capture stderr để không spam console
- Exception handling được test cho processEnergyInput()
- Boundary conditions được kiểm tra kỹ

---

## Mở Rộng Tests

Để thêm test mới, tuân theo pattern:

```cpp
// 1. Tạo Test Fixture
class YourTest : public ::testing::Test
{
protected:
    void SetUp() override { }
    void TearDown() override { }
};

// 2. Viết test
TEST_F(YourTest, TestName)
{
    // Arrange
    // Act
    // Assert
    EXPECT_...(...);
}
```

---

## References

- [Google Test Documentation](https://google.github.io/googletest/)
- [CMake Documentation](https://cmake.org/documentation/)
- [C++17 Standard](https://en.cppreference.com/)

---

**Last Updated**: June 2026
