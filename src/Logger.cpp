#include "Logger.hpp"
#include <sqlite3.h>
#include <iostream>
#include <sstream>
#include <iomanip>

Logger::Logger(const std::string& dbPath) : dbPath(dbPath), db(nullptr)
{
}

Logger::~Logger()
{
    if (db)
    {
        sqlite3_close(static_cast<sqlite3*>(db));
    }
}

bool Logger::init()
{
    if (!db) {
        if (sqlite3_open(dbPath.c_str(), reinterpret_cast<sqlite3**>(&db)) != SQLITE_OK)
        {
            std::cerr << "[Logger] Blad otwarcia bazy danych: " << sqlite3_errmsg(static_cast<sqlite3*>(db)) << std::endl;
            return false;
        }
    }

    if (!execute("PRAGMA journal_mode=WAL;") or !execute("PRAGMA synchronous=NORMAL;")) 
    {
        return false;
    }

    std::string createTable = R"SQL(
        CREATE TABLE IF NOT EXISTS sensor_logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            temperature REAL,
            humidity REAL,
            pressure REAL,
            light REAL,
            soil_moisture REAL,
            grass_height REAL,
            rain_detected INTEGER
        );
    )SQL";

    if (!execute(createTable)) {
        return false;
    }

    return true;
}

bool Logger::logReading(const SensorReadings& reading)
{
    if (!db) 
    {
        std::cerr << "[Logger] Brak otwartej bazy danych" << std::endl;
        return false;
    }

    const char* sql = R"SQL(
        INSERT INTO sensor_logs (temperature, humidity, pressure, light, soil_moisture, grass_height, rain_detected)
        VALUES (?, ?, ?, ?, ?, ?, ?);
    )SQL";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(static_cast<sqlite3*>(db), sql, -1, &stmt, nullptr) != SQLITE_OK) 
    {
        std::cerr << "[Logger] SQLite prepare blad: " << sqlite3_errmsg(static_cast<sqlite3*>(db)) << std::endl;
        return false;
    }

    sqlite3_bind_double(stmt, 1, reading.temperature);
    sqlite3_bind_double(stmt, 2, reading.humidity);
    sqlite3_bind_double(stmt, 3, reading.pressure);
    sqlite3_bind_double(stmt, 4, reading.light);
    sqlite3_bind_double(stmt, 5, reading.soilMoisture);
    sqlite3_bind_double(stmt, 6, reading.grassHeight);
    sqlite3_bind_int   (stmt, 7, reading.rainDetected ? 1 : 0);

    bool ok = true;
    if (sqlite3_step(stmt) != SQLITE_DONE) 
    {
        std::cerr << "[Logger] SQLite step blad: " << sqlite3_errmsg(static_cast<sqlite3*>(db)) << std::endl;
        ok = false;
    }

    sqlite3_finalize(stmt);
    return ok;
}

bool Logger::execute(const std::string& sql)
{
    char* errMsg = nullptr;
    if (sqlite3_exec(static_cast<sqlite3*>(db), sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << "[Logger] SQLite blad: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}
