/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifndef MODEL_SHARED_H
#define MODEL_SHARED_H

typedef enum synctype_e {ST_SYNC=0, ST_RAND } synctype_t;

/*

d*_t structures are on-disk representations
m*_t structures are in-memory

*/

typedef enum modtype_e {mod_invalid, mod_brushq1, mod_sprite, mod_alias, mod_brushq2, mod_brushq3} modtype_t;

typedef struct animscene_s
{
	char name[32]; // for viewthing support
	int firstframe;
	int framecount;
	int loop; // true or false
	float framerate;
}
animscene_t;

typedef struct skinframe_s
{
	rtexture_t *stain; // inverse modulate with background (used for decals and such)
	rtexture_t *merged; // original texture without glow
	rtexture_t *base; // original texture without pants/shirt/glow
	rtexture_t *pants; // pants only (in greyscale)
	rtexture_t *shirt; // shirt only (in greyscale)
	rtexture_t *nmap; // normalmap (bumpmap for dot3)
	rtexture_t *gloss; // glossmap (for dot3)
	rtexture_t *glow; // glow only (fullbrights)
	rtexture_t *fog; // alpha of the base texture (if not opaque)
}
skinframe_t;

#define MAX_SKINS 256

typedef struct overridetagname_s
{
	char name[MAX_QPATH];
}
overridetagname_t;

// a replacement set of tag names, per skin
typedef struct overridetagnameset_s
{
	int num_overridetagnames;
	overridetagname_t *data_overridetagnames;
}
overridetagnameset_t;

struct md3vertex_s;
struct trivertex_s;

// used for mesh lists in q1bsp/q3bsp map models
// (the surfaces reference portions of these meshes)
typedef struct surfmesh_s
{
	int num_triangles; // number of triangles in the mesh
	int *data_element3i; // int[tris*3] triangles of the mesh, 3 indices into vertex arrays for each
	int *data_neighbor3i; // int[tris*3] neighboring triangle on each edge (-1 if none)
	int num_vertices; // number of vertices in the mesh
	float *data_vertex3f; // float[verts*3] vertex locations
	float *data_svector3f; // float[verts*3] direction of 'S' (right) texture axis for each vertex
	float *data_tvector3f; // float[verts*3] direction of 'T' (down) texture axis for each vertex
	float *data_normal3f; // float[verts*3] direction of 'R' (out) texture axis for each vertex
	float *data_texcoordtexture2f; // float[verts*2] texcoords for surface texture
	float *data_texcoordlightmap2f; // float[verts*2] texcoords for lightmap texture
	float *data_lightmapcolor4f;
	int *data_lightmapoffsets; // index into surface's lightmap samples for vertex lighting
	// morph blending, these are zero if model is skeletal or static
	int num_morphframes;
	struct md3vertex_s *data_morphmd3vertex;
	struct trivertx_s *data_morphmdlvertex;
	float *data_morphmd2framesize6f;
	float num_morphmdlframescale[3];
	float num_morphmdlframetranslate[3];
	// skeletal blending, these are NULL if model is morph or static
	int *data_vertexweightindex4i;
	float *data_vertexweightinfluence4f;
	// set if there is some kind of animation on this model
	qboolean isanimated;
}
surfmesh_t;

#define SHADOWMESHVERTEXHASH 1024
typedef struct shadowmeshvertexhash_s
{
	struct shadowmeshvertexhash_s *next;
}
shadowmeshvertexhash_t;

typedef struct shadowmesh_s
{
	// next mesh in chain
	struct shadowmesh_s *next;
	// used for light mesh (NULL on shadow mesh)
	rtexture_t *map_diffuse;
	rtexture_t *map_specular;
	rtexture_t *map_normal;
	// buffer sizes
	int numverts, maxverts;
	int numtriangles, maxtriangles;
	// used always
	float *vertex3f;
	// used for light mesh (NULL on shadow mesh)
	float *svector3f;
	float *tvector3f;
	float *normal3f;
	float *texcoord2f;
	// used always
	int *element3i;
	// used for shadow mesh (NULL on light mesh)
	int *neighbor3i;
	// these are NULL after Mod_ShadowMesh_Finish is performed, only used
	// while building meshes
	shadowmeshvertexhash_t **vertexhashtable, *vertexhashentries;
}
shadowmesh_t;

