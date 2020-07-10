#include "stdafx.h"
#include "Layers/xrRender/du_cone.h"
#include "Layers/xrRender/BufferUtils.h"

/*
Fvector du_cone_vertices[DU_CONE_NUMVERTEX]=
{
    0.0000f,    0.0000f, 0.0000f,
    0.5000f,   0.0000f,	 1.0000f,
    0.4619f,   0.1913f,	 1.0000f,
    0.3536f,   0.3536f,	 1.0000f,
    0.1913f,   0.4619f,	 1.0000f,
    -0.0000f,  0.5000f,	 1.0000f,
    -0.1913f,  0.4619f,	 1.0000f,
    -0.3536f,  0.3536f,	 1.0000f,
    -0.4619f,  0.1913f,	 1.0000f,
    -0.5000f,  -0.0000f, 1.0000f,
    -0.4619f,  -0.1913f, 1.0000f,
    -0.3536f,  -0.3536f, 1.0000f,
    -0.1913f,  -0.4619f, 1.0000f,
    0.0000f,   -0.5000f, 1.0000f,
    0.1913f,   -0.4619f, 1.0000f,
    0.3536f,   -0.3536f, 1.0000f,
    0.4619f,   -0.1913f, 1.0000f,
    0.0000f,   0.0000f,	 1.0000f+EPS_L
};
u16 du_cone_faces[DU_CONE_NUMFACES*3]=
{
    0,	2,	1,
    0,	3,	2,
    0,	4,	3,
    0,  5,	4,
    0,	6,	5,
    0,  7,  6,
    0,  8,  7,
    0,  9,  8,
    0, 10,  9,
    0, 11, 10,
    0, 12, 11,
    0, 13, 12,
    0, 14, 13,
    0, 15, 14,
    0, 16, 15,
    0,  1, 16,
    17, 1, 2,
    17, 2, 3,
    17, 3, 4,
    17, 4, 5,
    17, 5, 6,
    17, 6, 7,
    17, 7, 8,
    17, 8, 9,
    17, 9, 10,
    17,10, 11,
    17,11, 12,
    17,12, 13,
    17,13, 14,
    17,14, 15,
    17,15, 16,
    17,16, 1
};
*/

void CRenderTarget::accum_spot_geom_create()
{
    // Vertices
    {
        constexpr size_t vCount = DU_CONE_NUMVERTEX;
        constexpr size_t vSize = 3 * 4;

        g_accum_spot_vb.Create(vCount * vSize);
        BYTE* pData = static_cast<BYTE*>(g_accum_spot_vb.Map());
        CopyMemory(pData, du_cone_vertices, vCount * vSize);
        g_accum_spot_vb.Unmap(true); // upload vertex data
    }

    // Indices
    {
        constexpr size_t iCount = DU_CONE_NUMFACES * 3;

        g_accum_spot_ib.Create(iCount * 2);
        BYTE* pData = static_cast<BYTE*>(g_accum_spot_ib.Map());
        CopyMemory(pData, du_cone_faces, iCount * 2);
        g_accum_spot_ib.Unmap(true); // upload index data
    }
}

void CRenderTarget::accum_spot_geom_destroy()
{
#ifdef DEBUG
    _SHOW_REF("g_accum_spot_ib", &g_accum_spot_ib);
#endif
    g_accum_spot_ib.Release();

#ifdef DEBUG
    _SHOW_REF("g_accum_spot_vb", &g_accum_spot_vb);
#endif
    g_accum_spot_vb.Release();
}

struct Slice
{
    Fvector m_Vert[4];
};

void CRenderTarget::accum_volumetric_geom_create()
{
    // Vertices
    {
        //	VOLUMETRIC_SLICES quads
        constexpr size_t vCount = VOLUMETRIC_SLICES * 4;
        constexpr size_t vSize = 3 * 4;
        constexpr float dt = 1.0f / (VOLUMETRIC_SLICES - 1);

        g_accum_volumetric_vb.Create(vCount * vSize);
        Slice* pSlice = static_cast<Slice*>(g_accum_volumetric_vb.Map());
        float t = 0;
        for (int i = 0; i < VOLUMETRIC_SLICES; ++i)
        {
            pSlice[i].m_Vert[0] = Fvector().set(0, 0, t);
            pSlice[i].m_Vert[1] = Fvector().set(0, 1, t);
            pSlice[i].m_Vert[2] = Fvector().set(1, 0, t);
            pSlice[i].m_Vert[3] = Fvector().set(1, 1, t);
            t += dt;
        }
        g_accum_volumetric_vb.Unmap(true); // upload vertex data
    }

    // Indices
    {
        constexpr size_t iCount = VOLUMETRIC_SLICES * 6;

        g_accum_volumetric_ib.Create(iCount * 2);
        u16* pInd = static_cast<u16*>(g_accum_volumetric_ib.Map());
        for (u16 i = 0; i < VOLUMETRIC_SLICES; ++i, pInd += 6)
        {
            const u16 basevert = i * 4;
            pInd[0] = basevert;
            pInd[1] = basevert + 1;
            pInd[2] = basevert + 2;
            pInd[3] = basevert + 2;
            pInd[4] = basevert + 1;
            pInd[5] = basevert + 3;
        }
        g_accum_volumetric_ib.Unmap(true); // upload index data
    }
}

void CRenderTarget::accum_volumetric_geom_destroy()
{
#ifdef DEBUG
    _SHOW_REF("g_accum_volumetric_ib", &g_accum_volumetric_ib);
#endif
    g_accum_volumetric_ib.Release();

#ifdef DEBUG
    _SHOW_REF("g_accum_volumetric_vb", &g_accum_volumetric_vb);
#endif
    g_accum_volumetric_vb.Release();
}
