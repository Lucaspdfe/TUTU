#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

//
// FAT 12 Allocation:
// | Reserved | File Allocation Tables | Root directory | Data |
// Size of each part: (measured in sectors)
//   Reserved = bdb_reserved_sectors = 1;
//   File Allocation Tables = bdb_fat_count * bdb_sectors_per_fat = 18;
//   Root directory = (bdb_dir_entries_count * 32 + bdb_bytes_per_sector - 1)/ bdb_bytes_per_sector = 14
//

//
// Reading files in FAT 12:
//   lba = (Reserved + File Allocation Tables + Root directory) + (cluster - 2) * bdb_sectors_per_cluster
//

typedef struct {
    char     bootJumpInstruction[3]; // Jmp short and nop instructions
    char     oem[8];                 // "MSWIN4.1"
    uint16_t bytesPerSector;         // 0x0200
    uint8_t  sectorsPerCluster;      // 0x01
    uint16_t reservedSectors;        // 0x0001
    uint8_t  fatCount;               // 0x02
    uint16_t dirEntriesCount;        // 0x00E0
    uint16_t totalSectors;           // 0x0B40
    uint8_t  mediaDescriptorType;    // 0xF0
    uint16_t sectorsPerFat;          // 0x0009
    uint16_t sectorsPerTrack;        // 0x0012
    uint16_t heads;                  // 0x0002
    uint32_t hiddenSectors;          // 0x00000000
    uint32_t largeSectorCount;       // 0x00000000

    // Extended Boot Record (EBR)
    uint8_t  driveNumber;            // 0x00
    uint8_t  reserved;               // 0x00
    uint8_t  signature;              // 0x29
    uint8_t  volumeId[4];            // 0x26, 0x12, 0x20, 0x13
    char     volumeLabel[11];        // "TUTU OS    "
    char     systemId[8];            // "FAT12   "
} __attribute__((packed)) BootSector;

typedef struct {
    uint8_t name[11];
    uint8_t attributes;
    uint8_t _reserved;
    uint8_t createdTimeTenths;
    uint16_t createdTime;
    uint16_t createdDate;
    uint16_t accessDate;
    uint16_t firstClusterHigh;
    uint16_t modifiedTime;
    uint16_t modifiedDate;
    uint16_t firstClusterLow;
    uint32_t size;
} __attribute__((packed)) DirectoryEntry;

BootSector g_BootSector;
uint8_t* g_Fat = NULL;
DirectoryEntry* g_rootDirectory = NULL;
uint32_t g_rootDirectoryEnd;

bool readBootSector(FILE* disk) {
    return fread(&g_BootSector, sizeof(g_BootSector), 1, disk) > 0;
}

bool readSectors(FILE* disk, uint32_t lba, uint32_t count, void* bufferOut) {
    bool ok = true;
    ok = ok && (fseek(disk, lba * g_BootSector.bytesPerSector, SEEK_SET) == 0);
    ok = ok && (fread(bufferOut, g_BootSector.bytesPerSector, count, disk) == count);
    return ok;
}

bool readFat(FILE* disk) {
    g_Fat = (uint8_t*)malloc(g_BootSector.sectorsPerFat * g_BootSector.bytesPerSector);
    return readSectors(disk, g_BootSector.reservedSectors, g_BootSector.sectorsPerFat, g_Fat);
}

bool readRootDirectory(FILE* disk) {
    uint32_t lba = g_BootSector.reservedSectors + (g_BootSector.fatCount * g_BootSector.sectorsPerFat);
    uint32_t size = sizeof(DirectoryEntry) * g_BootSector.dirEntriesCount;
    uint32_t sectors = size / g_BootSector.bytesPerSector;
    if (size % g_BootSector.bytesPerSector > 0) {
        sectors++;
    }

    g_rootDirectoryEnd = lba + sectors;
    g_rootDirectory = (DirectoryEntry*)malloc(size);
    return readSectors(disk, lba, sectors, g_rootDirectory);
}

DirectoryEntry* findFile(const char* name) {
    for (uint32_t i = 0; i < g_BootSector.dirEntriesCount; i++) {
        if (memcmp(name, g_rootDirectory[i].name, 11) == 0) {
            return &g_rootDirectory[i];
        }
    }
    return NULL;
}

bool readFile(DirectoryEntry* fileEntry, FILE* disk, uint8_t* outputBuffer) {
    bool ok = true;
    uint32_t currentCluster = fileEntry->firstClusterLow;

    do {
        uint32_t lba = g_rootDirectoryEnd + (currentCluster - 2) * g_BootSector.sectorsPerCluster;
        ok = ok && readSectors(disk, lba, g_BootSector.sectorsPerCluster, outputBuffer);
        outputBuffer += g_BootSector.sectorsPerCluster * g_BootSector.bytesPerSector;

        uint32_t fatIndex = currentCluster * 3 / 2;
        if (currentCluster % 2 == 0) {
            currentCluster = (*(uint16_t*)(g_Fat + fatIndex)) & 0x0FFF;
        } else {
            currentCluster = (*(uint16_t*)(g_Fat + fatIndex)) >> 4;
        }

    } while(ok && currentCluster < 0x0FF8);

    return ok;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Syntax: %s <disk image> <file name>\n", argv[0]);
        return -1;
    }

    FILE* disk = fopen(argv[1], "rb");
    if (!disk) { 
        fprintf(stderr, "Cannot open disk image %s!\n", argv[1]); 
        return -1; 
    }

    if (!readBootSector(disk)) {
        fprintf(stderr, "Couldn't read boot sector!\n");
        return -2;
    }

    if (!readFat(disk)) {
        fprintf(stderr, "Couldn't read FAT!\n");
        free(g_Fat);
        return -3;
    }

    if (!readRootDirectory(disk)) {
        fprintf(stderr, "Couldn't read root directory!\n");
        free(g_rootDirectory);
        free(g_Fat);
        return -4;
    }

    DirectoryEntry* fileEntry = findFile(argv[2]);
    if (!fileEntry) {
        fprintf(stderr, "Couldn't find %s!\n", argv[2]);
        free(g_rootDirectory);
        free(g_Fat);
        return -5;
    }

    uint8_t* buffer = (uint8_t*)malloc(fileEntry->size + g_BootSector.bytesPerSector);
    if (!readFile(fileEntry, disk, buffer)) {
        fprintf(stderr, "Couldn't read %s!\n", argv[2]);
        free(g_rootDirectory);
        free(g_Fat);
        free(buffer);
        return -6;
    }

    for (size_t i = 0; i < fileEntry->size; i++) {
        if (isprint(buffer[i])) fputc(buffer[i], stdout);
        else printf("<%02x>", buffer[i]);
    }
    printf("\n");

    free(g_rootDirectory);
    free(g_Fat);
    free(buffer);
    return 0;
}