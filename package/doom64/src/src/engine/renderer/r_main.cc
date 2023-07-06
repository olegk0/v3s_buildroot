// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// Copyright(C) 1993-1997 Id Software, Inc.
// Copyright(C) 2007-2012 Samuel Villarreal
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION: Main rendering code.
//
//-----------------------------------------------------------------------------

#include <math.h>
#include <t_bsp.h>

#include "doomdef.h"
#include "doomstat.h"
#include "i_video.h"
#include "d_devstat.h"
#include "r_local.h"
#include "r_sky.h"
#include "r_clipper.h"
#include "gl_texture.h"
#include "gl_main.h"
#include "m_fixed.h"
#include "tables.h"
#include "i_system.h"
#include "m_random.h"
#include "d_net.h"
#include "p_local.h"
#include "z_zone.h"
#include "con_console.h"
#include "r_drawlist.h"
#include "gl_draw.h"
#include "g_actions.h"

int             skytexture;

fixed_t         viewx=0;
fixed_t         viewy=0;
fixed_t         viewz=0;
float           fviewx=0;
float           fviewy=0;
float           fviewz=0;
angle_t         viewangle=0;
angle_t         viewpitch=0;
fixed_t         quakeviewx = 0;
fixed_t         quakeviewy = 0;
rcolor          flashcolor = 0;
angle_t         viewangleoffset=0;
float           viewoffset=0;
float           viewsin[2];
float           viewcos[2];
player_t        *renderplayer;

// [d64] for the interpolated water flats
fixed_t         scrollfrac;

int             logoAlpha = 0;

int             vertCount = 0;
unsigned int    renderTic = 0;
unsigned int    spriteRenderTic = 0;
unsigned int    glBindCalls = 0;

dboolean        bRenderSky = false;

FloatProperty r_fov("r_fov", "Field of view angle", 74.0f);
BoolProperty r_fillmode("r_fillmode", "", true);
BoolProperty r_uniformtime("r_uniformtime", "", false);
BoolProperty r_fog("r_fog", "", true);
BoolProperty r_wipe("r_wipe", "", true);
BoolProperty r_drawtris("r_drawtris", "", false);
BoolProperty r_drawmobjbox("r_drawmobjbox", "", false);
BoolProperty r_drawblockmap("r_drawblockmap", "", false);
BoolProperty r_drawtrace("r_drawtrace", "", false);
BoolProperty r_rendersprites("r_rendersprites", "", true);
BoolProperty r_drawfill("r_drawfill", "", false);
BoolProperty r_skybox("r_skybox", "", false);

IntProperty r_colorscale("r_colorscale", "", 0, 0,
                         [](const IntProperty&, int, int&)
                         {
                             GL_SetColorScale();
                         });

BoolProperty r_filter("r_filter", "", false, 0,
                      [](const BoolProperty&, bool, bool&)
                      {
                          GL_DumpTextures();
                          GL_SetTextureFilter();
                      });

BoolProperty r_texnonpowresize("r_texnonpowresize", "", false, 0,
                               [](const BoolProperty&, bool, bool&)
                               {
                                   GL_DumpTextures();
                               });

BoolProperty r_anisotropic("r_anisotropic", "", false, 0,
                           [](const BoolProperty&, bool, bool&)
                           {
                               GL_DumpTextures();
                               GL_SetTextureFilter();
                           });

extern BoolProperty r_texturecombiner;
extern BoolProperty i_interpolateframes;
extern BoolProperty p_usecontext;

//
// CMD_Wireframe
//

static CMD(Wireframe) {
    dboolean b;

    if(!param[0]) {
        return;
    }

    b = datoi(param[0]) & 1;
    R_DrawWireframe(b);
}

//
// R_PointToAngle
// To get a global angle from cartesian coordinates,
// the coordinates are flipped until they are in
// the first octant of the coordinate system, then
// the y (<=x) is scaled and divided by x to get a
// tangent (slope) value which is looked up in the
// tantoangle[] table.
//Note not same as software version, which gets angle from (viewx, viewy) rather than (0, 0)

