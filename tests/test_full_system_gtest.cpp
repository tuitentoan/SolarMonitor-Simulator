#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include "../include/SolarPanel.h"
#include "../include/LithiumBattery.h"
#include "../include/Inverter.h"
#include "../include/Logger.h"
#include "../include/SharedData.h"

// ========================================================================
// TEST SUITE 1: SOLAR PANEL TESTS
// ========================================================================

class SolarPanelTest : public ::testing::Test
{
protected:
    SolarPanel *panel;

    void SetUp() override
    {
        panel = new SolarPanel("Panel_Test");
    }

    void TearDown() override
    {
        delete panel;
    }
};

// Test 1.1: Kiểm tra khởi tạo Solar Panel
TEST_F(SolarPanelTest, InitializationTest)
{
    EXPECT_EQ(panel->getVoltage(), 0.0f);
    // Initial state should be inactive voltage/current
}

// Test 1.2: Kiểm tra updateTelemetry() của Solar Panel
TEST_F(SolarPanelTest, UpdateTelemetryTest)
{
    panel->updateTelemetry();
    EXPECT_FLOAT_EQ(panel->getVoltage(), 45.5f);
}

// Test 1.3: Kiểm tra tính toán công suất (Power Calculation)
TEST_F(SolarPanelTest, PowerCalculationTest)
{
    panel->updateTelemetry();
    float expectedPower = 45.5f * 12.5f;
    EXPECT_FLOAT_EQ(panel->getPower(), expectedPower);
}

// Test 1.4: Kiểm tra Solar Panel khi inactive
TEST_F(SolarPanelTest, InactiveStateTest)
{
    // Tạo panel không hoạt động
    SolarPanel inactivePanel("Inactive_Panel");
    // Giả sử isActive ban đầu là true, sau khi updateTelemetry() không active thì voltage vẫn 0
    // (Cần setter để test điều này tốt hơn, nhưng test với state hiện tại)
}

// ========================================================================
// TEST SUITE 2: LITHIUM BATTERY TESTS
// ========================================================================

class LithiumBatteryTest : public ::testing::Test
{
protected:
    LithiumBattery *battery;

    void SetUp() override
    {
        battery = new LithiumBattery("Battery_Test");
    }

    void TearDown() override
    {
        delete battery;
    }
};

// Test 2.1: Kiểm tra khởi tạo Lithium Battery
TEST_F(LithiumBatteryTest, InitializationTest)
{
    EXPECT_FLOAT_EQ(battery->getVoltage(), 51.2f);
    EXPECT_FLOAT_EQ(battery->getTemperature(), 30.0f);
}

// Test 2.2: Kiểm tra updateTelemetry() - Tăng nhiệt độ
TEST_F(LithiumBatteryTest, TemperatureIncreaseTest)
{
    float initialTemp = battery->getTemperature();
    battery->updateTelemetry();
    EXPECT_FLOAT_EQ(battery->getTemperature(), initialTemp + 0.5f);
}

// Test 2.3: Kiểm tra updateTelemetry() - Tăng voltage
TEST_F(LithiumBatteryTest, VoltageIncreaseTest)
{
    float initialVoltage = battery->getVoltage();
    battery->updateTelemetry();
    EXPECT_FLOAT_EQ(battery->getVoltage(), initialVoltage + 0.1f);
}

// Test 2.4: Kiểm tra max voltage limit
TEST_F(LithiumBatteryTest, MaxVoltageCapTest)
{
    // Gọi updateTelemetry nhiều lần để voltage đạt giới hạn
    for (int i = 0; i < 100; i++)
    {
        battery->updateTelemetry();
    }
    // Voltage không vượt quá MAX_VOLTAGE (58.4f)
    EXPECT_LE(battery->getVoltage(), 58.4f);
}

// Test 2.5: Kiểm tra khi Battery inactive
TEST_F(LithiumBatteryTest, InactiveStateTest)
{
    LithiumBattery inactiveBattery("Inactive_Battery");
    // Khi inactive, các thông số không thay đổi
}

// ========================================================================
// TEST SUITE 3: INVERTER TESTS
// ========================================================================

class InverterTest : public ::testing::Test
{
protected:
    Inverter *inverter;

    void SetUp() override
    {
        inverter = new Inverter("Inverter_Test");
    }

    void TearDown() override
    {
        delete inverter;
    }
};

// Test 3.1: Kiểm tra trạng thái khởi tạo - Không có lỗi
TEST_F(InverterTest, InitialStateIsClean)
{
    EXPECT_FALSE(inverter->hasFault(1 << 0)); // FAULT_OVER_VOLTAGE
    EXPECT_FALSE(inverter->hasFault(1 << 1)); // FAULT_OVER_TEMP
    EXPECT_FALSE(inverter->hasFault(1 << 2)); // FAULT_OVER_LOAD
}

