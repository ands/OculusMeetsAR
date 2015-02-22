#ifndef OCAM_H
#define OCAM_H

namespace ARLib {

	struct ocam_model;

	// loads camera undistortion parameters
	ocam_model * ocam_get_model(const char *filename);
	// single undistortion conversion
	void ocam_world2cam(const ocam_model *m, const double point3D[3], double point2D[2]);

}; // ARLib namespace

#endif OCAM_H