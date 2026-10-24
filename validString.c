#include <stdio.h>
#include <string.h>

int main() {
    char s[100005];
    scanf("%s", s);

    int freq[26] = {0};
    for (int i = 0; s[i] != '\0'; i++){
        freq[s[i] - 'a']++;
    }

    int cnt[100005] = {0};
    int distinct = 0;
    for (int i = 0; i < 26; i++) {
        if (freq[i] > 0) {
            cnt[freq[i]]++;
        }
    }

    int f1 = 0, f2 = 0;
    for (int f = 1; f <= 100000; f++) {
        if (cnt[f] > 0) {
            distinct++;
            if (distinct == 1) f1 = f;
            else if (distinct == 2) f2 = f;
        }
    }

    if (distinct == 1) {
        printf("YES\n");
    }else if (distinct == 2) {

        if ((f1 == 1 && cnt[f1] == 1) || (f2 == 1 && cnt[f2] == 1)) {
            printf("YES\n");
        }
        else if (f2 - f1 == 1 && cnt[f2] == 1) {
            printf("YES\n");
        }
        else {
            printf("NO\n");
        }
    }
    else {
        printf("NO\n");
    }

    return 0;
}