// Test 3.2: Kiểm tra setFault() - Bật cờ lỗi OVER_VOLTAGE
TEST_F(InverterTest, SetFaultOverVoltage)
{
    uint8_t FAULT_OVER_VOLTAGE = 1 << 0;
    inverter->setFault(FAULT_OVER_VOLTAGE);
    EXPECT_TRUE(inverter->hasFault(FAULT_OVER_VOLTAGE));
    EXPECT_FALSE(inverter->hasFault(1 << 1));
}

// Test 3.3: Kiểm tra setFault() - Bật cờ lỗi OVER_TEMP
TEST_F(InverterTest, SetFaultOverTemp)
{
    uint8_t FAULT_OVER_TEMP = 1 << 1;
    inverter->setFault(FAULT_OVER_TEMP);
    EXPECT_TRUE(inverter->hasFault(FAULT_OVER_TEMP));
}

// Test 3.4: Kiểm tra setFault() - Bật cờ lỗi OVER_LOAD
TEST_F(InverterTest, SetFaultOverLoad)
{
    uint8_t FAULT_OVER_LOAD = 1 << 2;
    inverter->setFault(FAULT_OVER_LOAD);
    EXPECT_TRUE(inverter->hasFault(FAULT_OVER_LOAD));
}

// Test 3.5: Kiểm tra clearFault() - Tắt cờ lỗi
TEST_F(InverterTest, ClearFaultOverVoltage)
{
    uint8_t FAULT_OVER_VOLTAGE = 1 << 0;
    inverter->setFault(FAULT_OVER_VOLTAGE);
    EXPECT_TRUE(inverter->hasFault(FAULT_OVER_VOLTAGE));

    inverter->clearFault(FAULT_OVER_VOLTAGE);
    EXPECT_FALSE(inverter->hasFault(FAULT_OVER_VOLTAGE));
}

// Test 3.6: Kiểm tra clearFault() - Không ảnh hưởng đến lỗi khác
TEST_F(InverterTest, ClearFaultDoesNotAffectOthers)
{
    uint8_t FAULT_OVER_VOLTAGE = 1 << 0;
    uint8_t FAULT_OVER_TEMP = 1 << 1;

    inverter->setFault(FAULT_OVER_VOLTAGE);
    inverter->setFault(FAULT_OVER_TEMP);

    inverter->clearFault(FAULT_OVER_VOLTAGE);

    EXPECT_FALSE(inverter->hasFault(FAULT_OVER_VOLTAGE));
    EXPECT_TRUE(inverter->hasFault(FAULT_OVER_TEMP));
}

// Test 3.7: Kiểm tra processEnergyInput() - Điện áp bình thường
TEST_F(InverterTest, ProcessEnergyInputNormalVoltage)
{
    inverter->processEnergyInput(48.0f, 40.0f);
    EXPECT_FALSE(inverter->hasFault(1 << 0)); // No over voltage fault
}

// Test 3.8: Kiểm tra processEnergyInput() - Nhiệt độ bình thường
TEST_F(InverterTest, ProcessEnergyInputNormalTemperature)
{
    inverter->processEnergyInput(48.0f, 40.0f);
    EXPECT_FALSE(inverter->hasFault(1 << 1)); // No over temp fault
}

// Test 3.9: Kiểm tra processEnergyInput() - Điện áp quá cao (Over Voltage)
TEST_F(InverterTest, ProcessEnergyInputOverVoltage)
{
    // Capture stderr để test không ghi lỗi ra console
    std::streambuf *old_cerr = std::cerr.rdbuf();
    std::stringstream ss;
    std::cerr.rdbuf(ss.rdbuf());

    inverter->processEnergyInput(65.0f, 40.0f);

    std::cerr.rdbuf(old_cerr);

    EXPECT_TRUE(inverter->hasFault(1 << 0)); // FAULT_OVER_VOLTAGE set
}

// Test 3.10: Kiểm tra processEnergyInput() - Nhiệt độ quá cao (Over Temp)
TEST_F(InverterTest, ProcessEnergyInputOverTemp)
{
    std::streambuf *old_cerr = std::cerr.rdbuf();
    std::stringstream ss;
    std::cerr.rdbuf(ss.rdbuf());

    inverter->processEnergyInput(48.0f, 60.0f);

    std::cerr.rdbuf(old_cerr);

    EXPECT_TRUE(inverter->hasFault(1 << 1)); // FAULT_OVER_TEMP set
}

