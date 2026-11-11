#include <stdio.h>
#include <stdlib.h>

int main() {
    int n, q;

    scanf("%d %d", &n, &q);

    long long *arr = calloc(n + 2, sizeof(long long));
    for(int i = 0; i < q; i++) {

        int a, b;
        long long k;

        scanf("%d %d %lld", &a, &b, &k);

        arr[a] += k;
        arr[b + 1] -= k;
    }

    long long sum = 0;
    long long max = 0;

    for(int i = 1; i <= n; i++) {

        sum += arr[i];

        if(sum > max) {
            max = sum;
        }
    }

    printf("%lld\n", max);

    free(arr);

    return 0;
}
