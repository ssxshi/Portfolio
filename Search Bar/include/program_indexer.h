#ifndef PROGRAM_INDEXER_H
#define PROGRAM_INDEXER_H

#include <string>
#include <vector>

struct ProgramEntry {
    std::string name;
    std::string path;
    std::string lowerName;
};

// Build the program index (call once at startup, preferably in background thread)
void buildProgramIndex();

// Check if indexing is complete
bool isIndexingComplete();

// Search for programs matching the query (returns up to 10 results)
std::vector<ProgramEntry> searchPrograms(const std::string& query);

// Launch a program by its path
bool launchProgram(const std::string& path);

// Get total number of indexed programs
int getIndexedProgramCount();

#endif // PROGRAM_INDEXER_H