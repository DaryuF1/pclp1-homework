#include <stdio.h>
#include <stdlib.h>
#define NMAX 10001
#define ull unsigned long long
typedef struct {
	ull t1, t2;
	ull cmmmc, cmmdc;
} pair;

ull cmmdc(ull a, ull b)
{
	while (b) {
		ull r = a % b;
		a = b;
		b = r;
	}
	return a;
}

ull cmmmc(ull a, ull b)
{
	return a * b / cmmdc(a, b);
}

ull win, t[NMAX], v[NMAX];
int n, cnt;
pair arr[NMAX * 10];

int main(void)
{
	scanf("%llu", &win);
	for (int i = 1; ; i++) {
		ull x, y;
		scanf("%llu%llu", &x, &y);
		if (x == 0 && y == 0)
			break;
		t[i] = x;
		v[i] = y;
		n++;
	}
	for (int i = 2; i <= n; i++)
		for (int j = 1; j < i; j++) {
			if (t[i] - t[j] <= win) {
				arr[cnt].t1 = t[i];
				arr[cnt].t2 = t[j];
				arr[cnt].cmmdc = cmmdc(v[i], v[j]);
				arr[cnt].cmmmc = cmmmc(v[i], v[j]);
				cnt++;
			}
		}
	for (int i = 0; i < cnt; i++)
		for (int j = i + 1; j < cnt; j++) {
			if (arr[i].t1 > arr[j].t1) {
				pair p = arr[i];
				arr[i] = arr[j];
				arr[j] = p;
			} else if (arr[i].t2 > arr[j].t2) {
				pair p = arr[i];
				arr[i] = arr[j];
				arr[j] = p;
			}
		}
	for (int i = 0; i < cnt; i++)
		printf("%llu %llu\n", arr[i].cmmmc, arr[i].cmmdc);
	return 0;
}
