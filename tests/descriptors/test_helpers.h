/**
* @file test_helpers.h
* @brief Shared helpers for the per-descriptor Catch2 test files.
* @see https://github.com/Puara/puara-gestures
*/
#pragma once

#include <rapidcsv.h>

#include <filesystem>
#include <string>
#include <string_view>

// Read a single double cell from a rapidcsv document.
inline double
readCsvDouble(const rapidcsv::Document& doc, const std::string& column, size_t row)
{
  return doc.GetCell<double>(column, row);
}

// Resolve a file under tests/data/ relative to this header's location
// (tests/descriptors/test_helpers.h -> tests/data/<filename>).
inline std::filesystem::path getTestDataPath(std::string_view filename)
{
  const auto sourceDir = std::filesystem::path(__FILE__).parent_path().parent_path();
  return sourceDir / "data" / filename;
}
