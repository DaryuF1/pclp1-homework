// Udroiu Darius-Adrian 312CB
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// dim. stack-urilor pt. undo/redo si pt. pozitiile testoasei
#define MAX_STACK 100
#define TURTLE_STACK_SIZE 10000 
#define MAX_LINE_LEN 2048

// reprezinta un singur pixel rgb
typedef struct { unsigned char r, g, b; } Pixel;

// dimensiunea si matricea de pixeli pt. imagine
typedef struct {
	int w, h;
	Pixel **pixels;
} Image;

// structura care imi permite sa retin in stringul "to" caracterul "from"
typedef struct {
	char from;
	char *to;
} Rule;

// starrea sistemului:axioma initiala, lista de reguli, fisierul sursa
typedef struct {
	char *axiom;
	Rule *rules;
	int nrules;
	char *filename; 
} LSystemState;

// structura unui caracter (glyph) incarcat din fisierul bdf
typedef struct {
	int encoding;           // cod asci
	int dwx, dwy;           // DWIDTH - cat de mult avanseaza cursorul dupa desenare
	int bb_w, bb_h;         // bounding box width/height - dim. bitmapului
	int bb_xoff, bb_yoff;   // bounding box offsets - deplasarea fata de originea cursorului
	unsigned char *bitmap;  // matrice liniarizată de 0 și 1 - o pixel stins, 1 pizel aprins
} Glyph;

// structura fontului complet
typedef struct {
	char name[128];         // numele fontului
	Glyph *glyphs[256];     // vector de pointeri la glyphuri
	char *filename;
} Font;

// aceasta structura tine tot ce poate fi modificat si trebuie salvat la undo
typedef struct {
	LSystemState *lsys;
	Image *img;
	Font *font;             // font curent
	int last_cmd_type;      // pt. mesajele de redo
} ProgramState;

// structura care tine starea curenta si istoricul
typedef struct {
	ProgramState *current;
	ProgramState *undoStack[MAX_STACK];
	int undoTop;
	ProgramState *redoStack[MAX_STACK];
	int redoTop;
} MasterState;

// structura folosita pt a salva pozitia si unghiul cand intalnim '['
typedef struct {
	double x, y, theta;
} TurtleState;

// implementare strdup de mana, caci nu stiu de ce nu merge programul cu functia propriu-zisa :))
char *my_strdup(const char *s) {
	if (!s) return NULL;
	char *p = malloc(strlen(s) + 1);
	if (p) strcpy(p, s);
	return p;
}

// convertor caracter hex in int 
int hex_val(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	return 0;
}

// eliberare memorie alocata pt. o imagine 
void free_image(Image *img) {
	if (!img) return;
	for (int i = 0; i < img->h; i++) free(img->pixels[i]);
	free(img->pixels);
	free(img);
}

// copie a imaginii, pt. ca undo sa nu modifice starea curenta
Image* copy_image(Image *src) {
	if (!src) return NULL;
	Image *dst = malloc(sizeof(Image));
	dst->w = src->w; dst->h = src->h;
	dst->pixels = malloc(dst->h * sizeof(Pixel*));
	for (int i = 0; i < dst->h; i++) {
		dst->pixels[i] = malloc(dst->w * sizeof(Pixel));
		memcpy(dst->pixels[i], src->pixels[i], dst->w * sizeof(Pixel));
	}
	return dst;
}

// salvare in format binar
void save_ppm(Image *img, const char *path) {
	if (!img || !path) return;
	FILE *fo = fopen(path, "wb");
	if (fo) {
		fprintf(fo, "P6\n%d %d\n255\n", img->w, img->h);
		for (int i = 0; i < img->h; i++)
			fwrite(img->pixels[i], sizeof(Pixel), img->w, fo);
		fclose(fo);
	}
}


// eliberare memorie alocata pt. L-System
void free_lsys(LSystemState *lsys) {
	if (!lsys) return;
	if (lsys->axiom) free(lsys->axiom);
	if (lsys->filename) free(lsys->filename);
	if (lsys->rules) {
		for (int i = 0; i < lsys->nrules; i++) free(lsys->rules[i].to);
		free(lsys->rules);
	}
	free(lsys);
}