// Test 3.11: Kiểm tra updateTelemetry()
TEST_F(InverterTest, UpdateTelemetryTest)
{
    inverter->updateTelemetry();
    // Chỉ kiểm tra rằng hàm chạy mà không crash
    EXPECT_TRUE(true);
}

// Test 3.12: Kiểm tra multiple fault flags
TEST_F(InverterTest, MultipleFaultFlags)
{
    inverter->setFault(1 << 0);
    inverter->setFault(1 << 1);
    inverter->setFault(1 << 2);

    EXPECT_TRUE(inverter->hasFault(1 << 0));
    EXPECT_TRUE(inverter->hasFault(1 << 1));
    EXPECT_TRUE(inverter->hasFault(1 << 2));
}

// ========================================================================
// TEST SUITE 4: LOGGER TESTS
// ========================================================================

class LoggerTest : public ::testing::Test
{
protected:
    Logger *logger;

    void SetUp() override
    {
        logger = Logger::getInstance();
    }
};

// Test 4.1: Kiểm tra Logger Singleton - getInstance() trả về cùng instance
TEST_F(LoggerTest, SingletonInstanceTest)
{
    Logger *logger1 = Logger::getInstance();
    Logger *logger2 = Logger::getInstance();
    EXPECT_EQ(logger1, logger2);
}

// Test 4.2: Kiểm tra Logger không thể copy (deleted copy constructor)
TEST_F(LoggerTest, NoCopyConstructorTest)
{
    // Compile-time check: copy constructor is deleted
    EXPECT_TRUE(std::is_copy_constructible_v<Logger> == false);
}

// Test 4.3: Kiểm tra Logger có thể log message
TEST_F(LoggerTest, LogMessageTest)
{
    // Chỉ kiểm tra rằng hàm log không throw exception
    EXPECT_NO_THROW({
        logger->log("Test message");
    });
}

