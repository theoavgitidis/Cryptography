#include <stdio.h> // Erlaubt für printf

// --- Typ-Definitionen (statt stdint.h) ---
// Wir gehen davon aus, dass 'unsigned long long' 64 Bit
// und 'unsigned int' 32 Bit hat.
typedef unsigned long long uint64;
typedef unsigned int       uint32;

/*
 * Die folgenden Tabellen sind der 1-zu-1-Standard für DES.
 * Sie sind 1-indiziert (Bit 1 ist das MSB, Bit 64 das LSB),
 * was wir in unseren Permutationsfunktionen berücksichtigen müssen.
 */

// --- 1. Permutationstabellen ---

// Initial Permutation (IP) - 64 Bit
static const int IP[64] = {
    58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9,  1, 59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7
};

// Final Permutation (IP-1) - 64 Bit
static const int FP[64] = {
    40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41,  9, 49, 17, 57, 25
};

// --- 2. Schlüsselerzeugungs-Tabellen ---

// Permuted Choice 1 (PC-1) - 64 -> 56 Bits
static const int PC1[56] = {
    57, 49, 41, 33, 25, 17, 9,  1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4
};

// Permuted Choice 2 (PC-2) - 56 -> 48 Bits
static const int PC2[48] = {
    14, 17, 11, 24, 1,  5,  3,  28, 15, 6,  21, 10,
    23, 19, 12, 4,  26, 8,  16, 7,  27, 20, 13, 2,
    41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};

// Anzahl der Links-Shifts pro Runde für C und D
static const int key_shifts[16] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

// --- 3. Feistel-Funktion (f) Tabellen ---

// Expansion (E-Tabelle) - 32 -> 48 Bits
static const int E_table[48] = {
    32, 1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
    8,  9,  10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1
};