angle_t R_PointToAngle(fixed_t x, fixed_t y) {
    if((!x) && (!y)) {
        return 0;
    }

    if(x >= 0) {
        // x >=0
        if(y>= 0) {
            // y>= 0

            if(x > y) {
                // octant 0
                return tantoangle[SlopeDiv(y, x)];
            }
            else {
                // octant 1
                return ANG90-1-tantoangle[SlopeDiv(x, y)];
            }
        }
        else {
            // y<0
            y = -y;

            if(x > y) {
                // octant 8
                return 0-tantoangle[SlopeDiv(y,x)];
            }
            else {
                // octant 7
                return ANG270+tantoangle[SlopeDiv(x,y)];
            }
        }
    }
    else {
        // x<0
        x = -x;

        if(y >= 0) {
            // y>= 0
            if(x > y) {
                // octant 3
                return ANG180-1-tantoangle[SlopeDiv(y,x)];
            }
            else {
                // octant 2
                return ANG90+ tantoangle[SlopeDiv(x,y)];
            }
        }
        else {
            // y<0
            y = -y;

            if(x > y) {
                // octant 4
                return ANG180+tantoangle[SlopeDiv(y,x)];
            }
            else {
                // octant 5
                return ANG270-1-tantoangle[SlopeDiv(x,y)];
            }
        }
    }
}

//
// R_PointOnSide
// Traverse BSP (sub) tree,
// check point against partition plane.
// Returns side 0 (front) or 1 (back).
//

int R_PointOnSide(fixed_t x, fixed_t y, node_t* node) {
    fixed_t    dx;
    fixed_t    dy;
    fixed_t    left;
    fixed_t    right;

    if(!node->dx) {
        if(x <= node->x) {
            return node->dy > 0;
        }

        return node->dy < 0;
    }
    if(!node->dy) {
        if(y <= node->y) {
            return node->dx < 0;
        }

        return node->dx > 0;
    }

    dx = (x - node->x);
    dy = (y - node->y);

    // Try to quickly decide by looking at sign bits.
    if((node->dy ^ node->dx ^ dx ^ dy)&0x80000000) {
        if((node->dy ^ dx) & 0x80000000) {
            // (left is negative)
            return 1;
        }
        return 0;
    }

    left = FixedMul(F2INT(node->dy), dx);
    right = FixedMul(dy , F2INT(node->dx));

    if(right < left) {
        // front side
        return 0;
    }
    // back side
    return 1;
}

//
// R_PointToAngle2
//

angle_t R_PointToAngle2(fixed_t x1, fixed_t y1, fixed_t x2, fixed_t y2) {
    return R_PointToAngle(x2-x1, y2-y1);
}

//
// R_PointToPitch
//

angle_t R_PointToPitch(fixed_t z1, fixed_t z2, fixed_t dist) {
    return R_PointToAngle2(0, z1, dist, z2);
}

//
// R_Init
//

void R_Init(void) {
    int i = 0;
    int a = 0;
    double an;

    //
    // [d64] build finesine table
    //
    for(i = 0; i < (5 * FINEANGLES / 4); i++) {
        an = a * M_PI / (double)FINEANGLES;
        finesine[i] = (fixed_t)(sin(an) * (double)FRACUNIT);
        a += 2;
    }

    GL_InitTextures();
    GL_ResetTextures();

    G_AddCommand("wireframe", CMD_Wireframe, 0);
}

//
// R_PointInSubsector
//

subsector_t* R_PointInSubsector(fixed_t x, fixed_t y) {
    node_t*    node;
    int        side;
    int        nodenum;

    // single subsector is a special case
    if(!numnodes) {
        return subsectors;
    }

    nodenum = numnodes-1;

    while(!(nodenum & NF_SUBSECTOR)) {
        node = &nodes[nodenum];
        side = R_PointOnSide(x, y, node);
        nodenum = node->children[side];
    }

    return &subsectors[nodenum & ~NF_SUBSECTOR];
}

//
// R_SetViewAngleOffset
//