typedef enum texturelayertype_e
{
	TEXTURELAYERTYPE_INVALID,
	TEXTURELAYERTYPE_LITTEXTURE,
	TEXTURELAYERTYPE_TEXTURE,
	TEXTURELAYERTYPE_FOG,
}
texturelayertype_t;

typedef enum texturelayerflag_e
{
	// indicates that the pass should apply fog darkening; used on
	// transparent surfaces where simply blending an alpha fog as a final
	// pass would not behave properly, so all the surfaces must be darkened,
	// and the fog color added as a separate pass
	TEXTURELAYERFLAG_FOGDARKEN = 1,
}
texturelayerflag_t;

typedef struct texturelayer_s
{
	texturelayertype_t type;
	qboolean depthmask;
	int blendfunc1;
	int blendfunc2;
	rtexture_t *texture;
	matrix4x4_t texmatrix;
	vec4_t color;
	int flags;
}
texturelayer_t;

typedef struct texture_s
{
	// q1bsp
	// name
	//char name[16];
	// size
	unsigned int width, height;
	// SURF_ flags
	//unsigned int flags;

	// base material flags
	int basematerialflags;
	// current material flags (updated each bmodel render)
	int currentmaterialflags;

	// loaded the same as model skins
	skinframe_t skin;

	// total frames in sequence and alternate sequence
	int anim_total[2];
	// direct pointers to each of the frames in the sequences
	// (indexed as [alternate][frame])
	struct texture_s *anim_frames[2][10];
	// set if animated or there is an alternate frame set
	// (this is an optimization in the renderer)
	int animated;

	// the current alpha of this texture (may be affected by r_wateralpha)
	float currentalpha;
	// the current texture frame in animation
	struct texture_s *currentframe;
	// current texture transform matrix (used for water scrolling)
	matrix4x4_t currenttexmatrix;

	qboolean colormapping;
	rtexture_t *basetexture;
	rtexture_t *glosstexture;
	float specularscale;
	float specularpower;

	// from q3 shaders
	int customblendfunc[2];

	int currentnumlayers;
	texturelayer_t currentlayers[16];

	// q3bsp
	char name[64];
	int surfaceflags;
	int supercontents;
	int surfaceparms;
	int textureflags;

	//skinframe_t skin;
}
texture_t;

typedef struct mtexinfo_s
{
	float vecs[2][4];
	texture_t *texture;
	int flags;
}
mtexinfo_t;

typedef struct msurface_lightmapinfo_s
{
	// texture mapping properties used by this surface
	mtexinfo_t *texinfo; // q1bsp
	// index into r_refdef.lightstylevalue array, 255 means not used (black)
	unsigned char styles[MAXLIGHTMAPS]; // q1bsp
	// RGB lighting data [numstyles][height][width][3]
	unsigned char *samples; // q1bsp
	// RGB normalmap data [numstyles][height][width][3]
	unsigned char *nmapsamples; // q1bsp
	// stain to apply on lightmap (soot/dirt/blood/whatever)
	unsigned char *stainsamples; // q1bsp
	int texturemins[2]; // q1bsp
	int extents[2]; // q1bsp
	int lightmaporigin[2]; // q1bsp
}
msurface_lightmapinfo_t;

