#include "Config.h"

#include <fstream>
#include <filesystem>

Config::Config(const char* fileName) {
    namespace fs = std::filesystem;

    if (!fs::exists(fileName)) {
        std::ofstream file{ fileName };

        m_JSON["fps_param"] = 119.88f;
        m_JSON["offsets"]["toStruct"] = 0x10ed230;
        m_JSON["offsets"]["toFPSParam"] = 0xe8;
        file << m_JSON.dump(4);
    } else {
        std::ifstream file{fileName};
        file >> m_JSON;
    }
}
