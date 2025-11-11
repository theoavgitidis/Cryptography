#include <stdio.h>
#include <stdlib.h>

#define P 29        // Modulo 29 (alphabet size + 3 Sonderzeichen)
#define N 3         // Codierungs-Matrix: Matrixdimension (3x3)
#define BLOCKS 2    // Text-Matrix: Anzahl Bearbeitungsblöcke (Spalten)

// Matrix-Multiplikation: result = mat * vec mod P 
void mat_mult(int result[N], int mat[N][N], const int vec[N]) {
    for (int i = 0; i < N; i++) {
        long long sum = 0;
        for (int j = 0; j < N; j++) {
            long long prod = (long long)mat[i][j] * (long long)vec[j];
            prod %= P;
            if (prod < 0) prod += P;
            sum += prod;
        }
        sum %= P;
        if (sum < 0) sum += P;
        result[i] = (int)sum;
    }
}

// print Block
void print_vec(const char *label, const int v[N]) {
    printf("%s: ", label);
    for (int i = 0; i < N; i++) printf("%d ", v[i]);
    printf("\n");
}

// print Block as chars
void print_vec_as_chars(const char *label, const int v[N]) {
    printf("%s (chars): ", label);
    for (int i = 0; i < N; i++) {
        int x = v[i] % 26;
        if (x < 0) x += 26;
        printf("%c", 'A' + x);
    }
    printf("\n");
}

int main(void) {
    // konstante Codierungs-Matrix  A, invertierbar modulo 29
    int A[N][N] = {
        {1, 2, 3},
        {0, 1, 4},
        {5, 6, 0}
    };
    // konstante Decodierungs-Matrix A^-1
    int A_inv[N][N] = {
        {5, 18, 5},
        {20, 14, 25},
        {24, 4, 1}
    };

    // Text-Matrix: 1 Spalte = 1 Block (N x BLOCKS) 
    int plaintext[N][BLOCKS] = {    // "codeAA"
        {2, 4},     // C, E
        {14, 0},    // O, A
        {3, 0}      // D, A
    };

    // für temporäres speichern einzelner Blocks
    int ciphertext[N];
    int decrypted[N];

    // pro text block
    for (int b = 0; b < BLOCKS; b++) {
        // hole b-te Spalte als Vektor 
        int block[N];
        for (int i = 0; i < N; i++) block[i] = plaintext[i][b];

        // original text
        print_vec("Plaintext block", block);
        print_vec_as_chars("Plaintext block", block);

        // Verschluesselung 
        mat_mult(ciphertext, A, block);                 // A * Block
        print_vec("Ciphertext", ciphertext);
        print_vec_as_chars("Ciphertext", ciphertext);

        // Entschluesselung 
        mat_mult(decrypted, A_inv, ciphertext);         // A^-1 * Block
        print_vec("Decrypted", decrypted);
        print_vec_as_chars("Decrypted", decrypted);

        printf("-----\n");
    }

    return 0;
}