#include "binfile_writer.hpp"
#include <stdexcept>
#include <cstring>
#include <algorithm>

namespace BinFileUtils {

BinFileWriter::BinFileWriter(const std::string& type, uint32_t ver)
    : fileType(type), version(ver), writingSection(false), currentSectionType(0) {
    validateFileType(type);
}

void BinFileWriter::validateFileType(const std::string& type) {
    if (type.empty() || type.length() > 4) {
        throw std::invalid_argument("File type must be 1-4 characters long");
    }

    // Pad with null bytes if less than 4 characters
    fileType = type;
    while (fileType.length() < 4) {
        fileType += '\0';
    }
}

void BinFileWriter::startSection(uint32_t sectionType) {
    if (writingSection) {
        throw std::runtime_error("Already writing a section. Call endSection() first.");
    }

    writingSection = true;
    currentSectionType = sectionType;
    currentSectionBuffer.clear();
}

void BinFileWriter::writeU8(uint8_t value) {
    if (!writingSection) {
        throw std::runtime_error("No section started. Call startSection() first.");
    }
    currentSectionBuffer.push_back(value);
}

void BinFileWriter::writeU32LE(uint32_t value) {
    if (!writingSection) {
        throw std::runtime_error("No section started. Call startSection() first.");
    }

    currentSectionBuffer.push_back(value & 0xFF);
    currentSectionBuffer.push_back((value >> 8) & 0xFF);
    currentSectionBuffer.push_back((value >> 16) & 0xFF);
    currentSectionBuffer.push_back((value >> 24) & 0xFF);
}

void BinFileWriter::writeU32BE(uint32_t value) {
    if (!writingSection) {
        throw std::runtime_error("No section started. Call startSection() first.");
    }

    currentSectionBuffer.push_back((value >> 24) & 0xFF);
    currentSectionBuffer.push_back((value >> 16) & 0xFF);
    currentSectionBuffer.push_back((value >> 8) & 0xFF);
    currentSectionBuffer.push_back(value & 0xFF);
}

void BinFileWriter::writeU64LE(uint64_t value) {
    if (!writingSection) {
        throw std::runtime_error("No section started. Call startSection() first.");
    }

    for (int i = 0; i < 8; i++) {
        currentSectionBuffer.push_back((value >> (i * 8)) & 0xFF);
    }
}

void BinFileWriter::writeU64BE(uint64_t value) {
    if (!writingSection) {
        throw std::runtime_error("No section started. Call startSection() first.");
    }

    for (int i = 7; i >= 0; i--) {
        currentSectionBuffer.push_back((value >> (i * 8)) & 0xFF);
    }
}

void BinFileWriter::write(const void* data, size_t size) {
    if (!writingSection) {
        throw std::runtime_error("No section started. Call startSection() first.");
    }

    const uint8_t* byteData = static_cast<const uint8_t*>(data);
    currentSectionBuffer.insert(currentSectionBuffer.end(), byteData, byteData + size);
}

void BinFileWriter::writeString(const std::string& str) {
    write(str.data(), str.size());
}

void BinFileWriter::endSection() {
    if (!writingSection) {
        throw std::runtime_error("No section being written. Call startSection() first.");
    }

    sections.emplace_back(currentSectionType, currentSectionBuffer.data(), currentSectionBuffer.size());
    currentSectionBuffer.clear();
    writingSection = false;
}

void BinFileWriter::addSection(uint32_t sectionType, const void* data, size_t size) {
    if (writingSection) {
        throw std::runtime_error("Cannot add section while writing another section. Call endSection() first.");
    }

    sections.emplace_back(sectionType, data, size);
}

void BinFileWriter::addSection(uint32_t sectionType, const std::vector<uint8_t>& data) {
    if (writingSection) {
        throw std::runtime_error("Cannot add section while writing another section. Call endSection() first.");
    }

    sections.emplace_back(sectionType, data.data(), data.size());
}

void BinFileWriter::writeToFile(const std::string& filename) {
    std::vector<uint8_t> buffer = writeToBuffer();

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());

    if (file.fail()) {
        throw std::runtime_error("Error writing to file: " + filename);
    }
}

std::vector<uint8_t> BinFileWriter::writeToBuffer() {
    if (writingSection) {
        throw std::runtime_error("Cannot write file while writing a section. Call endSection() first.");
    }

    std::vector<uint8_t> buffer;

    // Calculate total size
    size_t totalSize = getFileSize();
    buffer.reserve(totalSize);

    // Write file header
    writeHeader(buffer);

    // Write sections with inline headers
    writeSectionsWithInlineHeaders(buffer);

    return buffer;
}

size_t BinFileWriter::getFileSize() const {
    size_t size = 12; // File header: 4 bytes type + 4 bytes version + 4 bytes section count

    // Each section: 12 bytes header (4 bytes type + 8 bytes size) + data size
    for (const auto& section : sections) {
        size += 12; // Section header (type + size)
        size += section.data.size(); // Section data
    }

    return size;
}

void BinFileWriter::clear() {
    if (writingSection) {
        writingSection = false;
        currentSectionBuffer.clear();
    }
    sections.clear();
}

void BinFileWriter::writeHeader(std::vector<uint8_t>& buffer) {
    // Write file type (4 bytes)
    for (size_t i = 0; i < 4; i++) {
        buffer.push_back(i < fileType.length() ? fileType[i] : 0);
    }

    // Write version (4 bytes, little-endian)
    writeLE(buffer, version);

    // Write number of sections (4 bytes, little-endian)
    writeLE(buffer, static_cast<uint32_t>(sections.size()));
}

void BinFileWriter::writeSectionsWithInlineHeaders(std::vector<uint8_t>& buffer) {
    for (const auto& section : sections) {
        // Write section type (4 bytes, little-endian)
        writeLE(buffer, section.type);

        // Write section size (8 bytes, little-endian)
        writeLE(buffer, static_cast<uint64_t>(section.data.size()));

        // Write section data immediately after header
        buffer.insert(buffer.end(), section.data.begin(), section.data.end());
    }
}

template<typename T>
void BinFileWriter::writeLE(std::vector<uint8_t>& buffer, T value) {
    for (size_t i = 0; i < sizeof(T); i++) {
        buffer.push_back((value >> (i * 8)) & 0xFF);
    }
}

} // namespace BinFileUtils
