/*  ASCII Values
A 65
B 66
C 67
D 68
E 69
F 70
G 71
H 72
I 73
J 74
K 75
L 76
M 77
N 78
O 79
P 80
Q 81
R 82
S 83 
T 84 
U 85
V 86
W 87
X 88
Y 89 
Z 90
*/
/*
This Program demonstrates the encoding making use of the very old Ceasar Method. Each Letter's ASCII value is beeing
increased by a key, so that the encoded text is not readable. In order to avoid an Overflow the function "Modulo" is beeing
made of use. If a character is entered that is not a letter, it will not be encoded.
*/
#include <stdio.h>

/**
 * Helper function: ENCODES a *single* character.
 *
 * @param c   The character to encode.
 * @param key The shift (key) to apply.
 * @return    The encoded character.
 */
char encode_char(char c, int key) {
    // CHECK: Is it an uppercase letter?
    if (c >= 'A' && c <= 'Z') {
        // We add the key and use modulo 26
        // for the wrap-around (e.g., 'Z' + 1 becomes 'A'), as we have 26 letters
        return (c - 'A' + key) % 26 + 'A';
    } 
    // CHECK: Is it a lowercase letter?
    else if (c >= 'a' && c <= 'z') {
        // Same logic for lowercase letters
        return (c - 'a' + key) % 26 + 'a';
    } 
    // IF NOT A LETTER
    else {
        // Return the character unchanged
        return c;
    }
}


/**
 * ENCODES a complete string (char* plaintext)
 * and prints the result directly to the console.
 *
 * @param plaintext A pointer to the char array (the string).
 * @param key       The shift to be applied.
 */
void encode(char* plaintext, int key) {
    
    printf("Plaintext: %s\n", plaintext);
    printf("Key: %d\n", key);
    printf("Encoded: ");

    int i = 0;
    // The loop runs until it finds the string-end character '\0'.
    while (plaintext[i] != '\0') {
        
        // Get the character at the current position
        char current_char = plaintext[i];
        
        // Call the helper function to encode the character
        char encoded_val = encode_char(current_char, key);
        
        // Print the encoded character directly
        putchar(encoded_val);
        
        // Move to the next character in the string
        i++;
    }
    
    // Add two newlines at the end for clean separation
    printf("\n\n"); 
}

int main() {
    //imagine to circles with letters, the inner one (on which plain text is typed) is moved 5 places
    //to the right, the corresponding letter from the outer one is the encoded letter
    encode("Hello World!", 5);

    return 0;
}