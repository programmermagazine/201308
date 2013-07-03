#include <stdio.h>
#define BYTE unsigned char

int pseudoRandom(char key, char pseudoKey[], int len) {
  int i;
  printf("偽隨機數=");
  pseudoKey[0] = key;
  for (i=1; i<len; i++) {
    BYTE b = (BYTE) pseudoKey[i-1];
    BYTE bnew = (b * 7) % 255;
    pseudoKey[i] = (char) bnew;
	printf("%2x ", bnew);
	b = bnew;
  }
  printf("\n");
}

int encrypt(char plan[], char cipher[], char pseudoKey[], int len) {
  int i;
  for (i=0; i<len; i++) {
    cipher[i] = plan[i] ^ pseudoKey[i];
  }
}

int decrypt(char cipher[], char plan[], char pseudoKey[], int len) {
  int i;
  for (i=0; i<len; i++) {
    plan[i] = cipher[i] ^ pseudoKey[i];
  }
}

int main() {
  char plan[] = "Zero-hour is July/4th 3.30 am";
  char cipher[100], plan2[100], pseudoKey[100];
  char key = 0x6D;
  int  len = strlen(plan);
  
  printf("明文=%s\n", plan);
  pseudoRandom(key, pseudoKey, len);
  encrypt(plan, cipher, pseudoKey, len);
  printf("密文=%s\n", cipher);
  decrypt(cipher, plan2, pseudoKey, len);
  printf("還原=%s\n", plan2);
}

