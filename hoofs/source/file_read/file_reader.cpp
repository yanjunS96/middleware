//
// Created by syj on 2023/2/23.
//
#include "file_read/file_reader.hpp"
#include "logger/mlog.h"

using namespace middleware::hoofs;

constexpr char FileReader::PATH_SEPARATOR[];

FileReader::FileReader(const std::string &f_filename, const std::string &f_filePath) noexcept
{
    m_file = f_filePath.empty()?f_filename:f_filePath+PATH_SEPARATOR+f_filename;
    m_fileStream.open(m_file, std::fstream::in);

    if(!isOpen())
    {
        MLOG_ERROR("file open failed! file_name: {}", f_filename);
    }
}

bool FileReader::isOpen() const noexcept
{
    return m_fileStream.is_open();
}

bool FileReader::readLine(std::string &f_string) noexcept
{
    return static_cast<bool>(std::getline(m_fileStream, f_string));
}
