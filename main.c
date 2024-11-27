#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define DATA_SIZE 4096

// Размер ECC памяти в байтах (7 бит данных + 3 бита четности = 1 байт)
#define ECC_SIZE ((DATA_SIZE * 8 + 6) / 7)

// Структура для менеджера памяти
typedef struct {
    uint8_t data[DATA_SIZE];
    uint8_t ecc[ECC_SIZE];
    uint8_t bitmap[DATA_SIZE / 4]; // Bitmap для отслеживания выделенных блоков
} MemoryManager;

// Функция для вычисления бита четности
static inline uint8_t parity(uint8_t x) {
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;
    return x & 1;
}

// Функция для вычисления ECC для 4 бит данных
void ec_calculate(uint8_t *data, uint8_t *ecc) {
    uint8_t p1 = parity(data[0] & 0x01) ^ parity(data[1] & 0x01) ^ parity(data[3] & 0x01);
    uint8_t p2 = parity(data[0] & 0x01) ^ parity(data[2] & 0x01) ^ parity(data[3] & 0x01);
    uint8_t p3 = parity(data[1] & 0x01) ^ parity(data[2] & 0x01) ^ parity(data[3] & 0x01);

    *ecc = (p1 << 2) | (p2 << 1) | p3;
}

// Функция для проверки и исправления ошибок
void ecc_check_and_correct(uint8_t *data, uint8_t *ecc) {
    uint8_t p1 = parity(data[0] & 0x01) ^ parity(data[1] & 0x01) ^ parity(data[3] & 0x01) ^ ((*ecc >> 2) & 0x01);
    uint8_t p2 = parity(data[0] & 0x01) ^ parity(data[2] & 0x01) ^ parity(data[3] & 0x01) ^ ((*ecc >> 1) & 0x01);
    uint8_t p3 = parity(data[1] & 0x01) ^ parity(data[2] & 0x01) ^ parity(data[3] & 0x01) ^ (*ecc & 0x01);

    uint8_t syndrome = (p1 << 2) | (p2 << 1) | p3;

    if (syndrome != 0) {
        // Исправление ошибки
        data[syndrome - 1] ^= 0x01;
        printf("Error corrected at bit position %d.\n", syndrome - 1);
    }
}

// Функция для выделения памяти
int ecc_allocate(MemoryManager *mm) {
    for (int i = 0; i < DATA_SIZE / 4; i++) {
        if (mm->bitmap[i] == 0) {
            mm->bitmap[i] = 1;
            printf("Allocated block at index %d.\n", i * 4);
            return i * 4;
        }
    }
    return -1; // Нет свободных блоков
}

// Функция для освобождения памяти
void ecc_deallocate(MemoryManager *mm, int index) {
    if (index >= 0 && index < DATA_SIZE && mm->bitmap[index / 4] == 1) {
        mm->bitmap[index / 4] = 0;
        printf("Deallocated block at index %d.\n", index);
    }
}

// Функция для записи данных
void ecc_write(MemoryManager *mm, int index, uint8_t *data, int size) {
    if (index < 0 || index + size > DATA_SIZE) return;

    printf("Writing data: ");
    for (int i = 0; i < size; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");

    memcpy(&mm->data[index], data, size);

    for (int i = 0; i < size * 8; i += 7) {
        uint8_t bits[4];
        int ecc_index = (index * 8 + i) / 7;
        if (ecc_index >= ECC_SIZE) break;

        for (int j = 0; j < 4; j++) {
            int bit_pos = i + j;
            if (bit_pos >= size * 8) break;
            bits[j] = (data[bit_pos / 8] >> (bit_pos % 8)) & 0x01;
        }

        ec_calculate(bits, &mm->ecc[ecc_index]);
        printf("ECC value at index %d: %02X\n", ecc_index, mm->ecc[ecc_index]);
    }
}

// Функция для чтения данных
void ecc_read(MemoryManager *mm, int index, uint8_t *data, int size) {
    if (index < 0 || index + size > DATA_SIZE) return;

    printf("Reading data...\n");
    memcpy(data, &mm->data[index], size);

    for (int i = 0; i < size * 8; i += 7) {
        uint8_t bits[4];
        int ecc_index = (index * 8 + i) / 7;
        if (ecc_index >= ECC_SIZE) break;

        for (int j = 0; j < 4; j++) {
            int bit_pos = i + j;
            if (bit_pos >= size * 8) break;
            bits[j] = (data[bit_pos / 8] >> (bit_pos % 8)) & 0x01;
        }

        ecc_check_and_correct(bits, &mm->ecc[ecc_index]);

        for (int j = 0; j < 4; j++) {
            int bit_pos = i + j;
            if (bit_pos >= size * 8) break;
            data[bit_pos / 8] = (data[bit_pos / 8] & ~(0x01 << (bit_pos % 8))) | (bits[j] << (bit_pos % 8));
        }
    }

    printf("Data after error correction: ");
    for (int i = 0; i < size; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

int main() {
    MemoryManager *mm = malloc(sizeof(MemoryManager));
    if (mm == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }
    memset(mm, 0, sizeof(MemoryManager));
    printf("MemoryManager allocated.\n");

    // Выделение памяти
    int index = ecc_allocate(mm);
    if (index == -1) {
        printf("Memory allocation failed.\n");
        free(mm);
        return 1;
    }

    // Запись данных
    uint8_t data[4] = {0x12, 0x34, 0x56, 0x78};
    ecc_write(mm, index, data, 4);

    // Моделирование ошибки (например, инвертируем один бит)
    mm->data[index + 2] ^= 0x01;
    printf("Error introduced at position %d.\n", index + 2);
    for (int i = 0; i < 4; i++) {
        printf("%02X ",  mm->data[i]);
    }
    printf("\n");

    // Чтение данных
    uint8_t read_data[4];
    ecc_read(mm, index, read_data, 4);

    // Проверка, что ошибка была исправлена
    if (memcmp(data, read_data, 4) == 0) {
        printf("Error fixed!\n");
    } else {
        printf("Fatal error.\n");
    }

    // Освобождение памяти
    ecc_deallocate(mm, index);

    free(mm);
    return 0;
}