struct q3deffect_s;
typedef struct msurface_s
{
	// bounding box for onscreen checks
	vec3_t mins;
	vec3_t maxs;
	// the texture to use on the surface
	texture_t *texture;
	// the lightmap texture fragment to use on the rendering mesh
	rtexture_t *lightmaptexture;
	// the lighting direction texture fragment to use on the rendering mesh
	rtexture_t *deluxemaptexture;

	// surfaces own ranges of vertices and triangles in the model->surfmesh
	int num_triangles; // number of triangles
	int num_firsttriangle; // first triangle
	int num_vertices; // number of vertices
	int num_firstvertex; // first vertex

	// shadow volume building information
	int num_firstshadowmeshtriangle; // index into model->brush.shadowmesh

	// lightmaptexture rebuild information not used in q3bsp
	int cached_dlight; // q1bsp // forces rebuild of lightmaptexture
	msurface_lightmapinfo_t *lightmapinfo; // q1bsp

	// mesh information for collisions (only used by q3bsp curves)
	int num_collisiontriangles; // q3bsp
	int *data_collisionelement3i; // q3bsp
	int num_collisionvertices; // q3bsp
	float *data_collisionvertex3f; // q3bsp
	struct q3deffect_s *effect; // q3bsp
	// FIXME: collisionmarkframe should be kept in a separate array
	int collisionmarkframe; // q3bsp // don't collide twice in one trace
}
msurface_t;

#include "matrixlib.h"

#include "model_brush.h"
#include "model_sprite.h"
#include "model_alias.h"

typedef struct model_sprite_s
{
	int				sprnum_type;
	mspriteframe_t	*sprdata_frames;
}
model_sprite_t;

struct trace_s;

typedef struct model_brush_s
{
	// true if this model is a HalfLife .bsp file
	qboolean ishlbsp;
	// true if this model is a Martial Concert .bsp file
	qboolean ismcbsp;
	// string of entity definitions (.map format)
	char *entities;

	// if non-zero this is a submodel
	// (this is the number of the submodel, an index into submodels)
	int submodel;

	// number of submodels in this map (just used by server to know how many
	// submodels to load)
	int numsubmodels;
	// pointers to each of the submodels if .isworldmodel is true
	struct model_s **submodels;

	int num_planes;
	mplane_t *data_planes;

	int num_nodes;
	mnode_t *data_nodes;

	// visible leafs, not counting 0 (solid)
	int num_visleafs;
	// number of actual leafs (including 0 which is solid)
	int num_leafs;
	mleaf_t *data_leafs;

	int num_leafbrushes;
	int *data_leafbrushes;

	int num_leafsurfaces;
	int *data_leafsurfaces;

	int num_portals;
	mportal_t *data_portals;

	int num_portalpoints;
	mvertex_t *data_portalpoints;

	int num_brushes;
	q3mbrush_t *data_brushes;

	int num_brushsides;
	q3mbrushside_t *data_brushsides;

	// pvs
	int num_pvsclusters;
	int num_pvsclusterbytes;
	unsigned char *data_pvsclusters;
	// example
	//pvschain = model->brush.data_pvsclusters + mycluster * model->brush.num_pvsclusterbytes;
	//if (pvschain[thatcluster >> 3] & (1 << (thatcluster & 7)))

	// a mesh containing all shadow casting geometry for the whole model (including submodels), portions of this are referenced by each surface's num_firstshadowmeshtriangle
	shadowmesh_t *shadowmesh;

	// common functions
	int (*SuperContentsFromNativeContents)(struct model_s *model, int nativecontents);
	int (*NativeContentsFromSuperContents)(struct model_s *model, int supercontents);
	unsigned char *(*GetPVS)(struct model_s *model, const vec3_t p);
	int (*FatPVS)(struct model_s *model, const vec3_t org, vec_t radius, unsigned char *pvsbuffer, int pvsbufferlength);
	int (*BoxTouchingPVS)(struct model_s *model, const unsigned char *pvs, const vec3_t mins, const vec3_t maxs);
	int (*BoxTouchingLeafPVS)(struct model_s *model, const unsigned char *pvs, const vec3_t mins, const vec3_t maxs);
	int (*BoxTouchingVisibleLeafs)(struct model_s *model, const unsigned char *visibleleafs, const vec3_t mins, const vec3_t maxs);
	int (*FindBoxClusters)(struct model_s *model, const vec3_t mins, const vec3_t maxs, int maxclusters, int *clusterlist);
	void (*LightPoint)(struct model_s *model, const vec3_t p, vec3_t ambientcolor, vec3_t diffusecolor, vec3_t diffusenormal);
	void (*FindNonSolidLocation)(struct model_s *model, const vec3_t in, vec3_t out, vec_t radius);
	mleaf_t *(*PointInLeaf)(struct model_s *model, const float *p);
	// these are actually only found on brushq1, but NULL is handled gracefully
	void (*AmbientSoundLevelsForPoint)(struct model_s *model, const vec3_t p, unsigned char *out, int outsize);
	void (*RoundUpToHullSize)(struct model_s *cmodel, const vec3_t inmins, const vec3_t inmaxs, vec3_t outmins, vec3_t outmaxs);

	char skybox[MAX_QPATH];

	rtexture_t *solidskytexture;
	rtexture_t *alphaskytexture;

	// QuakeWorld
	int qw_md4sum;
	int qw_md4sum2;
}
model_brush_t;

