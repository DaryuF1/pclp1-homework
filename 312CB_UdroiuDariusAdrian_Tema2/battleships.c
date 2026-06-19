#include <stdio.h> // Udroiu Darius-Adrian 312CB
#include <stdlib.h>
#include <string.h>
/* structura ajutatoare pt. a retine mai usor informatii pt. fiecare barca */
typedef struct {
	int x_start, y_start, x_end, y_end, size, hit_point;
} Ship;
/* structura care ma ajuta sa retin codul pe care il primesc la taskul 2 in structura cod */
typedef struct {
	char c1;
	char c2;
	int natural;
	double real;
	char suffix;
} cod; // nu are sens sa retin MIN sau MAX pt. fiecare cod citit, asa ca voi retine doar ultima litera, N, respectiv X
/* structura care ma ajuta sa retin acuratetea si nr. de nave pe meci */
typedef struct {
	double acc;
	int nave;
} Meci;

/* functie pt. qsort, necesara pt. a rezolva taskul 2 */
int cmp_desc(const void *a, const void *b)
{
	Meci *x = (Meci *)a;
	Meci *y = (Meci *)b;
	return y->nave - x->nave;
}
/* functie pt. qsort, necesara pt. a rezolva taskul 2 */
int cmp_asc(const void *a, const void *b)
{
	Meci *x = (Meci *)a;
	Meci *y = (Meci *)b;
	return x->nave - y->nave;
}
/* functie care imi aloca dinamic tablourile pe care le voi folosi in rezolvarea taskurilor */
void alloc(int n, int m, int ***a, int ***b, int ***pa, int ***pb,
	   Ship ***ships_a, Ship ***ships_b)
{
	*a = (int **)malloc((n + 2) * sizeof(int *));
	*b = (int **)malloc((n + 2) * sizeof(int *));
	*pa = (int **)malloc((n + 2) * sizeof(int *));
	*pb = (int **)malloc((n + 2) * sizeof(int *));

	for (int i = 0; i <= n + 1; i++) {
		(*a)[i] = (int *)calloc(m + 2, sizeof(int));
		(*b)[i] = (int *)calloc(m + 2, sizeof(int));
		(*pa)[i] = (int *)calloc(m + 2, sizeof(int));
		(*pb)[i] = (int *)calloc(m + 2, sizeof(int));
	}

	*ships_a = (Ship **)malloc(n * m * sizeof(Ship *));
	*ships_b = (Ship **)malloc(n * m * sizeof(Ship *));
}
/* functie care imi permite sa citesc datele pt. fiecare dintre jucatori si sa construiesc tablele asociate acestora */
void read_and_parsing(int n, int m, int **a, int **b, int **pa, int **pb,
		              Ship **ships_a, Ship **ships_b, int *count_a,
		              int *count_b)
{
	char tip;
	char o;
	int x, y;
	int player = 1;
	*count_a = *count_b = 0;

	int total = (n * m / 70 + n * m / 55 + n * m / 40 + n * m / 30 +
		     n * m / 20) * 2; // calculez cate nave vor plasa cei 2 jucatori adunat
	int l = 1;

	while (l <= total) {
		scanf(" %c %c %d %d", &tip, &o, &x, &y);
		int lungime = (tip == 'S') ? 5 : (tip == 'Y') ? 4 : (tip == 'B') ? 3 : (tip == 'L') ? 2 : 1;
		int valid = 1;

		int **mat = (player == 1) ? a : b; // formez tabla de joc pt. jucatorul a, respectiv b
		int **p_mat = (player == 1) ? pa : pb; // retin coordonatele unde este plasat centrul fiecarei nave pt jucatorul a, respectiv b
		Ship **ships = (player == 1) ? ships_a : ships_b; // retin informatii pt. fiecare avion pe care il citesc pt. jucatorul a, respectiv b
		int *count = (player == 1) ? count_a : count_b;

		if (o == 'H' && y + lungime - 1 > m)
			valid = 0;
		if (o == 'V' && x - lungime + 1 < 1)
			valid = 0;
		if (!(x >= 1 && x <= n && y >= 1 && y <= m))
			valid = 0;

		if (o == 'H' && valid == 1) {
			for (int k = 0; k < lungime; k++)
				if (mat[x][y + k] > 0)
					valid = 0;
		} else if (o == 'V' && valid == 1) {
			for (int k = 0; k < lungime; k++)
				if (mat[x - k][y] > 0)
					valid = 0;
		}

		if (*count >= n * m)
			valid = 0;

		if (!valid) {
			const char *nume = (tip == 'S') ? "Shinano" : (tip == 'Y') ? "Yamato" : (tip == 'B') ? "Belfast" : (tip == 'L') ? "Laffey" : "Albacore";
			printf("Nava %s nu poate fi amplasata %s la coordonatele (%d, %d).\n",
				nume, (o == 'V') ? "vertical" : "orizontal",
				x, y);
			continue;
		}

		Ship *s = (Ship *)malloc(sizeof(Ship));
		s->size = lungime;
		s->hit_point = lungime;

		if (o == 'H') {
			s->x_start = s->x_end = x;
			s->y_start = y;
			s->y_end = y + lungime - 1;
			for (int k = y; k <= y + lungime - 1; k++)
				mat[x][k] = lungime;
		} else {
			s->y_start = s->y_end = y;
			s->x_start = x - lungime + 1;
			s->x_end = x;
			for (int k = x - lungime + 1; k <= x; k++)
				mat[k][y] = lungime;
		}

		p_mat[x][y] = 1;
		ships[*count] = s;
		(*count)++;
		player = 3 - player; // schimb randul jucatorului
		l++;
	}
/* afisez matricele asociate celor 2 jucatori, asa cum cere taskul 1 */

	for (int i = 1; i <= n; i++) {
		for (int j = 1; j <= m; j++)
			printf("%d ", a[i][j]);
		printf("\n");
	}

	printf("\n");
	for (int i = 1; i <= n; i++) {
		for (int j = 1; j <= m; j++)
			printf("%d ", b[i][j]);
		printf("\n");
	}
}
/* functie care simuleaza jocul celor 2 playeri */
void solve(int n, int m, int **a, int **b, int **pa, int **pb,
	       Ship **ships_a, Ship **ships_b, int count_a, int count_b,
	       int *barci, int *lovituri1, int *lovituri2, int *lovituri_bune1,
	       int *lovituri_bune2, int k)
{
	int game_over = 0;
	int player = 1;
	int l1 = 0; // numarul de lovituri pt. jucatorul a
	int l2 = 0; // numarul de lovituri pt. jucatorul b
	int lb1 = 0; // numarul de lovituri bune pt. jucatorul a
	int lb2 = 0; // numarul de lovituri bune pt. jucatorul b
	//prin lovitura buna inteleg faptul ca am lovit o barca
	barci[k] = n * m / 70 + n * m / 55 + n * m / 40 + n * m / 30 +
		  n * m / 20; // calculez nr de barci la meciul k pt jucatorul a sau b (nu are sens sa fac 2 vectori pt. fiecare jucator, caci acestia vor folosi acelasi nr. de nave)

	int **hits_a = (int **)malloc((n + 2) * sizeof(int *)); // matrice care retine unde am lovit
	int **hits_b = (int **)malloc((n + 2) * sizeof(int *));

	for (int i = 0; i <= n + 1; i++) {
		hits_a[i] = (int *)calloc(m + 2, sizeof(int));
		hits_b[i] = (int *)calloc(m + 2, sizeof(int));
	}
/* simulez jocul celor 2 */
	while (!game_over) {
		int x, y;
		scanf("%d %d", &x, &y);

		if (player == 1)
			l1++;
		else
			l2++;

		int **enemy = (player == 1) ? b : a;
		int **p_enemy = (player == 1) ? pb : pa;
		int **hits = (player == 1) ? hits_b : hits_a;
		Ship **ships = (player == 1) ? ships_b : ships_a;
		int count = (player == 1) ? count_b : count_a;

		int processed = 0;

		if (x < 1 || x > n || y < 1 || y > m) {
			printf("Coordonate invalide (%d, %d)\n", x, y);
			processed = 1;
		} else if (hits[x][y] != 0) {
			printf("Coordonatele (%d, %d) au fost deja atacate de jucatorul %d.\n",
				x, y, player);
			processed = 1;
		} else {
			hits[x][y] = 1;
			int ship_hit_index = -1;

			for (int i = 0; i < count; i++) {
				Ship *s = ships[i];
				if (x >= s->x_start && x <= s->x_end &&
				    y >= s->y_start && y <= s->y_end) {
					ship_hit_index = i;
					break;
				}
			}

			if (ship_hit_index == -1 || enemy[x][y] == 0) {
				printf("Jucatorul %d a ratat o lovitura la coordonatele (%d, %d).\n",
					player, x, y);
				processed = 1;
			} else {
				if (player == 1)
					lb1++;
				else
					lb2++;

				Ship *s = ships[ship_hit_index];
				int distrusa = p_enemy[x][y] == 1;

				const char *nume = (s->size == 5) ? "Shinano" : (s->size == 4) ? "Yamato" : (s->size == 3) ? "Belfast" : (s->size == 2) ? "Laffey" : "Albacore";

				if (distrusa) {
					int xx = s->x_start;
					int yy = s->y_start;
					if (x == xx && y == yy) {
						xx = s->x_end;
						yy = s->y_end;
					}
					printf("Jucatorul %d a distrus o nava %s plasata intre coordonatele (%d, %d) si (%d, %d).\n",
						player, nume, x, y, xx, yy);

					s->hit_point = 0;
					for (int i = s->x_start; i <= s->x_end; i++)
						for (int j = s->y_start; j <= s->y_end; j++)
							enemy[i][j] = 0;

					p_enemy[x][y] = 0;
				} else {
					printf("Jucatorul %d a lovit o nava %s la coordonatele (%d, %d).\n",
						player, nume, x, y);
					enemy[x][y] = 0;
					s->hit_point--;
				}

				int enemy_alive = 0;
				for (int i = 0; i < count; i++)
					if (ships[i]->hit_point > 0) {
						enemy_alive = 1;
						break;
					}

				if (!enemy_alive) {
					printf("Jucatorul %d a castigat.\n", player);
					game_over = 1;
				}
				processed = 1;
			}
		}

		if (processed)
			player = 3 - player;
	}
/* eliberez memoria */
	for (int i = 0; i < count_a; i++)
		free(ships_a[i]);
	for (int i = 0; i < count_b; i++)
		free(ships_b[i]);
	free(ships_a);
	free(ships_b);

	for (int i = 0; i <= n + 1; i++) {
		free(hits_a[i]);
		free(hits_b[i]);
		free(a[i]);
		free(b[i]);
		free(pa[i]);
		free(pb[i]);
	}

	free(hits_a);
	free(hits_b);
	free(a);
	free(b);
	free(pa);
	free(pb);
	/* retin cele 4 valori calculate in 4 vectori, caci imi trebuie informatia pt. fiecare meci, k*/
	lovituri1[k] = l1;
	lovituri_bune1[k] = lb1;
	lovituri2[k] = l2;
	lovituri_bune2[k] = lb2;
}
/* decodific codul de la taskul 2 si dupa il retin in structura cod */
cod parse_code(const char *s)
{
	cod x;

	x.c1 = s[0];
	x.c2 = s[1];

	char nn[4];
	char r[16];

	nn[0] = s[2];
	nn[1] = s[3];
	nn[2] = s[4];
	nn[3] = '\0';

	int idx = 5;
	int k_loop = 0;

	while ((s[idx] >= '0' && s[idx] <= '9') || s[idx] == '.') {
		r[k_loop++] = s[idx++];
	}
	r[k_loop] = '\0';

	x.natural = atoi(nn);
	x.real = atof(r);

	x.suffix = s[strlen(s) - 1];

	return x;
}
/* calculez cele 4 statistici pt. taskul 2 */
void calc_stats(int *nave, int *lov, int *bun, int k, double x, int a,
		        char t1, char t2)
{
	double *acc_meci = (double *)malloc((k + 1) * sizeof(double));
	Meci *v1 = (Meci *)malloc((k + 1) * sizeof(Meci));
	Meci *v2 = (Meci *)malloc((k + 1) * sizeof(Meci));
	int min100 = 0;
	int need_hits = 0;
	int max0 = 0;
	int max_miss = 0;
	double acc_total = 0;
	int total_nave = 0;
/* calculez acuratetea totala */
	for (int i = 1; i <= k; i++) {
		acc_meci[i] = (lov[i] == 0 ? 0.0 : (double)bun[i] / lov[i]);
		acc_total += acc_meci[i] * nave[i];
		total_nave += nave[i];
	}
	acc_total /= total_nave;

	for (int i = 1; i <= k; i++) {
		v1[i].acc = acc_meci[i];
		v1[i].nave = nave[i];
		v2[i].acc = acc_meci[i];
		v2[i].nave = nave[i];
	}

	/* prima cerinta */
	qsort(v1 + 1, k, sizeof(Meci), cmp_desc);
	double target = x / 100.0;
	double acc = acc_total;
	int cnt = 0;
	for (int i = 1; i <= k && acc < target; i++) {
		if (v1[i].acc < 1.0) {
			acc += (1.0 - v1[i].acc) * v1[i].nave / total_nave;
			cnt++;
		}
	}
	min100 = cnt;

	/* a 2a cerinta */
	if (lov[a] == 0)
		need_hits = 0;
	else {
		double needed_hits = x / 100 * lov[a];
		int nh = (int)(bun[a] - needed_hits); /* rotunjim în sus */
		need_hits = (nh > 0 ? nh : 0);
	}

	/* a 3a cerinta */
	qsort(v2 + 1, k, sizeof(Meci), cmp_asc);
	acc = acc_total;
	cnt = 0;
	for (int i = 1; i <= k && acc >= target; i++) {
		if (v2[i].acc > 0.0) {
			acc -= v2[i].acc * v2[i].nave / total_nave;
			cnt++;
		}
	}
	if (acc < target)
		cnt--;
	if (cnt < 0)
		cnt = 0;
	max0 = cnt;

	/* a 4a cerinta */
	if (lov[a] == 0)
		max_miss = 0;
	else {
		int mm = (int)(x / 100 * lov[a] + 0.999) - bun[a];
		max_miss = (mm > 0 ? mm : 0);
	}
/* afisez raspunsul in functie de codul primit la input */
	if (t1 == 'T') {
		acc_total = (int)(acc_total * 10000);
		acc_total = acc_total / 100;
		if (acc_total < 100)
			printf("0");
		if (t2 == 'N')
			printf("%.2f.%d\n", acc_total, min100);
		else if (t2 == 'X')
			printf("%.2f.%d\n", acc_total, max0);
	} else if (t1 == 'U') {
		if (a < 1 || a > k) {
			printf("Nu exista date pentru meciul cerut.\n");
		} else {
			double acc_match = (lov[a] == 0 ? 0.0 :
						(double)bun[a] / lov[a]) *
					   10000;
			acc_match = (int)acc_match;
			acc_match = acc_match / 100.0;
			if (acc_match < 100)
				printf("0");
			if (t2 == 'X')
				printf("%.2f.%d\n", acc_match, need_hits);
			else if (t2 == 'N')
				printf("%.2f.%d\n", acc_match, max_miss);
		}
	}

	free(acc_meci);
	free(v1);
	free(v2);
}

