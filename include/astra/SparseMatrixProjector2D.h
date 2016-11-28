/*
-----------------------------------------------------------------------
Copyright: 2010-2016, iMinds-Vision Lab, University of Antwerp
           2014-2016, CWI, Amsterdam

Contact: astra@uantwerpen.be
Website: http://www.astra-toolbox.com/

This file is part of the ASTRA Toolbox.


The ASTRA Toolbox is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

The ASTRA Toolbox is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with the ASTRA Toolbox. If not, see <http://www.gnu.org/licenses/>.

-----------------------------------------------------------------------
*/

#ifndef _INC_ASTRA_SPARSEMATRIXPROJECTOR2D
#define _INC_ASTRA_SPARSEMATRIXPROJECTOR2D

#include "SparseMatrixProjectionGeometry2D.h"
#include "SparseMatrix.h"
#include "Float32Data2D.h"
#include "Projector2D.h"

namespace astra
{


/** This class implements a two-dimensional projector using a projection geometry defined by an arbitrary sparse matrix.
 *
 * \par XML Configuration
 * \astra_xml_item{ProjectionGeometry, xml node, The geometry of the projection.}
 * \astra_xml_item{VolumeGeometry, xml node, The geometry of the volume.}
 *
 * \par MATLAB example
 * \astra_code{
 *		cfg = astra_struct('sparse_matrix');\n
 *		cfg.ProjectionGeometry = proj_geom;\n
 *		cfg.VolumeGeometry = vol_geom;\n
 *		proj_id = astra_mex_projector('create'\, cfg);\n
 * }
 */
class _AstraExport CSparseMatrixProjector2D : public CProjector2D {

protected:
	
	/** Initial clearing. Only to be used by constructors.
	 */
	virtual void _clear();

	/** Check the values of this object.  If everything is ok, the object can be set to the initialized state.
	 * The following statements are then guaranteed to hold:
	 * - no NULL pointers
	 * - all sub-objects are initialized properly
	 * - matrix dimensions match volume geometry
	 */
	virtual bool _check();

public:

	// type of the projector, needed to register with CProjectorFactory
	static std::string type;

	/** Default constructor.
	 */
	CSparseMatrixProjector2D();

	/** Constructor.
	 * 
	 * @param _pProjectionGeometry		Information class about the geometry of the projection.  Will be HARDCOPIED.
	 * @param _pReconstructionGeometry	Information class about the geometry of the reconstruction volume. Will be HARDCOPIED.
	 */
	CSparseMatrixProjector2D(CSparseMatrixProjectionGeometry2D* _pProjectionGeometry, 
	                         CVolumeGeometry2D* _pReconstructionGeometry);
	
	/** Destructor, is virtual to show that we are aware subclass destructor are called.
	 */	
	~CSparseMatrixProjector2D();

	/** Initialize the projector with a config object.
	 *
	 * @param _cfg Configuration Object
	 * @return initialization successful?
	 */
	virtual bool initialize(const Config& _cfg);

	/** Initialize the projector.
	 *
	 * @param _pProjectionGeometry		Information class about the geometry of the projection.  Will be HARDCOPIED.
	 * @param _pReconstructionGeometry	Information class about the geometry of the reconstruction volume.  Will be HARDCOPIED.
	 * @return initialization successful?
	 */
	virtual bool initialize(CSparseMatrixProjectionGeometry2D* _pProjectionGeometry, 
		                    CVolumeGeometry2D* _pReconstructionGeometry);

	/** Clear this class.
	 */
	virtual void clear();

	/** Returns the number of weights required for storage of all weights of one projection.
	 *
	 * @param _iProjectionIndex Index of the projection (zero-based).
	 * @return Size of buffer (given in SPixelWeight elements) needed to store weighted pixels.
	 */
	virtual int getProjectionWeightsCount(int _iProjectionIndex);

	/** Compute the pixel weights for a single ray, from the source to a detector pixel. 
	 *
	 * @param _iProjectionIndex	Index of the projection 
	 * @param _iDetectorIndex	Index of the detector pixel
	 * @param _pWeightedPixels	Pointer to a pre-allocated array, consisting of _iMaxPixelCount elements
	 *							of type SPixelWeight. On return, this array contains a list of the index
	 *							and weight for all pixels on the ray.
	 * @param _iMaxPixelCount	Maximum number of pixels (and corresponding weights) that can be stored in _pWeightedPixels.
	 *							This number MUST be greater than the total number of pixels on the ray.
	 * @param _iStoredPixelCount On return, this variable contains the total number of pixels on the 
	 *                           ray (that have been stored in the list _pWeightedPixels). 
     */
	virtual void computeSingleRayWeights(int _iProjectionIndex, 
										 int _iDetectorIndex, 
										 SPixelWeight* _pWeightedPixels,
		                                 int _iMaxPixelCount, 
										 int& _iStoredPixelCount);
	
	/** Create a list of detectors that are influenced by point [_iRow, _iCol].
	 *
	 * @param _iRow row of the point
	 * @param _iCol column of the point
	 * @return list of SDetector2D structs
	 */
	virtual std::vector<SDetector2D> projectPoint(int _iRow, int _iCol);

	/** Policy-based projection of all rays.  This function will calculate each non-zero projection 
	 * weight and use this value for a task provided by the policy object.
	 *
	 * @param _policy Policy object.  Should contain prior, addWeight and posterior function.
	 */
	template <typename Policy>
	void project(Policy& _policy);

	/** Policy-based projection of all rays of a single projection.  This function will calculate 
	 * each non-zero projection weight and use this value for a task provided by the policy object.
	 *
	 * @param _iProjection Wwhich projection should be projected?
	 * @param _policy Policy object.  Should contain prior, addWeight and posterior function.
	 */
	template <typename Policy>
	void projectSingleProjection(int _iProjection, Policy& _policy);

	/** Policy-based projection of a single ray.  This function will calculate each non-zero 
	 * projection  weight and use this value for a task provided by the policy object.
	 *
	 * @param _iProjection Which projection should be projected?
	 * @param _iDetector Which detector should be projected?
	 * @param _policy Policy object.  Should contain prior, addWeight and posterior function.
	 */
	template <typename Policy>
	void projectSingleRay(int _iProjection, int _iDetector, Policy& _policy);

	/** Policy-based voxel-projection of a single pixel.  This function will calculate 
	 * each non-zero projection weight and use this value for a task provided by the policy object.
	 *
	 * @param _iRow
	 * @param _iCol
	 * @param _policy Policy object.  Should contain prior, addWeight and posterior function.
	 */
	template <typename Policy>
	void projectSingleVoxel(int _iRow, int _iCol, Policy& _policy) {}

	/** Policy-based voxel-projection of all voxels.  This function will calculate 
	 * each non-zero projection weight and use this value for a task provided by the policy object.
	 *
	 * @param _policy Policy object.  Should contain prior, addWeight and posterior function.
	 */
	template <typename Policy>
	void projectAllVoxels(Policy& _policy) {}

protected:	

	/** Return the  type of this projector.
	 *
	 * @return identification type of this projector
	 */
	virtual std::string getType();

};

//----------------------------------------------------------------------------------------

inline std::string CSparseMatrixProjector2D::getType() 
{ 
	return type; 
}

} // namespace astra

#endif 