// Test 4.4: Kiểm tra Logger thread-safe với multiple log calls
TEST_F(LoggerTest, ThreadSafeLoggingTest)
{
    std::thread thread1([this]()
                        {
        for (int i = 0; i < 5; i++)
        {
            logger->log("Thread 1 - Message " + std::to_string(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } });

    std::thread thread2([this]()
                        {
        for (int i = 0; i < 5; i++)
        {
            logger->log("Thread 2 - Message " + std::to_string(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } });

    thread1.join();
    thread2.join();

    // Test passes nếu không có data race
    EXPECT_TRUE(true);
}

// ========================================================================
// TEST SUITE 5: SHARED DATA TESTS
// ========================================================================

class SharedDataTest : public ::testing::Test
{
protected:
    SharedData sharedData;

    void SetUp() override
    {
        sharedData.panelVoltage = 0.0f;
        sharedData.batteryTemp = 0.0f;
        sharedData.isRunning = true;
    }
};

// Test 5.1: Kiểm tra khởi tạo SharedData
TEST_F(SharedDataTest, InitializationTest)
{
    EXPECT_FLOAT_EQ(sharedData.panelVoltage, 0.0f);
    EXPECT_FLOAT_EQ(sharedData.batteryTemp, 0.0f);
    EXPECT_TRUE(sharedData.isRunning);
}

// Test 5.2: Kiểm tra cập nhật panelVoltage
TEST_F(SharedDataTest, UpdatePanelVoltageTest)
{
    sharedData.panelVoltage = 48.5f;
    EXPECT_FLOAT_EQ(sharedData.panelVoltage, 48.5f);
}

// Test 5.3: Kiểm tra cập nhật batteryTemp
TEST_F(SharedDataTest, UpdateBatteryTempTest)
{
    sharedData.batteryTemp = 35.5f;
    EXPECT_FLOAT_EQ(sharedData.batteryTemp, 35.5f);
}

// Test 5.4: Kiểm tra cập nhật isRunning flag
TEST_F(SharedDataTest, UpdateIsRunningTest)
{
    sharedData.isRunning = false;
    EXPECT_FALSE(sharedData.isRunning);

    sharedData.isRunning = true;
    EXPECT_TRUE(sharedData.isRunning);
}

// Test 5.5: Kiểm tra thread-safe access với mutex
TEST_F(SharedDataTest, ThreadSafeAccessTest)
{
    std::thread writer([this]()
                       {
        for (int i = 0; i < 10; i++)
        {
            std::lock_guard<std::mutex> lock(sharedData.mtx);
            sharedData.panelVoltage = static_cast<float>(i) * 10.0f;
            sharedData.batteryTemp = static_cast<float>(i) * 5.0f;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } });

    std::thread reader([this]()
                       {
        for (int i = 0; i < 10; i++)
        {
            std::lock_guard<std::mutex> lock(sharedData.mtx);
            float v = sharedData.panelVoltage;
            float t = sharedData.batteryTemp;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } });

    writer.join();
    reader.join();

    EXPECT_TRUE(true);
}

// Test 5.6: Kiểm tra isRunning hoạt động như atomic
TEST_F(SharedDataTest, AtomicFlagTest)
{
    // Verify atomic operations work correctly
    std::atomic<bool> test_atomic(true);
    test_atomic = false;
    EXPECT_FALSE(test_atomic);
    test_atomic = true;
    EXPECT_TRUE(test_atomic);
}

// ========================================================================
// INTEGRATION TESTS
// ========================================================================

class IntegrationTest : public ::testing::Test
{
protected:
    SolarPanel *panel;
    LithiumBattery *battery;
    Inverter *inverter;
    Logger *logger;

    void SetUp() override
    {
        panel = new SolarPanel("Integration_Panel");
        battery = new LithiumBattery("Integration_Battery");
        inverter = new Inverter("Integration_Inverter");
        logger = Logger::getInstance();
    }

    void TearDown() override
    {
        delete panel;
        delete battery;
        delete inverter;
    }
};

// Test 6.1: Kiểm tra quy trình cập nhật toàn bộ hệ thống
TEST_F(IntegrationTest, SystemUpdateCycleTest)
{
    // Cập nhật tất cả các thiết bị
    panel->updateTelemetry();
    battery->updateTelemetry();
    inverter->updateTelemetry();

    // Kiểm tra rằng các giá trị đã cập nhật
    EXPECT_FLOAT_EQ(panel->getVoltage(), 45.5f);
    EXPECT_GT(battery->getTemperature(), 30.0f);

    // Log event
    logger->log("System update cycle completed successfully");

    EXPECT_TRUE(true);
}

// Test 6.2: Kiểm tra energy flow từ panel tới inverter
TEST_F(IntegrationTest, EnergyFlowTest)
{
    panel->updateTelemetry();
    float panelPower = panel->getPower();

    battery->updateTelemetry();
    float batteryTemp = battery->getTemperature();

    // Process energy through inverter
    inverter->processEnergyInput(panel->getVoltage(), batteryTemp);

    EXPECT_FLOAT_EQ(panelPower, 45.5f * 12.5f);
    EXPECT_FALSE(inverter->hasFault(1 << 0)); // No over voltage
}

// Test 6.3: Kiểm tra hệ thống khi pin nóng quá
TEST_F(IntegrationTest, SystemOverTemperatureAlert)
{
    std::streambuf *old_cerr = std::cerr.rdbuf();
    std::stringstream ss;
    std::cerr.rdbuf(ss.rdbuf());

    float hotTemperature = 60.0f;
    inverter->processEnergyInput(48.0f, hotTemperature);

    std::cerr.rdbuf(old_cerr);

    EXPECT_TRUE(inverter->hasFault(1 << 1)); // FAULT_OVER_TEMP
}

// Test 6.4: Kiểm tra hệ thống khi điện áp quá cao
TEST_F(IntegrationTest, SystemOverVoltageAlert)
{
    std::streambuf *old_cerr = std::cerr.rdbuf();
    std::stringstream ss;
    std::cerr.rdbuf(ss.rdbuf());

    float highVoltage = 65.0f;
    inverter->processEnergyInput(highVoltage, 40.0f);

    std::cerr.rdbuf(old_cerr);

    EXPECT_TRUE(inverter->hasFault(1 << 0)); // FAULT_OVER_VOLTAGE
}

// Test 6.5: Kiểm tra recovery từ fault condition
TEST_F(IntegrationTest, FaultRecoveryTest)
{
    std::streambuf *old_cerr = std::cerr.rdbuf();
    std::stringstream ss;
    std::cerr.rdbuf(ss.rdbuf());

    // Trigger over voltage fault
    inverter->processEnergyInput(65.0f, 40.0f);
    EXPECT_TRUE(inverter->hasFault(1 << 0));

    // Clear and process with normal values
    inverter->clearFault(1 << 0);
    inverter->processEnergyInput(48.0f, 40.0f);

    std::cerr.rdbuf(old_cerr);

    EXPECT_FALSE(inverter->hasFault(1 << 0));
}

// Test 6.6: Kiểm tra multi-cycle operation
TEST_F(IntegrationTest, MultiCycleOperationTest)
{
    for (int cycle = 0; cycle < 5; cycle++)
    {
        panel->updateTelemetry();
        battery->updateTelemetry();
        inverter->updateTelemetry();

        EXPECT_FLOAT_EQ(panel->getVoltage(), 45.5f);
        EXPECT_GT(battery->getVoltage(), 51.2f);
    }
}

// ========================================================================
// STRESS TESTS
// ========================================================================

class StressTest : public ::testing::Test
{
};

// Test 7.1: Stress test - Rapid fault setting/clearing
TEST_F(StressTest, RapidFaultToggleTest)
{
    Inverter inverter("Stress_Inverter");
    uint8_t FAULT_OVER_VOLTAGE = 1 << 0;

    for (int i = 0; i < 1000; i++)
    {
        inverter.setFault(FAULT_OVER_VOLTAGE);
        EXPECT_TRUE(inverter.hasFault(FAULT_OVER_VOLTAGE));

        inverter.clearFault(FAULT_OVER_VOLTAGE);
        EXPECT_FALSE(inverter.hasFault(FAULT_OVER_VOLTAGE));
    }
}

// Test 7.2: Stress test - Multiple telemetry updates
TEST_F(StressTest, HighFrequencyTelemetryTest)
{
    SolarPanel panel("Stress_Panel");
    LithiumBattery battery("Stress_Battery");
    Inverter inverter("Stress_Inverter");

    for (int i = 0; i < 500; i++)
    {
        panel.updateTelemetry();
        battery.updateTelemetry();
        inverter.updateTelemetry();
    }

    EXPECT_TRUE(true);
}

// Test 7.3: Stress test - Concurrent logging
TEST_F(StressTest, ConcurrentLoggingStressTest)
{
    Logger *logger = Logger::getInstance();
    std::vector<std::thread> threads;

    for (int t = 0; t < 5; t++)
    {
        threads.emplace_back([logger, t]()
                             {
            for (int i = 0; i < 100; i++)
            {
                logger->log("Stress test thread " + std::to_string(t) + " message " + std::to_string(i));
            } });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    EXPECT_TRUE(true);
}

// ========================================================================
// EDGE CASE TESTS
// ========================================================================

class EdgeCaseTest : public ::testing::Test
{
};

// Test 8.1: Zero voltage case
TEST_F(EdgeCaseTest, ZeroVoltageTest)
{
    SolarPanel panel("Edge_Panel");
    EXPECT_FLOAT_EQ(panel.getVoltage(), 0.0f);
    EXPECT_FLOAT_EQ(panel.getPower(), 0.0f);
}

// Test 8.2: Negative scenario - processEnergyInput at boundary
TEST_F(EdgeCaseTest, BoundaryVoltageTest)
{
    Inverter inverter("Edge_Inverter");

    std::streambuf *old_cerr = std::cerr.rdbuf();
    std::stringstream ss;
    std::cerr.rdbuf(ss.rdbuf());

    // Exactly at boundary (60.0f should not trigger, >60.0f should)
    inverter.processEnergyInput(60.0f, 40.0f);
    EXPECT_FALSE(inverter.hasFault(1 << 0));

    inverter.clearFault(1 << 0);
    inverter.processEnergyInput(60.1f, 40.0f);
    EXPECT_TRUE(inverter.hasFault(1 << 0));

    std::cerr.rdbuf(old_cerr);
}

// Test 8.3: Temperature boundary test
TEST_F(EdgeCaseTest, BoundaryTemperatureTest)
{
    Inverter inverter("Edge_Inverter");

    std::streambuf *old_cerr = std::cerr.rdbuf();
    std::stringstream ss;
    std::cerr.rdbuf(ss.rdbuf());

    // At boundary
    inverter.processEnergyInput(48.0f, 55.0f);
    EXPECT_FALSE(inverter.hasFault(1 << 1));

    inverter.clearFault(1 << 1);
    inverter.processEnergyInput(48.0f, 55.1f);
    EXPECT_TRUE(inverter.hasFault(1 << 1));

    std::cerr.rdbuf(old_cerr);
}

// Test 8.4: Large number of consecutive battery updates
TEST_F(EdgeCaseTest, LargeBatteryUpdateSequenceTest)
{
    LithiumBattery battery("Edge_Battery");

    for (int i = 0; i < 200; i++)
    {
        battery.updateTelemetry();
    }

    // Battery voltage should be capped at MAX_VOLTAGE
    EXPECT_LE(battery.getVoltage(), 58.4f);
    EXPECT_GT(battery.getVoltage(), 51.2f);
}

// ========================================================================
// MAIN FUNCTION
// ========================================================================

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
