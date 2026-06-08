#include <gtest/gtest.h>
#include "../include/Inverter.h"

// ---------------------------------------------------------
// TEST SUITE: InverterTest
// ---------------------------------------------------------

// Test 1: Kiểm tra trạng thái khởi tạo
TEST(InverterTest, InitialStateIsClean)
{
    Inverter inv("Test_Inv");

    // EXPECT_FALSE: Kỳ vọng biểu thức bên trong là Sai (false)
    EXPECT_FALSE(inv.hasFault(1 << 0));
    EXPECT_FALSE(inv.hasFault(1 << 1));
    EXPECT_FALSE(inv.hasFault(1 << 2));
}

// Test 2: Kiểm tra hàm BẬT cờ lỗi (setFault)
TEST(InverterTest, SetFaultWorksCorrectly)
{
    Inverter inv("Test_Inv");
    uint8_t FAULT_OVER_TEMP = 1 << 1;

    inv.setFault(FAULT_OVER_TEMP);

    // EXPECT_TRUE: Kỳ vọng lỗi Quá nhiệt đã được bật
    EXPECT_TRUE(inv.hasFault(FAULT_OVER_TEMP));

    // Đảm bảo các lỗi khác không bị bật nhầm
    EXPECT_FALSE(inv.hasFault(1 << 0));
}

// Test 3: Kiểm tra hàm TẮT cờ lỗi (clearFault)
TEST(InverterTest, ClearFaultWorksCorrectly)
{
    Inverter inv("Test_Inv");
    uint8_t FAULT_OVER_LOAD = 1 << 2;

    inv.setFault(FAULT_OVER_LOAD);
    EXPECT_TRUE(inv.hasFault(FAULT_OVER_LOAD)); // Kiểm tra đã bật chưa

    inv.clearFault(FAULT_OVER_LOAD);
    EXPECT_FALSE(inv.hasFault(FAULT_OVER_LOAD)); // Kiểm tra đã tắt thành công chưa
}

// ---------------------------------------------------------
// HÀM MAIN CHO GTEST
// ---------------------------------------------------------
int main(int argc, char **argv)
{
    // Khởi tạo môi trường GTest
    ::testing::InitGoogleTest(&argc, argv);

    // Chạy tất cả các TEST() đã định nghĩa ở trên
    return RUN_ALL_TESTS();
}