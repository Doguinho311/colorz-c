#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

typedef struct {
	uint8_t r, g, b;
} Pixel;

typedef struct {
	double r, g, b;
	double acc_r, acc_g, acc_b;
	int pixel_count;
} Centroid;

double get_dist_sq(Pixel p1, Centroid k)
{
	double dr = p1.r - k.r;
	double dg = p1.g - k.g;
	double db = p1.b - k.b;

	return dr * dr + dg * dg + db * db;
}

void init_centroids(unsigned char* data, int num_pixels, Centroid *centroids, int k_count)
{
	for (int i = 0; i < k_count; i++)
	{
		int seed = rand() % num_pixels;
		centroids[i].r = data[seed * 3];
		centroids[i].g = data[seed * 3 + 1];
		centroids[i].b = data[seed * 3 + 2];
		centroids[i].pixel_count = 0;
	}

	printf("Centroids criados.\n");
}

void k_init_centroids(unsigned char* data, int num_pixels, Centroid *centroids, int k_count)
{
	int seed = rand() % num_pixels;

	centroids[0].r = data[seed * 3];
	centroids[0].g = data[seed * 3 + 1];
	centroids[0].b = data[seed * 3 + 2];
	centroids[0].pixel_count = 0;

	for (int k = 1; k < k_count; k++)
	{
		double max_dist = -1;
		Pixel choosen_pixel = {0, 0, 0};

		for (int p = 0; p < num_pixels; p += 100)
		{
			Pixel pixel = {data[p * 3], data[p * 3 + 1], data[p * 3 + 2]};

			// Ignora pixeis "chatos (muito claros ou muito escuros)."
			int brilho_total = pixel.r + pixel.g + pixel.b;
			if (brilho_total > 700 || brilho_total < 30) continue;

			double min_dist_to_existing = 1e18; 

			for (int j = 0; j < k; j++)
			{
				double d = get_dist_sq(pixel, centroids[j]);
				if (d < min_dist_to_existing) min_dist_to_existing = d;
			}

			if (min_dist_to_existing > max_dist)
			{
				max_dist = min_dist_to_existing;
				choosen_pixel = pixel;
			}
		}

		centroids[k].r = choosen_pixel.r;
		centroids[k].g = choosen_pixel.g;
		centroids[k].b = choosen_pixel.b;
	}

	printf("Centroides criados.\n");
}

void free_centroids(Centroid *centroids)
{
	free(centroids);
	printf("Centroides liberados.\n");
}

void centroid_main_loop(unsigned char* data, int num_pixels, Centroid *centroids, int k_count)
{
	int max_iteractions = 100;
	double threshold = 0.1;

	for (int iter = 0; iter < max_iteractions; iter++){
		// Reseta acumuladores
		for (int k = 0; k < k_count; k++)
		{
			centroids[k].acc_r = centroids[k].acc_g = centroids[k].acc_b = 0;
			centroids[k].pixel_count = 0;
		}


		for (int p = 0; p < num_pixels; p += 100)
		{	
			double min_dist = 1e18;
			int choosen_centroid = 0;

			Pixel pixel;

			pixel.r = data[p*3];
			pixel.g = data[p*3 + 1];
			pixel.b = data[p*3 + 2];
	
			for (int k = 0; k < k_count; k++)
			{
				double dist = get_dist_sq(pixel, centroids[k]);

				if (dist < min_dist)
				{
					choosen_centroid = k;
				}
			}

			centroids[choosen_centroid].acc_r += pixel.r;
			centroids[choosen_centroid].acc_g += pixel.g;
			centroids[choosen_centroid].acc_b += pixel.b;
			centroids[choosen_centroid].pixel_count++;
		}

		// double max_movement = 0; Feita para testes
		for (int k = 0; k < k_count; k++)
		{
			if (centroids[k].pixel_count > 0)
			{
				double new_r = centroids[k].acc_r / centroids[k].pixel_count;
				double new_g = centroids[k].acc_g / centroids[k].pixel_count;
				double new_b = centroids[k].acc_b / centroids[k].pixel_count;
			
				double movement = (new_r - centroids[k].r)*(new_r - centroids[k].r) +
						(new_g - centroids[k].g)*(new_g - centroids[k].g) +
						(new_b - centroids[k].b)*(new_b - centroids[k].b);

				// if (max_movement < movement) max_movement = movement;

				centroids[k].r = new_r;
				centroids[k].g = new_g;
				centroids[k].b = new_b;
			}

		}

		// printf("Iteração %d, movimento máximo %.4f\n", iter, max_movement);

		// if (max_movement < threshold)
		// {
		// 	printf("Convergiu na iteração: %d\n", iter);
		// 	break;
		// }

	}
}

void print_palette(Centroid* centroids, int k_count)
{
	printf("\n --- Paleta gerada --- \n");

	for (int k = 0; k < k_count; k++)
	{
		int r = (int)(centroids[k].r + 0.5);
		int g = (int)(centroids[k].g + 0.5);
		int b = (int)(centroids[k].b + 0.5);

		printf("\033[48;2;%d;%d;%dm  \033[0m", r, g, b);

		printf(" #%02X%02X%02X (R: %3d, G: %3d, B: %3d)\n", r, g, b, r, g, b);
	}

	printf("-------------------------\n");
}


int compare_luminance(const void *a, const void *b)
{
	Centroid *c1 = (Centroid*)a;
	Centroid *c2 = (Centroid*)b;

	double lum1 = (0.2126 * c1->r) + (0.7152 * c1->g) + (0.0722 * c1->b);
	double lum2 = (0.2126 * c2->r) + (0.7152 * c2->g) + (0.0722 * c2->b);

	if (lum1 < lum2) return -1;
	if (lum1 > lum2) return 1;
	return 0;
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("Uso: ./main <nome do arquivo>.\n");
		exit(1);
	}

	const char* filename = argv[1];

	int width, height, channels;
	unsigned char* img = stbi_load(filename, &width, &height, &channels, 3);
	int centroid_count = 18;

	srand(time(NULL));

	if (img == NULL)
	{
		printf("Falha ao carregar a imagem '%s'.\n", filename);
		exit(1);
	}

	printf("Imagem carregada: %dx%d, com %d canais.\n", width, height, channels);

	Centroid* centroids = malloc(centroid_count * sizeof(Centroid));

	k_init_centroids(img, width * height, centroids, centroid_count);

	centroid_main_loop(img, width * height, centroids, centroid_count);

	qsort(centroids, centroid_count, sizeof(Centroid), compare_luminance);

	print_palette(centroids, centroid_count);

	free_centroids(centroids);
	stbi_image_free(img);

	return 0;
}