typedef struct model_brushq1_s
{
	// lightmap format, set to r_lightmaprgba when model is loaded
	int				lightmaprgba;

	dmodel_t		*submodels;

	int				numvertexes;
	mvertex_t		*vertexes;

	int				numedges;
	medge_t			*edges;

	int				numtexinfo;
	mtexinfo_t		*texinfo;

	int				numsurfedges;
	int				*surfedges;

	int				numclipnodes;
	dclipnode_t		*clipnodes;

	hull_t			hulls[MAX_MAP_HULLS];

	int				num_compressedpvs;
	unsigned char			*data_compressedpvs;

	int				num_lightdata;
	unsigned char			*lightdata;
	unsigned char			*nmaplightdata; // deluxemap file

	// lightmap update chains for light styles
	int				light_styles;
	unsigned char			*light_style;
	int				*light_stylevalue;
	msurface_t		***light_styleupdatechains;
	msurface_t		**light_styleupdatechainsbuffer;
}
model_brushq1_t;

/* MSVC can't compile empty structs, so this is commented out for now
typedef struct model_brushq2_s
{
}
model_brushq2_t;
*/

typedef struct model_brushq3_s
{
	int num_models;
	q3dmodel_t *data_models;

	// used only during loading - freed after loading!
	int num_vertices;
	float *data_vertex3f;
	float *data_normal3f;
	float *data_texcoordtexture2f;
	float *data_texcoordlightmap2f;
	float *data_color4f;

	// freed after loading!
	int num_triangles;
	int *data_element3i;

	int num_effects;
	q3deffect_t *data_effects;

	// lightmap textures
	int num_lightmaps;
	rtexture_t **data_lightmaps;

	// voxel light data with directional shading
	int num_lightgrid;
	q3dlightgrid_t *data_lightgrid;
	// size of each cell (may vary by map, typically 64 64 128)
	float num_lightgrid_cellsize[3];
	// 1.0 / num_lightgrid_cellsize
	float num_lightgrid_scale[3];
	// dimensions of the world model in lightgrid cells
	int num_lightgrid_imins[3];
	int num_lightgrid_imaxs[3];
	int num_lightgrid_isize[3];
	// indexing/clamping
	int num_lightgrid_dimensions[3];
	// transform modelspace coordinates to lightgrid index
	matrix4x4_t num_lightgrid_indexfromworld;

	// true if this q3bsp file has been detected as using deluxemapping
	// (lightmap texture pairs, every odd one is never directly refernced,
	//  and contains lighting normals, not colors)
	qboolean deluxemapping;
	// true if the detected deluxemaps are the modelspace kind, rather than
	// the faster tangentspace kind
	qboolean deluxemapping_modelspace;
}
model_brushq3_t;