void R_SetViewAngleOffset(angle_t angle) {
    viewangleoffset = angle;
}

//
// R_SetViewOffset
//

void R_SetViewOffset(int offset) {
    viewoffset=((float)offset)/10.0f;
}

//
// R_SetupLevel
//

void R_SetupLevel(void) {
    R_AllocSubsectorBuffer();
    R_RefreshBrightness();

    DL_Init();

    bRenderSky = true;
}

//
// R_PrecacheLevel
// Loads and binds all world textures before level startup
//

void R_PrecacheLevel(void) {
    char *texturepresent;
    char *spritepresent;
    int    i;
    int j;
    int    p;
    int num;
    mobj_t* mo;

    CON_DPrintf("--------R_PrecacheLevel--------\n");
    GL_DumpTextures();

    texturepresent = (char*)Z_Alloca(numtextures);
    spritepresent = (char*)Z_Alloca(NUMSPRITES);

    for(i = 0; i < numsides; i++) {
        texturepresent[sides[i].toptexture] = 1;
        texturepresent[sides[i].midtexture] = 1;
        texturepresent[sides[i].bottomtexture] = 1;
    }

    for(i = 0; i < numsectors; i++) {
        texturepresent[sectors[i].ceilingpic] = 1;
        texturepresent[sectors[i].floorpic] = 1;

        if(sectors[i].flags & MS_LIQUIDFLOOR) {
            texturepresent[sectors[i].floorpic + 1] = 1;
        }
    }

    num = 0;

    for(i = 0; i < numtextures; i++) {
        if(texturepresent[i]) {
            GL_BindWorldTexture(i, 0, 0);
            num++;

            for(p = 0; p < numanimdef; p++) {
                auto l = wad::find(animdefs[p].name);
                int lump = l->section_index();

                if(lump != i) {
                    continue;
                }

                //
                // TODO - add support for precaching palettes
                //
                if(!animdefs[p].palette) {
                    for(j = 1; j < animdefs[p].frames; j++) {
                        GL_BindWorldTexture(i + j, 0, 0);
                        num++;
                    }
                }
            }
        }
    }

    CON_DPrintf("%i world textures cached\n", num);

    for(mo = mobjhead.next; mo != &mobjhead; mo = mo->next) {
        spritepresent[mo->sprite] = 1;
    }

    num = 0;

    //
    // TODO - add support for precaching palettes
    //
    for(i = 0; i < NUMSPRITES; i++) {
        if(spritepresent[i]) {
            spritedef_t    *sprdef;
            int k;

            sprdef = &spriteinfo[i];

            for(k = 0; k < sprdef->numframes; k++) {
                spriteframe_t *sprframe;
                int p;

                sprframe = &sprdef->spriteframes[k];
                if(sprframe->rotate) {
                    for(p = 0; p < 8; p++) {
                        GL_BindSpriteTexture(sprframe->lump[p], 0);
                        num++;
                    }
                }
                else {
                    GL_BindSpriteTexture(sprframe->lump[0], 0);
                    num++;
                }
            }
        }
    }

    CON_DPrintf("%i sprites cached\n", num);

    if(GLAD_GL_ARB_multitexture) {
        GL_SetTextureUnit(1, true);
        GL_BindEnvTexture();

        GL_SetTextureUnit(2, true);
        GL_BindDummyTexture();

        GL_SetTextureUnit(3, true);
        GL_BindDummyTexture();
    }

    GL_SetDefaultCombiner();
}

//
// R_SetupFrame
//