// copiea starii L-System
LSystemState* copy_lsys(LSystemState *src) {
	if (!src) return NULL;
	LSystemState *dst = malloc(sizeof(LSystemState));
	dst->axiom = my_strdup(src->axiom);
	dst->filename = my_strdup(src->filename);
	dst->nrules = src->nrules;
	dst->rules = malloc(dst->nrules * sizeof(Rule));
	for (int i = 0; i < dst->nrules; i++) {
		dst->rules[i].from = src->rules[i].from;
		dst->rules[i].to = my_strdup(src->rules[i].to);
	}
	return dst;
}

// elibereaza un singur caracter
void free_glyph(Glyph *g) {
	if (!g) return;
	if (g->bitmap) free(g->bitmap);
	free(g);
}

// elibereaza tot fontul
void free_font(Font *font) {
	if (!font) return;
	if (font->filename) free(font->filename);
	for (int i = 0; i < 256; i++) {
		if (font->glyphs[i]) free_glyph(font->glyphs[i]);
	}
	free(font);
}

// copie pt. font font (necesar la undo/eedo)
Font* copy_font(Font *src) {
	if (!src) return NULL;
	Font *dst = malloc(sizeof(Font));
	strcpy(dst->name, src->name);
	dst->filename = my_strdup(src->filename);
	
	for (int i = 0; i < 256; i++) {
		if (src->glyphs[i]) {
			dst->glyphs[i] = malloc(sizeof(Glyph));
			*dst->glyphs[i] = *src->glyphs[i]; 
			

			int bitmap_size = src->glyphs[i]->bb_w * src->glyphs[i]->bb_h;
			dst->glyphs[i]->bitmap = malloc(bitmap_size);
			memcpy(dst->glyphs[i]->bitmap, src->glyphs[i]->bitmap, bitmap_size);
		} else {
			dst->glyphs[i] = NULL;
		}
	}
	return dst;
}

// parsarea fișierului bdf
// citeste si converteste sirurile hexazecimale in matrici de 0 si 1
Font* load_font_bdf(const char *path) {
	FILE *f = fopen(path, "r");
	if (!f) return NULL;

	Font *font = calloc(1, sizeof(Font));
	font->filename = my_strdup(path);

	char line[MAX_LINE_LEN];
	Glyph *current_glyph = NULL;
	int reading_bitmap = 0;
	int bitmap_row_idx = 0;

	while (fgets(line, sizeof(line), f)) {
		line[strcspn(line, "\r\n")] = 0;
		
		char keyword[64];
		sscanf(line, "%s", keyword);

		if (strcmp(keyword, "FONT") == 0) {
			sscanf(line, "FONT %s", font->name);
		}
		else if (strcmp(keyword, "STARTCHAR") == 0) {
			current_glyph = calloc(1, sizeof(Glyph));
		}
		else if (strcmp(keyword, "ENCODING") == 0 && current_glyph) {
			sscanf(line, "ENCODING %d", &current_glyph->encoding);
		}
		else if (strcmp(keyword, "DWIDTH") == 0 && current_glyph) {
			sscanf(line, "DWIDTH %d %d", &current_glyph->dwx, &current_glyph->dwy);
		}
		else if (strcmp(keyword, "BBX") == 0 && current_glyph) {
			// bbx defineste dreptunghiul care incadreaza caracterul
			sscanf(line, "BBX %d %d %d %d", 
				   &current_glyph->bb_w, &current_glyph->bb_h, 
				   &current_glyph->bb_xoff, &current_glyph->bb_yoff);
			current_glyph->bitmap = calloc(current_glyph->bb_w * current_glyph->bb_h, 1);
		}
		else if (strcmp(keyword, "BITMAP") == 0) {
			reading_bitmap = 1;
			bitmap_row_idx = 0;
		}
		else if (strcmp(keyword, "ENDCHAR") == 0) {
			// salvam glyphul in vectorul fontului doar daca codul asci este valid
			if (current_glyph && current_glyph->encoding >= 0 && current_glyph->encoding < 256) {
				font->glyphs[current_glyph->encoding] = current_glyph;
			} else {
				free_glyph(current_glyph);
			}
			current_glyph = NULL;
			reading_bitmap = 0;
		}
		else if (reading_bitmap && current_glyph) {

			int line_len = (int)strlen(line);
			int row_len_bytes = (current_glyph->bb_w + 7) / 8; // cati octeti ocupa un rand
			
			for (int i = 0; i < row_len_bytes; i++) {
				char hex_pair[3] = {0};
				if (2*i < line_len) hex_pair[0] = line[2*i];
				if (2*i+1 < line_len) hex_pair[1] = line[2*i+1];
				
				int val = (hex_val(hex_pair[0]) << 4) | hex_val(hex_pair[1]);
				
				// extragem bitii din octetul curent
				for (int bit = 0; bit < 8; bit++) {
					int col = i * 8 + bit;
					if (col < current_glyph->bb_w) {
						int is_set = (val >> (7 - bit)) & 1;
						current_glyph->bitmap[bitmap_row_idx * current_glyph->bb_w + col] = is_set;
					}
				}
			}
			bitmap_row_idx++;
		}
	}

	fclose(f);
	return font;
}

