/******************************************************************************
 * highlighter.cpp - Color the text in the editor
 *
 * qtpov - A Qt IDE frontend for POV-Ray
 * Copyright(c) 2017 - Dick Balaska, and BuckoSoft.
 *
 * qtpov is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * qtpov is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include "highlighter.h"

PCSTR common_list[] =  {
	"aa_level", 			"aa_threshold",				"absorption",				"accuracy",
	"adaptive",				"adc_bailout",				"all",						"all_intersections",
	"altitude",				"always_sample",			"ambient",					"ambient_light",
	"angle",				"aperture",					"append",					"area_light",
	"array",				"asc",						"ascii2",					"assumed_gamma",
	"autostop",				"average",
	"b_spline",				"background",				"bezier_spline",			"bicubic_patch",
	"black_hole",			"blur_samples",				"bounded_by",				"boxed",
	"bozo",					"break",					"brick_size",				"bump_map",
	"bump_size",
	"camera",				"case",						"caustics",
	"cells",				"charset",					"chr",						"circular",
	"clipped_by",
	"collect",				"component",				"composite",				"concat",
	"confidence",			"conserve_energy",			"contained_by",				"control0",
	"control1",				"coords",					"count",					"crand",
	"debug",				"declare",					"default",					"defined",
	"density_file",			"density_map",				"df3",						"difference",
	"diffuse",				"dimension_size",			"dimensions",				"direction",
	"dispersion",			"dispersion_samples",		"dist_exp",					"distance",
	"div",					"double_illuminate",
	"else",					"elseif",					"emission",					"end",
	"error",
	"error_bound",			"evaluate",					"exp",						"expand_thresholds",
	"exponent",				"exterior",					"extinction",
	"face_indices",			"facets",					"false",					"fclose",
	"file_exists",			"filter",
	"fisheye",				"focal_point",				"fog_alt",					"fog_offset",
	"fog_type",				"fopen",					"for",						"form",
	"fresnel",				"function",
	"gather",				"gif",						"global_lights",			"global_settings",
	"gray_threshold",
	"hf_gray_16",			"hierarchy",				"hypercomplex",
	"if",					"ifdef",					"iff",						"ifndef",
	"image_height",			"image_width",				"include",					"initial_clock",
	"initial_frame",		"inside",					"inside_vector",			"int",
	"internal",				"interpolate",				"intersection",				"intervals",
	"inverse",				"irid_wavelength",
	"jitter",				"jpeg",
	"lambda",				"light_group",				"light_source",				"load_file",
	"local",				"location",					"log",						"look_at",
	"looks_like",			"low_error_factor",
	"macro",				"magnet",					"major_radius",				"map_type",
	"matrix",				"max_extent",				"max_gradient",
	"max_intersections",	"max_iteration",			"max_sample",				"max_trace",
	"max_trace_level",		"media_attenuation",		"media_interaction",		"merge",
	"method",				"metric",					"min_extent",
	"minimum_reuse",		"mortar",
	"natural_spline",		"nearest_count",			"no_bump_scale",
	"no_image",				"no_shadow",				"number_of_waves",
	"octaves",				"offset",					"omega",
	"omnimax",				"once",						"open",
	"orient",				"orientation",				"orthographic",
	"panoramic",			"parallel",					"parametric",				"pass_through",
	"perspective",			"pgm",						"phase",					"pi",
	"png",					"point_at",					"poly",						"poly_wave",
	"ppm",					"precision",				"precompute",				"pretrace_end",
	"pretrace_start",		"prod",						"projected_through",
	"quaternion",
	"radiosity",			"radius",					"ramp_wave",				"range",
	"ratio",				"read",						"reciprocal",				"recursion_limit",
	"render",				"repeat",					"right",
	"samples",				"save_file",				"scallop_wave",				"select",
	"shadowless",			"sine_wave",				"size",						"slice",
	"slope",				"slope_map",				"smooth",					"smooth_triangle",
	"sor",					"spacing",					"spotlight",				"statistics",
	"str",					"strlwr",
	"strupr",				"substr",					"switch",					"sys",
	"t",					"target",					"tga",						"tiff",
	"trace",				"true",						"ttf",						"type",
	"u",					"u_steps",					"ultra_wide_angle",			"undef",
	"union",				"up",						"use_alpha",				"use_color",
	"use_colour",			"use_index",				"utf8",						"uv_indices",
	"uv_mapping",			"uv_vectors",
	"v",					"v_steps",					"val",						"vaxis_rotate",
	"vcross",				"vdot",						"version",					"vertex_vectors",
	"vlength",				"vnormalize",				"vrotate",					"vstr",
	"vturbulence",
	"warning",				"warp",						"water_level",				"while",
	"width",				"write",
	NULL
};
PCSTR math_list[] = {
	"abs",					"acos",						"acosh",				"asin",
	"asinh",				"atan",						"atan2",				"atanh",
	"arc_angle",
	"ceil",					"clock",					"clock_delta",			"clock_on",
	"cos",					"cosh",
	"degrees",
	"final_clock",			"final_frame",				"floor",				"frame_number",
	"ln",
	"min",					"max",						"mod",
	"no",
	"off",					"on",
	"pot",					"pow",						"pwr",
	"radians",				"rand",
	"seed",					"sin",						"sinh",					"sqr",
	"sqrt",					"strcmp",					"strlen",				"sum",
	"tan",					"tanh",
	"yes",
	NULL
};

PCSTR modifier_list[] = {
	"brightness",			"brilliance",				"bumps",
	"conic_sweep",			"crackle",					"cylindrical",
	"density",				"dents",
	"eccentricity",
	"flatness",				"flip",						"frequency",
	"gradient",
	"hollow",
	"phong",				"phong_size",				"photons",
	"planar",
	"quilted",
	"radial",				"reflection",				"reflection_exponent",		"refraction",
	"ripples",				"rotate",					"roughness",
	"scale",				"scattering",				"solid",					"specular",
	"sphere_sweep",			"spherical",				"strength",					"sturm",
	"thickness",			"threshold",				"tightness",				"tolerance",
	"toroidal",				"transform",				"translate",				"transmit",
	"turb_depth",			"turbulence",
	"variance",
	"wrinkles",
	NULL

};

PCSTR object_list[] = {
	"blob",					"box",						"brick",					"checker",
	"cone",					"cube",						"cubic",					"cubic_spline",
	"cubic_wave",			"cylinder",
	"disc",
	"fog",
	"height_field",			"hexagon",
	"isosurface",
	"julia",				"julia_fractal",
	"lathe",				"linear_spline",			"linear_sweep",
	"mandel",				"mesh",						"mesh2",
	"object",
	"plane",				"polygon",					"prism",
	"quadratic_spline",		"quadric",					"quartic",
	"sky",					"sky_sphere",				"sphere",					"spiral1",
	"spiral2",				"spline",					"split_union",				"superellipsoid",
	"text",					"tile2",					"tiles",					"torus",
	"triangle",				"triangle_wave",
	"waves",
	NULL
};

PCSTR texture_list[] = {
	"agate",				"agate_turb",
	"color",				"color_map",				"colour",					"colour_map",
	"cutaway_textures",
	"fade_color",			"fade_colour",				"fade_distance",			"fade_power",
	"falloff",				"falloff_angle",			"finish",
	"granite",
	"image_map",			"image_pattern",			"interior",					"interior_texture",
	"ior",					"irid",
	"leopard",
	"marble",				"material",					"material_map",				"media",
	"metallic",
	"no_reflection",		"noise_generator",			"normal",					"normal_indices",
	"normal_map",			"normal_vectors",
	"onion",
	"pattern",				"pigment",					"pigment_map",				"pigment_pattern",
	"rainbow",
	"spotted",
	"texture",				"texture_list",				"texture_map",
	"wood",
	NULL
};

PCSTR color_list[] = {
	"alpha",
	"blue",
	"gray",					"green",
	"quick_color",			"quick_colour",
	"red",					"rgb",						"rgbf",						"rgbft",
	"rgbt",
	NULL
};

enum {
	inCode,
	inComment,
	inString
};

Highlighter::Highlighter(PreferenceData* prefs, QTextDocument* parent)
	: QSyntaxHighlighter(parent)
{
	//HighlightingRule rule;

	setupType(commonFormat, common_list, prefs->getEditorColors()->common);
	setupType(mathFormat, math_list, prefs->getEditorColors()->math);
	setupType(modifierFormat, modifier_list, prefs->getEditorColors()->modifier);
	setupType(objectFormat, object_list, prefs->getEditorColors()->object);
	setupType(textureFormat, texture_list, prefs->getEditorColors()->texture);
	setupType(colorFormat, color_list, prefs->getEditorColors()->color);

	quotationFormat.setForeground(prefs->getEditorColors()->string.getColor());
	quotationFormat.setFontWeight(prefs->getEditorColors()->string.isBold() ? QFont::Bold : QFont::Normal);

	singleLineCommentFormat.setForeground(prefs->getEditorColors()->comment.getColor());
	singleLineCommentFormat.setFontWeight(prefs->getEditorColors()->comment.isBold() ? QFont::Bold : QFont::Normal);

	multiLineCommentFormat.setForeground(prefs->getEditorColors()->comment.getColor());
	multiLineCommentFormat.setFontWeight(prefs->getEditorColors()->comment.isBold() ? QFont::Bold : QFont::Normal);


	commentStartExpression = QRegularExpression("/\\*");
	commentEndExpression = QRegularExpression("\\*/");
	singleLineCommentExpression = QRegularExpression("//[^\n]*");
	stringMarkerExpression = QRegularExpression("\"");
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Highlighter::highlightBlock
/// \param text The line of text to decorate
/// This big loop is to track where comments and strings begin and end.
/// We also cache bracket data while we're here.
///
void Highlighter::highlightBlock(const QString &text) {
	foreach (const HighlightingRule &rule, highlightingRules) {
		QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
		while (matchIterator.hasNext()) {
			QRegularExpressionMatch match = matchIterator.next();
			setFormat(match.capturedStart(), match.capturedLength(), rule.format);
		}
	}
	setCurrentBlockState(previousBlockState());
	int index = 0;
	BlockData* blockData = new BlockData;
	int commentStart = 0;

	while (index >= 0) {
		if (currentBlockState() == inComment) {
			QRegularExpressionMatch match = commentEndExpression.match(text, index);
			int endIndex = match.capturedStart();
			int commentLength = 0;
			if (endIndex == -1) {
				commentLength = text.length() - commentStart;
				index = -1;
			} else {
				commentLength = endIndex - commentStart + match.capturedLength();
				setCurrentBlockState(inCode);
			}
			setFormat(commentStart, commentLength, multiLineCommentFormat);
			continue;
		} else if (currentBlockState() == inString) {
			QRegularExpressionMatch match = stringMarkerExpression.match(text, index);
			int endIndex = match.capturedStart();
			int sstart = index;
			int stringLength = 0;
			if (endIndex == -1) {
				stringLength = text.length() - index;
				index = -1;
			} else {
				stringLength = endIndex - index + match.capturedLength()-1;
				setCurrentBlockState(inCode);
				index = endIndex+1;
			}
			setFormat(sstart, stringLength, quotationFormat);
			continue;
		}
		// running through code. Figure out how far we can go.
		QRegularExpressionMatch smatch = stringMarkerExpression.match(text, index);
		QRegularExpressionMatch cmatch = commentStartExpression.match(text, index);
		QRegularExpressionMatch omatch = singleLineCommentExpression.match(text, index);
		int lowest = -1;
		int sindex = smatch.capturedStart();
		if (sindex != -1)
			lowest = sindex;
		int cindex = cmatch.capturedStart();
		if (cindex != -1) {
			if (lowest == -1 || cindex < lowest)
				lowest = cindex;
		}
		int oindex = omatch.capturedStart();
		if (oindex != -1) {
			if (lowest == -1 || oindex < lowest)
				lowest = oindex;
		}
		int codeEnd = lowest;
		if (codeEnd == -1)
			codeEnd = text.length();
		for (int i = index; i<codeEnd; i++) {
			QChar c = text[i];
			if (c =='[' || c == ']' || c == '{' || c == '}'
			 || c == '(' || c == ')' || c == '<' || c == '>') {
				BracketInfo pi(c, i);
				blockData->brackets.append(pi);
			}
		}
		if (lowest == -1)
			break;
		index = codeEnd;
		if (lowest == oindex) {
			setFormat(index, text.length()-index, singleLineCommentFormat);
			setCurrentBlockState(inCode);
			break;
		}
		if (lowest == sindex) {
			index++;
			setCurrentBlockState(inString);
			continue;
		}
		// lowest == cindex
		commentStart = index;
		index += 2;
		setCurrentBlockState(inComment);
	}
	setCurrentBlockUserData(blockData);
}

void Highlighter::setupType(QTextCharFormat& format, PCSTR* list, Highlight& highlight) {
	HighlightingRule rule;
	QString qs;
	PCSTR* p = list;
	format.setForeground(highlight.getColor());
	format.setFontWeight(highlight.isBold() ? QFont::Bold : QFont::Normal);
	while (*p != NULL) {
		qs = "\\b"; qs.append(*p).append("\\b");
		rule.pattern = QRegularExpression(qs);
		rule.format = format;
		highlightingRules.append(rule);
		p++;
	}
}
