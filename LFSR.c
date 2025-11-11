#include <stdio.h>
#include <stdint.h>

// Funktion zum Berechnen des nächsten LFSR-Zustands
uint8_t lfsr_4bit(uint8_t reg) {
    // Die Rückkopplung erfolgt durch XOR der Bits an den Positionen 4 und 1 (bit 3 und bit 0)
    // Für 4-Bit Register z.B. Polynom: x^4 + x^3 + 1
    uint8_t bit = ((reg >> 3) ^ (reg)) & 1; // XOR der bits 3 und 0 (0-basiert)
    
    // Schiebe Register um 1 bit nach rechts
    // Setze neues Bit ganz links (höchstes Bit)
    reg = (reg >> 1) | (bit << 3); 

    return reg;
}

int main() {
    uint8_t lfsr = 0xF; // Initialer 4-Bit Vektor: 1111
    uint8_t start = lfsr;

    printf("Initialer LFSR-Seed: %u (binaer ", lfsr);
    for (int i = 3; i >= 0; i--) {
        printf("%u", (lfsr >> i) & 1);
    }
    printf(")\n\n");

    int count = 0;
    do {
        // Ausgabe aktueller Zustand
        printf("Zyklus %2d: LFSR-Wert = %u (binaer ", count, lfsr);
        for (int i = 3; i >= 0; i--) {
            printf("%u", (lfsr >> i) & 1);
        }
        printf(")\n");

        // Berechne nächsten Zustand
        lfsr = lfsr_4bit(lfsr);
        count++;

        // Sicherheitsabbruch: max 16 Schritte (2^4), falls Endlosschleife
        if (count > 16) {
            printf("Fehler: Periodenlaenge ueberschritten!\n");
            break;
        }
    } while (lfsr != start); // Wiederhole bis zum Anfangszustand

    printf("\nPeriodenlaenge des 4-Bit LFSR: %d\n", count);
    return 0;
}