int main()
{
	int test;
	int k = 1;
	scanf("%d", &test);

	int max_size = 2000;
	int *barci = (int *)malloc(max_size * sizeof(int));
	int *lovituri1 = (int *)malloc(max_size * sizeof(int));
	int *lovituri_bune1 = (int *)malloc(max_size * sizeof(int));
	int *lovituri2 = (int *)malloc(max_size * sizeof(int));
	int *lovituri_bune2 = (int *)malloc(max_size * sizeof(int));
/* rezolv taskul 1 */
	while (test--) {
		int n, m;
		scanf("%d %d", &n, &m);

		int **a, **b, **pa, **pb;
		Ship **ships_a, **ships_b;
		int count_a, count_b;

		alloc(n, m, &a, &b, &pa, &pb, &ships_a, &ships_b);
		read_and_parsing(n, m, a, b, pa, pb, ships_a, ships_b,
				 &count_a, &count_b);
		solve(n, m, a, b, pa, pb, ships_a, ships_b, count_a, count_b,
		      barci, lovituri1, lovituri2, lovituri_bune1,
		      lovituri_bune2, k);
		k++;
	}
	k--;
	char cc;
	scanf(" %c", &cc);
	if (cc == 'Q') {
		free(lovituri1);
		free(lovituri_bune1);
		free(lovituri2);
		free(lovituri_bune2);
		free(barci);
		return 0;
	}
/* rezolv taskul 2 */
	while (1) {
		char *s = (char *)malloc(100 * sizeof(char));
		scanf(" %s", s);
		if (strcmp(s, "Q") == 0) {
			free(s);
			break;
		}
		cod x = parse_code(s);
		if (x.c1 == 'T')
			calc_stats(barci, lovituri2, lovituri_bune2, k, x.real,
				   x.natural, x.c2, x.suffix);
		else if (x.c1 == 'O')
			calc_stats(barci, lovituri1, lovituri_bune1, k, x.real,
				   x.natural, x.c2, x.suffix);
		free(s);
	}

	free(lovituri1);
	free(lovituri_bune1);
	free(lovituri2);
	free(lovituri_bune2);
	free(barci);

	return 0;
}