void free_program_state(ProgramState *ps) {
	if (!ps) return;
	free_lsys(ps->lsys);
	free_image(ps->img);
	free_font(ps->font);
	free(ps);
}

// copiere totala ( adica copiez L-sys, Img, Font)
ProgramState* copy_program_state(ProgramState *ps) {
	if (!ps) return NULL;
	ProgramState *new_ps = calloc(1, sizeof(ProgramState));
	new_ps->lsys = copy_lsys(ps->lsys);
	new_ps->img = copy_image(ps->img);
	new_ps->font = copy_font(ps->font);
	new_ps->last_cmd_type = ps->last_cmd_type;
	return new_ps;
}


// functie pt. a desena un pixel doar daca e in interiorul imaginii
void draw_pixel_safe(Image *img, int x, int y, Pixel color) {
	int matrix_y = img->h - 1 - y; // inversam y pt. coord. carteziene
	int matrix_x = x;

	if (matrix_x >= 0 && matrix_x < img->w && matrix_y >= 0 && matrix_y < img->h) {
		img->pixels[matrix_y][matrix_x] = color;
	}
}

// alg. lui Bresenham pt. desenarea liniilor
void draw_line(Image *img, int x0, int y0, int x1, int y1, Pixel color) {
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;
	while (1) {
		draw_pixel_safe(img, x0, y0, color);
		if (x0 == x1 && y0 == y1) break;
		e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; }
	}
}

// functie pt. desenarea unui text folosind fontul incarcat
void draw_text(Image *img, Font *font, const char *text, int start_x, int start_y, Pixel color) {
	int cursor_x = start_x;
	int cursor_y = start_y;

	for (int i = 0; text[i]; i++) {
		int code = (unsigned char)text[i];
		if (font->glyphs[code]) {
			Glyph *g = font->glyphs[code];
			// calculam pozitia unde incepe dreptunghiu al caracterului
			int bbx_origin_x = cursor_x + g->bb_xoff;
			int bbx_origin_y = cursor_y + g->bb_yoff;

			// iteram prin bitmapul caracterului si desenam pixelii activi
			for (int r = 0; r < g->bb_h; r++) {
				for (int c = 0; c < g->bb_w; c++) {
					if (g->bitmap[r * g->bb_w + c]) {
						int px = bbx_origin_x + c;
						int py = bbx_origin_y + (g->bb_h - 1 - r);
						draw_pixel_safe(img, px, py, color);
					}
				}
			}
			// avansam cursorul pt. urmatorul caracter
			cursor_x += g->dwx;
			cursor_y += g->dwy;
		}
	}
}

// functie care aplica regulile de n ori asupra axiomei
char* derive(LSystemState *ls, int n) {
	char *curr = my_strdup(ls->axiom);
	for (int i = 0; i < n; i++) {
		// estimare initiala bufer, se va mari daca e nevoie
		size_t cap = strlen(curr) * 2 + 100, len = 0;
		char *next = malloc(cap);
		if(!next) { free(curr); return NULL; }

		for (int j = 0; curr[j]; j++) {
			char *rep = NULL;
			// cautam daca exista regula pentru caracterul curent
			for (int r = 0; r < ls->nrules; r++) {
				if (curr[j] == ls->rules[r].from) { rep = ls->rules[r].to; break; }
			}
			// daca nu exista regula, caracterul ramane neschimbat
			char *to_add = rep ? rep : (char[]){curr[j], 0};
			size_t add_len = strlen(to_add);
			
			// realocare buffer daca nu e destul loc
			while (len + add_len >= cap) { 
				cap *= 2; 
				char *temp = realloc(next, cap);
				if (!temp) { free(curr); free(next); return NULL; }
				next = temp;
			}
			strcpy(next + len, to_add);
			len += add_len;
		}
		free(curr);
		curr = next;
	}
	return curr;
}

