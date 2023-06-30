/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 * 2022 GEGL Glowstick - Beaver.
 */

/*
Recreation of Glow Sticks GEGL Graph 

gimp:desaturate
invert-gamma
id=1
gimp:layer-mode layer-mode=hsl-color aux=[ ref=1 color-overlay value=#ffa2f8 ]
softglow brightness=0.1
bloom radius=3
exposure black-level=6 exposure=-0.4
noise-reduction
id=2
multiply aux=[ ref=2  ]
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_color (color, _("Color of Glowstick"), "#ffacf9")
    description (_("The color to render (defaults to 'black')"))
    ui_meta     ("role", "color-primary")



#define TUTORIAL \
" gimp:desaturate invert-gamma  :\n"\


enum_start (gegl_blend_mode_typeglowstick)
  enum_value (GEGL_BLEND_MODE_TYPE_GRAINMERGE, "grainmerge",
              N_("GrainMerge"))
  enum_value (GEGL_BLEND_MODE_TYPE_HSLCOLOR,      "hslcolor",
              N_("HSLColor"))
  enum_value (GEGL_BLEND_MODE_TYPE_SOFTLIGHT,      "softlight",
              N_("SoftLight"))
  enum_value (GEGL_BLEND_MODE_TYPE_OVERLAY,      "overlay",
              N_("Overlay"))
  enum_value (GEGL_BLEND_MODE_TYPE_BURN,      "burn",
              N_("Burn"))
  enum_value (GEGL_BLEND_MODE_TYPE_LCHCOLOR,      "lchcolor",
              N_("LChColor"))
  enum_value (GEGL_BLEND_MODE_TYPE_MULTIPLY,      "multiply",
              N_("Multiply"))
  enum_value (GEGL_BLEND_MODE_TYPE_LINEARLIGHT,      "linearlight",
              N_("LinearLight"))
 enum_value (GEGL_BLEND_MODE_TYPE_HARDLIGHT,      "hardlight",
              N_("HardLight"))

property_enum (blendmode, _("Internal Blend Mode of Glowstick"),
    GeglBlendModeTypeglowstick, gegl_blend_mode_typeglowstick,
    GEGL_BLEND_MODE_TYPE_SOFTLIGHT)
enum_end (GeglBlendModeTypeglowstick)

property_int  (noisereduction, _("Smooth Original Image"), 2)
  description (_("Controls the number of iterations"))
  value_range (0, 6)
  ui_range    (0, 6)

property_double (sat, _("Chroma"), 0)
    description(_("Scale, strength of effect"))
    value_range (0, 15.0)
    ui_range (0, 15.0)

property_double (lightness, _("Darkness to light"), 0.0)
   description  (_("Lightness adjustment"))
   value_range  (-24, 7.0)


property_double (strength, _("Bloom Glow Strength (turns on Bloom)"), 0.0)
    description (_("Glow strength"))
    value_range (0.0, G_MAXDOUBLE)
    ui_range    (0.0, 50.0)

property_double (softness, _("Bloom Glow Softness"), 7.0)
    description (_("Glow-area edge softness"))
    value_range (7.0, G_MAXDOUBLE)
    ui_range    (7.0, 77.0)


property_double (radius, _("Bloom Glow Radius"), 10.0)
    description (_("Glow radius"))
    value_range (0.0, 100.0)
    ui_range    (0.0, 100.0)
    ui_gamma    (2.0)
    ui_meta     ("unit", "pixel-distance")


property_double (brightness, _("Soft Glow Brightness (turns on Softglow)"), 0.0)
    value_range (0.0, 0.25)


property_double (glow_radius, _("Soft Glow radius"), 0.0)
    value_range (1.0, 150.0)
    ui_meta    ("unit", "pixel-distance")


#else

#define GEGL_OP_META
#define GEGL_OP_NAME     glowstick
#define GEGL_OP_C_SOURCE glowstick.c

#include "gegl-op.h"


typedef struct
{
  GeglNode *input;
  GeglNode *nop; 
  GeglNode *bloom; 
  GeglNode *softglow; 
  GeglNode *grainmerge; 
  GeglNode *softlight; 
  GeglNode *hardlight; 
  GeglNode *hslcolor; 
  GeglNode *crop; 
  GeglNode *lchcolor; 
  GeglNode *burn; 
  GeglNode *multiply; 
  GeglNode *linearlight; 
  GeglNode *overlay; 
  GeglNode *gegl1;
  GeglNode *color;
  GeglNode *lightchroma;
  GeglNode *noisereduction;  
  GeglNode *output;
}State;

static void
update_graph (GeglOperation *operation)
{
  GeglProperties *o = GEGL_PROPERTIES (operation);
  State *state = o->user_data;
  if (!state) return;

  GeglNode *usethis = state->softlight; /* the default */
  switch (o->blendmode) {
    case GEGL_BLEND_MODE_TYPE_GRAINMERGE: usethis = state->grainmerge; break;
    case GEGL_BLEND_MODE_TYPE_HSLCOLOR: usethis = state->hslcolor; break;
    case GEGL_BLEND_MODE_TYPE_SOFTLIGHT: usethis = state->softlight; break;
    case GEGL_BLEND_MODE_TYPE_OVERLAY: usethis = state->overlay; break;
    case GEGL_BLEND_MODE_TYPE_BURN: usethis = state->burn; break;
    case GEGL_BLEND_MODE_TYPE_LCHCOLOR: usethis = state->lchcolor; break;
    case GEGL_BLEND_MODE_TYPE_MULTIPLY: usethis = state->multiply; break;
    case GEGL_BLEND_MODE_TYPE_LINEARLIGHT: usethis = state->linearlight; break;
    case GEGL_BLEND_MODE_TYPE_HARDLIGHT: usethis = state->hardlight; break;
  }
  gegl_node_link_many (state->input,  state->noisereduction, state->gegl1, state->nop, usethis, state->crop, state->lightchroma, state->softglow, state->bloom, state->output,  NULL);
  gegl_node_connect_from (usethis, "aux", state->color, "output");

}

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
GeglProperties *o = GEGL_PROPERTIES (operation);
  GeglNode *input, *output, *nop, *color, *gegl1, *bloom, *linearlight, *hardlight, *crop, *lightchroma, *burn, *multiply, *softglow, *hslcolor, *lchcolor, *overlay, *softlight, *grainmerge, *noisereduction;

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");


  color    = gegl_node_new_child (gegl,
                                  "operation", "gegl:color",
                                  NULL);

  crop    = gegl_node_new_child (gegl,
                                  "operation", "gegl:crop",
                                  NULL);


  bloom    = gegl_node_new_child (gegl,
                                  "operation", "gegl:bloom",
                                  NULL);

  softglow    = gegl_node_new_child (gegl,
                                  "operation", "gegl:softglow",
                                  NULL);

  nop    = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);

