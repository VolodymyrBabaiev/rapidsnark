#ifndef BINFILE_WRITER_H
#define BINFILE_WRITER_H

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <cstdint>

namespace BinFileUtils {

    class BinFileWriter {
    public:
        struct SectionData {
            uint32_t type;
            std::vector<uint8_t> data;

            SectionData(uint32_t t) : type(t) {}
            SectionData(uint32_t t, const void* ptr, size_t size)
                : type(t), data(static_cast<const uint8_t*>(ptr),
                               static_cast<const uint8_t*>(ptr) + size) {}
        };

    private:
        std::string fileType;
        uint32_t version;
        std::vector<SectionData> sections;
        std::vector<uint8_t> currentSectionBuffer;
        bool writingSection;
        uint32_t currentSectionType;

    public:
        /**
         * Constructor
         * @param type File type identifier (4 characters max)
         * @param ver File version
         */
        BinFileWriter(const std::string& type, uint32_t ver);

        /**
         * Start writing a new section
         * @param sectionType Type identifier for this section
         */
        void startSection(uint32_t sectionType);

        /**
         * Write a single byte to current section
         */
        void writeU8(uint8_t value);

        /**
         * Write a 32-bit little-endian integer to current section
         */
        void writeU32LE(uint32_t value);

        /**
         * Write a 32-bit big-endian integer to current section
         */
        void writeU32BE(uint32_t value);

        /**
         * Write a 64-bit little-endian integer to current section
         */
        void writeU64LE(uint64_t value);

        /**
         * Write a 64-bit big-endian integer to current section
         */
        void writeU64BE(uint64_t value);

        /**
         * Write raw bytes to current section
         */
        void write(const void* data, size_t size);

        /**
         * Write a string to current section (without null terminator)
         */
        void writeString(const std::string& str);

        /**
         * Finish current section and add it to the file
         */
        void endSection();

        /**
         * Add a complete section at once
         * @param sectionType Type identifier for this section
         * @param data Pointer to section data
         * @param size Size of section data
         */
        void addSection(uint32_t sectionType, const void* data, size_t size);

        /**
         * Add a complete section from vector
         */
        void addSection(uint32_t sectionType, const std::vector<uint8_t>& data);

        /**
         * Write the complete file to disk
         * @param filename Output filename
         */
        void writeToFile(const std::string& filename);

        /**
         * Write the complete file to a buffer
         * @return Vector containing the complete file data
         */
        std::vector<uint8_t> writeToBuffer();

        /**
         * Get current number of sections
         */
        size_t getSectionCount() const { return sections.size(); }

        /**
         * Clear all sections and reset
         */
        void clear();

        /**
         * Get total file size that would be written
         */
        size_t getFileSize() const;

    private:
        void validateFileType(const std::string& type);
        void writeHeader(std::vector<uint8_t>& buffer);
        void writeSectionsWithInlineHeaders(std::vector<uint8_t>& buffer);

        template<typename T>
        void writeLE(std::vector<uint8_t>& buffer, T value);
    };

} // namespace BinFileUtils

#endif // BINFILE_WRITER_H
