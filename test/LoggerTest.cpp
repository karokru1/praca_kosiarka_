#include <gtest/gtest.h>
#define private public
#include "Logger.hpp"
#undef private
#include <filesystem>
#include <sqlite3.h>
#include <fstream>

TEST(LoggerTest, writesToDatabase)
{
    std::filesystem::remove("test.db");
    Logger logger("test.db");

    ASSERT_TRUE(logger.init());

    SensorReadings r{20.0f, 50.0f, 1013.0f, 300.0f, 400.0f, 8.0f, false};

    EXPECT_TRUE(logger.logReading(r));
    std::filesystem::remove("test.db");
}

TEST(LoggerTest, failsToLogWithoutInit)
{
    std::filesystem::remove("test.db");
    Logger logger("test.db");
    SensorReadings r{20.0f, 50.0f, 1013.0f, 300.0f, 400.0f, 8.0f, false};

    EXPECT_FALSE(logger.logReading(r));
}

TEST(LoggerTest, initFailsOnDirectoryPath)
{
    std::filesystem::remove_all("test_dir");
    std::filesystem::create_directory("test_dir");
    Logger logger("test_dir");

    EXPECT_FALSE(logger.init());

    std::filesystem::remove_all("test_dir");
}

TEST(LoggerTest, initFailsOnReadOnlyDatabase)
{
    const std::string dbPath = "readonly.db";
    std::filesystem::remove(dbPath);
    {
        sqlite3* db = nullptr;
        ASSERT_EQ(sqlite3_open(dbPath.c_str(), &db), SQLITE_OK);
        sqlite3_close(db);
    }
    std::filesystem::permissions(dbPath, std::filesystem::perms::owner_read,
                                 std::filesystem::perm_options::replace);

    Logger logger(dbPath);
    EXPECT_FALSE(logger.init());

    std::filesystem::permissions(dbPath, std::filesystem::perms::owner_all,
                                 std::filesystem::perm_options::replace);
    std::filesystem::remove(dbPath);
}

TEST(LoggerTest, initFailsWhenCreateTableDenied)
{
    sqlite3* db = nullptr;
    ASSERT_EQ(sqlite3_open(":memory:", &db), SQLITE_OK);

    auto authorizer = [](void*, int actionCode, const char*, const char*, const char*, const char*) {
        if (actionCode == SQLITE_CREATE_TABLE) {
            return SQLITE_DENY;
        }
        return SQLITE_OK;
    };
    sqlite3_set_authorizer(db, authorizer, nullptr);

    Logger logger;
    logger.db = db;
    EXPECT_FALSE(logger.init());
}

TEST(LoggerTest, logReadingFailsWhenNotAuthorized)
{
    const std::string dbPath = "auth_fail.db";
    std::filesystem::remove(dbPath);
    Logger logger(dbPath);
    ASSERT_TRUE(logger.init());

    auto authorizer = [](void*, int actionCode, const char*, const char*, const char*, const char*) {
        if (actionCode == SQLITE_INSERT) {
            return SQLITE_DENY;
        }
        return SQLITE_OK;
    };
    sqlite3_set_authorizer(static_cast<sqlite3*>(logger.db), authorizer, nullptr);

    SensorReadings r{20.0f, 50.0f, 1013.0f, 300.0f, 400.0f, 8.0f, false};
    EXPECT_FALSE(logger.logReading(r));

    std::filesystem::remove(dbPath);
}

TEST(LoggerTest, logReadingFailsWhenTableMissing)
{
    const std::string dbPath = "missing_table.db";
    std::filesystem::remove(dbPath);
    Logger logger(dbPath);
    ASSERT_TRUE(logger.init());

    sqlite3* db = nullptr;
    ASSERT_EQ(sqlite3_open(dbPath.c_str(), &db), SQLITE_OK);
    ASSERT_EQ(sqlite3_exec(db, "DROP TABLE sensor_logs;", nullptr, nullptr, nullptr), SQLITE_OK);
    sqlite3_close(db);

    SensorReadings r{20.0f, 50.0f, 1013.0f, 300.0f, 400.0f, 8.0f, false};
    EXPECT_FALSE(logger.logReading(r));

    std::filesystem::remove(dbPath);
}

TEST(LoggerTest, logReadingFailsWhenConstraintViolated)
{
    const std::string dbPath = "constraint.db";
    std::filesystem::remove(dbPath);
    Logger logger(dbPath);
    ASSERT_TRUE(logger.init());

    sqlite3* db = nullptr;
    ASSERT_EQ(sqlite3_open(dbPath.c_str(), &db), SQLITE_OK);
    ASSERT_EQ(sqlite3_exec(db, "DROP TABLE sensor_logs;", nullptr, nullptr, nullptr), SQLITE_OK);
    const char* createSql =
        "CREATE TABLE sensor_logs ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "temperature REAL CHECK(temperature > 1000),"
        "humidity REAL,"
        "pressure REAL,"
        "light REAL,"
        "soil_moisture REAL,"
        "grass_height REAL,"
        "rain_detected INTEGER"
        ");";
    ASSERT_EQ(sqlite3_exec(db, createSql, nullptr, nullptr, nullptr), SQLITE_OK);
    sqlite3_close(db);

    SensorReadings r{20.0f, 50.0f, 1013.0f, 300.0f, 400.0f, 8.0f, false};
    EXPECT_FALSE(logger.logReading(r));

    std::filesystem::remove(dbPath);
}
