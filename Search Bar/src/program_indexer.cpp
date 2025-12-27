#include "../include/program_indexer.h"
#include <windows.h>
#include <shlobj.h>
#include <algorithm>

static std::vector<ProgramEntry> programIndex;
static bool indexingComplete = false;

// Helper to convert string to lowercase
static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Get special folder path (like AppData, ProgramData)
static std::string getSpecialFolder(int csidl) {
    char path[MAX_PATH];
    if (SHGetFolderPathA(NULL, csidl, NULL, 0, path) == S_OK) {
        return std::string(path);
    }
    return "";
}

// Recursively search for .lnk and .exe files
static void searchDirectory(const std::string& dir, std::vector<ProgramEntry>& entries, int maxDepth = 3) {
    if (maxDepth <= 0) return;
    
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA((dir + "\\*").c_str(), &findData);
    
    if (hFind == INVALID_HANDLE_VALUE) return;
    
    do {
        std::string fileName = findData.cFileName;
        
        // Skip . and ..
        if (fileName == "." || fileName == "..") continue;
        
        std::string fullPath = dir + "\\" + fileName;
        
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // Recurse into subdirectory
            searchDirectory(fullPath, entries, maxDepth - 1);
        } else {
            // Check if it's a shortcut or executable
            size_t len = fileName.length();
            if (len > 4) {
                std::string ext = toLower(fileName.substr(len - 4));
                if (ext == ".lnk" || ext == ".exe" || ext == ".url") {
                    ProgramEntry entry;
                    // Remove extension from name
                    entry.name = fileName.substr(0, len - 4);
                    entry.path = fullPath;
                    entry.lowerName = toLower(entry.name);
                    entries.push_back(entry);
                }
            }
        }
    } while (FindNextFileA(hFind, &findData));
    
    FindClose(hFind);
}

// Get programs from PATH environment variable
static void indexPathPrograms(std::vector<ProgramEntry>& entries) {
    char* pathEnv = getenv("PATH");
    if (!pathEnv) return;
    
    std::string path = pathEnv;
    size_t start = 0;
    size_t end = path.find(';');
    
    while (end != std::string::npos) {
        std::string dir = path.substr(start, end - start);
        if (!dir.empty()) {
            searchDirectory(dir, entries, 1);
        }
        start = end + 1;
        end = path.find(';', start);
    }
    
    // Last directory
    if (start < path.length()) {
        std::string dir = path.substr(start);
        if (!dir.empty()) {
            searchDirectory(dir, entries, 1);
        }
    }
}

// Index Start Menu shortcuts
static void indexStartMenu(std::vector<ProgramEntry>& entries) {
    // Common Start Menu
    std::string commonStartMenu = getSpecialFolder(CSIDL_COMMON_PROGRAMS);
    if (!commonStartMenu.empty()) {
        searchDirectory(commonStartMenu, entries);
    }
    
    // User Start Menu
    std::string userStartMenu = getSpecialFolder(CSIDL_PROGRAMS);
    if (!userStartMenu.empty()) {
        searchDirectory(userStartMenu, entries);
    }
}

// Index common program directories
static void indexProgramDirectories(std::vector<ProgramEntry>& entries) {
    // Program Files - search deeper for games
    std::string programFiles = getSpecialFolder(CSIDL_PROGRAM_FILES);
    if (!programFiles.empty()) {
        searchDirectory(programFiles, entries, 4);  // Deeper search
    }
    
    // Program Files (x86) - most games are here
    char pf86[MAX_PATH];
    if (SHGetSpecialFolderPathA(NULL, pf86, CSIDL_PROGRAM_FILESX86, FALSE)) {
        searchDirectory(std::string(pf86), entries, 4);  // Deeper search
    }
    
    // User's local programs
    std::string localAppData = getSpecialFolder(CSIDL_LOCAL_APPDATA);
    if (!localAppData.empty()) {
        searchDirectory(localAppData + "\\Programs", entries, 3);
    }
    
    // Roaming AppData (some games install here)
    std::string roamingAppData = getSpecialFolder(CSIDL_APPDATA);
    if (!roamingAppData.empty()) {
        searchDirectory(roamingAppData, entries, 2);
    }
}