void run_bitcheck(Image *img) {
	if (!img) {
		printf("No image loaded\n");
		return;
	}

	// liniarizam imaginea intr-un singur buffer de octeti
	int total_bytes = img->w * img->h * 3;
	unsigned char *stream = malloc(total_bytes);
	if (!stream) return;

	int p = 0;
	for (int i = 0; i < img->h; i++) {
		for (int j = 0; j < img->w; j++) {
			stream[p++] = img->pixels[i][j].r;
			stream[p++] = img->pixels[i][j].g;
			stream[p++] = img->pixels[i][j].b;
		}
	}

	// iteram prin bitii stream-ului cu o fereastra glisanta de 4 biti
	int total_bits = total_bytes * 8;
	// avem nevoie de 4 biti consecutivi, deci mergem pana la total_bits - 4
	for (int k = 0; k <= total_bits - 4; k++) {
		// extragem 4 biti consecutivi
		int b[4];
		for (int m = 0; m < 4; m++) {
			int bit_idx = k + m;
			int byte_idx = bit_idx / 8;
			int bit_offset = 7 - (bit_idx % 8); // MSB first (bitul 7 e cel mai semnificativ)
			b[m] = (stream[byte_idx] >> bit_offset) & 1;
		}

		// verificam patternurile specifice de eroare
		// 0010 -> se presupune ca trebuia sa fie 0000 (bitul 3 devine 0)
		// 1101 -> se presupune ca trebuia sa fie 1111 (bitul 3 devine 1)
		int pattern_found = 0;
		
		if (b[0] == 0 && b[1] == 0 && b[2] == 1 && b[3] == 0) pattern_found = 1;
		if (b[0] == 1 && b[1] == 1 && b[2] == 0 && b[3] == 1) pattern_found = 1;

		if (pattern_found) {
			// bitul "stricat" este al 3-lea din secventa
			int target_idx = k + 2;
			int byte_idx = target_idx / 8;
			int bit_offset = 7 - (target_idx % 8);

			// identificam coordonatele pixelului care contine acest bit
			int pixel_linear_idx = byte_idx / 3;
			int channel_idx = byte_idx % 3; // 0=R, 1=G, 2=B

			int row_in_file = pixel_linear_idx / img->w;
			int col_x = pixel_linear_idx % img->w;
			
			// conversie coordonata y: 
			// in fisier, randul 0 este "sus". in cartezian, "sus" este h-1.
			int coord_y = img->h - 1 - row_in_file;

			// calculam culoarea eronata (cea care ar rezulta daca bitul ramane inversat)
			Pixel orig = img->pixels[row_in_file][col_x];
			int vals[3] = {orig.r, orig.g, orig.b};
			
			// inversam bitul pentru a simula citirea gresita
			vals[channel_idx] ^= (1 << bit_offset);

			printf("Warning: pixel at (%d, %d) may be read as (%d, %d, %d)\n",
				   col_x, coord_y, vals[0], vals[1], vals[2]);
		}
	}

	free(stream);
}


