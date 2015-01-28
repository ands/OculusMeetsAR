#ifndef OCAM_H
#define OCAM_H

namespace ARLib {

	struct ocam_model;

	// loads camera undistortion parameters
	ocam_model * ocam_get_model(const char *filename);
	// creates a lookup table to undistort camera images
	void ocam_create_perspecive_undistortion_map(const ocam_model *m, float *mapxy, int width, int height, float sf);

}; // ARLib namespace

#endif OCAM_H