#include "stdafx.h"

void CRenderTarget::phase_smap_spot_clear()
{
    HW.ClearDepth(rt_smap_depth->pZRT, 1.0f);
}

void CRenderTarget::phase_smap_spot(light* L)
{
    // Targets + viewport
    //	TODO: DX10: CHeck if we don't need old-style SMAP
    if (RImplementation.o.HW_smap)
        u_setrt(rt_smap_surf, NULL, NULL, rt_smap_depth->pZRT);
    // else								u_setrt	(rt_smap_surf, NULL, NULL, rt_smap_ZB);
    else
        VERIFY(!"Use HW SMap only for DX10!");
    const D3D_VIEWPORT viewport = { L->X.S.posX, L->X.S.posY, L->X.S.size, L->X.S.size, 0.f, 1.f };
    RCache.SetViewport(viewport);

    // Misc		- draw only front-faces //back-faces
    RCache.set_CullMode(CULL_CCW);
    RCache.set_Stencil(FALSE);

    // no transparency
#pragma todo("can optimize for multi-lights covering more than say 50%...")
    if (RImplementation.o.HW_smap)
        RCache.set_ColorWriteEnable(FALSE);

    //	Do it once per smap generation pass in phase_smap_spot_clear
    // HW.ClearDepth(rt_smap_depth->pZRT, 1.0f);
}

void CRenderTarget::phase_smap_spot_tsh(light* L)
{
    VERIFY(!"Implement clear of the buffer for tsh!");
    VERIFY(RImplementation.o.Tshadows);
    RCache.set_ColorWriteEnable();
    if (IRender_Light::OMNIPART == L->flags.type)
    {
        // omni-part
        HW.ClearRenderTarget(RCache.get_RT(), { 1.0f, 1.0f, 1.0f, 1.0f });
    }
    else
    {
        // real-spot
        // Select color-mask
        ref_shader shader = L->s_spot;
        if (!shader)
            shader = s_accum_spot;
        RCache.set_Element(shader->E[SE_L_FILL]);

        // Fill vertex buffer
        Fvector2 p0, p1;
        u32 Offset;
        u32 C = color_rgba(255, 255, 255, 255);
        float _w = float(L->X.S.size);
        float _h = float(L->X.S.size);
        float d_Z = EPS_S;
        float d_W = 1.f;
        p0.set(.5f / _w, .5f / _h);
        p1.set((_w + .5f) / _w, (_h + .5f) / _h);

        FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
        pv->set(EPS, float(_h + EPS), d_Z, d_W, C, p0.x, p1.y);
        pv++;
        pv->set(EPS, EPS, d_Z, d_W, C, p0.x, p0.y);
        pv++;
        pv->set(float(_w + EPS), float(_h + EPS), d_Z, d_W, C, p1.x, p1.y);
        pv++;
        pv->set(float(_w + EPS), EPS, d_Z, d_W, C, p1.x, p0.y);
        pv++;
        RCache.Vertex.Unlock(4, g_combine->vb_stride);
        RCache.set_Geometry(g_combine);

        // draw
        RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }
}