hslcolor = gegl_node_new_child (gegl,
                                  "operation", "gimp:layer-mode", "layer-mode", 39, "composite-mode", 0, NULL);

grainmerge = gegl_node_new_child (gegl,
                                  "operation", "gimp:layer-mode", "layer-mode", 47, "composite-mode", 0, NULL);

softlight = gegl_node_new_child (gegl,
                                  "operation", "gimp:layer-mode", "layer-mode", 45, "composite-mode", 0, NULL);

hardlight = gegl_node_new_child (gegl,
                                  "operation", "gimp:layer-mode", "layer-mode", 44, "composite-mode", 0, NULL);


overlay = gegl_node_new_child (gegl,
                                  "operation", "gimp:layer-mode", "layer-mode", 23, "composite-mode", 0, NULL);

burn = gegl_node_new_child (gegl,
                                  "operation", "gimp:layer-mode", "layer-mode", 43, "composite-mode", 0,  "blend-space", 2, NULL);

lchcolor = gegl_node_new_child (gegl,
                                    "operation", "gimp:layer-mode", "layer-mode", 26, "composite-mode", 0,  "blend-space", 3, NULL);

multiply = gegl_node_new_child (gegl,
                                    "operation", "gimp:layer-mode", "layer-mode", 30, "composite-mode", 0,  "blend-space", 2, NULL);

linearlight = gegl_node_new_child (gegl,
                                    "operation", "gimp:layer-mode", "layer-mode", 50, "composite-mode", 0,  "blend-space", 2, NULL);


  lightchroma    = gegl_node_new_child (gegl,
                                  "operation", "gegl:hue-chroma",
                                  NULL);

  noisereduction    = gegl_node_new_child (gegl,
                                  "operation", "gegl:noise-reduction",
                                  NULL);

  gegl1    = gegl_node_new_child (gegl,
                                  "operation", "gegl:gegl", "string", TUTORIAL,
                                  NULL);


  gegl_operation_meta_redirect (operation, "sat", lightchroma, "chroma");
  gegl_operation_meta_redirect (operation, "lightness", lightchroma, "lightness");
  gegl_operation_meta_redirect (operation, "noisereduction", noisereduction, "iterations");
  gegl_operation_meta_redirect (operation, "string1", gegl1, "string");
  gegl_operation_meta_redirect (operation, "brightness", softglow, "brightness");
  gegl_operation_meta_redirect (operation, "glow_radius", softglow, "glow-radius");
  gegl_operation_meta_redirect (operation, "radius", bloom, "radius");
  gegl_operation_meta_redirect (operation, "softness", bloom, "softness");
  gegl_operation_meta_redirect (operation, "strength", bloom, "strength");
  gegl_operation_meta_redirect (operation, "color", color, "value");


  gegl_node_link_many (input,  noisereduction,  gegl1, nop, softlight, crop, lightchroma, bloom, softglow, output, NULL);
  gegl_node_connect_from (softlight, "aux", color, "output");
  gegl_node_link_many (nop, lightchroma, NULL);

  /* now save references to the gegl nodes so we can use them
   * later, when update_graph() is called
   */
  State *state = g_malloc0 (sizeof (State));
  state->input = input;
  state->noisereduction = noisereduction;
  state->gegl1 = gegl1;
  state->nop = nop;
  state->softlight = softlight;
  state->burn = burn;
  state->lchcolor = lchcolor;
  state->grainmerge = grainmerge;
  state->multiply = multiply;
  state->hardlight = hardlight;
  state->linearlight = linearlight;
  state->overlay = overlay;
  state->hslcolor = hslcolor;
  state->crop = crop;
  state->color = color;
  state->lightchroma = lightchroma;
  state->bloom = bloom;
  state->softglow = softglow;
  state->output = output;

  o->user_data = state;
}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;
GeglOperationMetaClass *operation_meta_class = GEGL_OPERATION_META_CLASS (klass);
  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;
  operation_meta_class->update = update_graph;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:glowstick",
    "title",       _("Glow Stick"),
    "categories",  "Artistic",
    "reference-hash", "ha3g451fv0nyesyeesg5sgac",
    "description", _("GEGL makes a image neon like a glow stick"
                     ""),
    NULL);
}

#endif