typedef struct model_s
{
	// name and path of model, for example "progs/player.mdl"
	char			name[MAX_QPATH];
	// model needs to be loaded if this is false
	qboolean		loaded;
	// set if the model is used in current map, models which are not, are purged
	qboolean		used;
	// true if this is the world model (I.E. defines what sky to use, and may contain submodels)
	qboolean		isworldmodel;
	// CRC of the file this model was loaded from, to reload if changed
	unsigned int	crc;
	// mod_brush, mod_alias, mod_sprite
	modtype_t		type;
	// memory pool for allocations
	mempool_t		*mempool;
	// all models use textures...
	rtexturepool_t	*texturepool;
	// flags from the model file
	int				flags;
	// engine calculated flags, ones that can not be set in the file
	int				flags2;
	// number of QC accessible frame(group)s in the model
	int				numframes;
	// number of QC accessible skin(group)s in the model
	int				numskins;
	// whether to randomize animated framegroups
	synctype_t		synctype;
	// bounding box at angles '0 0 0'
	vec3_t			normalmins, normalmaxs;
	// bounding box if yaw angle is not 0, but pitch and roll are
	vec3_t			yawmins, yawmaxs;
	// bounding box if pitch or roll are used
	vec3_t			rotatedmins, rotatedmaxs;
	// sphere radius, usable at any angles
	float			radius;
	// squared sphere radius for easier comparisons
	float			radius2;
	// skin animation info
	animscene_t		*skinscenes; // [numskins]
	// skin animation info
	animscene_t		*animscenes; // [numframes]
	// range of surface numbers in this (sub)model
	int				firstmodelsurface;
	int				nummodelsurfaces;
	// range of collision brush numbers in this (sub)model
	int				firstmodelbrush;
	int				nummodelbrushes;
	// list of surface numbers in this (sub)model
	int				*surfacelist;
	// for md3 models
	int				num_tags;
	int				num_tagframes;
	aliastag_t		*data_tags;
	// for skeletal models
	int				num_bones;
	aliasbone_t		*data_bones;
	int				num_poses;
	float			*data_poses;
	float			*data_basebonepose;
	float			*data_baseboneposeinverse;
	// textures of this model
	int				num_textures;
	texture_t		*data_textures;
	// surfaces of this model
	int				num_surfaces;
	msurface_t		*data_surfaces;
	// optional lightmapinfo data for surface lightmap updates
	msurface_lightmapinfo_t *data_surfaces_lightmapinfo;
	// all surfaces belong to this mesh
	surfmesh_t		surfmesh;
	// draw the model's sky polygons (only used by brush models)
	void(*DrawSky)(struct entity_render_s *ent);
	// draw the model using lightmap/dlight shading
	void(*Draw)(struct entity_render_s *ent);
	// gathers info on which clusters and surfaces are lit by light, as well as calculating a bounding box
	void(*GetLightInfo)(struct entity_render_s *ent, vec3_t relativelightorigin, float lightradius, vec3_t outmins, vec3_t outmaxs, int *outleaflist, unsigned char *outleafpvs, int *outnumleafspointer, int *outsurfacelist, unsigned char *outsurfacepvs, int *outnumsurfacespointer);
	// compile a shadow volume for the model based on light source
	void(*CompileShadowVolume)(struct entity_render_s *ent, vec3_t relativelightorigin, float lightradius, int numsurfaces, const int *surfacelist);
	// draw a shadow volume for the model based on light source
	void(*DrawShadowVolume)(struct entity_render_s *ent, vec3_t relativelightorigin, float lightradius, int numsurfaces, const int *surfacelist, const vec3_t lightmins, const vec3_t lightmaxs);
	// draw the lighting on a model (through stencil)
	void(*DrawLight)(struct entity_render_s *ent, int numsurfaces, const int *surfacelist);
	// trace a box against this model
	void (*TraceBox)(struct model_s *model, int frame, struct trace_s *trace, const vec3_t start, const vec3_t boxmins, const vec3_t boxmaxs, const vec3_t end, int hitsupercontentsmask);
	// fields belonging to some types of model
	model_sprite_t	sprite;
	model_brush_t	brush;
	model_brushq1_t	brushq1;
	/* MSVC can't handle an empty struct, so this is commented out for now
	model_brushq2_t	brushq2;
	*/
	model_brushq3_t	brushq3;
	// skin files can have different tags for each skin
	overridetagnameset_t	*data_overridetagnamesforskin;
	// flags this model for offseting sounds to the model center (used by brush models)
	int soundfromcenter;
}
model_t;

