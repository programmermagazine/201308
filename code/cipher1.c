#include <stdio.h>

int encrypt(char plan[], char cipher[], char key, int len) {
  int i;
  for (i=0; i<len; i++) {
    cipher[i] = plan[i] ^ key;
  }
}

int decrypt(char cipher[], char plan[], char key, int len) {
  int i;
  for (i=0; i<len; i++) {
    plan[i] = cipher[i] ^ key;
  }
}

int main() {
  char plan[] = "Zero-hour is July/4th 3.30 am";
  char cipher[100], plan2[100];
  char key = 0x6D;
  int  len = strlen(plan);
  
  printf("明文=%s\n", plan);
  encrypt(plan, cipher, key, len);
  printf("密文=%s\n", cipher);
  decrypt(cipher, plan2, key, len);
  printf("還原=%s\n", plan2);
}