// S-Boxen (Substitution) - [8 S-Boxen][4 Zeilen][16 Spalten]
// Dies ist das nicht-lineare Herzstück von DES.
static const int S_boxes[8][4][16] = {
    // S1
    {{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
     {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
     {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
     {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},
    // S2
    {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
     {3, 13, 4, 7, 15, 5, 8, 14, 12, 0, 1, 10, 6, 9, 11, 2},
     {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
     {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
    // S3
    {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
     {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
     {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
     {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},
    // S4
    {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
     {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
     {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
     {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},
    // S5
    {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
     {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
     {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
     {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},
    // S6
    {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
     {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
     {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
     {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},
    // S7
    {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
     {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
     {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
     {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
    // S8
    {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
     {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
     {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
     {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}
};

// Permutation P-Box - 32 Bit
static const int P_box[32] = {
    16, 7,  20, 21, 29, 12, 28, 17, 1,  15, 23, 26, 5,  18, 31, 10,
    2,  8,  24, 14, 32, 27, 3,  9,  19, 13, 30, 6,  22, 11, 4,  25
};


// --- Hilfsfunktion: Bit-Permutation ---

/**
 * @brief Führt eine Bit-Permutation auf einem 'input' aus.
 *
 * @param input     Der Eingabewert.
 * @param table     Die zu verwendende Permutationstabelle (1-indiziert).
 * @param in_width  Die Bitbreite des 'input' (z.B. 64).
 * @param out_width Die Bitbreite der 'output' (z.B. 56 für PC1).
 * @return Der permutierte Wert.
 */
uint64 permute_bits(uint64 input, const int *table, int in_width, int out_width) {
    uint64 output = 0;
    for (int i = 0; i < out_width; i++) {
        int bit_pos = table[i]; // 1-indizierte Bit-Position
        
        // Finde das Bit im Input. (in_width - bit_pos) rechnet 1-indiziert (MSB)
        // in 0-indiziert (LSB) um, wenn man von rechts zählt.
        // Einfacher: (in_width - bit_pos) ist der Shift von rechts.
        // Beispiel: Bit 1 (MSB) in 64-bit: (64-1) = 63. Shift um 63.
        // Korrektur: Die Konvention ist MSB=1. Also Bit 1 ist (1ULL << (in_width - 1)).
        // Der Shift von rechts ist (in_width - bit_pos).
        if ((input >> (in_width - bit_pos)) & 1ULL) {
            
            // Setze das entsprechende Bit im Output
            // (out_width - 1 - i) ist der Shift von rechts für das i-te Output-Bit (MSB)
            output |= (1ULL << (out_width - 1 - i));
        }
    }
    return output;
}


// --- Schlüsselerzeugung (Key Schedule) ---

/**
 * @brief Erzeugt die 16 48-Bit-Rundenschlüssel aus einem 64-Bit-Hauptschlüssel.
 * @param key       Der 64-Bit-Hauptschlüssel.
 * @param subkeys   Ein Array, das die 16 Rundenschlüssel speichern wird.
 */
void generate_keys(uint64 key, uint64 subkeys[16]) {
    // 1. PC-1 (64 -> 56 Bits)
    // Beachte: 64-Bit-Schlüssel enthalten Paritätsbits, die hier entfernt werden.
    uint64 permuted_key = permute_bits(key, PC1, 64, 56);

    // 2. Aufteilen in 28-Bit C und D Hälften
    // 0x0FFFFFFF ist die Maske für 28 Bits (28x '1')
    uint32 C = (uint32)(permuted_key >> 28);
    uint32 D = (uint32)(permuted_key & 0x0FFFFFFF);

    // 3. 16 Runden rotieren und PC-2 anwenden
    for (int i = 0; i < 16; i++) {
        // Rotieren C und D
        int shift = key_shifts[i];
        
        // C
        C = ((C << shift) | (C >> (28 - shift))) & 0x0FFFFFFF;
        // D
        D = ((D << shift) | (D >> (28 - shift))) & 0x0FFFFFFF;

        // C und D kombinieren
        uint64 C_D = (((uint64)C) << 28) | D;

        // 4. PC-2 (56 -> 48 Bits)
        // Erzeugt den finalen Rundenschlüssel
        subkeys[i] = permute_bits(C_D, PC2, 56, 48);
    }
}


// --- Die Feistel-Funktion (f) ---
// Dies ist die von Ihnen gewünschte abstrahierte Funktion.

/**
 * @brief Die DES Feistel-Funktion (f).
 *
 * @param R         Die 32-Bit-Rechtshälfte des Blocks.
 * @param subkey    Der 48-Bit-Rundenschlüssel für diese Runde.
 * @return Das 32-Bit-Ergebnis der f-Funktion.
 */
uint32 feistel_f(uint32 R, uint64 subkey) {
    
    // 1. Expansion (E-Tabelle): 32 -> 48 Bits
    uint64 expanded_R = permute_bits(R, E_table, 32, 48);

    // 2. XOR mit Rundenschlüssel
    // Das expandierte R wird mit dem 48-Bit-Rundenschlüssel XOR-verknüpft
    uint64 xor_result = expanded_R ^ subkey;

    // 3. S-Box-Substitution: 48 -> 32 Bits
    uint32 s_box_output = 0;
    for (int i = 0; i < 8; i++) {
        // Extrahiere den 6-Bit-Block für die i-te S-Box
        // (48 - 6 * (i + 1)) ist der Shift von rechts
        int chunk = (xor_result >> (48 - 6 * (i + 1))) & 0x3F; // 0x3F = 111111

        // S-Box-Lookup:
        // Zeile: Bit 1 und 6 (die äußeren Bits)
        // (chunk >> 5) ist Bit 1. (chunk & 0x01) ist Bit 6.
        int row = ((chunk & 0x20) >> 4) | (chunk & 0x01); // (Bit_1 << 1) | Bit_6
        
        // Spalte: Bits 2, 3, 4, 5 (die inneren Bits)
        int col = (chunk >> 1) & 0x0F; // 0x0F = 1111

        // Hole den 4-Bit-Wert aus der S-Box
        int val = S_boxes[i][row][col];

        // Füge den 4-Bit-Wert an das 32-Bit-Ergebnis an
        // (32 - 4 * (i + 1)) ist der Shift von rechts
        s_box_output |= (val << (32 - 4 * (i + 1)));
    }

    // 4. Permutation (P-Box): 32 -> 32 Bits
    // Das Ergebnis der S-Boxen wird final permutiert.
    uint32 final_result = (uint32)permute_bits(s_box_output, P_box, 32, 32);

    return final_result;
}


// --- Haupt-DES-Funktionen ---

/**
 * @brief Führt den Kern-DES-Prozess (16 Runden) aus.
 * @param plaintext Der 64-Bit-Klartextblock.
 * @param subkeys   Das Array der 16 Rundenschlüssel.
 * @param decrypt   0 für Verschlüsselung, 1 für Entschlüsselung.
 * @return Der 64-Bit-Chiffretext oder Klartext.
 */
uint64 des_process(uint64 block, uint64 subkeys[16], int mode_decrypt) {
    
    // 1. Initial Permutation (IP)
    uint64 permuted_block = permute_bits(block, IP, 64, 64);

    // 2. Aufteilen in L (32 Bit) und R (32 Bit)
    uint32 L = (uint32)(permuted_block >> 32);
    uint32 R = (uint32)(permuted_block & 0xFFFFFFFF);

    // 3. 16 Runden des Feistel-Netzwerks
    for (int i = 0; i < 16; i++) {
        // Bestimme den zu verwendenden Schlüssel
        // Verschlüsselung: 0, 1, ..., 15
        // Entschlüsselung: 15, 14, ..., 0
        int key_index = mode_decrypt ? (15 - i) : i;
        uint64 K = subkeys[key_index];

        // Feistel-Struktur
        uint32 R_next = L ^ feistel_f(R, K);
        L = R;
        R = R_next;
    }

    // 4. Finaler Swap (L und R nach 16 Runden tauschen)
    // Wir kombinieren (R, L) statt (L, R)
    uint64 pre_final_block = ((uint64)R << 32) | L;

    // 5. Final Permutation (IP-1)
    uint64 final_block = permute_bits(pre_final_block, FP, 64, 64);

    return final_block;
}


// --- Hauptprogramm (Main) ---

int main() {
    // Ein Standard-DES-Testvektor
    // https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldkey_1/index.html
    
    // Klartext: "Now is the time for all " (nur die ersten 8 Bytes)
    // ASCII: N o w ' ' i s ' '
    uint64 plaintext = 0x4E6F772069732074; // (Hex für den String)
    
    // Schlüssel: "some key"
    uint64 key       = 0x736F6D65206B6579; // (Hex für den String)

    // Erwarteter Chiffretext (NIST-Testvektor)
    // uint64 expected_ciphertext = 0x5de29a6b99183b54;
    // Anmerkung: Der obige Oracle-Testvektor ist für TDEA (3DES).
    
    // Wir verwenden einen bekannteren, einfacheren Vektor:
    // Klartext: 0x0123456789ABCDEF
    // Schlüssel: 0x133457799BBCDFF1
    // Chiffretext: 0x85E813540F0AB405
    
    uint64 test_plaintext  = 0x0123456789ABCDEF;
    uint64 test_key        = 0x133457799BBCDFF1;
    uint64 expected_cipher = 0x85E813540F0AB405;

    printf("--- DES (Data Encryption Standard) Implementierung ---\n");
    printf("WARNUNG: Dieser Algorithmus ist unsicher. Nur zu Bildungszwecken.\n\n");
    
    printf("Klartext:  %016llx\n", test_plaintext);
    printf("Schlüssel: %016llx\n", test_key);

    // 1. Schlüsselerzeugung
    uint64 subkeys[16];
    generate_keys(test_key, subkeys);
    printf("\nSchlüsselerzeugung abgeschlossen.\n");
    // (Optional: Rundenschlüssel ausgeben)
    // printf("Runde 0 Schlüssel: %012llx\n", subkeys[0]);
    // printf("Runde 15 Schlüssel: %012llx\n", subkeys[15]);

    // 2. Verschlüsselung
    uint64 ciphertext = des_process(test_plaintext, subkeys, 0); // mode = 0 (encrypt)
    printf("\n--- Verschlüsselung ---\n");
    printf("Ergebnis:   %016llx\n", ciphertext);
    printf("Erwartet:   %016llx\n", expected_cipher);

    if (ciphertext == expected_cipher) {
        printf("Test: ERFOLGREICH (Verschlüsselung korrekt)\n");
    } else {
        printf("Test: FEHLGESCHLAGEN\n");
    }

    // 3. Entschlüsselung
    // Wir verwenden dieselbe Funktion, nur mit mode = 1
    // und denselben (vorwärts generierten) Schlüsseln.
    uint64 decrypted_text = des_process(ciphertext, subkeys, 1); // mode = 1 (decrypt)
    printf("\n--- Entschlüsselung ---\n");
    printf("Ergebnis:   %016llx\n", decrypted_text);
    printf("Erwartet:   %016llx\n", test_plaintext);

    if (decrypted_text == test_plaintext) {
        printf("Test: ERFOLGREICH (Entschlüsselung korrekt)\n");
    } else {
        printf("Test: FEHLGESCHLAGEN\n");
    }

    return 0;
}