int main() {
	// initializare stare master
	MasterState ms = { .current = calloc(1, sizeof(ProgramState)), .undoTop = -1, .redoTop = -1 };
	
	char line[MAX_LINE_LEN], cmd[64];
	while (fgets(line, sizeof(line), stdin)) {
		if (sscanf(line, "%s", cmd) != 1) continue;



		if (strcmp(cmd, "LSYSTEM") == 0) {
			char path[512]; sscanf(line, "%*s %s", path);
			FILE *f = fopen(path, "r");
			if (!f) { printf("Failed to load %s\n", path); continue; }

			// salvare stare in undo inainte de modificare
			if (ms.undoTop < MAX_STACK - 1) ms.undoStack[++ms.undoTop] = copy_program_state(ms.current);
			// golire stiva redo cand facem o actiune noua
			while (ms.redoTop >= 0) free_program_state(ms.redoStack[ms.redoTop--]);

			if (ms.current->lsys) free_lsys(ms.current->lsys);
			ms.current->lsys = calloc(1, sizeof(LSystemState));
			
			// citire axioma si reguli
			char aux[1024]; 
			fgets(aux, 1024, f); aux[strcspn(aux, "\r\n")] = 0; 
			ms.current->lsys->axiom = my_strdup(aux);
			
			fscanf(f, "%d", &ms.current->lsys->nrules);
			ms.current->lsys->rules = malloc(ms.current->lsys->nrules * sizeof(Rule));
			for (int i = 0; i < ms.current->lsys->nrules; i++) {
				char from, to[1024]; fscanf(f, " %c %s", &from, to);
				ms.current->lsys->rules[i].from = from;
				ms.current->lsys->rules[i].to = my_strdup(to);
			}
			ms.current->lsys->filename = my_strdup(path);
			ms.current->last_cmd_type = 1; 
			fclose(f);
			printf("Loaded %s (L-system with %d rules)\n", path, ms.current->lsys->nrules);
		}
		else if (strcmp(cmd, "LOAD") == 0) {
			char path[512]; sscanf(line, "%*s %s", path);
			FILE *f = fopen(path, "rb");
			if (!f) { printf("Failed to load %s\n", path); continue; }

			// salvare undo
			if (ms.undoTop < MAX_STACK - 1) ms.undoStack[++ms.undoTop] = copy_program_state(ms.current);
			while (ms.redoTop >= 0) free_program_state(ms.redoStack[ms.redoTop--]);

			char m[3]; int w, h, mx; 
			fscanf(f, "%s %d %d %d", m, &w, &h, &mx); fgetc(f); 
			
			if (ms.current->img) free_image(ms.current->img);
			ms.current->img = malloc(sizeof(Image));
			ms.current->img->w = w; ms.current->img->h = h;
			ms.current->img->pixels = malloc(h * sizeof(Pixel*));
			for (int i = 0; i < h; i++) {
				ms.current->img->pixels[i] = malloc(w * sizeof(Pixel));
				fread(ms.current->img->pixels[i], sizeof(Pixel), w, f);
			}
			ms.current->last_cmd_type = 2;
			fclose(f);
			printf("Loaded %s (PPM image %dx%d)\n", path, w, h);
		}
		else if (strcmp(cmd, "SAVE") == 0) {
			char path[512];
			if (sscanf(line, "%*s %s", path) == 1) {
				if (ms.current->img) {
					save_ppm(ms.current->img, path);
					printf("Saved %s\n", path);
				} else printf("No image loaded\n");
			}
		}
		else if (strcmp(cmd, "TURTLE") == 0) {
			double x, y, d, angle_deg, dl_deg; 
			int n, r, g, b;
			if (sscanf(line, "%*s %lf %lf %lf %lf %lf %d %d %d %d", &x, &y, &d, &angle_deg, &dl_deg, &n, &r, &g, &b) == 9) {
				if (!ms.current->img) { printf("No image loaded\n"); continue; }
				if (!ms.current->lsys) { printf("No L-system loaded\n"); continue; }

				// salvare undo inainte de desenare
				if (ms.undoTop < MAX_STACK - 1) ms.undoStack[++ms.undoTop] = copy_program_state(ms.current);
				while (ms.redoTop >= 0) free_program_state(ms.redoStack[ms.redoTop--]);

				char *s = derive(ms.current->lsys, n);
				double cx = x, cy = y;
				double th = angle_deg * M_PI / 180.0;
				double dr = dl_deg * M_PI / 180.0;
				
				TurtleState st[TURTLE_STACK_SIZE]; 
				int tp = -1;

				// interpretare string L-System
				for (int i = 0; s[i]; i++) {
					if (s[i] == 'F') {
						double nx = cx + d * cos(th);
						double ny = cy + d * sin(th);
						draw_line(ms.current->img, (int)round(cx), (int)round(cy), (int)round(nx), (int)round(ny), (Pixel){(unsigned char)r, (unsigned char)g, (unsigned char)b});
						cx = nx; cy = ny;
					} else if (s[i] == '+') th += dr;
					else if (s[i] == '-') th -= dr;
					else if (s[i] == '[') { 
						// push pozitie curenta in stiva testoasei
						if(tp < TURTLE_STACK_SIZE - 1) { tp++; st[tp].x = cx; st[tp].y = cy; st[tp].theta = th; } 
					}
					else if (s[i] == ']') { 
						// pop pozitie din stiva testoasei
						if(tp >= 0) { cx = st[tp].x; cy = st[tp].y; th = st[tp].theta; tp--; } 
					}
				}
				free(s);
				ms.current->last_cmd_type = 3;
				printf("Drawing done\n");
			}
		}
		else if (strcmp(cmd, "DERIVE") == 0) {
			int n; sscanf(line, "%*s %d", &n);
			if (!ms.current->lsys) printf("No L-system loaded\n");
			else { char *res = derive(ms.current->lsys, n); printf("%s\n", res); free(res); }
		}
		else if (strcmp(cmd, "FONT") == 0) {
			char path[512]; sscanf(line, "%*s %s", path);
			Font *new_font = load_font_bdf(path);
			if (new_font) {
				// salvare undo
				if (ms.undoTop < MAX_STACK - 1) ms.undoStack[++ms.undoTop] = copy_program_state(ms.current);
				while (ms.redoTop >= 0) free_program_state(ms.redoStack[ms.redoTop--]);
				
				if (ms.current->font) free_font(ms.current->font);
				ms.current->font = new_font;
				ms.current->last_cmd_type = 4;
				printf("Loaded %s (bitmap font %s)\n", path, ms.current->font->name);
			} else {
				printf("Failed to load %s\n", path);
			}
		}
		else if (strcmp(cmd, "TYPE") == 0) {
			if (!ms.current->img) { printf("No image loaded\n"); continue; }
			if (!ms.current->font) { printf("No font loaded\n"); continue; }

			// parsare text intre ghilimele si coordonate/culoare
			char *start_quote = strchr(line, '"');
			char *end_quote = strrchr(line, '"');
			
			if (start_quote && end_quote && end_quote > start_quote) {
				*end_quote = '\0';
				char *text = start_quote + 1;
				int x, y, r, g, b;
				
				if (sscanf(end_quote + 1, "%d %d %d %d %d", &x, &y, &r, &g, &b) == 5) {
					if (ms.undoTop < MAX_STACK - 1) ms.undoStack[++ms.undoTop] = copy_program_state(ms.current);
					while (ms.redoTop >= 0) free_program_state(ms.redoStack[ms.redoTop--]);

					draw_text(ms.current->img, ms.current->font, text, x, y, (Pixel){(unsigned char)r, (unsigned char)g, (unsigned char)b});
					ms.current->last_cmd_type = 5;
					printf("Text written\n");
				}
			} else {
				printf("Invalid command\n");
			}
		}
		else if (strcmp(cmd, "BITCHECK") == 0) {
			run_bitcheck(ms.current->img);
		}
		else if (strcmp(cmd, "UNDO") == 0) {
			if (ms.undoTop >= 0) {
				// mutam starea curenta in redo si scoatem starea veche din undo
				ms.redoStack[++ms.redoTop] = ms.current;
				ms.current = ms.undoStack[ms.undoTop--];
			} else printf("Nothing to undo\n");
		}
		else if (strcmp(cmd, "REDO") == 0) {
			if (ms.redoTop >= 0) {
				// mutam starea curenta in undo si scoatem starea din redo
				ms.undoStack[++ms.undoTop] = ms.current;
				ms.current = ms.redoStack[ms.redoTop--];
				
				// afisam mesajul corespunzator operatiei refacute
				if (ms.current->last_cmd_type == 1) 
					printf("Loaded %s (L-system with %d rules)\n", ms.current->lsys->filename, ms.current->lsys->nrules);
				else if (ms.current->last_cmd_type == 2)
					printf("Loaded input.ppm (PPM image %dx%d)\n", ms.current->img->w, ms.current->img->h);
				else if (ms.current->last_cmd_type == 3)
					printf("Drawing done\n");
				else if (ms.current->last_cmd_type == 4)
					printf("Loaded %s (bitmap font %s)\n", ms.current->font->filename, ms.current->font->name);
				else if (ms.current->last_cmd_type == 5)
					printf("Text written\n");
			} else printf("Nothing to redo\n");
		}
		else if (strcmp(cmd, "EXIT") == 0) {
			break;
		}
		else {
			printf("Invalid command\n");
		}
	}

	free_program_state(ms.current);
	while (ms.undoTop >= 0) free_program_state(ms.undoStack[ms.undoTop--]);
	while (ms.redoTop >= 0) free_program_state(ms.redoStack[ms.redoTop--]);

	return 0;
}