void R_SetupFrame(player_t *player) {
    angle_t pitch;
    angle_t angle;
    fixed_t cam_z;
    mobj_t* viewcamera;

    //
    // reset list indexes
    //
    drawlist[DLT_WALL].index = 0;
    drawlist[DLT_FLAT].index = 0;
    drawlist[DLT_SPRITE].index = 0;

    renderplayer = player;

    //
    // reset active textures
    //
    GL_ResetTextures();

    //
    // setup view rotation/position
    //
    viewcamera = player->cameratarget;
    angle = (viewcamera->angle + quakeviewx) + viewangleoffset;
    pitch = viewcamera->pitch + ANG90;
    cam_z = (viewcamera == player->mo ? player->viewz : viewcamera->z) + quakeviewy;

    if(viewcamera == player->mo) {
        pitch += player->recoilpitch;
    }

    viewangle   = R_Interpolate(angle, frame_angle, *i_interpolateframes);
    viewpitch   = R_Interpolate(pitch, frame_pitch, *i_interpolateframes);
    viewx       = R_Interpolate(viewcamera->x, frame_viewx, *i_interpolateframes);
    viewy       = R_Interpolate(viewcamera->y, frame_viewy, *i_interpolateframes);
    viewz       = R_Interpolate(cam_z, frame_viewz, *i_interpolateframes);

    fviewx      = F2D3D(viewx);
    fviewy      = F2D3D(viewy);
    fviewz      = F2D3D(viewz);

    viewsin[0]  = F2D3D(dsin(viewangle));
    viewsin[1]  = F2D3D(dsin(viewpitch - ANG90));

    viewcos[0]  = F2D3D(dcos(viewangle));
    viewcos[1]  = F2D3D(dcos(viewpitch - ANG90));

    D_IncValidCount();
}

//
// R_SetViewClipping
//

static void R_SetViewClipping(angle_t angle) {
    R_Clipper_Clear();
    R_Clipper_SafeAddClipRange(viewangle + angle, viewangle - angle);
    R_FrustrumSetup();
}

//
// R_DrawWireframe
//

