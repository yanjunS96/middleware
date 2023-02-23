//
// Created by syj on 2023/2/23.
//

#ifndef MIDDLEWARE_FILE_READER_HPP
#define MIDDLEWARE_FILE_READER_HPP

#include <fstream>

namespace middleware{
    namespace hoofs{
        class FileReader{
        public:
            FileReader(const std::string& f_filename,
                       const std::string& f_filePath = "") noexcept;
            FileReader(const FileReader&) = delete;
            FileReader(const FileReader&&) = delete;
            FileReader& operator=(const FileReader&) = delete;
            FileReader& operator=(FileReader&&) = delete;

            ~FileReader()noexcept;
            bool isOpen() const noexcept;
            bool readLine(std::string & f_string) noexcept;

        private:
            static constexpr char PATH_SEPARATOR[] = "/"; // _WIN32 "\\"
            std::fstream m_fileStream;
            std::string m_file;
        };
    }
}

#endif //MIDDLEWARE_FILE_READER_HPP
