//  SMESH SMESH : implementaion of SMESH idl descriptions
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : SMESH_Hexa_3D.cxx
//  Author : Paul RASCLE, EDF
//  Module : SMESH
//  $Header$

using namespace std;
#include "SMESH_Hexa_3D.hxx"
#include "SMESH_Quadrangle_2D.hxx"
#include "SMESH_Gen.hxx"
#include "SMESH_Mesh.hxx"

#include "SMDS_MeshElement.hxx"
#include "SMDS_MeshNode.hxx"
#include "SMDS_FacePosition.hxx"

#include <TopExp.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <BRep_Tool.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <Handle_Geom2d_Curve.hxx>
#include <Handle_Geom_Curve.hxx>

#include "utilities.h"

//=============================================================================
/*!
 *  
 */
//=============================================================================

SMESH_Hexa_3D::SMESH_Hexa_3D(int hypId, int studyId,
	SMESH_Gen * gen):SMESH_3D_Algo(hypId, studyId, gen)
{
	MESSAGE("SMESH_Hexa_3D::SMESH_Hexa_3D");
	_name = "Hexa_3D";
//   _shapeType = TopAbs_SOLID;
	_shapeType = (1 << TopAbs_SHELL) | (1 << TopAbs_SOLID);	// 1 bit /shape type
//   MESSAGE("_shapeType octal " << oct << _shapeType);
	for (int i = 0; i < 6; i++)
		_quads[i] = 0;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

SMESH_Hexa_3D::~SMESH_Hexa_3D()
{
	MESSAGE("SMESH_Hexa_3D::~SMESH_Hexa_3D");
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

bool SMESH_Hexa_3D::CheckHypothesis(SMESH_Mesh & aMesh,
	const TopoDS_Shape & aShape)
{
	MESSAGE("SMESH_Hexa_3D::CheckHypothesis");

	bool isOk = true;

	// nothing to check

	return isOk;
}

//=============================================================================
/*!
 * Hexahedron mesh on hexaedron like form
 * -0.  - shape and face mesh verification
 * -1.  - identify faces and vertices of the "cube"
 * -2.  - Algorithm from:
 * "Application de l'interpolation transfinie � la cr�ation de maillages
 *  C0 ou G1 continus sur des triangles, quadrangles, tetraedres, pentaedres
 *  et hexaedres d�form�s."
 * Alain PERONNET - 8 janvier 1999
 */
//=============================================================================

bool SMESH_Hexa_3D::Compute(SMESH_Mesh & aMesh,
	const TopoDS_Shape & aShape)throw(SALOME_Exception)
{
	MESSAGE("SMESH_Hexa_3D::Compute");

	bool isOk = false;
	SMESHDS_Mesh * meshDS = aMesh.GetMeshDS();
	SMESH_subMesh *theSubMesh = aMesh.GetSubMesh(aShape);
	//const SMESHDS_SubMesh *& subMeshDS = theSubMesh->GetSubMeshDS();

	// 0.  - shape and face mesh verification
	// 0.1 - shape must be a solid (or a shell) with 6 faces
	MESSAGE("---");

	vector < SMESH_subMesh * >meshFaces;
	for (TopExp_Explorer exp(aShape, TopAbs_FACE); exp.More(); exp.Next())
	{
		SMESH_subMesh *aSubMesh = aMesh.GetSubMeshContaining(exp.Current());
		ASSERT(aSubMesh);
		meshFaces.push_back(aSubMesh);
	}
	if (meshFaces.size() != 6)
	{
		SCRUTE(meshFaces.size());
		ASSERT(0);
		return false;
	}

	// 0.2 - is each face meshed with Quadrangle_2D? (so, with a wire of 4 edges)
	MESSAGE("---");

	for (int i = 0; i < 6; i++)
	{
		TopoDS_Shape aShape = meshFaces[i]->GetSubShape();
		SMESH_Algo *algo = _gen->GetAlgo(aMesh, aShape);
		string algoName = algo->GetName();
		if (algoName != "Quadrangle_2D")
		{
			// *** delete _quads
			SCRUTE(algoName);
			ASSERT(0);
			return false;
		}
		SMESH_Quadrangle_2D *quadAlgo =
			dynamic_cast < SMESH_Quadrangle_2D * >(algo);
		ASSERT(quadAlgo);
		try
		{
			_quads[i] = quadAlgo->CheckAnd2Dcompute(aMesh, aShape);
			// *** to delete after usage
		}
		catch(SALOME_Exception & S_ex)
		{
			// *** delete _quads
			// *** throw exception
			ASSERT(0);
		}
	}

	// 1.  - identify faces and vertices of the "cube"
	// 1.1 - ancestor maps vertex->edges in the cube
	MESSAGE("---");

	TopTools_IndexedDataMapOfShapeListOfShape MS;
	TopExp::MapShapesAndAncestors(aShape, TopAbs_VERTEX, TopAbs_EDGE, MS);

	// 1.2 - first face is choosen as face Y=0 of the unit cube
	MESSAGE("---");

	const TopoDS_Shape & aFace = meshFaces[0]->GetSubShape();
	const TopoDS_Face & F = TopoDS::Face(aFace);

	// 1.3 - identify the 4 vertices of the face Y=0: V000, V100, V101, V001
	MESSAGE("---");

	int i = 0;
	TopoDS_Edge E = _quads[0]->edge[i];	//edge will be Y=0,Z=0 on unit cube
	double f, l;
	Handle(Geom2d_Curve) C2d = BRep_Tool::CurveOnSurface(E, F, f, l);
	TopoDS_Vertex VFirst, VLast;
	TopExp::Vertices(E, VFirst, VLast);	// corresponds to f and l
	bool isForward =
		(((l - f) * (_quads[0]->last[i] - _quads[0]->first[i])) > 0);

	if (isForward)
	{
		_cube.V000 = VFirst;	// will be (0,0,0) on the unit cube
		_cube.V100 = VLast;		// will be (1,0,0) on the unit cube
	}
	else
	{
		_cube.V000 = VLast;
		_cube.V100 = VFirst;
	}

	i = 1;
	E = _quads[0]->edge[i];
	C2d = BRep_Tool::CurveOnSurface(E, F, f, l);
	TopExp::Vertices(E, VFirst, VLast);
	isForward = (((l - f) * (_quads[0]->last[i] - _quads[0]->first[i])) > 0);
	if (isForward)
		_cube.V101 = VLast;		// will be (1,0,1) on the unit cube
	else
		_cube.V101 = VFirst;

	i = 2;
	E = _quads[0]->edge[i];
	C2d = BRep_Tool::CurveOnSurface(E, F, f, l);
	TopExp::Vertices(E, VFirst, VLast);
	isForward = (((l - f) * (_quads[0]->last[i] - _quads[0]->first[i])) > 0);
	if (isForward)
		_cube.V001 = VLast;		// will be (0,0,1) on the unit cube
	else
		_cube.V001 = VFirst;

	// 1.4 - find edge X=0, Z=0 (ancestor of V000 not in face Y=0)
	//     - find edge X=1, Z=0 (ancestor of V100 not in face Y=0)
	//     - find edge X=1, Z=1 (ancestor of V101 not in face Y=0) 
	//     - find edge X=0, Z=1 (ancestor of V001 not in face Y=0)
	MESSAGE("---");

	TopoDS_Edge E_0Y0 = EdgeNotInFace(aMesh, aShape, F, _cube.V000, MS);
	ASSERT(!E_0Y0.IsNull());

	TopoDS_Edge E_1Y0 = EdgeNotInFace(aMesh, aShape, F, _cube.V100, MS);
	ASSERT(!E_1Y0.IsNull());

	TopoDS_Edge E_1Y1 = EdgeNotInFace(aMesh, aShape, F, _cube.V101, MS);
	ASSERT(!E_1Y1.IsNull());

	TopoDS_Edge E_0Y1 = EdgeNotInFace(aMesh, aShape, F, _cube.V001, MS);
	ASSERT(!E_0Y1.IsNull());

	// 1.5 - identify the 4 vertices in face Y=1: V010, V110, V111, V011
	MESSAGE("---");

	TopExp::Vertices(E_0Y0, VFirst, VLast);
	if (VFirst.IsSame(_cube.V000))
		_cube.V010 = VLast;
	else
		_cube.V010 = VFirst;

	TopExp::Vertices(E_1Y0, VFirst, VLast);
	if (VFirst.IsSame(_cube.V100))
		_cube.V110 = VLast;
	else
		_cube.V110 = VFirst;

	TopExp::Vertices(E_1Y1, VFirst, VLast);
	if (VFirst.IsSame(_cube.V101))
		_cube.V111 = VLast;
	else
		_cube.V111 = VFirst;

	TopExp::Vertices(E_0Y1, VFirst, VLast);
	if (VFirst.IsSame(_cube.V001))
		_cube.V011 = VLast;
	else
		_cube.V011 = VFirst;

	// 1.6 - find remaining faces given 4 vertices
	MESSAGE("---");

	_indY0 = 0;
	_cube.quad_Y0 = _quads[_indY0];

	_indY1 = GetFaceIndex(aMesh, aShape, meshFaces,
		_cube.V010, _cube.V011, _cube.V110, _cube.V111);
	_cube.quad_Y1 = _quads[_indY1];

	_indZ0 = GetFaceIndex(aMesh, aShape, meshFaces,
		_cube.V000, _cube.V010, _cube.V100, _cube.V110);
	_cube.quad_Z0 = _quads[_indZ0];

	_indZ1 = GetFaceIndex(aMesh, aShape, meshFaces,
		_cube.V001, _cube.V011, _cube.V101, _cube.V111);
	_cube.quad_Z1 = _quads[_indZ1];

	_indX0 = GetFaceIndex(aMesh, aShape, meshFaces,
		_cube.V000, _cube.V001, _cube.V010, _cube.V011);
	_cube.quad_X0 = _quads[_indX0];

	_indX1 = GetFaceIndex(aMesh, aShape, meshFaces,
		_cube.V100, _cube.V101, _cube.V110, _cube.V111);
	_cube.quad_X1 = _quads[_indX1];

	MESSAGE("---");

	// 1.7 - get convertion coefs from face 2D normalized to 3D normalized

	Conv2DStruct cx0;			// for face X=0
	Conv2DStruct cx1;			// for face X=1
	Conv2DStruct cy0;
	Conv2DStruct cy1;
	Conv2DStruct cz0;
	Conv2DStruct cz1;

	GetConv2DCoefs(*_cube.quad_X0, meshFaces[_indX0]->GetSubShape(),
		_cube.V000, _cube.V010, _cube.V011, _cube.V001, cx0);
	GetConv2DCoefs(*_cube.quad_X1, meshFaces[_indX1]->GetSubShape(),
		_cube.V100, _cube.V110, _cube.V111, _cube.V101, cx1);
	GetConv2DCoefs(*_cube.quad_Y0, meshFaces[_indY0]->GetSubShape(),
		_cube.V000, _cube.V100, _cube.V101, _cube.V001, cy0);
	GetConv2DCoefs(*_cube.quad_Y1, meshFaces[_indY1]->GetSubShape(),
		_cube.V010, _cube.V110, _cube.V111, _cube.V011, cy1);
	GetConv2DCoefs(*_cube.quad_Z0, meshFaces[_indZ0]->GetSubShape(),
		_cube.V000, _cube.V100, _cube.V110, _cube.V010, cz0);
	GetConv2DCoefs(*_cube.quad_Z1, meshFaces[_indZ1]->GetSubShape(),
		_cube.V001, _cube.V101, _cube.V111, _cube.V011, cz1);

	// 1.8 - create a 3D structure for normalized values

	MESSAGE("---");
	int nbx = _cube.quad_Y0->nbPts[0];
	int nby = _cube.quad_Y0->nbPts[1];
	int nbz;
	if (cx0.a1 != 0)
		nbz = _cube.quad_X0->nbPts[1];
	else
		nbz = _cube.quad_X0->nbPts[0];
	//SCRUTE(nbx);
	//SCRUTE(nby);
	//SCRUTE(nbz);
	int nbxyz = nbx * nby * nbz;
	Point3DStruct *np = new Point3DStruct[nbxyz];

	// 1.9 - store node indexes of faces

	{
		const TopoDS_Face & F = TopoDS::Face(meshFaces[_indX0]->GetSubShape());
		const vector<int> & indElt
			= aMesh.GetSubMesh(F)->GetSubMeshDS()->GetIDNodes();

		faceQuadStruct *quad = _cube.quad_X0;
		int i = 0;				// j = x/face , k = y/face
		int nbdown = quad->nbPts[0];
		int nbright = quad->nbPts[1];

		for (int itf=0; itf<indElt.size(); itf++)
		{
			int nodeId = indElt[itf];
			const SMDS_MeshNode * node = meshDS->FindNode(nodeId);
			const SMDS_FacePosition* fpos
				= static_cast<const SMDS_FacePosition*>(node->GetPosition());
			double ri = fpos->GetUParameter();
			double rj = fpos->GetVParameter();
			int i1 = int (ri);
			int j1 = int (rj);
			int ij1 = j1 * nbdown + i1;
			quad->uv_grid[ij1].nodeId = nodeId;
		}

		for (int i1 = 0; i1 < nbdown; i1++)
			for (int j1 = 0; j1 < nbright; j1++)
			{
				int ij1 = j1 * nbdown + i1;
				int j = cx0.ia * i1 + cx0.ib * j1 + cx0.ic;	// j = x/face
				int k = cx0.ja * i1 + cx0.jb * j1 + cx0.jc;	// k = y/face
				int ijk = k * nbx * nby + j * nbx + i;
				//MESSAGE(" "<<ij1<<" "<<i<<" "<<j<<" "<<ijk);
				np[ijk].nodeId = quad->uv_grid[ij1].nodeId;
				//SCRUTE(np[ijk].nodeId);
			}
	}

	{
		const TopoDS_Face & F = TopoDS::Face(meshFaces[_indX1]->GetSubShape());
		const vector<int> & indElt
			= aMesh.GetSubMesh(F)->GetSubMeshDS()->GetIDNodes();

		faceQuadStruct *quad = _cube.quad_X1;
		int i = nbx - 1;		// j = x/face , k = y/face
		int nbdown = quad->nbPts[0];
		int nbright = quad->nbPts[1];

		for (int itf=0; itf<indElt.size(); itf++)
		{
			int nodeId = indElt[itf];
			const SMDS_MeshNode * node = meshDS->FindNode(nodeId);
			const SMDS_FacePosition* fpos
				= static_cast<const SMDS_FacePosition*>(node->GetPosition());
			double ri = fpos->GetUParameter();
			double rj = fpos->GetVParameter();
			int i1 = int (ri);
			int j1 = int (rj);
			int ij1 = j1 * nbdown + i1;
			quad->uv_grid[ij1].nodeId = nodeId;
		}

		for (int i1 = 0; i1 < nbdown; i1++)
			for (int j1 = 0; j1 < nbright; j1++)
			{
				int ij1 = j1 * nbdown + i1;
				int j = cx1.ia * i1 + cx1.ib * j1 + cx1.ic;	// j = x/face
				int k = cx1.ja * i1 + cx1.jb * j1 + cx1.jc;	// k = y/face
				int ijk = k * nbx * nby + j * nbx + i;
				//MESSAGE(" "<<ij1<<" "<<i<<" "<<j<<" "<<ijk);
				np[ijk].nodeId = quad->uv_grid[ij1].nodeId;
				//SCRUTE(np[ijk].nodeId);
			}
	}

	{
		const TopoDS_Face & F = TopoDS::Face(meshFaces[_indY0]->GetSubShape());
		const vector<int> & indElt
			= aMesh.GetSubMesh(F)->GetSubMeshDS()->GetIDNodes();

		faceQuadStruct *quad = _cube.quad_Y0;
		int j = 0;				// i = x/face , k = y/face
		int nbdown = quad->nbPts[0];
		int nbright = quad->nbPts[1];

		for (int itf=0; itf<indElt.size(); itf++)
		{
			int nodeId = indElt[itf];
			const SMDS_MeshNode * node = meshDS->FindNode(nodeId);
			const SMDS_FacePosition * fpos
				= static_cast<const SMDS_FacePosition*>(node->GetPosition());
			double ri = fpos->GetUParameter();
			double rj = fpos->GetVParameter();
			int i1 = int (ri);
			int j1 = int (rj);
			int ij1 = j1 * nbdown + i1;
			quad->uv_grid[ij1].nodeId = nodeId;
		}

		for (int i1 = 0; i1 < nbdown; i1++)
			for (int j1 = 0; j1 < nbright; j1++)
			{
				int ij1 = j1 * nbdown + i1;
				int i = cy0.ia * i1 + cy0.ib * j1 + cy0.ic;	// i = x/face
				int k = cy0.ja * i1 + cy0.jb * j1 + cy0.jc;	// k = y/face
				int ijk = k * nbx * nby + j * nbx + i;
				//MESSAGE(" "<<ij1<<" "<<i<<" "<<j<<" "<<ijk);
				np[ijk].nodeId = quad->uv_grid[ij1].nodeId;
				//SCRUTE(np[ijk].nodeId);
			}
	}

	{
		const TopoDS_Face & F = TopoDS::Face(meshFaces[_indY1]->GetSubShape());
		const vector<int> & indElt
			= aMesh.GetSubMesh(F)->GetSubMeshDS()->GetIDNodes();

		faceQuadStruct *quad = _cube.quad_Y1;
		int j = nby - 1;		// i = x/face , k = y/face
		int nbdown = quad->nbPts[0];
		int nbright = quad->nbPts[1];

		for (int itf=0; itf<indElt.size(); itf++)
		{
			int nodeId = indElt[itf];
			const SMDS_MeshNode * node = meshDS->FindNode(nodeId);
			const SMDS_FacePosition* fpos =
				static_cast<const SMDS_FacePosition *>(node->GetPosition());
			double ri = fpos->GetUParameter();
			double rj = fpos->GetVParameter();
			int i1 = int (ri);
			int j1 = int (rj);
			int ij1 = j1 * nbdown + i1;
			quad->uv_grid[ij1].nodeId = nodeId;
		}

		for (int i1 = 0; i1 < nbdown; i1++)
			for (int j1 = 0; j1 < nbright; j1++)
			{
				int ij1 = j1 * nbdown + i1;
				int i = cy1.ia * i1 + cy1.ib * j1 + cy1.ic;	// i = x/face
				int k = cy1.ja * i1 + cy1.jb * j1 + cy1.jc;	// k = y/face
				int ijk = k * nbx * nby + j * nbx + i;
				//MESSAGE(" "<<ij1<<" "<<i<<" "<<j<<" "<<ijk);
				np[ijk].nodeId = quad->uv_grid[ij1].nodeId;
				//SCRUTE(np[ijk].nodeId);
			}
	}

	{
		const TopoDS_Face & F = TopoDS::Face(meshFaces[_indZ0]->GetSubShape());
		const vector<int> & indElt
			= aMesh.GetSubMesh(F)->GetSubMeshDS()->GetIDNodes();

		faceQuadStruct *quad = _cube.quad_Z0;
		int k = 0;				// i = x/face , j = y/face
		int nbdown = quad->nbPts[0];
		int nbright = quad->nbPts[1];

		for (int itf=0; itf<indElt.size(); itf++)
		{
			int nodeId = indElt[itf];
			const SMDS_MeshNode * node = meshDS->FindNode(nodeId);
			const SMDS_FacePosition * fpos
				= static_cast<const SMDS_FacePosition*>(node->GetPosition());
			double ri = fpos->GetUParameter();
			double rj = fpos->GetVParameter();
			int i1 = int (ri);
			int j1 = int (rj);
			int ij1 = j1 * nbdown + i1;
			quad->uv_grid[ij1].nodeId = nodeId;
		}

		for (int i1 = 0; i1 < nbdown; i1++)
			for (int j1 = 0; j1 < nbright; j1++)
			{
				int ij1 = j1 * nbdown + i1;
				int i = cz0.ia * i1 + cz0.ib * j1 + cz0.ic;	// i = x/face
				int j = cz0.ja * i1 + cz0.jb * j1 + cz0.jc;	// j = y/face
				int ijk = k * nbx * nby + j * nbx + i;
				//MESSAGE(" "<<ij1<<" "<<i<<" "<<j<<" "<<ijk);
				np[ijk].nodeId = quad->uv_grid[ij1].nodeId;
				//SCRUTE(np[ijk].nodeId);
			}
	}

	{
		const TopoDS_Face & F = TopoDS::Face(meshFaces[_indZ1]->GetSubShape());
		const vector<int> & indElt
			= aMesh.GetSubMesh(F)->GetSubMeshDS()->GetIDNodes();

		faceQuadStruct *quad = _cube.quad_Z1;
		int k = nbz - 1;		// i = x/face , j = y/face
		int nbdown = quad->nbPts[0];
		int nbright = quad->nbPts[1];

		for(int itf=0; itf<indElt.size(); itf++)
		{
			int nodeId = indElt[itf];
			const SMDS_MeshNode * node = meshDS->FindNode(nodeId);
			const SMDS_FacePosition* fpos
				= static_cast<const SMDS_FacePosition*>(node->GetPosition());
			double ri = fpos->GetUParameter();
			double rj = fpos->GetVParameter();
			int i1 = int (ri);
			int j1 = int (rj);
			int ij1 = j1 * nbdown + i1;
			quad->uv_grid[ij1].nodeId = nodeId;
		}

		for (int i1 = 0; i1 < nbdown; i1++)
			for (int j1 = 0; j1 < nbright; j1++)
			{
				int ij1 = j1 * nbdown + i1;
				int i = cz1.ia * i1 + cz1.ib * j1 + cz1.ic;	// i = x/face
				int j = cz1.ja * i1 + cz1.jb * j1 + cz1.jc;	// j = y/face
				int ijk = k * nbx * nby + j * nbx + i;
				//MESSAGE(" "<<ij1<<" "<<i<<" "<<j<<" "<<ijk);
				np[ijk].nodeId = quad->uv_grid[ij1].nodeId;
				//SCRUTE(np[ijk].nodeId);
			}
	}

	// 2.0 - for each node of the cube:
	//       - get the 8 points 3D = 8 vertices of the cube
	//       - get the 12 points 3D on the 12 edges of the cube
	//       - get the 6 points 3D on the 6 faces with their ID
	//       - compute the point 3D
	//       - store the point 3D in SMESHDS, store its ID in 3D structure

	TopoDS_Shell aShell;
	TopExp_Explorer exp(aShape, TopAbs_SHELL);
	if (exp.More())
	{
		aShell = TopoDS::Shell(exp.Current());
	}
	else
	{
		MESSAGE("no shell...");
		ASSERT(0);
	}

	Pt3 p000, p001, p010, p011, p100, p101, p110, p111;
	Pt3 px00, px01, px10, px11;
	Pt3 p0y0, p0y1, p1y0, p1y1;
	Pt3 p00z, p01z, p10z, p11z;
	Pt3 pxy0, pxy1, px0z, px1z, p0yz, p1yz;

	GetPoint(p000, 0, 0, 0, nbx, nby, nbz, np, meshDS);
	GetPoint(p001, 0, 0, nbz - 1, nbx, nby, nbz, np, meshDS);
	GetPoint(p010, 0, nby - 1, 0, nbx, nby, nbz, np, meshDS);
	GetPoint(p011, 0, nby - 1, nbz - 1, nbx, nby, nbz, np, meshDS);
	GetPoint(p100, nbx - 1, 0, 0, nbx, nby, nbz, np, meshDS);
	GetPoint(p101, nbx - 1, 0, nbz - 1, nbx, nby, nbz, np, meshDS);
	GetPoint(p110, nbx - 1, nby - 1, 0, nbx, nby, nbz, np, meshDS);
	GetPoint(p111, nbx - 1, nby - 1, nbz - 1, nbx, nby, nbz, np, meshDS);

	for (int i = 1; i < nbx - 1; i++)
	{
		for (int j = 1; j < nby - 1; j++)
		{
			for (int k = 1; k < nbz - 1; k++)
			{
				// *** seulement maillage regulier
				// 12 points on edges
				GetPoint(px00, i, 0, 0, nbx, nby, nbz, np, meshDS);
				GetPoint(px01, i, 0, nbz - 1, nbx, nby, nbz, np, meshDS);
				GetPoint(px10, i, nby - 1, 0, nbx, nby, nbz, np, meshDS);
				GetPoint(px11, i, nby - 1, nbz - 1, nbx, nby, nbz, np, meshDS);

				GetPoint(p0y0, 0, j, 0, nbx, nby, nbz, np, meshDS);
				GetPoint(p0y1, 0, j, nbz - 1, nbx, nby, nbz, np, meshDS);
				GetPoint(p1y0, nbx - 1, j, 0, nbx, nby, nbz, np, meshDS);
				GetPoint(p1y1, nbx - 1, j, nbz - 1, nbx, nby, nbz, np, meshDS);

				GetPoint(p00z, 0, 0, k, nbx, nby, nbz, np, meshDS);
				GetPoint(p01z, 0, nby - 1, k, nbx, nby, nbz, np, meshDS);
				GetPoint(p10z, nbx - 1, 0, k, nbx, nby, nbz, np, meshDS);
				GetPoint(p11z, nbx - 1, nby - 1, k, nbx, nby, nbz, np, meshDS);

				// 12 points on faces
				GetPoint(pxy0, i, j, 0, nbx, nby, nbz, np, meshDS);
				GetPoint(pxy1, i, j, nbz - 1, nbx, nby, nbz, np, meshDS);
				GetPoint(px0z, i, 0, k, nbx, nby, nbz, np, meshDS);
				GetPoint(px1z, i, nby - 1, k, nbx, nby, nbz, np, meshDS);
				GetPoint(p0yz, 0, j, k, nbx, nby, nbz, np, meshDS);
				GetPoint(p1yz, nbx - 1, j, k, nbx, nby, nbz, np, meshDS);

				int ijk = k * nbx * nby + j * nbx + i;
				double x = double (i) / double (nbx - 1);	// *** seulement
				double y = double (j) / double (nby - 1);	// *** maillage
				double z = double (k) / double (nbz - 1);	// *** regulier

				Pt3 X;
				for (int i = 0; i < 3; i++)
				{
					X[i] =
						(1 - x) * p0yz[i] + x * p1yz[i]
						+ (1 - y) * px0z[i] + y * px1z[i]
						+ (1 - z) * pxy0[i] + z * pxy1[i]
						- (1 - x) * ((1 - y) * p00z[i] + y * p01z[i])
						- x * ((1 - y) * p10z[i] + y * p11z[i])
						- (1 - y) * ((1 - z) * px00[i] + z * px01[i])
						- y * ((1 - z) * px10[i] + z * px11[i])
						- (1 - z) * ((1 - x) * p0y0[i] + x * p1y0[i])
						- z * ((1 - x) * p0y1[i] + x * p1y1[i])
						+ (1 - x) * ((1 - y) * ((1 - z) * p000[i] + z * p001[i])
						+ y * ((1 - z) * p010[i] + z * p011[i]))
						+ x * ((1 - y) * ((1 - z) * p100[i] + z * p101[i])
						+ y * ((1 - z) * p110[i] + z * p111[i]));
				}

				SMDS_MeshNode * node = meshDS->AddNode(X[0], X[1], X[2]);
				np[ijk].nodeId = node->GetID();
				//meshDS->SetNodeInVolume(node, TopoDS::Solid(aShape));
				meshDS->SetNodeInVolume(node, aShell);
			}
		}
	}

	//2.1 - for each node of the cube (less 3 *1 Faces):
	//      - store hexahedron in SMESHDS
	MESSAGE("Storing hexahedron into the DS");
	for (int i = 0; i < nbx - 1; i++)
		for (int j = 0; j < nby - 1; j++)
			for (int k = 0; k < nbz - 1; k++)
			{
				int n1 = k * nbx * nby + j * nbx + i;
				int n2 = k * nbx * nby + j * nbx + i + 1;
				int n3 = k * nbx * nby + (j + 1) * nbx + i + 1;
				int n4 = k * nbx * nby + (j + 1) * nbx + i;
				int n5 = (k + 1) * nbx * nby + j * nbx + i;
				int n6 = (k + 1) * nbx * nby + j * nbx + i + 1;
				int n7 = (k + 1) * nbx * nby + (j + 1) * nbx + i + 1;
				int n8 = (k + 1) * nbx * nby + (j + 1) * nbx + i;

//    MESSAGE(" "<<n1<<" "<<n2<<" "<<n3<<" "<<n4<<" "<<n5<<" "<<n6<<" "<<n7<<" "<<n8);
				//MESSAGE(" "<<np[n1].nodeId<<" "<<np[n2].nodeId<<" "<<np[n3].nodeId<<" "<<np[n4].nodeId<<" "<<np[n5].nodeId<<" "<<np[n6].nodeId<<" "<<np[n7].nodeId<<" "<<np[n8].nodeId);

				SMDS_MeshVolume * elt = meshDS->AddVolume(np[n1].nodeId,
					np[n2].nodeId,
					np[n3].nodeId,
					np[n4].nodeId,
					np[n5].nodeId,
					np[n6].nodeId,
					np[n7].nodeId,
					np[n8].nodeId);
				 ;
				meshDS->SetMeshElementOnShape(elt, aShell);

				// *** 5 tetrahedres ... verifier orientations,
				//     mettre en coherence &vec quadrangles-> triangles
				//     choisir afficher 1 parmi edges, face et volumes
//    int tetra1 = meshDS->AddVolume(np[n1].nodeId,
//                   np[n2].nodeId,
//                   np[n4].nodeId,
//                   np[n5].nodeId);
//    int tetra2 = meshDS->AddVolume(np[n2].nodeId,
//                   np[n3].nodeId,
//                   np[n4].nodeId,
//                   np[n7].nodeId);
//    int tetra3 = meshDS->AddVolume(np[n5].nodeId,
//                   np[n6].nodeId,
//                   np[n7].nodeId,
//                   np[n2].nodeId);
//    int tetra4 = meshDS->AddVolume(np[n5].nodeId,
//                   np[n7].nodeId,
//                   np[n8].nodeId,
//                   np[n4].nodeId);
//    int tetra5 = meshDS->AddVolume(np[n5].nodeId,
//                   np[n7].nodeId,
//                   np[n2].nodeId,
//                   np[n4].nodeId);

			}

	MESSAGE("End of SMESH_Hexa_3D::Compute()");
	return true;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

void SMESH_Hexa_3D::GetPoint(Pt3 p, int i, int j, int k, int nbx, int nby,
	int nbz, Point3DStruct * np, const SMESHDS_Mesh * meshDS)
{
	int ijk = k * nbx * nby + j * nbx + i;
	const SMDS_MeshNode * node = meshDS->FindNode(np[ijk].nodeId);
	p[0] = node->X();
	p[1] = node->Y();
	p[2] = node->Z();
	//MESSAGE(" "<<i<<" "<<j<<" "<<k<<" "<<p[0]<<" "<<p[1]<<" "<<p[2]);
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

int SMESH_Hexa_3D::GetFaceIndex(SMESH_Mesh & aMesh,
	const TopoDS_Shape & aShape,
	const vector < SMESH_subMesh * >&meshFaces,
	const TopoDS_Vertex & V0,
	const TopoDS_Vertex & V1,
	const TopoDS_Vertex & V2, const TopoDS_Vertex & V3)
{
	MESSAGE("SMESH_Hexa_3D::GetFaceIndex");
	int faceIndex = -1;
	for (int i = 1; i < 6; i++)
	{
		const TopoDS_Shape & aFace = meshFaces[i]->GetSubShape();
		//const TopoDS_Face& F = TopoDS::Face(aFace);
		TopTools_IndexedMapOfShape M;
		TopExp::MapShapes(aFace, TopAbs_VERTEX, M);
		bool verticesInShape = false;
		if (M.Contains(V0))
			if (M.Contains(V1))
				if (M.Contains(V2))
					if (M.Contains(V3))
						verticesInShape = true;
		if (verticesInShape)
		{
			faceIndex = i;
			break;
		}
	}
	ASSERT(faceIndex > 0);
	SCRUTE(faceIndex);
	return faceIndex;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

TopoDS_Edge
	SMESH_Hexa_3D::EdgeNotInFace(SMESH_Mesh & aMesh,
	const TopoDS_Shape & aShape,
	const TopoDS_Face & aFace,
	const TopoDS_Vertex & aVertex,
	const TopTools_IndexedDataMapOfShapeListOfShape & MS)
{
	MESSAGE("SMESH_Hexa_3D::EdgeNotInFace");
	TopTools_IndexedDataMapOfShapeListOfShape MF;
	TopExp::MapShapesAndAncestors(aFace, TopAbs_VERTEX, TopAbs_EDGE, MF);
	const TopTools_ListOfShape & ancestorsInSolid = MS.FindFromKey(aVertex);
	const TopTools_ListOfShape & ancestorsInFace = MF.FindFromKey(aVertex);
	SCRUTE(ancestorsInSolid.Extent());
	SCRUTE(ancestorsInFace.Extent());
	ASSERT(ancestorsInSolid.Extent() == 6);	// 6 (edges doublees)
	ASSERT(ancestorsInFace.Extent() == 2);

	TopoDS_Edge E;
	E.Nullify();
	TopTools_ListIteratorOfListOfShape its(ancestorsInSolid);
	for (; its.More(); its.Next())
	{
		TopoDS_Shape ancestor = its.Value();
		TopTools_ListIteratorOfListOfShape itf(ancestorsInFace);
		bool isInFace = false;
		for (; itf.More(); itf.Next())
		{
			TopoDS_Shape ancestorInFace = itf.Value();
			if (ancestorInFace.IsSame(ancestor))
			{
				isInFace = true;
				break;
			}
		}
		if (!isInFace)
		{
			E = TopoDS::Edge(ancestor);
			break;
		}
	}
	return E;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

void SMESH_Hexa_3D::GetConv2DCoefs(const faceQuadStruct & quad,
	const TopoDS_Shape & aShape,
	const TopoDS_Vertex & V0,
	const TopoDS_Vertex & V1,
	const TopoDS_Vertex & V2, const TopoDS_Vertex & V3, Conv2DStruct & conv)
{
	MESSAGE("SMESH_Hexa_3D::GetConv2DCoefs");
	const TopoDS_Face & F = TopoDS::Face(aShape);
	TopoDS_Edge E = quad.edge[0];
	double f, l;
	Handle(Geom2d_Curve) C2d = BRep_Tool::CurveOnSurface(E, F, f, l);
	TopoDS_Vertex VFirst, VLast;
	TopExp::Vertices(E, VFirst, VLast);	// corresponds to f and l
	bool isForward = (((l - f) * (quad.last[0] - quad.first[0])) > 0);
	TopoDS_Vertex VA, VB;
	if (isForward)
	{
		VA = VFirst;
		VB = VLast;
	}
	else
	{
		VA = VLast;
		VB = VFirst;
	}
	int a1, b1, c1, a2, b2, c2;
	if (VA.IsSame(V0))
		if (VB.IsSame(V1))
		{
			a1 = 1;
			b1 = 0;
			c1 = 0;				// x
			a2 = 0;
			b2 = 1;
			c2 = 0;				// y
		}
		else
		{
			ASSERT(VB.IsSame(V3));
			a1 = 0;
			b1 = 1;
			c1 = 0;				// y
			a2 = 1;
			b2 = 0;
			c2 = 0;				// x
		}
	if (VA.IsSame(V1))
		if (VB.IsSame(V2))
		{
			a1 = 0;
			b1 = -1;
			c1 = 1;				// 1-y
			a2 = 1;
			b2 = 0;
			c2 = 0;				// x
		}
		else
		{
			ASSERT(VB.IsSame(V0));
			a1 = -1;
			b1 = 0;
			c1 = 1;				// 1-x
			a2 = 0;
			b2 = 1;
			c2 = 0;				// y
		}
	if (VA.IsSame(V2))
		if (VB.IsSame(V3))
		{
			a1 = -1;
			b1 = 0;
			c1 = 1;				// 1-x
			a2 = 0;
			b2 = -1;
			c2 = 1;				// 1-y
		}
		else
		{
			ASSERT(VB.IsSame(V1));
			a1 = 0;
			b1 = -1;
			c1 = 1;				// 1-y
			a2 = -1;
			b2 = 0;
			c2 = 1;				// 1-x
		}
	if (VA.IsSame(V3))
		if (VB.IsSame(V0))
		{
			a1 = 0;
			b1 = 1;
			c1 = 0;				// y
			a2 = -1;
			b2 = 0;
			c2 = 1;				// 1-x
		}
		else
		{
			ASSERT(VB.IsSame(V2));
			a1 = 1;
			b1 = 0;
			c1 = 0;				// x
			a2 = 0;
			b2 = -1;
			c2 = 1;				// 1-y
		}
	MESSAGE("X = " << c1 << "+ " << a1 << "*x + " << b1 << "*y");
	MESSAGE("Y = " << c2 << "+ " << a2 << "*x + " << b2 << "*y");
	conv.a1 = a1;
	conv.b1 = b1;
	conv.c1 = c1;
	conv.a2 = a2;
	conv.b2 = b2;
	conv.c2 = c2;

	int nbdown = quad.nbPts[0];
	int nbright = quad.nbPts[1];
	conv.ia = int (a1);
	conv.ib = int (b1);
	conv.ic =
		int (c1 * a1 * a1) * (nbdown - 1) + int (c1 * b1 * b1) * (nbright - 1);
	conv.ja = int (a2);
	conv.jb = int (b2);
	conv.jc =
		int (c2 * a2 * a2) * (nbdown - 1) + int (c2 * b2 * b2) * (nbright - 1);
	MESSAGE("I " << conv.ia << " " << conv.ib << " " << conv.ic);
	MESSAGE("J " << conv.ja << " " << conv.jb << " " << conv.jc);
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

ostream & SMESH_Hexa_3D::SaveTo(ostream & save)
{
	return save << this;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

istream & SMESH_Hexa_3D::LoadFrom(istream & load)
{
	return load >> (*this);
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

ostream & operator <<(ostream & save, SMESH_Hexa_3D & hyp)
{
	return save;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

istream & operator >>(istream & load, SMESH_Hexa_3D & hyp)
{
	return load;
}
