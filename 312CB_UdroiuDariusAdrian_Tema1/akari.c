#include <stdio.h>
#include <ctype.h>

#define NMAX 55

void solve1(void);
void solve2(void);

char a[NMAX][NMAX];
int di[] = {-1, 1, 0, 0};
int dj[] = {0, 0, 1, -1};

int task, n, m, v[NMAX][NMAX], x[NMAX * NMAX], y[NMAX * NMAX], k;
int l1[NMAX * NMAX], l2[NMAX * NMAX], ok1 = 1;
char ch = ' ';

int inmat(int i, int j)
{
	return i >= 1 && i <= n && j >= 1 && j <= m;
}

void fill(int i, int j)
{
	v[i][j] = -1;
	a[i][j] = 'L';

	for (int q = 0; q < 4; q++) {
		int dx = i + di[q], dy = j + dj[q];
		while (inmat(dx, dy) && v[dx][dy] >= 0) {
			v[dx][dy] = 1;
			a[dx][dy] = 'x';
			dx += di[q];
			dy += dj[q];
		}
	}
}

void fill_ak(int i, int j)
{
	int nr = a[i][j] - '0', k = 0;

	for (int q = 0; q < 4; q++) {
		int dx = i + di[q], dy = j + dj[q];
		if (a[dx][dy] == 'L')
			k++;
	}

	if (k == nr)
		for (int q = 0; q < 4; q++) {
			int dx = i + di[q], dy = j + dj[q];
			if (a[dx][dy] == '-')
				a[dx][dy] = 'x';
		}
}

int ver1(int i, int j)
{
	v[i][j] = -1;

	for (int q = 0; q < 4; q++) {
		int dx = i + di[q], dy = j + dj[q];
		while (inmat(dx, dy) && v[dx][dy] == 0) {
			if (a[dx][dy] == 'L')
				return 0;
			dx += di[q];
			dy += dj[q];
		}
	}
	return 1;
}

int ver2(int i, int j)
{
	int nr = a[i][j] - '0', k = 0, k1 = 0, k2 = 0;

	for (int q = 0; q < 4; q++) {
		int dx = i + di[q], dy = j + dj[q];
		if (a[dx][dy] == 'L')
			k++;
		if (a[dx][dy] == 'x')
			k1++;
		if (a[dx][dy] == '-')
			k2++;
	}

	if (k + k2 >= nr)
		return 1;
	return 0;
}

int main(void)
{
	scanf("%d%d%d", &task, &n, &m);

	for (int i = 1; i <= n; i++)
		for (int j = 1; j <= m; j++) {
			scanf(" %c", &a[i][j]);
			if ((a[i][j] >= '0' && a[i][j] <= '4') || a[i][j] == '#')
				v[i][j] = -1;
			if (a[i][j] >= '0' && a[i][j] <= '4') {
				x[++k] = i;
				y[k] = j;
			}
		}

	if (task == 1)
		solve1();
	else
		solve2();

	return 0;
}

void solve1(void)
{
	for (int i = 1; i <= n; i++)
		for (int j = 1; j <= m; j++)
			if (a[i][j] == 'L')
				fill(i, j);

	for (int i = 1; i <= k; i++)
		fill_ak(x[i], y[i]);

	for (int i = 1; i <= n; i++) {
		for (int j = 1; j <= m; j++) {
			if (j == m)
				printf("%c%c\n", a[i][j], ch);
			else
				printf("%c%c", a[i][j], ch);
		}
	}
}

void solve2(void)
{
	for (int i = 1; i <= n; i++)
		for (int j = 1; j <= m; j++)
			if (a[i][j] == 'L')
				if (ver1(i, j) == 0)
					ok1 = 0;

	for (int i = 1; i <= n; i++)
		for (int j = 1; j <= m; j++)
			if (a[i][j] == 'L')
				fill(i, j);

	if (ok1 == 1) {
		for (int i = 1; i <= k; i++)
			if (ver2(x[i], y[i]) == 0)
				ok1 = 0;
		if (ok1 == 1)
			printf("ichi");
		else
			printf("zero");
	} else
		printf("zero");
}