//============================================================================

// model loading
extern model_t *loadmodel;
extern unsigned char *mod_base;
// sky/water subdivision
//extern cvar_t gl_subdivide_size;
// texture fullbrights
extern cvar_t r_fullbrights;

void Mod_Init (void);
model_t *Mod_LoadModel(model_t *mod, qboolean crash, qboolean checkdisk, qboolean isworldmodel);
model_t *Mod_FindName (const char *name);
model_t *Mod_ForName (const char *name, qboolean crash, qboolean checkdisk, qboolean isworldmodel);
void Mod_UnloadModel (model_t *mod);

void Mod_ClearUsed(void);
void Mod_PurgeUnused(void);
void Mod_RemoveStaleWorldModels(model_t *skip); // only used during loading!

extern model_t *loadmodel;
extern char loadname[32];	// for hunk tags

int Mod_BuildVertexRemapTableFromElements(int numelements, const int *elements, int numvertices, int *remapvertices);
void Mod_BuildTriangleNeighbors(int *neighbors, const int *elements, int numtriangles);
void Mod_ValidateElements(int *elements, int numtriangles, int firstvertex, int numverts, const char *filename, int fileline);
void Mod_BuildNormals(int firstvertex, int numvertices, int numtriangles, const float *vertex3f, const int *elements, float *normal3f, qboolean areaweighting);
void Mod_BuildTextureVectorsFromNormals(int firstvertex, int numvertices, int numtriangles, const float *vertex3f, const float *texcoord2f, const float *normal3f, const int *elements, float *svector3f, float *tvector3f, qboolean areaweighting);

void Mod_AllocSurfMesh(mempool_t *mempool, int numvertices, int numtriangles, qboolean lightmapoffsets, qboolean vertexcolors, qboolean neighbors);

shadowmesh_t *Mod_ShadowMesh_Alloc(mempool_t *mempool, int maxverts, int maxtriangles, rtexture_t *map_diffuse, rtexture_t *map_specular, rtexture_t *map_normal, int light, int neighbors, int expandable);
shadowmesh_t *Mod_ShadowMesh_ReAlloc(mempool_t *mempool, shadowmesh_t *oldmesh, int light, int neighbors);
int Mod_ShadowMesh_AddVertex(shadowmesh_t *mesh, float *vertex14f);
void Mod_ShadowMesh_AddTriangle(mempool_t *mempool, shadowmesh_t *mesh, rtexture_t *map_diffuse, rtexture_t *map_specular, rtexture_t *map_normal, float *vertex14f);
void Mod_ShadowMesh_AddMesh(mempool_t *mempool, shadowmesh_t *mesh, rtexture_t *map_diffuse, rtexture_t *map_specular, rtexture_t *map_normal, const float *vertex3f, const float *svector3f, const float *tvector3f, const float *normal3f, const float *texcoord2f, int numtris, const int *element3i);
shadowmesh_t *Mod_ShadowMesh_Begin(mempool_t *mempool, int maxverts, int maxtriangles, rtexture_t *map_diffuse, rtexture_t *map_specular, rtexture_t *map_normal, int light, int neighbors, int expandable);
shadowmesh_t *Mod_ShadowMesh_Finish(mempool_t *mempool, shadowmesh_t *firstmesh, int light, int neighbors);
void Mod_ShadowMesh_CalcBBox(shadowmesh_t *firstmesh, vec3_t mins, vec3_t maxs, vec3_t center, float *radius);
void Mod_ShadowMesh_Free(shadowmesh_t *mesh);

