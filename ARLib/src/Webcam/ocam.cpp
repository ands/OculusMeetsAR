#include <cstdio>
#include <cmath>

#define CMV_MAX_BUF 1024
#define MAX_POL_LENGTH 64

namespace ARLib {

struct ocam_model
{
	double invpol[MAX_POL_LENGTH]; // the coefficients of the inverse polynomial
	int length_invpol;             // length of inverse polynomial
	double xc, yc;                 // center coordinate
	double c, d, e;                // affine parameters
};

ocam_model * ocam_get_model(const char *filename)
{
	char buf[CMV_MAX_BUF];
	
	//Open file
	FILE *f = fopen(filename,"r");
	if(!f)
	{
		printf("File %s cannot be opened\n", filename);	
		return NULL;
	}

	ocam_model *m = new ocam_model;

	//Read polynomial coefficients // NOTE: ignored
	fgets(buf,CMV_MAX_BUF,f);
	fscanf(f,"\n");
	int length_pol;
	fscanf(f,"%d", &length_pol);
	double pol_dummy;
	for (int i = 0; i < length_pol; i++)
		fscanf(f," %lf",&pol_dummy);

	//Read inverse polynomial coefficients
	fscanf(f,"\n");
	fgets(buf,CMV_MAX_BUF,f);
	fscanf(f,"\n");
	fscanf(f,"%d", &m->length_invpol);
	for (int i = 0; i < m->length_invpol; i++)
		fscanf(f," %lf",&m->invpol[i]);

	//Read center coordinates
	fscanf(f,"\n");
	fgets(buf,CMV_MAX_BUF,f);
	fscanf(f,"\n");
	fscanf(f,"%lf %lf\n", &m->xc, &m->yc);

	//Read affine coefficients
	fgets(buf,CMV_MAX_BUF,f);
	fscanf(f,"\n");
	fscanf(f,"%lf %lf %lf\n", &m->c,&m->d,&m->e);

	//Read image size // NOTE: ignored
	/*fgets(buf,CMV_MAX_BUF,f);
	fscanf(f,"\n");
	fscanf(f,"%d %d", &m->height, &m->width);*/

	fclose(f);
	return m;
}

static inline void ocam_world2cam(const ocam_model *m, const double point3D[3], double point2D[2])
{
	double norm_square = point3D[0] * point3D[0] + point3D[1] * point3D[1];
	if (norm_square != 0) 
	{
		double invnorm = 1.0 / sqrt(norm_square);
		double t = atan(point3D[2] * invnorm);
		double rho = m->invpol[0];
		double t_i = 1;

		for (int i = 1; i < m->length_invpol; i++)
		{
			t_i *= t;
			rho += t_i * m->invpol[i];
		}

		double x = point3D[0] * invnorm * rho;
		double y = point3D[1] * invnorm * rho;

		point2D[0] = x * m->c + y * m->d + m->xc;
		point2D[1] = x * m->e + y        + m->yc;
	}
	else
	{
		point2D[0] = m->xc;
		point2D[1] = m->yc;
	}
}

void ocam_create_perspecive_undistortion_map(const ocam_model *m, float *mapxy, int width, int height, float sf)
{
	float Nxc = height / 2.0;
	float Nyc = width / 2.0;
	float Nz  = -width / sf;
	double world[3];
	
	world[2] = Nz;
	for (int i = 0; i < height; i++)
	{
		world[0] = (i - Nxc);
		for (int j = 0; j < width; j++)
		{   
			world[1] = (j - Nyc);
			double cam[2];
			ocam_world2cam(m, world, cam);
			*mapxy++ = (float)cam[1];
			*mapxy++ = (float)cam[0];
		}
	}
}

}; // ARLib namespace