// Index common game launchers and directories
static void indexGameDirectories(std::vector<ProgramEntry>& entries) {
    // Steam common directories
    const char* steamPaths[] = {
        "C:\\Program Files (x86)\\Steam\\steamapps\\common",
        "C:\\Program Files\\Steam\\steamapps\\common",
        "D:\\SteamLibrary\\steamapps\\common",
        "E:\\SteamLibrary\\steamapps\\common"
    };
    
    for (const char* steamPath : steamPaths) {
        searchDirectory(steamPath, entries, 3);
    }
    
    // Steam shortcuts in Start Menu (user-specific)
    std::string userStartMenu = getSpecialFolder(CSIDL_PROGRAMS);
    if (!userStartMenu.empty()) {
        searchDirectory(userStartMenu + "\\Steam", entries, 2);
    }
    
    // Epic Games
    searchDirectory("C:\\Program Files\\Epic Games", entries, 3);
    
    // GOG Galaxy
    searchDirectory("C:\\Program Files (x86)\\GOG Galaxy\\Games", entries, 3);
    searchDirectory("C:\\GOG Games", entries, 3);
    
    // Origin
    searchDirectory("C:\\Program Files (x86)\\Origin Games", entries, 3);
    
    // Ubisoft Connect
    searchDirectory("C:\\Program Files (x86)\\Ubisoft\\Ubisoft Game Launcher\\games", entries, 3);
    
    // Xbox/Microsoft Store games
    std::string programFiles = getSpecialFolder(CSIDL_PROGRAM_FILES);
    if (!programFiles.empty()) {
        searchDirectory(programFiles + "\\WindowsApps", entries, 3);
    }
    
    // Battle.net
    searchDirectory("C:\\Program Files (x86)\\Battle.net", entries, 3);
    
    // Riot Games
    searchDirectory("C:\\Riot Games", entries, 3);
}

// Build the complete index
void buildProgramIndex() {
    programIndex.clear();
    indexingComplete = false;
    
    indexStartMenu(programIndex);
    indexPathPrograms(programIndex);
    indexProgramDirectories(programIndex);
    indexGameDirectories(programIndex);  // Add game directories
    
    // Remove duplicates (same name)
    std::sort(programIndex.begin(), programIndex.end(), 
        [](const ProgramEntry& a, const ProgramEntry& b) {
            return a.lowerName < b.lowerName;
        });
    
    auto last = std::unique(programIndex.begin(), programIndex.end(),
        [](const ProgramEntry& a, const ProgramEntry& b) {
            return a.lowerName == b.lowerName;
        });
    
    programIndex.erase(last, programIndex.end());
    
    indexingComplete = true;
}

// Check if indexing is complete
bool isIndexingComplete() {
    return indexingComplete;
}

// Search for programs matching query
std::vector<ProgramEntry> searchPrograms(const std::string& query) {
    std::vector<ProgramEntry> results;
    if (query.empty() || !indexingComplete) return results;
    
    std::string lowerQuery = toLower(query);
    
    for (const auto& entry : programIndex) {
        if (entry.lowerName.find(lowerQuery) != std::string::npos) {
            results.push_back(entry);
            if (results.size() >= 10) break;
        }
    }
    
    return results;
}

// Launch a program
bool launchProgram(const std::string& path) {
    SHELLEXECUTEINFOA sei = { sizeof(sei) };
    sei.lpVerb = "open";
    sei.lpFile = path.c_str();
    sei.nShow = SW_SHOW;
    
    return ShellExecuteExA(&sei);
}

// Get total number of indexed programs
int getIndexedProgramCount() {
    return programIndex.size();
}