int Mod_LoadSkinFrame(skinframe_t *skinframe, const char *basename, int textureflags, int loadpantsandshirt, int loadglowtexture);
int Mod_LoadSkinFrame_Internal(skinframe_t *skinframe, const char *basename, int textureflags, int loadpantsandshirt, int loadglowtexture, const unsigned char *skindata, int width, int height, int bitsperpixel, const unsigned int *palette, const unsigned int *alphapalette);

extern cvar_t r_mipskins;

typedef struct skinfileitem_s
{
	struct skinfileitem_s *next;
	char name[MAX_QPATH];
	char replacement[MAX_QPATH];
}
skinfileitem_t;

typedef struct skinfile_s
{
	struct skinfile_s *next;
	skinfileitem_t *items;
}
skinfile_t;

skinfile_t *Mod_LoadSkinFiles(void);
void Mod_FreeSkinFiles(skinfile_t *skinfile);
int Mod_CountSkinFiles(skinfile_t *skinfile);

void Mod_SnapVertices(int numcomponents, int numvertices, float *vertices, float snap);
int Mod_RemoveDegenerateTriangles(int numtriangles, const int *inelement3i, int *outelement3i, const float *vertex3f);

// bsp models
void Mod_BrushInit(void);
// used for talking to the QuakeC mainly
int Mod_Q1BSP_NativeContentsFromSuperContents(struct model_s *model, int supercontents);
int Mod_Q1BSP_SuperContentsFromNativeContents(struct model_s *model, int nativecontents);

// a lot of model formats use the Q1BSP code, so here are the prototypes...
struct entity_render_s;
void R_Q1BSP_DrawSky(struct entity_render_s *ent);
void R_Q1BSP_Draw(struct entity_render_s *ent);
void R_Q1BSP_GetLightInfo(struct entity_render_s *ent, vec3_t relativelightorigin, float lightradius, vec3_t outmins, vec3_t outmaxs, int *outleaflist, unsigned char *outleafpvs, int *outnumleafspointer, int *outsurfacelist, unsigned char *outsurfacepvs, int *outnumsurfacespointer);
void R_Q1BSP_CompileShadowVolume(struct entity_render_s *ent, vec3_t relativelightorigin, float lightradius, int numsurfaces, const int *surfacelist);
void R_Q1BSP_DrawShadowVolume(struct entity_render_s *ent, vec3_t relativelightorigin, float lightradius, int numsurfaces, const int *surfacelist, const vec3_t lightmins, const vec3_t lightmaxs);
void R_Q1BSP_DrawLight(struct entity_render_s *ent, int numsurfaces, const int *surfacelist);

// alias models
struct frameblend_s;
void Mod_AliasInit(void);
void Mod_Alias_GetMesh_Vertices(const model_t *model, const struct frameblend_s *frameblend, float *vertex3f, float *normal3f, float *svector3f, float *tvector3f);
int Mod_Alias_GetTagMatrix(const model_t *model, int poseframe, int tagindex, matrix4x4_t *outmatrix);
int Mod_Alias_GetTagIndexForName(const model_t *model, unsigned int skin, const char *tagname);

// sprite models
void Mod_SpriteInit(void);

// loaders
void Mod_Q1BSP_Load(model_t *mod, void *buffer, void *bufferend);
void Mod_IBSP_Load(model_t *mod, void *buffer, void *bufferend);
void Mod_MAP_Load(model_t *mod, void *buffer, void *bufferend);
void Mod_IDP0_Load(model_t *mod, void *buffer, void *bufferend);
void Mod_IDP2_Load(model_t *mod, void *buffer, void *bufferend);
void Mod_IDP3_Load(model_t *mod, void *buffer, void *bufferend);
void Mod_ZYMOTICMODEL_Load(model_t *mod, void *buffer, void *bufferend);
void Mod_DARKPLACESMODEL_Load(model_t *mod, void *buffer, void *bufferend);
void Mod_PSKMODEL_Load(model_t *mod, void *buffer, void *bufferend);
void Mod_IDSP_Load(model_t *mod, void *buffer, void *bufferend);
void Mod_IDS2_Load(model_t *mod, void *buffer, void *bufferend);

#endif	// MODEL_SHARED_H

