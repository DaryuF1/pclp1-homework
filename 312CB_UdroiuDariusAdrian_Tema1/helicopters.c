#include <stdio.h>

#define NMAX 1005

int n, m, k;
int a[NMAX][NMAX];
int v[NMAX * NMAX], w[NMAX * NMAX];
int nn, cnt, nn1;

void solve(int q, int l1, int c1, int l2, int c2, int p)
{
	int kkk = 0, kk = 0;

	if (l1 == l2 || c1 == c2) {
		w[++nn1] = q;
		return;
	}

	if (l1 > l2) {
		int aux = l1;
		l1 = l2;
		l2 = aux;

		int aux1 = c1;
		c1 = c2;
		c2 = aux1;
	}

	if (p == -1) {
		if (c1 < c2) {
			for (int b = 1, i1 = l1; i1 <= l2; i1++, b++) {
				for (int c = 1, j1 = c1; c <= b; j1++, c++) {
					if (a[i1][j1] == 0) {
						kk++;
					}
					kkk++;
				}
			}
		} else {
			for (int b = 1, i1 = l1; i1 <= l2; i1++, b++) {
				for (int c = 1, j1 = c1; c <= b; j1--, c++) {
					if (a[i1][j1] == 0) {
						kk++;
					}
					kkk++;
				}
			}
		}
	} else {
		if (c1 > c2) {
			for (int b = 1, i1 = l2; i1 >= l1; i1--, b++) {
				for (int c = 1, j1 = c2; c <= b; j1++, c++) {
					if (a[i1][j1] == 0) {
						kk++;
					}
					kkk++;
				}
			}
		} else {
			for (int b = 1, i1 = l2; i1 >= l1; i1--, b++) {
				for (int c = 1, j1 = c2; c <= b; j1--, c++) {
					if (a[i1][j1] == 0) {
						kk++;
					}
					kkk++;
				}
			}
		}
	}

	if (kkk / 2 < kk) {
		v[++nn] = q;
	}

	if (kk == 0) {
		cnt++;
	}
}

int main(void)
{
	scanf("%d%d", &n, &m);

	for (int i = 1; i <= n; i++) {
		for (int j = 1; j <= m; j++) {
			scanf("%d", &a[i][j]);
		}
	}

	scanf("%d", &k);

	for (int q = 1; q <= k; q++) {
		int l1, c1, l2, c2, p;
		scanf("%d %d %d %d %d", &l1, &c1, &l2, &c2, &p);

		solve(q, l1, c1, l2, c2, p);
	}

	for (int i = 1; i <= nn1; i++) {
		printf("Elicopterul %d este pozitionat necorespunzator!\n", w[i]);
	}

	printf("%d\n", cnt);

	if (nn == 0) {
		printf("0\n");
		return 0;
	}

	printf("%d\n", nn);

	for (int i = 1; i <= nn; i++) {
		printf("%d ", v[i]);
	}

	return 0;
}
