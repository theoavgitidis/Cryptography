#include <stdio.h> // Nur für printf

// --- Unveränderte Hilfsfunktionen ---

// Eigene strlen-Funktion (da string.h nicht erlaubt ist)
size_t my_strlen(const char *str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

// PRNG-Zustand
static unsigned long lcg_state;

// PRNG-Seed-Funktion
void seed_prng(unsigned long seed) {
    lcg_state = (seed == 0) ? 1 : seed;
}

// PRNG-Funktion (LCG)
unsigned char get_random_byte() {
    const unsigned long a = 48271;
    const unsigned long m = 2147483647; // (2^31 - 1)
    lcg_state = (a * lcg_state) % m;
    return (unsigned char)(lcg_state & 0xFF);
}

// --- Neue Hilfsfunktion zur Visualisierung ---

/**
 * @brief Gibt ein einzelnes Byte (char) als 8-stellige Binärzahl auf der Konsole aus.
 * z.B. 'A' (65) -> "01000001"
 * @param byte Das Byte, das gedruckt werden soll.
 */
void print_byte_as_binary(unsigned char byte) {
    // Wir iterieren von Bit 7 (MSB) herunter zu Bit 0 (LSB)
    for (int i = 7; i >= 0; i--) {
        // Wir verschieben das i-te Bit an die 0-te Stelle...
        // ...und maskieren es mit 1, um nur dieses Bit zu erhalten.
        int bit = (byte >> i) & 1;
        printf("%d", bit);
    }
    printf(" "); // Leerzeichen zur besseren Lesbarkeit
}

// --- Angepasstes Hauptprogramm ---

int main() {
    // Wichtig: modifizierbares Array
    char message[] = "Hallo"; 
    unsigned long secret_key = 42;
    size_t len = my_strlen(message);

    printf("--- Stromchiffre-Demonstration (Explizit) ---\n");

    // 1. KLARTEXTSTRING
    printf("Klartextstring:     %s\n", message);

    // 2. KLARTEXTBITSTROM
    printf("Klartextbitstrom:    ");
    for (size_t i = 0; i < len; i++) {
        // Gebe jedes Zeichen als Binärfolge aus
        print_byte_as_binary((unsigned char)message[i]);
    }
    printf("\n");

    // 3. CHIFFRETEXTBITSTROM (und Verschlüsselung)
    // Wir müssen den PRNG jetzt starten, um den Keystream zu erzeugen
    // und *währenddessen* den Chiffretext-Bitstrom ausgeben.
    
    printf("Chiffretextbitstrom: ");
    seed_prng(secret_key); // PRNG mit Schlüssel initialisieren
    
    for (size_t i = 0; i < len; i++) {
        unsigned char keystream_byte = get_random_byte();
        unsigned char klartext_byte = (unsigned char)message[i];
        
        // Die XOR-Operation
        unsigned char chiffre_byte = klartext_byte ^ keystream_byte;
        
        // Bitstrom des Chiffre-Bytes ausgeben
        print_byte_as_binary(chiffre_byte);
        
        // Das Ergebnis zur späteren Verwendung zurück in den String schreiben
        message[i] = (char)chiffre_byte;
    }
    printf("\n");

    // 4. CHIFFRETEXT IN ASCII
    // 'message' enthält jetzt die verschlüsselten (meist unlesbaren) Zeichen
    printf("Chiffretext in Ascii: %s\n", message);
    
    printf("\n--- Entschlüsselung zur Kontrolle ---\n");

    // Zur Demonstration: Entschlüsselung
    // Wir müssen den PRNG exakt gleich initialisieren!
    seed_prng(secret_key); 
    
    for (size_t i = 0; i < len; i++) {
        unsigned char keystream_byte = get_random_byte();
        // 'message[i]' ist jetzt das Chiffre-Byte
        message[i] = message[i] ^ keystream_byte;
    }
    
    printf("Wiederhergestellt:   %s\n", message);

    return 0;
}