void R_DrawWireframe(dboolean enable) {
    r_fillmode = !enable;
    if (!enable) {
        dglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

//
// R_Interpolate
//

fixed_t R_Interpolate(fixed_t ticframe, fixed_t updateframe, dboolean enable) {
    return !enable ? ticframe : updateframe + FixedMul(rendertic_frac, ticframe - updateframe);
}

//
// R_InterpolateSectors
//

static void R_InterpolateSectors(void) {
    int i;

    for(i = 0; i < numsectors; i++) {
        sector_t* s = &sectors[i];

        s->frame_z1[1] = R_Interpolate(s->floorheight, s->frame_z1[0], 1);
        s->frame_z2[1] = R_Interpolate(s->ceilingheight, s->frame_z2[0], 1);
    }
}

//
// R_DrawReadDisk
//

static void R_DrawReadDisk(void) {
    if(!BusyDisk) {
        return;
    }

    Draw_Text(296, 8, WHITE, 1, 0, "**");

    BusyDisk=true;
}

//
// R_DrawBlockMap
//

static void R_DrawBlockMap(void) {
    float   fx;
    float   fy;
    float   fz;
    int     x;
    int     y;
    mobj_t* mo;

    dglDisable(GL_TEXTURE_2D);
    dglDepthRange(0.0f, 0.0f);
    dglColor4ub(0, 128, 255, 255);
    dglPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    mo = players[displayplayer].mo;
    fz = F2D3D(mo->floorz);

    for(x = bmaporgx; x < ((bmapwidth << MAPBLOCKSHIFT) + bmaporgx); x += INT2F(MAPBLOCKUNITS)) {
        for(y = bmaporgy; y < ((bmapheight << MAPBLOCKSHIFT) + bmaporgy); y += INT2F(MAPBLOCKUNITS)) {
            fx = F2D3D(x);
            fy = F2D3D(y);

            dglBegin(GL_POLYGON);
            dglVertex3f(fx, fy, fz);
            dglVertex3f(fx + MAPBLOCKUNITS, fy, fz);
            dglVertex3f(fx + MAPBLOCKUNITS, fy + MAPBLOCKUNITS, fz);
            dglVertex3f(fx, fy + MAPBLOCKUNITS, fz);
            dglEnd();
        }
    }

    dglDepthRange(0.0f, 1.0f);
    dglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    dglEnable(GL_TEXTURE_2D);
}

//
// R_DrawRayTrace
//

static void R_DrawRayTrace(void) {
    thinker_t* thinker;
    tracedrawer_t* tdrawer;

    for(thinker = thinkercap.next; thinker != &thinkercap; thinker = thinker->next) {
        if(thinker->function.acp1 == (actionf_p1)T_TraceDrawer) {
            rcolor c = WHITE;

            tdrawer = ((tracedrawer_t*)thinker);

            if(tdrawer->flags == PT_ADDLINES) {
                c = D_RGBA(0, 0xff, 0, 0xff);
            }
            else if(tdrawer->flags == PT_ADDTHINGS) {
                c = D_RGBA(0, 0, 0xff, 0xff);
            }
            else if(tdrawer->flags == PT_EARLYOUT) {
                c = D_RGBA(0xff, 0, 0, 0xff);
            }
            else if(tdrawer->flags == (PT_ADDLINES | PT_ADDTHINGS)) {
                c = D_RGBA(0, 0xff, 0xff, 0xff);
            }

            dglDepthRange(0.0f, 0.0f);
            dglDisable(GL_TEXTURE_2D);
            dglColor4ubv((byte*)&c);
            dglBegin(GL_LINES);
            dglVertex3f(F2D3D(tdrawer->x1), F2D3D(tdrawer->y1), F2D3D(tdrawer->z) - 8);
            dglVertex3f(F2D3D(tdrawer->x2), F2D3D(tdrawer->y2), F2D3D(tdrawer->z) - 8);
            dglEnd();
            dglEnable(GL_TEXTURE_2D);
            dglDepthRange(0.0f, 1.0f);
        }
    }
}

//
// R_DrawContextWall
// Displays an hightlight over the useable linedef
//

extern line_t* contextline; // from p_map.c
dboolean R_GenerateSwitchPlane(void *line, vtx_t *v); // from r_bsp.c

static vertex_t* TraverseVertex(vertex_t* vertex, line_t* line) {
    int i;
    line_t** l;

    for(i = 0, l = line->frontsector->lines; i < line->frontsector->linecount; i++) {
        if(l[i] == line) {
            continue;
        }

        if(l[i]->v1 == vertex) {
            if(l[i]->angle != line->angle) {
                return vertex;
            }

            if(l[i]->special != line->special) {
                return vertex;
            }

            // keep searching
            return TraverseVertex(l[i]->v2, l[i]);
        }
        else if(l[i]->v2 == vertex) {
            if(l[i]->angle != line->angle) {
                return vertex;
            }

            if(l[i]->special != line->special) {
                return vertex;
            }

            // keep searching
            return TraverseVertex(l[i]->v1, l[i]);
        }
    }

    // stop here
    return vertex;
}

static void R_DrawContextWall(line_t* line) {
    vtx_t vtx[4];

    if(!line) {
        return;
    }

    if(!SWITCHMASK(line->flags)) {
        vertex_t *v1;
        vertex_t *v2;

        //
        // try to merge all parallel lines by
        // finding the farthest left and right vertex
        //
        v1 = TraverseVertex(line->v1, line);
        v2 = TraverseVertex(line->v2, line);

        vtx[0].x = F2D3D(v1->x);
        vtx[1].x = F2D3D(v2->x);
        vtx[2].x = F2D3D(v2->x);
        vtx[3].x = F2D3D(v1->x);
        vtx[0].y = F2D3D(v1->y);
        vtx[1].y = F2D3D(v2->y);
        vtx[2].y = F2D3D(v2->y);
        vtx[3].y = F2D3D(v1->y);
        vtx[0].z = F2D3D(line->frontsector->floorheight);
        vtx[1].z = F2D3D(line->frontsector->floorheight);
        vtx[2].z = F2D3D(line->frontsector->ceilingheight);
        vtx[3].z = F2D3D(line->frontsector->ceilingheight);
    }
    else {
        int i;
        vtx_t v[4];
        seg_t* seg = NULL;

        for(i = 0; i < numsegs; i++) {
            if(segs[i].linedef == line) {
                seg = &segs[i];
                break;
            }
        }

        if(seg == NULL) {
            return;
        }

        R_GenerateSwitchPlane(seg, v);

        vtx[0].x = v[0].x;
        vtx[1].x = v[1].x;
        vtx[2].x = v[3].x;
        vtx[3].x = v[2].x;
        vtx[0].y = v[0].y;
        vtx[1].y = v[1].y;
        vtx[2].y = v[3].y;
        vtx[3].y = v[2].y;
        vtx[0].z = v[0].z;
        vtx[1].z = v[1].z;
        vtx[2].z = v[3].z;
        vtx[3].z = v[2].z;
    }

    //
    // do the actual drawing
    //
    GL_SetState(GLSTATE_BLEND, 1);

    dglDepthRange(0.0f, 0.0f);
    dglDisable(GL_TEXTURE_2D);
    dglDisable(GL_CULL_FACE);
    dglColor4ub(128, 128, 128, 64);
    dglBegin(GL_POLYGON);
    dglVertex3f(vtx[0].x, vtx[0].y, vtx[0].z);
    dglVertex3f(vtx[1].x, vtx[1].y, vtx[1].z);
    dglVertex3f(vtx[2].x, vtx[2].y, vtx[2].z);
    dglVertex3f(vtx[3].x, vtx[3].y, vtx[3].z);
    dglEnd();
    dglColor4ub(255, 255, 255, 255);
    dglPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    dglBegin(GL_POLYGON);
    dglVertex3f(vtx[0].x, vtx[0].y, vtx[0].z);
    dglVertex3f(vtx[1].x, vtx[1].y, vtx[1].z);
    dglVertex3f(vtx[2].x, vtx[2].y, vtx[2].z);
    dglVertex3f(vtx[3].x, vtx[3].y, vtx[3].z);
    dglEnd();
    dglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    dglEnable(GL_TEXTURE_2D);
    dglEnable(GL_CULL_FACE);
    dglDepthRange(0.0f, 1.0f);

    GL_SetState(GLSTATE_BLEND, 0);
}

//
// R_RenderPlayerView
//

void R_RenderPlayerView(player_t *player) {
    if(!r_fillmode) {
        dglPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if(devparm) {
        renderTic = I_GetTimeMS();
    }

    //
    // clear sprite list
    //
    R_ClearSprites();

    //
    // setup draw frame
    //
    R_SetupFrame(player);

    //
    // check for t-junction cracks
    //
    if(r_drawfill) {
        dglClearColor(1, 0, 1, 0);
        dglClear(GL_COLOR_BUFFER_BIT);
        bRenderSky = false;
    }

    //
    // draw sky
    //
    if(bRenderSky) {
        R_DrawSky();
    }

    bRenderSky = false;

    //
    // setup view matrix
    //
    R_SetViewMatrix();

    //
    // check for new console commands
    //
    NetUpdate();

    //
    // setup clipping
    //
    R_SetViewClipping(R_FrustumAngle());

    //
    // interpolate moving sectors before draw
    //
    if(i_interpolateframes) {
        R_InterpolateSectors();
    }

    //
    // traverse BSP for rendering
    //
    R_RenderBSPNode(numnodes-1);

    //
    // check for new console commands
    //
    NetUpdate();

    //
    // render world
    //
    R_RenderWorld();

    if(r_drawblockmap) {
        R_DrawBlockMap();
    }

    if(r_drawmobjbox) {
        R_DrawThingBBox();
    }

    if(r_drawtrace) {
        R_DrawRayTrace();
    }

    if(p_usecontext) {
        R_DrawContextWall(contextline);
    }

    //
    // render player weapon sprites
    //
    if(ShowGun && player->cameratarget == player->mo &&
            !(player->cheats & CF_SPECTATOR)) {
        R_RenderPlayerSprites(player);
    }

    if(devparm) {
        spriteRenderTic = (I_GetTimeMS() - spriteRenderTic);
    }

    if(devparm) {
        R_DrawReadDisk();
    }

    if(devparm) {
        renderTic = (I_GetTimeMS() - renderTic);
    }

    //
    // check for new console commands
    //
    NetUpdate();
}
